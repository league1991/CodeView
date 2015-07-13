#include "stdafx.h"

using namespace std;

bool CodeAtlas::RandomWalkClusterer::setVtxCount( int nVtx )
{
	if (nVtx <= 0)
	{
		return false;
	}
	m_vtx.resize(nVtx);
	return true;
}

void CodeAtlas::RandomWalkClusterer::clear()
{
	m_vtx.clear();
}

void CodeAtlas::RandomWalkClusterer::init()
{
	// assign every vertex a unique cluster
	for (int i = 0; i < m_vtx.size(); ++i)
	{
		m_vtx[i].m_clusterID = i;
	}
	m_nCluster = m_vtx.size();
}

bool CodeAtlas::RandomWalkClusterer::addDirectedEdge( int srcVtx, int tarVtx, float weight )
{
	int nVtx = m_vtx.size();
	if (ARRAY_OUT_OF_RANGE(srcVtx, nVtx) || ARRAY_OUT_OF_RANGE(tarVtx, nVtx))
	{
		return false;
	}

	Vertex& sVtx = m_vtx[srcVtx];
	Vertex& tVtx = m_vtx[tarVtx];

	if (sVtx.m_outEdge.contains(tarVtx))
		sVtx.m_outEdge[tarVtx] += weight;
	else
		sVtx.m_outEdge[tarVtx] = weight;


	if (tVtx.m_inEdge.contains(srcVtx))
		tVtx.m_inEdge[srcVtx] += weight;
	else
		tVtx.m_inEdge[srcVtx] = weight;
	return true;
}

bool CodeAtlas::RandomWalkClusterer::addUndirectedEdge( int srcVtx, int tarVtx, float weight )
{
	return addDirectedEdge(srcVtx, tarVtx, weight) &&
		addDirectedEdge(tarVtx, srcVtx, weight);
}

void CodeAtlas::RandomWalkClusterer::randomWalk()
{
	srand(m_seed);
	int nVtx = m_vtx.size();

	typedef QHash<int, double> ClusterMap;
	typedef QHash<int, float> EdgeMap; 
	for (int ithIter = 0 ; ithIter < m_maxIter;)
	{
		// choose a vtx to modify randomly
		int curVtxID = rand()%nVtx;//randInt(nVtx);
		//printf("vtx %d\n", curVtxID);
		Vertex& curVtx= m_vtx[curVtxID];

		// collect near cluster
		ClusterMap nearClusterWeight;
		for (EdgeMap::Iterator pE = curVtx.m_outEdge.begin(); pE != curVtx.m_outEdge.end(); ++pE)
		{
			int tarID = pE.key();
			double w   = pE.value();
			if (w == 0)
				continue;
			Vertex& nearVtx = m_vtx[tarID];
			if (nearClusterWeight.contains(nearVtx.m_clusterID))
			{
				nearClusterWeight[nearVtx.m_clusterID] += w;
			}
			else
			{
				nearClusterWeight[nearVtx.m_clusterID] = w;
			}
		}
		if (nearClusterWeight.size() == 0)
			continue;

		// compute probability
		double maxWeight = 0;
		for (ClusterMap::Iterator pC = nearClusterWeight.begin(); pC != nearClusterWeight.end(); ++pC)
		{
			//printf("%lf ", pC.value());
			maxWeight = max(maxWeight, pC.value());
		}
		//printf("\n");
		double weightSum = 0;
		double t = m_t;
		for (ClusterMap::Iterator pC = nearClusterWeight.begin(); pC != nearClusterWeight.end(); ++pC)
		{
			double w = exp((pC.value() - maxWeight)/ t);
			pC.value() = w;
			weightSum += w;
		}
		for (ClusterMap::Iterator pC = nearClusterWeight.begin(); pC != nearClusterWeight.end(); ++pC)
		{
			pC.value() /= weightSum;
			//printf("%lf ", pC.value());
		}
		//printf("\n");

		// choose new cluster
		double prob = randFloat();
		double accProb = 0;
		int   newCluster = -1;
		for (ClusterMap::Iterator pC = nearClusterWeight.begin(); pC != nearClusterWeight.end(); ++pC)
		{
			accProb += pC.value();
			if (accProb > prob)
			{
				newCluster = pC.key();
			}
		}
		// prevent round-off error
		if (newCluster == -1)
		{
			newCluster = (nearClusterWeight.end() - 1).key();
		}

		//printf("old: %d new: %d\n\n", curVtx.m_clusterID, newCluster);
		// update graph
		curVtx.m_clusterID = newCluster;
		++ithIter;
	}
}

void CodeAtlas::RandomWalkClusterer::collectResult()
{
	typedef QHash<int, int> ClusterMap;
	ClusterMap cMap;
	for (int i = 0; i < m_vtx.size(); ++i)
	{
		int& cID = m_vtx[i].m_clusterID;
		if (!cMap.contains(cID))
		{
			int nc = cMap.size();
			cMap[cID] = nc;
		}
		cID = cMap[cID];
	}
	m_nCluster = cMap.size();
}

bool CodeAtlas::RandomWalkClusterer::compute()
{
	init();
	randomWalk();
	collectResult();

	char buf[50];
	sprintf(buf, "codeView-t=%f.gexf", m_t);
	writeToGephi(buf);
	return true;
}

int CodeAtlas::RandomWalkClusterer::getClusterID( int ithVtx )
{
	if (ARRAY_OUT_OF_RANGE(ithVtx, m_vtx.size()))
	{
		return -1;
	}
	return m_vtx[ithVtx].m_clusterID;
}

void CodeAtlas::RandomWalkClusterer::writeToGephi( char* fileName )
{
	ofstream file(fileName);
	file << 
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<gexf xmlns=\"http://www.gexf.net/1.2draft\" version=\"1.2\" xmlns:viz=\"http://www.gexf.net/1.2draft/viz\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.gexf.net/1.2draft http://www.gexf.net/1.2draft/gexf.xsd\">\n"
		"<meta lastmodifieddate=\"2015-07-06\">\n"
		"<creator>Gephi 0.8.1</creator>\n"
		"<description></description>\n"
		"</meta>\n"
		"<graph defaultedgetype=\"directed\" mode=\"static\">\n";

	// write nodes
	file << "    <nodes>\n";
	for (int ithNode = 0; ithNode < m_vtx.size(); ++ithNode)
	{
		char buf[200];
		QColor c = QColor::fromHsv((m_vtx[ithNode].m_clusterID * 43)%359, 255, 255);
		sprintf(buf,       
			"<node id=\"%d\" label=\"%d\">\n"
			"<attvalues></attvalues>\n"
			"<viz:size value=\"1.0\"></viz:size>\n"
			"<viz:color r=\"%d\" g=\"%d\" b=\"%d\"></viz:color>\n"
			"</node>\n",
			ithNode, ithNode,
			c.red(), c.green(), c.blue());
		file << buf;
	}
	file << "    </nodes>\n";

	// write edges
	file << "    <edges>\n";
	int e = 0;
	for (int ithNode = 0; ithNode < m_vtx.size(); ++ithNode)
	{
		char buf[200];
		Vertex& v = m_vtx[ithNode];
		typedef QHash<int, float> EdgeMap;
		/*
		for (EdgeMap::Iterator pE = v.m_inEdge.begin(); pE != v.m_inEdge.end(); ++pE, ++e)
		{
			sprintf(buf, 
				"<edge source=\"%d\" target=\"%d\" label=\"%d\">\n"
				"<attvalues></attvalues>\n"
				"</edge>\n", pE.key(), ithNode, e);
			file << buf;
		}*/
		for (EdgeMap::Iterator pE = v.m_outEdge.begin(); pE != v.m_outEdge.end(); ++pE, ++e)
		{
			sprintf(buf, 
				"<edge source=\"%d\" target=\"%d\" label=\"%d\">\n"
				"<attvalues></attvalues>\n"
				"</edge>\n", ithNode, pE.key(), e);
			file << buf;
		}
	}
	file << "    </edges>\n";

	file << "  </graph>\n"
		"</gexf>";

	file.close();
}

