#include "stdafx.h"
#include "UIElementLocator.h"

using namespace CodeAtlas;


bool UIElementLocator::layoutTree( SymbolNode::Ptr& root )
{
	ComponentLayouter compLayouter;
	ClassLayouter	  classLayouter;
	TrivalLayouter	  trivalLayouter;


	// node subtree iterator, do not go into block node
	SymbolNode::SmartDepthIterator itNode(	root, 
		SmartDepthIterator::POSTORDER, 
		SymbolInfo::All, 
		SymbolInfo::All & ~(SymbolInfo::Block));

	// for each node, do the following two things:
	// 1. compute its children's location, set UIElementAttr's position attribute
	// 2. compute this node's size, add UIElement and set the size attribute
	for (SymbolNode::Ptr node; node = *itNode; ++itNode)
	{
		SymbolInfo info = node->getSymInfo();
		// reach leaf
		// a variable or function node will be leaf
		bool isLeafReached = false;
		if (itNode.isCurNodeLeaf() || info.elementType() & SymbolInfo::FunctionSignalSlot)
			isLeafReached = true;
		if (info.elementType() & SymbolInfo::Variable)
		{
			SymbolNode::Ptr parent = node->getParent().toStrongRef();
			if (!parent.isNull()) 
			{
				SymbolInfo parentInfo = parent->getSymInfo();
				if (parentInfo.elementType() & SymbolInfo::FunctionSignalSlot)
					continue;
			}
		}
		if (isLeafReached)
		{
			float r;
			LayoutSetting::getNodeRadius(node,r);
			getOrAddUIAttr(node,r);
			continue;
		}

		QList<SymbolNode::Ptr> childList;
		childList.reserve(node->childCount());
		unsigned typeFlag = 0;
		for (SymbolNode::ChildIterator pChild = node->childBegin();
			pChild != node->childEnd(); ++pChild)
		{
			childList.append(*pChild);
			if ((*pChild)->getSymInfo().elementType() & SymbolInfo::Folder)
				typeFlag |= 0x2;
			else
				typeFlag |= 0x1;
		}

		Layouter* layouter = NULL;
		SymbolInfo nodeInfo = node->getSymInfo();
		if (nodeInfo.isClassOrStruct())
			layouter = &classLayouter;
		else if (nodeInfo.isEnum() || nodeInfo.elementType() == SymbolInfo::Enumerator)
			layouter = &trivalLayouter;
		else
		{
			layouter = &compLayouter;
			if (typeFlag == 0x2)			// only folder
				compLayouter.setSparseFactor(1.0,1.f);
			else if (typeFlag == 0x3)		// folder and other items
				compLayouter.setSparseFactor(1.8,1.f);
			else							// only other items
				compLayouter.setSparseFactor(2.4,1.f);
		}

		layouter->setNodes(childList, node);
		layouter->compute();
		layouter->writeResults();
	}
	return true;
}


bool UIElementLocator::buildProjVEMat( const SymbolNode::Ptr& rootNode, Eigen::SparseMatrix<double>& vtxEdgeMat )
{
	// build node - index map
	QHash<SymbolNode::Ptr, int> nodeIdxMap;
	int ithChild = 0;
	for (SymbolNode::ConstChildIterator pChild = rootNode->childConstBegin();
		pChild != rootNode->childConstEnd(); ++pChild, ++ithChild)
		nodeIdxMap[pChild.value()] = ithChild;

	// collect edge set
	QSet<SymbolEdge::Ptr> edgeSet;
	for (SymbolNode::ConstChildIterator pChild = rootNode->childConstBegin();
		pChild != rootNode->childConstEnd(); ++pChild)
	{
		const SymbolNode::Ptr& proj = pChild.value();
		QList<SymbolEdge::Ptr> edgeList;
		proj->inEdges(SymbolEdge::EDGE_PROJ_BUILD_DEPEND, edgeList);
		for (int i = 0; i < edgeList.size(); ++i)
			edgeSet.insert(edgeList[i]);

		proj->outEdges(SymbolEdge::EDGE_PROJ_BUILD_DEPEND, edgeList);
		for (int i = 0; i < edgeList.size(); ++i)
			edgeSet.insert(edgeList[i]);
	}

	// build triplet array
	std::vector<Triplet> tripletArray;
	int ithEdge = 0;
	for (QSet<SymbolEdge::Ptr>::ConstIterator pEdge = edgeSet.constBegin(); 
		pEdge != edgeSet.constEnd(); ++pEdge)
	{
		const SymbolEdge::Ptr& edge = *pEdge;

		const SymbolNode::Ptr srcNode = edge->srcNode().toStrongRef();
		const SymbolNode::Ptr tarNode = edge->tarNode().toStrongRef();

		if (srcNode.isNull() || tarNode.isNull())
			continue;

		QHash<SymbolNode::Ptr, int>::ConstIterator pSrcIt = nodeIdxMap.find(srcNode);
		QHash<SymbolNode::Ptr, int>::ConstIterator pTarIt = nodeIdxMap.find(tarNode);

		if (pSrcIt == nodeIdxMap.constEnd() || pTarIt == nodeIdxMap.constEnd())
			continue;

		int srcIdx = pSrcIt.value();
		int tarIdx = pTarIt.value();

		tripletArray.push_back(Triplet(srcIdx, ithEdge ,1.0));
		tripletArray.push_back(Triplet(tarIdx, ithEdge ,-1.0));
		ithEdge++;
	}

	if (int nEdges = tripletArray.size()/2)
	{
		vtxEdgeMat.resize(nodeIdxMap.size(),nEdges);
		vtxEdgeMat.setFromTriplets(tripletArray.begin(), tripletArray.end());
		return true;
	}
	return false;
}



bool UIElementLocator::layoutProjects( const SymbolNode::Ptr& root, float& radius )
{
	QTime begTime = QTime::currentTime();
	QList<SymbolNode::Ptr> projectList;
	root->getChildList(projectList);

	ComponentLayouter layouter;
	layouter.setNodes(projectList, root);
	layouter.compute();
	layouter.writeResults();
	radius = layouter.getTotalRadius();

	QTime endTime = QTime::currentTime();
	int msecs = begTime.msecsTo(endTime);
	qDebug("MDS layout: %d msecs", msecs);
	return true;
}


void UIElementLocator::updateUIItemTree( SymbolNode::Ptr& globalSym,  SymbolNode::Ptr& proj)
{
	SymbolNode::SmartDepthIterator it(	globalSym, SmartDepthIterator::PREORDER, 
		SymbolInfo::All, 
		SymbolInfo::All & ~(SymbolInfo::Block));

	for (SymbolNode::Ptr node; node = *it; ++it)
	{
		SymbolInfo info = node->getSymInfo();
		if (info.elementType() == SymbolInfo::ClassStruct)
		{
			// compute convex hull
			int nChild = node->childCount();
			if (nChild > 0)
			{
				int ithChild = 0, ithValidChild = 0;
				for (SymbolNode::ConstChildIterator pChild = node->childConstBegin();
					pChild != node->childConstEnd(); ++pChild, ++ithChild)
				{
					UIElementAttr::Ptr childAttr = (*pChild)->getAttr<UIElementAttr>();
					if (!childAttr)
						continue;

					QPointF& p = childAttr->position();
					ithValidChild++;
				}
			}
		}

		// for global symbols, consider project node as parent
		SymbolNode::Ptr parent = node->getParent().toStrongRef();
		if (node == globalSym)
			parent = proj;

		if (UIElementAttr::Ptr uiAttr = node->getAttr<UIElementAttr>())
		{
			NodeUIItem* parentItem = NULL;
			if (parent)
			{
				if (UIElementAttr::Ptr parentAttr = parent->getAttr<UIElementAttr>())
				{
					parentItem = parentAttr->getUIItem().data();
				}
			}
			uiAttr->buildOrUpdateUIItem(node, parentItem);

			// add fuzzy edge depend
			if (parent)// && parent->getSymInfo().elementType() == SymbolInfo::Class)
			{
				SymbolEdge::Ptr edge;
				for (SymEdgeIter iter(node, SymbolNode::EDGE_OUT, SymbolEdge::EDGE_FUZZY_DEPEND); edge = *iter; ++iter)
				{
					EdgeUIElementAttr::Ptr edgeUIAttr = edge->getAttr<EdgeUIElementAttr>();
					if (edgeUIAttr)
					{
						edgeUIAttr->buildOrUpdateUIItem(edge.toWeakRef(), parentItem);
					}
				}
			}
			else
			{
				qDebug("no parent");
			}
		}

	}
}

void UIElementLocator::updateProjUIItem(QSet<SymbolNode::Ptr>& dirtyProj)
{
	// layout global symbols of each dirty project

	for (QSet<SymbolNode::Ptr>::ConstIterator pProj = dirtyProj.constBegin();
		pProj != dirtyProj.constEnd(); ++pProj)
	{
		// build proj uiAttr
		SymbolNode::Ptr proj = *pProj;
		UIElementAttr::Ptr projUIAttr = proj->getAttr<UIElementAttr>();
		if (!projUIAttr)
			continue;

		projUIAttr->buildOrUpdateUIItem(proj, 0);
		// compute global symbols' layout
		// for a interior proj, find attribute
		QList<SymbolNode::Ptr> globalSymList;
		SymbolTree::getGlobalSymbols(proj, globalSymList);
		for (int ithGloSym = 0; ithGloSym < globalSymList.size(); ++ithGloSym)
		{
			SymbolNode::Ptr& gloSym = globalSymList[ithGloSym];
			UIElementAttr::Ptr pAttr = gloSym->getAttr<UIElementAttr>();
			if (!pAttr)
				continue;
			updateUIItemTree(gloSym, proj);
		}
	}
}


void UIElementLocator::modifyTree()
{
	// layout global symbols of each dirty project
	SymbolNode::Ptr root = m_tree->getRoot();

// 	const QSet<SymbolNode::Ptr>& dirtyProj = m_tree->getDirtyProject();
	/*
	QSet<SymbolNode::Ptr> projList;
	for(SymbolNode::ChildIterator pChild = root->childBegin();
		pChild != root->childEnd(); pChild++)
	{
		projList.insert(*pChild);
	}
/*
 	for (QSet<SymbolNode::Ptr>::ConstIterator pProj = projList.constBegin();
 		pProj != projList.constEnd(); ++pProj)	
	{
		// build proj uiAttr
		SymbolNode::Ptr proj = *pProj;
		UIElementAttr::Ptr projUIAttr = proj->getOrAddAttr<UIElementAttr>();
		SymbolWordAttr::Ptr  wordAttr = proj->getOrAddAttr<SymbolWordAttr>();

		{
			// compute global symbols' layout
			// for a interior proj, find attribute
			QList<SymbolNode::Ptr> globalSymList;
			
			for (SymbolNode::ChildIterator pChild = proj->childBegin(); pChild != proj->childEnd(); ++pChild)
			{
				globalSymList.push_back(*pChild);
			}
			//SymbolTree::getGlobalSymbols(proj, globalSymList);

			// compute layout of global symbol's children
			for (int ithSym = 0; ithSym < globalSymList.size(); ++ithSym)
				layoutTree(globalSymList[ithSym]);

			// fill data used to compute layout
			ComponentLayouter cmpLayouter;
			cmpLayouter.setNodes(globalSymList, proj);				
			if (cmpLayouter.compute())
			{
				cmpLayouter.writeResults();
				projUIAttr->radius() = cmpLayouter.getTotalRadius();
			}
		}
	}*/

	layoutTree(root);

	UIElementAttr::Ptr rootUI = root->getOrAddAttr<UIElementAttr>();
	float totalRadius = rootUI->radius();
	//layoutProjects(root, totalRadius);

	qDebug("total radius: %f", totalRadius);  
	float  sceneR = totalRadius * 3.f;
	QRectF sceneRect = QRectF(-sceneR, -sceneR, sceneR*2.f, sceneR*2.f);
	m_tree->setTreeRect(sceneRect);
	UIManager::getScene().setSceneRect(sceneRect);

	// update uiItem
	//updateProjUIItem(projList);
	//updateUIItemTree(root, SymbolNode::Ptr());
	
	for (SymbolNode::ChildIterator pProj = root->childBegin();
		pProj != root->childConstEnd(); ++pProj)
	{
		SymbolNode::Ptr& proj = pProj.value();
		updateUIItemTree(proj, root);
	}

	// compute scene rect
	QRectF r(0,0,0,0);
	SmartDepthIterator it(m_tree->getRoot(), DepthIterator::PREORDER, SymbolInfo::All);
	for (SymbolNode::Ptr node; node = *it; ++it)
	{
		UIElementAttr::Ptr uiAttr = node->getAttr<UIElementAttr>();
		if (!uiAttr)
		{
			continue;
		}

		NodeUIItem::Ptr nodeUIItem = uiAttr->getUIItem();
		if (!nodeUIItem)
		{
			continue;
		}
		r = r.united(nodeUIItem->mapToScene(nodeUIItem->boundingRect()).boundingRect());

		SymbolInfo info = node->getSymInfo();
		if (info.elementType() & SymbolInfo::Folder || info.isTopLevel())
		{
			it.skipSubTree();
		}
	}
	UIManager::instance()->getScene().setSceneRect(r);
/*
	if (UIElementAttr::Ptr uiAttr = root->getAttr<UIElementAttr>())
	{
		uiAttr->buildOrUpdateUIItem(root, NULL);
	}*/

	//m_tree->print(SymbolInfo::All, SymbolNodeAttr::ATTR_UIELEMENT );
}

void UIElementLocator::getOrAddUIAttr( SymbolNode::Ptr& node, float radius /*= 1.f*/ )
{
	UIElementAttr::Ptr uiAttr = node->getOrAddAttr<UIElementAttr>();
	uiAttr->radius() = radius;
}

void CodeAtlas::UIElementLocator::writeResultToNode( QList<SymbolNode::Ptr>& nodeList, MatrixXf& pos2D, VectorXf& radiusVec, VectorXi* levelVec /*= NULL*/ )
{
	int ithNode = 0;
	foreach(SymbolNode::Ptr node, nodeList)
	{
		if (UIElementAttr::Ptr uiAttr = node->getOrAddAttr<UIElementAttr>())
		{
			uiAttr->position() = QPointF(pos2D(ithNode, 0), pos2D(ithNode, 1));
			uiAttr->radius()   = radiusVec[ithNode];
			if (levelVec && levelVec->size() == nodeList.size())
				uiAttr->level() = (*levelVec)[ithNode];
			else
				uiAttr->level() = GraphUtility::s_defaultBaseLevel;
		}
		ithNode++;
	}
}
