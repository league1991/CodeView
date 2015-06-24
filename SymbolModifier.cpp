#include "stdafx.h"

using namespace CodeAtlas;

void CodeAtlas::WordCollector::modifyTree(  )
{
	collectWords();
	//m_tree->print(SymbolInfo::Project | SymbolInfo::Class | SymbolInfo::Function, SymbolNodeAttr::ATTR_SYMWORD);
}

void CodeAtlas::WordCollector::updateDirtyData()
{
 
}

void CodeAtlas::WordCollector::collectWords()
{
	SmartDepthIterator it(m_tree->getRoot(), SmartDepthIterator::POSTORDER, SymbolInfo::All);

	for (SymbolNode::Ptr pNode; pNode = *it; ++it)
	{
		const UnderstandHandle& hdl = pNode->getDBHandle();
		SymbolWordAttr::Ptr pAttr = pNode->getOrAddAttr<SymbolWordAttr>();
		pAttr->clear();
		pAttr->setWords(pNode);

		// merge child words
		for (SymbolNode::ChildIterator pChild = pNode->childBegin();
			pChild != pNode->childEnd(); ++pChild)
		{
			SymbolWordAttr::Ptr pChildAttr = pChild.value()->getAttr<SymbolWordAttr>();
			if (!pChildAttr)
				continue;
			pAttr->unite(*pChildAttr);
		}
		pAttr->computeStatistics();
	}
	/*
	const QSet<SymbolNode::Ptr>& dirtyProj = m_tree->getDirtyProject();
	for (QSet<SymbolNode::Ptr>::ConstIterator pProj = dirtyProj.constBegin();
		pProj != dirtyProj.constEnd(); ++pProj)
	{
		// find child nodes that is not a project
		const SymbolNode::Ptr proj = *pProj;
		QList<SymbolNode::Ptr> childList;
		proj->findChild(
			SymbolInfo::All        &
			~(SymbolInfo::Project) &
			~(SymbolInfo::Unknown),
			childList);

		// no such nodes, so all the children are project nodes, ignore them
		if (childList.size() == 0)
			continue;

		// add word attr to subtree of the dirty project
		SymbolNode::SmartDepthIterator it(proj, SmartDepthIterator::POSTORDER);
		for (SymbolNode::Ptr pNode; pNode = *it; ++it)
		{
			SymbolWordAttr::Ptr pAttr = pNode->getOrAddAttr<SymbolWordAttr>();

			// project nodes has been processed in SymbolTreeBuilder
			if (pNode->getSymInfo().elementType() == SymbolInfo::Project &&
				pAttr->nWords() != 0)
				continue;
			pAttr->clear();
			pAttr->setWords(pNode);

			// merge child words
			for (SymbolNode::ChildIterator pChild = pNode->childBegin();
				pChild != pNode->childEnd(); ++pChild)
			{
				SymbolWordAttr::Ptr pChildAttr = pChild.value()->getAttr<SymbolWordAttr>();
				if (!pChildAttr)
					continue;
				pAttr->unite(*pChildAttr);
			}
			pAttr->computeStatistics();
		}
	}*/
}

void CodeAtlas::WordCollector::buildDocTermMat( QList<SymbolNode::Ptr>& nodeList, SparseMatrix& docTermMat, Eigen::VectorXf& radiusVec )
{
	int nSymbols = nodeList.size();
	int nWords   = SymbolWordAttr::totalWords();

	docTermMat = SparseMatrix(nSymbols, nWords);
	radiusVec.resize(nSymbols);
	for (int ithNode = 0; ithNode < nodeList.size(); ++ithNode)
	{
		SymbolNode::Ptr& pNode = nodeList[ithNode];
		SymbolWordAttr::Ptr pAttr = pNode->getAttr<SymbolWordAttr>();
		if (!pAttr)
			continue;

		for (QMap<int, float>::ConstIterator pWord = pAttr->getWordWeightMap().constBegin();
			pWord != pAttr->getWordWeightMap().constEnd(); ++pWord)
		{
			int wordID = pWord.key();
			float wordCount = pWord.value();
			docTermMat.insert(ithNode, wordID) = wordCount;
		}
		float r = qSqrt(1 + pAttr->getTotalWordWeight() * .1f);
		radiusVec(ithNode) = r;
	}
	docTermMat.makeCompressed();
}

void CodeAtlas::FuzzyDependBuilder::modifyTree(  )
{
	const QSet<SymbolNode::Ptr>& dirtyProj = m_tree->getDirtyProject();
	for (QSet<SymbolNode::Ptr>::ConstIterator pProj = dirtyProj.constBegin();
		pProj != dirtyProj.constEnd(); ++pProj)
	{
		buildProjDepend(*pProj);
	}
// 	m_tree->print(SymbolInfo::All & ~SymbolInfo::Block,
// 		SymNodeAttr::ATTR_FUZZYDEPEND,
// 		SymbolEdge::EDGE_FUZZY_DEPEND);
}

QString CodeAtlas::FuzzyDependBuilder::collectFuzzyDepend( SymbolNode::Ptr& node )
{
	// ignore dependency under a function
	SymbolInfo nodeInfo = node->getSymInfo();
	if (nodeInfo.elementType() & SymbolInfo::Variable)
		return nodeInfo.type() + " " + nodeInfo.name();

	QString    nodeCode;
	CodeAttr::Ptr pCode = node->getAttr<CodeAttr>();
	if (pCode)
		nodeCode = pCode->getCode();
	if (nodeInfo.elementType() & SymbolInfo::Function)
		return nodeCode;

	QMultiHash<QString, ChildPack>      childList;
	int ithChild = 0;
	
	bool* isNonVar = new bool[node->childCount()];
	for (SymbolNode::ChildIterator pChild = node->childBegin();
		pChild != node->childEnd(); ++pChild, ++ithChild)
	{
		ChildPack p;
		p.m_info = pChild.key();
		p.m_code = collectFuzzyDepend(pChild.value());
		p.m_pNode = pChild.value();
		p.m_index = ithChild;
		childList.insertMulti(p.m_info.name(), p);

		isNonVar[ithChild] = (p.m_info.elementType() & SymbolInfo::Variable) == 0;
		nodeCode += "\n" + p.m_code;
	}

	FuzzyDependAttr::Ptr pAttr = node->getOrAddAttr<FuzzyDependAttr>();
	buildChildDepend(childList, pAttr->vtxEdgeMatrix(), pAttr->edgeWeightVector(), pAttr->dependPairList());

	buildSubGraphLevel(pAttr->vtxEdgeMatrix(), isNonVar, ithChild, pAttr->levelVector());
	delete[] isNonVar;
	return nodeCode;
}

void CodeAtlas::FuzzyDependBuilder::buildSubGraphLevel(const SparseMatrix& veMat, const bool* const validMask, int nNodes, VectorXi& level)
{
	SparseMatrix subMat; VectorXi subLevel;
	if(!GraphUtility::getSubGraph(veMat, validMask, subMat))
	{
		level.setConstant(nNodes, GraphUtility::s_defaultBaseLevel);
		return;
	}
	GraphUtility::computeVtxLevel(subMat, subLevel, GraphUtility::s_defaultBaseLevel);	
	level.resize(nNodes);

	for (int i = 0, ithNon = 0; i < nNodes; ++i)
	{
		if (!validMask[i])
			level[i] = GraphUtility::s_defaultBaseLevel;
		else
			level[i] = subLevel[ithNon++];
	}
}
void CodeAtlas::FuzzyDependBuilder::buildChildDepend( QMultiHash<QString, ChildPack>& childList , 
													 Eigen::SparseMatrix<double>& vtxEdgeMat,
													 Eigen::VectorXd&             edgeWeightVec,
													 QList<FuzzyDependAttr::DependPair>& dependPair)
{
	QStringList codeList;
	QVector<ChildPack*> childPackPtr;
	for (QMultiHash<QString, ChildPack>::Iterator pChild = childList.begin();
		pChild != childList.end(); ++pChild)
	{
		codeList.push_back(pChild.value().m_code);
		childPackPtr.push_back(&pChild.value());
	}

	std::vector<Triplet> tripletArray;
	QVector<double>		 edgeWeightArray;

	// add dependency edges between child nodes
	int ithSrc = 0;
	for (QMultiHash<QString, ChildPack>::Iterator pChild = childList.begin();
		pChild != childList.end(); ++pChild, ++ithSrc)
	{
		// for each child, find number of occurrences of this child's name
		ChildPack& srcChild = pChild.value();
		const QString& srcName = pChild.key();
		QVector<int> occurence;
		WordExtractor::findOccurrence(srcName, codeList, occurence);

		for (int ithTar = 0; ithTar < childPackPtr.size(); ++ithTar)
		{
			int nOccur = occurence[ithTar];
			if (nOccur == 0 || ithTar == ithSrc)
				continue;

			ChildPack& tarChild = *childPackPtr[ithTar];

			SymbolEdge::Ptr pEdge = SymbolTree::getOrAddEdge(srcChild.m_pNode, tarChild.m_pNode, SymbolEdge::EDGE_FUZZY_DEPEND);
			pEdge->clear();
			pEdge->strength() = nOccur;

			int nEdge = tripletArray.size()/2;
			tripletArray.push_back(Triplet(srcChild.m_index, nEdge ,1.0));
			tripletArray.push_back(Triplet(tarChild.m_index, nEdge ,-1.0));

			edgeWeightArray.push_back(nOccur);
			dependPair.push_back(FuzzyDependAttr::DependPair(srcChild.m_pNode, tarChild.m_pNode, nOccur));
		}
	}

	if (int nEdges = tripletArray.size()/2)
	{
		vtxEdgeMat.resize(childList.size(),nEdges);
		vtxEdgeMat.setFromTriplets(tripletArray.begin(), tripletArray.end());
		edgeWeightVec.resize(nEdges);
		memcpy(edgeWeightVec.data(), edgeWeightArray.data(), sizeof(double)* nEdges);
	}
}

void CodeAtlas::FuzzyDependBuilder::buildProjDepend( const SymbolNode::Ptr& proj )
{
	QList<SymbolNode::Ptr> globalSymList;
	SymbolTree::getGlobalSymbols(proj, globalSymList);
	ProjectAttr::Ptr projAttr = proj->getOrAddAttr<ProjectAttr>();
	projAttr->setGlobalSymList(globalSymList);


	QMultiHash<QString, ChildPack>      childList;
	bool* isNonVar = new bool[globalSymList.size()];
	for (int ithGloSym = 0; ithGloSym < globalSymList.size(); ++ithGloSym)
	{
		ChildPack p;
		SymbolNode::Ptr pSym = globalSymList[ithGloSym];

		pSym->getOrAddAttr<GlobalSymAttr>();
		p.m_info = pSym->getSymInfo();
		p.m_code = collectFuzzyDepend(pSym);
		p.m_pNode = pSym;
		p.m_index = ithGloSym;
		childList.insertMulti(p.m_info.name(), p);

		isNonVar[ithGloSym] = (p.m_info.elementType() & SymbolInfo::Variable) == 0;
	}

	FuzzyDependAttr::Ptr pAttr = proj->getOrAddAttr<FuzzyDependAttr>();
	buildChildDepend(childList,pAttr->vtxEdgeMatrix(), pAttr->edgeWeightVector(), pAttr->dependPairList());

	buildSubGraphLevel(pAttr->vtxEdgeMatrix(), isNonVar, globalSymList.size(), pAttr->levelVector());
	delete[] isNonVar;
}

void CodeAtlas::SymbolModifier::setBuilder( SymbolTreeBuilder* builder )
{
	m_builder = builder;
}

void CodeAtlas::GeneralDependBuilder::buildDependPath()
{
	m_dependPath.clear();
	//QList<DBDependData>& dpData = m_builder->getDependData();
	QList<DBDependData>		   dpData;
	buildDependData(dpData);
	for (int i = 0; i < dpData.size(); ++i)
	{
		DBDependData& dp = dpData[i];
		SymbolPath srcPath, tarPath;
		if(!m_builder->getSymbolPath(dp.getSrc(), srcPath) ||
			!m_builder->getSymbolPath(dp.getTar(), tarPath))
			continue;

		m_dependPath.push_back(DependPath(srcPath, tarPath));
	}
}

bool CodeAtlas::GeneralDependBuilder::buildDependData(QList<DBDependData>& dependData)
{
	dependData.clear();
	SmartDepthIterator it(m_tree->getRoot(), SmartDepthIterator::PREORDER, SymbolInfo::All);
	for (SymbolNode::Ptr node; node = *it; ++it)
	{
		QList<UnderstandHandle> dependList;
		const UnderstandHandle& hdl = node->getDBHandle();
		m_builder->getDatabase()->findDepends(hdl, dependList);
		for (int i = 0; i < dependList.size(); ++i)
		{
			dependData.push_back(DBDependData(hdl, dependList[i]));
		}
	}
	return true;
}


void CodeAtlas::GeneralDependBuilder::modifyTree()
{
	buildDependPath();

	SymbolPath path;
	QList<int> dependList;
	for (int i = 0; i < m_dependPath.size(); ++i)
	{
		dependList.append(i);
	}
	buildNodeDepend(m_tree->getRoot(), path, dependList);
}

void CodeAtlas::GeneralDependBuilder::clearCache()
{
	m_dependPath.clear();
}

void CodeAtlas::GeneralDependBuilder::buildNodeDepend( SymbolNode::Ptr node, SymbolPath& nodePath, QList<int>& dependIDList )
{
	int depth = nodePath.getSymbolCount();

	QHash<SymbolInfo, int> childIDMap;
	QList<SymbolNode::Ptr> childList;
	bool* isNonVar = new bool[node->childCount()+1];
	for (SymbolNode::ChildIterator pChild = node->childBegin();
		pChild != node->childEnd(); ++pChild)
	{
		int ithChild = childIDMap.size();
		childIDMap[pChild.key()] = ithChild;
		childList.push_back(pChild.value());
		isNonVar[ithChild] =  (pChild.key().elementType() & SymbolInfo::Variable) == 0;
	}

	// collect depends
	QHash<Pair, int> edgeMap;
	for (int i = 0; i < dependIDList.size(); ++i)
	{
		int id = dependIDList[i];
		DependPath& dependPath = m_dependPath[id];
		SymbolPath& srcPath = dependPath.m_src;
		SymbolPath& tarPath = dependPath.m_tar;

		const SymbolInfo* srcInfo = srcPath.getSymbol(depth);
		const SymbolInfo* tarInfo = tarPath.getSymbol(depth);

		if (!srcInfo || !tarInfo)
			continue;

		QHash<SymbolInfo, int>::iterator pSrc = childIDMap.find(*srcInfo);
		QHash<SymbolInfo, int>::iterator pTar = childIDMap.find(*tarInfo);
		if (pSrc == childIDMap.end() || pTar == childIDMap.end() || pSrc.value() == pTar.value())
			continue;

		Pair idPair(pSrc.value(), pTar.value());
		QHash<Pair, int>::iterator pEdge = edgeMap.find(idPair);
		if (pEdge == edgeMap.end())
			edgeMap[idPair] = 1;
		else
			pEdge.value()++;
	}

	std::vector<Triplet<double>> tripletArray;
	vector<double>		 edgeWeightArray;
	QList<FuzzyDependAttr::DependPair> dependPairList;
	for (QHash<Pair, int>::iterator pPair = edgeMap.begin();
		pPair != edgeMap.end(); ++pPair)
	{
		int src = pPair.key().m_src;
		int tar = pPair.key().m_tar;

		int val = pPair.value();
		int nEdge= tripletArray.size()/2;

		tripletArray.push_back(Eigen::Triplet<double>(src, nEdge ,1.0));
		tripletArray.push_back(Eigen::Triplet<double>(tar, nEdge ,-1.0));

		edgeWeightArray.push_back(val);

		SymbolNode::Ptr srcNode = childList[src];
		SymbolNode::Ptr tarNode = childList[tar];
		SymbolEdge::Ptr pEdge = SymbolTree::getOrAddEdge(srcNode, tarNode, SymbolEdge::EDGE_FUZZY_DEPEND);
		pEdge->clear();
		pEdge->strength() = val;

		dependPairList.push_back(FuzzyDependAttr::DependPair(srcNode, tarNode, val));
	}

	// add attr
	FuzzyDependAttr::Ptr pAttr = node->getOrAddAttr<FuzzyDependAttr>();
	Eigen::SparseMatrix<double>& vtxEdgeMat = pAttr->vtxEdgeMatrix();
	Eigen::VectorXd&             edgeWeightVec = pAttr->edgeWeightVector();

	int nEdges = tripletArray.size()/2;
	if (nEdges)
	{
		vtxEdgeMat.resize(childIDMap.size(), nEdges);
		vtxEdgeMat.setFromTriplets(tripletArray.begin(), tripletArray.end());
		edgeWeightVec.resize(nEdges);
		memcpy(edgeWeightVec.data(), &edgeWeightArray[0], sizeof(double)* nEdges);
		GraphUtility::computeSubGraphVtxLevel(vtxEdgeMat, isNonVar, node->childCount(), pAttr->levelVector());

		pAttr->dependPairList() = dependPairList;
	}
	delete[] isNonVar;

	// get child
	for (SymbolNode::ChildIterator pChild = node->childBegin();
		pChild != node->childEnd(); ++pChild)
	{
		const SymbolInfo& childInfo = pChild.key();
		nodePath.addChildSymbol(childInfo);

		QList<int> childDependIDList;
		for (int ithDepend = 0; ithDepend < dependIDList.size(); ++ithDepend)
		{
			int id = dependIDList[ithDepend];
			DependPath& dependPath = m_dependPath[id];
			const SymbolInfo* srcInfo = dependPath.m_src.getSymbol(depth);
			const SymbolInfo* tarInfo = dependPath.m_tar.getSymbol(depth);
			if (!srcInfo || !tarInfo)
			{
				continue;
			}

			if (*srcInfo != childInfo || *tarInfo != childInfo)
			{
				continue;
			}
			childDependIDList.push_back(id);
		}
		buildNodeDepend(pChild.value(), nodePath, childDependIDList);
		nodePath.removeChildSymbol();
	}
}
