#include "stdafx.h"
bool CodeAtlas::CutModularizer::setDependency( const SparseMatrix& veMat, const VectorXd& edgeWeightList )
{
	if (veMat.cols() != edgeWeightList.size())
	{
		return false;
	}
	m_veMat = veMat;
	m_edgeWeightList = edgeWeightList;
	return true;
}

void CodeAtlas::CutModularizer::computeStructualSim()
{
	int nNodes = m_nodeList.size();
	for (int ithEdge = 0; ithEdge < m_veMat.cols(); ++ithEdge)
	{
		int src, tar;
		GraphUtility::getVtxFromEdge(m_veMat, ithEdge, src, tar);

		int row = min(src, tar);
		int col = max(src, tar);

		m_simMat(row, col) += m_edgeWeightList[ithEdge];
	}

	// normalize result to 0-1
	float minSim = FLT_MAX, maxSim = 0;
	for (int r = 0; r < nNodes; ++r)
	{
		for (int c = r+1; c < nNodes; ++c)
		{
			minSim = min(minSim, m_simMat(r,c));
			maxSim = max(maxSim, m_simMat(r,c));
		}
	}

	float invWeight = 1.f / (maxSim-minSim+1e-5f);
	for (int r = 0; r < nNodes; ++r)
	{
		for (int c = r+1; c < nNodes; ++c)
		{
			m_simMat(r,c) = (m_simMat(r,c) - minSim) * invWeight;
		}
	}
}

void CodeAtlas::CutModularizer::computeSemanticSim()
{
	int nNodes = m_nodeList.size();

	// build document-term matrix
	vector<Triplet>	tripletList;
	for (int i = 0; i < m_nodeList.size(); ++i)
	{
		SymbolWordAttr::Ptr attr = m_nodeList[i]->getAttr<SymbolWordAttr>();
		if (!attr)
			continue;

		typedef QMap<int,float> WordMap;
		const WordMap& wordMap = attr->getWordWeightMap();
		for (WordMap::ConstIterator pW = wordMap.begin(); pW != wordMap.end(); ++pW)
		{
			int wordID = pW.key();
			float wordFreq = pW.value();
			tripletList.push_back(Triplet(i, wordID, wordFreq));
		}
	}

	int nWords = SymbolWordAttr::totalWords();
	SparseMatrix	docTermMat(nNodes, nWords);
	docTermMat.setFromTriplets(tripletList.begin(), tripletList.end());

	// perform LSI
	int nTopic = min(50.f, 2 + sqrt(float(nNodes)));
	int t0 = clock();
	//printf("perform LSI: %d docs, %d words, topic count = %d\n", nNodes, nWords, nTopic);
	SVDSolver solver;
	solver.setMat(docTermMat);
	solver.compute(nTopic);
	MatrixXf US;
	solver.getUS(US);
	VectorXf S;
	solver.getS(S);
	int t1 = clock();
	//printf("SVD completed. %f s\n", (t1-t0) / float(CLOCKS_PER_SEC));

	/*printf("singular values\n");
	for (int i = 0; i < S.size(); ++i)
	{
		printf("%f ", S[i]);
	}
	printf("\n");*/


	// compute approx cosine sim
	int nComp = US.cols();
	float* p = US.data();
	for (int i = 0; i < nNodes; ++i, ++p)
	{
		float* q = p+1;
		for (int j = i+1; j < nNodes; ++j, ++q)
		{
			float product = 0;
			float pLength = 0, qLength = 0;
			float* pv = p, *qv = q;
			for (int ithComp = 0; ithComp < nComp; ++ithComp)
			{
				product += *pv * *qv;
				pLength += *pv * *pv; 
				qLength += *qv * *qv; 
				pv  += nNodes;
				qv  += nNodes;
			}

			m_simMat(j, i) = product / sqrt(pLength * qLength + 1e-5);
		}
	}
	int t2 = clock();
	//printf("cos similarity completed. %f s\n", (t2-t1) / float(CLOCKS_PER_SEC));

// 	std::string str = MathUtility::matToString(docTermMat, "docTermMat");
// 	str += MathUtility::matToString(m_simMat, "simMat");
// 	char fileName[30];
// 	sprintf(fileName, "doc%d.m", rand());
// 	ofstream file(fileName);
// 	file << str;
// 	file.close();
// 	printf(fileName);

	// normalize result to 0-1
	float minSim = FLT_MAX, maxSim = -FLT_MAX;
	for (int c = 0; c < nNodes; ++c)
	{
		for (int r = c+1; r < nNodes; ++r)
		{
			minSim = min(minSim, m_simMat(r,c));
			maxSim = max(maxSim, m_simMat(r,c));
		}
	}

	float invWeight = 1.f / (maxSim-minSim+1e-5f);
	for (int c = 0; c < nNodes; ++c)
	{
		for (int r = c+1; r < nNodes; ++r)
		{
			m_simMat(r,c) = (m_simMat(r,c) - minSim) * invWeight;
		}
	}

	//printf("\n\n\n\n");
}

bool CodeAtlas::CutModularizer::compute()
{
	if (m_nodeList.size() == 0)
	{
		return false;
	}

	bool hasEdge = m_veMat.cols() != 0;
	if (m_nodeList.size() != m_veMat.rows() && hasEdge)
	{
		return false;
	}

	int nNodes = m_nodeList.size();
	m_simMat.setZero(nNodes, nNodes);
	computeStructualSim();
	computeSemanticSim();

	m_componentList.clear();
	findCluster();

	return true;
}

void CodeAtlas::CutModularizer::findCluster()
{
	int nNodes = m_nodeList.size();
	if (nNodes < 1)
	{
		return;
	}
	if (nNodes == 1)
	{
		m_componentList.push_back(0);
		return;
	}

	// compute score
	/*
	int totScore = nNodes * (nNodes-1) / 2;
	vector<float> score;
	score.reserve(totScore);
	for (int r = 0, i = 0; r < nNodes; r++)
	{
		for (int c = r+1; c < nNodes; c++, ++i)
		{
			score.push_back(m_simMat(r,c) * (1-m_semanticWeight) + m_simMat(c,r) * m_semanticWeight);
		}
	}

	// find threshold
	sort(score.begin(), score.end());
	int thresholdID = max((score.size()-1) * 0.96f, score.size()-1.f-nNodes*3);
	*/
	float threshold = 0.75;//score[thresholdID];
	printf("threshold:%f\n", threshold);

	// extract edge
	m_componentList.resize(nNodes);
	for (int i = 0; i < nNodes; ++i)
	{
		m_componentList[i] = -1;
	}

	vector<int>	compSize;					// size of each component
	compSize.reserve(nNodes);
	int* queue = new int[nNodes];
	for (int seed = 0, nComp = 0; seed < nNodes; seed++)
	{
		// ignore processed nodes
		if (m_componentList[seed] != -1)
		{
			continue;
		}

		// BFS
		int  front = 0, back = 0;
		queue[back++] = seed;
		m_componentList[seed] = nComp;
		while (front < back)
		{
			// pop front
			int node = queue[front++];

			// find adjacent edges
			for (int other = 0; other < nNodes; ++other)
			{
				// ignore traversed nodes
				if (m_componentList[other] != -1 || other == node)
					continue;

				// ignore low-similarity nodes
				int   r = min(node, other);
				int   c = max(node, other);
				float s = getSimilarity(r,c);// m_simMat(r,c) * (1-m_semanticWeight) + m_simMat(c,r) * m_semanticWeight;
				if (s <= threshold)
				{
					continue;
				}

				// find adj node
				queue[back++] = other;
				m_componentList[other] = nComp;
			}
		}
		compSize.push_back(front);
		nComp++;
	}
	delete[] queue;

	// merge little components
	const int minCompSize = 3;
	for (int ithNode = 0; ithNode < nNodes; ++ithNode)
	{
		int oldCompID = m_componentList[ithNode];
		if (compSize[oldCompID] >= minCompSize)
			continue;

		// for small component, find its most similar big component
		float maxSim = 0;
		int   maxNode= -1;
		for (int other = 0; other < nNodes; ++other)
		{
			if (ithNode == other || compSize[m_componentList[other]] < minCompSize)
				continue;

			int r = min(other, ithNode);
			int c = max(other, ithNode);
			float sim = getSimilarity(r, c);
			if (sim > maxSim)
			{
				maxSim = sim;
				maxNode = other;
			}
		}
		if (maxNode != -1)
		{
			int newCompID = m_componentList[maxNode];
			compSize[oldCompID]--;
			compSize[newCompID]++;
			m_componentList[ithNode] = newCompID;
		}
	}

	// merge index
	vector<int> compMap(compSize.size());
	for (int ithComp = 0, validComp = 0; ithComp < compSize.size(); ++ithComp)
	{
		if (compSize[ithComp] == 0)
			continue;

		compMap[ithComp] = validComp++;
	}
	for (int ithNode = 0; ithNode < nNodes; ++ithNode)
	{
		m_componentList[ithNode] = compMap[m_componentList[ithNode]];
	}
}

int CodeAtlas::CutModularizer::getComponent( int ithNode )
{
	if (ARRAY_OUT_OF_RANGE(ithNode, m_componentList.size()))
	{
		return -1;
	}
	return m_componentList[ithNode];
}

