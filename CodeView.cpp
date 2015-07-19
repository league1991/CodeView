#include "StdAfx.h"
#include "CodeView.h"

using namespace CodeAtlas; 

QImage CodeAtlas::CodeView::m_cursorImg;

CodeView::CodeView( QWidget *parent /*= 0*/ ):
QGraphicsView(parent), m_receiveMsg(true), m_mutex(QMutex::Recursive),
m_alwaysSeeCursor(true), m_drawName(true), m_drawWordCloud(true), m_showModuleCluster(true)
{ 
	setScene(&UIManager::instance()->getScene());
	//setCacheMode(CacheBackground);
	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	setResizeAnchor(QGraphicsView::AnchorUnderMouse);
	setMouseTracking(true);
	scale(1, 1);
	//setMinimumSize(400, 400);
	setWindowTitle(tr("Code View")); 
	setCacheMode(CacheNone); 
	setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}
void CodeView::wheelEvent(QWheelEvent *event)
{
	const QPointF p0scene = mapToScene(event->pos());

	qreal factor = std::pow(1.0008, event->delta());
	scale(factor, factor);

	const QPointF p1mouse = mapFromScene(p0scene);
	const QPointF move = p1mouse - event->pos(); // The move
	horizontalScrollBar()->setValue(move.x() + horizontalScrollBar()->value());
	verticalScrollBar()->setValue(move.y() + verticalScrollBar()->value());

	//computeLod();
}


CodeAtlas::CodeView::~CodeView( void )
{

}

bool CodeAtlas::CodeView::event( QEvent * event )
{
	bool res = true;
	m_mutex.lock();
	if (m_receiveMsg)
	{
		//qDebug("view event");
		res = QGraphicsView::event(event);
		//update();
	}
	m_mutex.unlock();
	return res;
}

void CodeAtlas::CodeView::lockView()
{
	m_mutex.lock();
	m_receiveMsg = false;
	m_mutex.unlock();
}

void CodeAtlas::CodeView::unlockView()
{
	m_mutex.lock();
	m_receiveMsg = true;
	m_mutex.unlock();
}

void CodeAtlas::CodeView::drawBackground( QPainter * painter, const QRectF & rect )
{
	int t0 = clock();

	QSize   viewSize   = m_curSize;
	QPointF topLeftPnt = this->mapToScene(0,0);
	QPointF bottomRightPnt = this->mapToScene(viewSize.width(), viewSize.height());
	QRectF  sceneRect(topLeftPnt, bottomRightPnt);
	QGraphicsScene* scene = this->scene();
	if (!scene)
		return;

	BackgroundRenderer& renderer = UISetting::getBackgroundRenderer();
	renderer.clearSeaRegion();
	QList<QGraphicsItem*> itemList = scene->items(sceneRect, Qt::IntersectsItemBoundingRect);
	QList<QRect> rectList;
	QList<float> radiusList;
	QVector<float> valList;
	QVector<QRgb>  clrList;
	SymbolTree& tree = DBManager::instance()->getSymbolTree();
	SmartDepthIterator it(tree.getRoot(), SmartDepthIterator::PREORDER, SymbolInfo::All, SymbolInfo::All & ~(SymbolInfo::PrivateMember|SymbolInfo::ProtectedMember));
	/*
	for (SymbolNode::Ptr proj; proj = *it; ++it)
	{
		ProjectAttr::Ptr projAttr = proj->getAttr<ProjectAttr>();
		if (!projAttr)
			continue;
		
		const QList<SymbolNode::Ptr>& globalSymbols = projAttr->getGlobalSymList();
		foreach (SymbolNode::Ptr gloSym, globalSymbols)*/
	for (SymbolNode::Ptr classNode; classNode = *it; ++it)
	{
		SymbolInfo nodeInfo = classNode->getSymInfo();

		UIElementAttr::Ptr uiAttr = classNode->getAttr<UIElementAttr>();
		if (uiAttr.isNull())
			continue;

		NodeUIItem::Ptr uiItem = uiAttr->getUIItem();
		if (uiItem.isNull())
			continue;

		if (nodeInfo.elementType() & SymbolInfo::Folder)
		{
			FolderAttr::Ptr fAttr = classNode->getAttr<FolderAttr>();
			QSharedPointer<QPolygonF> pHull = uiAttr->getVisualHull();
			if (!pHull)
				continue;
			QPolygon poly = this->mapFromScene(uiItem->mapToScene(*pHull));
			renderer.addSeaRegion(poly, fAttr->depth());
			continue;
		}
		else if (!nodeInfo.isTopLevel())
		{
			it.skipSubTree();
			continue;
		}
		else
		{
			QRectF bRect = uiItem->boundingRect();
			QSizeF  bSize = bRect.size();
			QPoint topLeft = mapFromScene(uiItem->mapToScene(bRect.topLeft()));
			QPoint bottomRight = mapFromScene(uiItem->mapToScene(bRect.bottomRight()));
			QRect renderRect(topLeft, bottomRight);
			rectList.push_back(renderRect); 

			float ratio = renderRect.size().width() / bRect.size().width();
			radiusList.push_back(uiItem->getEntityRadius() * ratio);
			valList.push_back(1);//uiItem->getLevel());
			clrList.push_back(UISetting::getTerritoryColor(qHash(uiItem->name())).rgb());
		}

	}
	//}

	
	float lod = QStyleOptionGraphicsItem::levelOfDetailFromTransform(painter->worldTransform());
	renderer.setLod(lod);
	renderer.setLodInterLimit(0.02,0.15);
	renderer.setEmptyTerritory(qRgba(233,229,220,255));
	renderer.setRenderSize(m_curSize.width(), m_curSize.height(), 0.25);
	if (lod > 0.03  && 0)
	{
		renderer.setRenderMode(BackgroundRenderer::RENDER_PLAIN_CLR);
		renderer.setIsShaded(true);
	}
	else
	{
		renderer.setRenderMode(BackgroundRenderer::RENDER_TERRAIN);
		renderer.setIsShaded(false);
	}
	renderer.setPixelSizeFactor(10);
	renderer.render(rectList, radiusList, valList, clrList);
	painter->drawImage(sceneRect, renderer.getFinalImage());

	printf("view: %p\n", this);
	computeLod();

	int t1 = clock();
	printf("CodeView::drawBackground: %f\n", (t1-t0)/float(CLOCKS_PER_SEC));
}

void CodeAtlas::CodeView::computeLod()
{
	// set node's lod
	QRectF viewRect = this->mapToScene(this->rect()).boundingRect();
	m_lodManager.setViewRect(viewRect);
	m_lodManager.setLod(this->transform());
	m_lodManager.setRoot(DBManager::instance()->getSymbolTree().getRoot());
	m_lodManager.compute();
}


void CodeAtlas::CodeView::drawForeground( QPainter *painter, const QRectF &rect )
{	
	int t0 = clock();
	m_overlapSolver.resetViewBuf();
	if (m_drawName)
	{
		drawName(painter);
	}

	if (m_drawWordCloud)
	{
		drawWordCloud(painter);
	}

	// draw cursor
	drawCursorIcon(painter);

	int t1 = clock();
	printf("CodeView::drawForeground %f\n", (t1-t0)/float(CLOCKS_PER_SEC));
}

void CodeAtlas::CodeView::drawCursorIcon(QPainter* painter)
{
	SymbolNode::Ptr cursorNode =UIManager::instance()->getScene().getCurCursorNode();
	if (!cursorNode)
		return;

	UIElementAttr::Ptr uiAttr = cursorNode->getAttr<UIElementAttr>();
	if (!uiAttr)
		return;

	NodeUIItem::Ptr uiItem = uiAttr->getUIItem();
	if (!uiItem)
		return;
	QPointF pos = uiItem->mapToScene(QPointF());
	QPoint  posi = mapFromScene(pos);
	QRect   cursorRect(QPoint(), QSize(30,30));
	cursorRect.moveCenter(posi);
	QRect viewRect(QPoint(), m_curSize);

	QRect   outlineRect = UIUtility::scaleRect(cursorRect, 4);
	QPen cursorPen(QColor(47,91,140,200));
	cursorPen.setWidth(2);
	painter->setPen(cursorPen);
	QBrush cursorBrush(QColor(80,135,197,50));
	painter->setBrush(cursorBrush);
	painter->drawEllipse(outlineRect);

	//qDebug() << "draw icon";
	if (m_cursorImg.isNull())
		m_cursorImg.load("Resources/cursor.png");

	painter->drawImage(cursorRect, m_cursorImg);
	//painter->drawEllipse(cursorRect);
}
void CodeAtlas::CodeView::resizeEvent( QResizeEvent * event )
{
	QSize s = event->size();
	m_curSize = s;

	m_overlapSolver.setViewRect(this->rect());
	//computeLod();
}

void CodeAtlas::CodeView::paintEvent( QPaintEvent *event )
{
	bool res = true;
	m_mutex.lock();
	if (m_receiveMsg)
	{
		qDebug("\n\nview paint event begin\n");
		int t0 = clock();
		QGraphicsView::paintEvent(event);
		int t1 = clock();
		printf("view paint event end %f\n", (t1-t0)/float(CLOCKS_PER_SEC));
	}
	m_mutex.unlock();
}

void CodeAtlas::CodeView::centerViewWhenNecessary()
{
	if (!m_alwaysSeeCursor)
		return;
	SymbolNode::Ptr cursorNode =UIManager::instance()->getScene().getCurCursorNode();
	if (!cursorNode)
		return;

	UIElementAttr::Ptr uiAttr = cursorNode->getAttr<UIElementAttr>();
	if (!uiAttr)
		return;

	NodeUIItem::Ptr uiItem = uiAttr->getUIItem();
	if (!uiItem)
		return;
	QPointF pos = uiItem->mapToScene(QPointF());
	QPoint  posi = mapFromScene(pos);
	QRect viewRect(QPoint(), m_curSize);

	if (!viewRect.contains(posi))
	{
		centerOn(pos);
		return;
	}
}
/*
void CodeAtlas::CodeView::drawWordCloud( QPainter *painter )
{
	struct WordCloudItem
	{
		QString  m_txt;
		QPoint	 m_pos;
		QRect	 m_rect;
		int		 m_fontSize;
		int	     m_nodeID, m_wordID;
		float    m_priority;
		float	 m_classRadius;
	};

	QList<WordCloudItem> txtItemList;

	// use tfidf to compute priority
	TFIDFMaker     tfidfMaker;
	LodMerger	   lodMerger;
	WordCloudItem item;
	int ithNode = 0;
	float minRadius = FLT_MAX, maxRadius = 0;
	SymbolTree& tree = DBManager::instance()->getSymbolTree();
	SmartDepthIterator it(tree.getRoot(), SmartDepthIterator::PREORDER,SymbolInfo::ClassStruct | SymbolInfo::FunctionSignalSlot | SymbolInfo::Variable, SymbolInfo::All & ~SymbolInfo::Block );
	for (SymbolNode::Ptr node; node = *it; ++it, ++ithNode)
	{
		SymbolInfo nodeInfo = node->getSymInfo();
		if (!(nodeInfo.isTopLevel() || nodeInfo.elementType() & SymbolInfo::Folder) )
		{
			it.skipSubTree();
			continue;
		}

		UIElementAttr::Ptr uiAttr = node->getAttr<UIElementAttr>();
		if (uiAttr.isNull())continue;
		NodeUIItem::Ptr uiItem = uiAttr->getUIItem();
		if (uiItem.isNull())continue;
		if (uiItem->getLodStatus() & (LOD_INVISIBLE))continue;

		QRect nodeRect = mapFromScene(uiItem->mapToScene(uiItem->boundingRect())).boundingRect();
		if (!nodeRect.intersects(this->rect()))
		{
			continue;
		}

		SymbolWordAttr::Ptr wordAttr = node->getAttr<SymbolWordAttr>();
		if (!wordAttr)
		{
			continue;
		}
		const QMap<int,float>& wordWeightMap = wordAttr->getNameWordWeightMap();
		int ithWord = 0;
		float nodeRadius = uiItem->getEntityRadius() * 0.5;
		minRadius = min(minRadius, nodeRadius);
		maxRadius = max(maxRadius, nodeRadius);
		//lodMerger.addItem()
		srand(ithNode);
		for (QMap<int,float>::ConstIterator pW = wordWeightMap.begin(); pW != wordWeightMap.end(); ++pW, ++ithWord)
		{
			int wordID = pW.key();
			tfidfMaker.addWord(ithNode, wordID, pW.value());

			//printf("freq %f\n", pW.value());
			float x = (rand() / float(RAND_MAX) * 2.f - 1.f) * nodeRadius;
			float y = (rand() / float(RAND_MAX) * 2.f - 1.f) * nodeRadius;
			QPoint  posi = mapFromScene(uiItem->mapToScene(x,y));
			item.m_nodeID = ithNode;
			item.m_wordID = wordID;
			item.m_txt    = wordAttr->getWord(wordID);
			item.m_pos    = posi;
			item.m_classRadius = nodeRadius;
			txtItemList.push_back(item);
		}			
	}

	int nItem = txtItemList.size();
	tfidfMaker.computeTFIDF();
	float minP = FLT_MAX, maxP = 0;
	for (int i = 0; i <  txtItemList.size(); ++i)
	{
		WordCloudItem &item = txtItemList[i];
		float tfidf = tfidfMaker.getTFIDF(item.m_nodeID, item.m_wordID);
		item.m_priority = tfidf * sqrt(item.m_classRadius);//(item.m_classRadius - minRadius)/(maxRadius-minRadius));
		//item.m_priority = tfidf * log(1 + item.m_classRadius);
		//item.m_priority = sqrt(item.m_priority);
		minP = min(minP, item.m_priority);
		maxP = max(maxP, item.m_priority);
	}

	// resolve overlap
	m_overlapSolver.clearInputData();
	float minFontSize = 10, maxFontSize = 30, interval = 5;
	float radiusInterval = (maxRadius - minRadius) / 5;
	//QFont font;
	QFont font("Tahoma", 7, QFont::Light);
	for (int i = 0; i <  nItem; ++i)
	{
		WordCloudItem &item = txtItemList[i];
		float fOffset = (maxFontSize-minFontSize)*(item.m_priority-minP)/(maxP-minP+1e-3);
		int   iOffset = int(fOffset / interval) * interval;
		int fs = minFontSize + iOffset;
		item.m_fontSize = fs;

		//printf("pointsize: %d maxSize = %f min %f p%f\n", fs, maxP, minP, item.m_priority);
		font.setPointSize(item.m_fontSize);
		QFontMetrics fm = QFontMetrics(font);
		QSize size = fm.size(0, item.m_txt);

		item.m_rect=  QRect(0,0,size.width(), size.height());
		item.m_rect.moveCenter(item.m_pos);

		const int padding = 5;
		item.m_rect = UIUtility::expandRect(item.m_rect, padding);

		int level = (item.m_classRadius - minRadius) / radiusInterval;
		m_overlapSolver.addOverlapData(OverlapData(item.m_rect, Priority(level, item.m_priority)));
	}
	//m_overlapSolver.saveAsImg("pre.png");
	m_overlapSolver.compute();
	//m_overlapSolver.saveAsImg("after.png");

	painter->setPen(QPen(QColor(98,98,98,200)));//243,241,222,200)));
	painter->setBrush(Qt::NoBrush);
	painter->setWorldTransform(QTransform());
	for (int i = 0; i < nItem; ++i)
	{
		WordCloudItem &item = txtItemList[i];
		if(!m_overlapSolver.isShown(i))
			continue;

		font.setPointSizeF(item.m_fontSize);
		painter->setFont(font);
		painter->drawText(item.m_rect,Qt::AlignCenter,item.m_txt);
		//painter->drawRect(item.m_rect);
	}
}*/

void CodeAtlas::CodeView::drawWordCloud(QPainter* painter)
{
	float lod = QStyleOptionGraphicsItem::levelOfDetailFromTransform(this->viewportTransform());
	SymbolTree& tree = DBManager::instance()->getSymbolTree();
	LodAttr::Ptr lodAttr = tree.getRoot()->getAttr<LodAttr>();

	typedef LodAttr::WordCloudItem WordCloudItem;
	LodAttr::WordCloudList* wordList = lodAttr->findWordCloudList(lod);
	if (!wordList)
	{
		return;
	}

	m_overlapSolver.clearInputData();
	int nTxtItem = wordList->size();
	for (int i = 0; i < nTxtItem; ++i)
	{
		WordCloudItem &item = (*wordList)[i];
		QPoint pnt = mapFromScene(item.m_pos);

		QRect rect(0,0, item.m_viewSize.width(), item.m_viewSize.height());
		rect.moveCenter(pnt);

		const int padding = 5;
		rect = UIUtility::expandRect(rect, padding);

		m_overlapSolver.addOverlapData(OverlapData(rect, Priority(item.m_priority)));
	}
	//m_overlapSolver.saveAsImg("pre.png");
	m_overlapSolver.compute();
	//m_overlapSolver.saveAsImg("after.png");

	// paint
	QFont font("Tahoma", 7, QFont::Light);
	painter->setPen(QPen(QColor(98,98,98,200)));//243,241,222,200)));
	painter->setBrush(Qt::NoBrush);
	painter->setWorldTransform(QTransform());
	for (int i = 0; i < nTxtItem; ++i)
	{
		WordCloudItem &item = (*wordList)[i];
		if(!m_overlapSolver.isShown(i))
			continue;

		font.setPointSizeF(item.m_fontSize);
		painter->setFont(font);
		painter->drawText(m_overlapSolver.getRect(i),Qt::AlignCenter,item.m_txt);
	}

	//painter->setPen(QPen());
	//painter->drawRect(0,0,55,30);
	
}
/*
void CodeAtlas::CodeView::drawWordCloud( QPainter *painter )
{



/*
	// use tfidf to compute priority
	LodMerger	   lodMerger;
	int ithNode = 0;
	SymbolTree& tree = DBManager::instance()->getSymbolTree();
	QList<SymbolNode::Ptr> nodeList;
	SmartDepthIterator it(tree.getRoot(), SmartDepthIterator::PREORDER,SymbolInfo::Class, SymbolInfo::All & ~SymbolInfo::Block & ~SymbolInfo::Function & ~SymbolInfo::Variable);
	for (SymbolNode::Ptr node; node = *it; ++it, ++ithNode)
	{
		SymbolInfo info = node->getSymInfo();
		if (info.elementType() & SymbolInfo::Folder)
			continue;

		UIElementAttr::Ptr uiAttr = node->getAttr<UIElementAttr>();
		if (uiAttr.isNull())continue;
		NodeUIItem::Ptr uiItem = uiAttr->getUIItem();
		if (uiItem.isNull())continue;
		if (uiItem->getLodStatus() & (LOD_INVISIBLE))continue;
		
		QRect nodeRect = mapFromScene(uiItem->mapToScene(uiItem->entityRect())).boundingRect();
		if (0 && !nodeRect.intersects(this->rect()))
		{
			continue;
		}

		lodMerger.addItem(nodeRect.center(), nodeRect.width()* 0.5f);
		nodeList.push_back(node);	

		if (info.isTopLevel())
		{
			it.skipSubTree();
		}
	}
	if (!nodeList.size())
		return;

	// cluster nodes
	lodMerger.setMinMergeSize(QSizeF(55,30));
	lodMerger.compute();

	// collect words
	int nCluster = lodMerger.getClusterNum();
	if (!nCluster)
		return;
	typedef SymbolWordAttr::WordMap WordMap;
	vector<WordMap> clusterWordList(nCluster);
	for (int i = 0; i < nodeList.size(); ++i)
	{
		int clusterID = lodMerger.getClusterID(i);
		SymbolWordAttr::Ptr wordAttr = nodeList[i]->getAttr<SymbolWordAttr>();
		if (!wordAttr)
			continue;
		const WordMap& wordWeightMap = wordAttr->getNameWordWeightMap();
		SymbolWordAttr::insertWords(clusterWordList[clusterID], wordWeightMap);
	}

	// compute tfidf
	TFIDFMaker     tfidfMaker;
	for (int i = 0; i < clusterWordList.size(); ++i)
	{
		WordMap::Iterator pW;
		for (pW = clusterWordList[i].begin(); pW != clusterWordList[i].end(); ++pW)
		{
			tfidfMaker.addWord(i, pW.key(), pW.value());
		}
	}
	tfidfMaker.computeTFIDF();

	// extract words from each cluster
	struct WordCloudItem
	{
		QString  m_txt;
		QPoint	 m_pos;
		QRect	 m_rect;
		int		 m_fontSize;
		int	     m_nodeID, m_wordID;
		float    m_priority;
		float	 m_classRadius;
		int		 m_clusterID;
	}wcItem;
	QList<WordCloudItem> txtItemList;
	float minP = FLT_MAX, maxP = 0;
	for (int ithCluster = 0; ithCluster < nCluster; ++ithCluster)
	{
		const QRectF& rect = lodMerger.getClusterRect(ithCluster);
		float topLeft[2] = {rect.center().x(), rect.center().y()};
		float dim[2]     = {rect.width()*0.5f, rect.height()*0.5f};

		WordMap& wordMap = clusterWordList[ithCluster];
		srand(wordMap.size());
		for (WordMap::Iterator pW = wordMap.begin(); pW!= wordMap.end(); ++pW)
		{
			wcItem.m_clusterID = ithCluster;

			int wordID = pW.key();
			wcItem.m_txt = SymbolWordAttr::getWord(wordID);
			wcItem.m_priority = tfidfMaker.getTFIDF(ithCluster, wordID);
			minP = min(minP, wcItem.m_priority);
			maxP = max(maxP, wcItem.m_priority);
			
			float frag[2] = {0.5,0.5};//{rand()/float(RAND_MAX)*2.f-1.f, rand()/float(RAND_MAX)*2.f-1.f};
			wcItem.m_pos.rx() = topLeft[0] + frag[0] * dim[0] * 0.5f;
			wcItem.m_pos.ry() = topLeft[1] + frag[1] * dim[1] * 0.5f;
			txtItemList.push_back(wcItem);
		}
	}
	int nTxtItem = txtItemList.size();

	// resolve overlap
	m_overlapSolver.clearInputData();
	float minFontSize = 12, maxFontSize = 25, interval = 4;
	QFont font("Tahoma", 7, QFont::Light);
	for (int i = 0; i <  nTxtItem; ++i)
	{
		WordCloudItem &item = txtItemList[i];
		float fOffset = (maxFontSize-minFontSize)*(item.m_priority-minP)/(maxP-minP+1e-3);
		int   iOffset = int(fOffset / interval) * interval;
		int fs = minFontSize + iOffset;
		item.m_fontSize = fs;

		//printf("pointsize: %d maxSize = %f min %f p%f\n", fs, maxP, minP, item.m_priority);
		font.setPointSize(item.m_fontSize);
		QFontMetrics fm = QFontMetrics(font);
		QSize size = fm.size(0, item.m_txt);

		item.m_rect=  QRect(0,0,size.width(), size.height());
		item.m_rect.moveCenter(item.m_pos);

		const int padding = 5;
		item.m_rect = UIUtility::expandRect(item.m_rect, padding);

		m_overlapSolver.addOverlapData(OverlapData(item.m_rect, Priority(item.m_priority)));
	}
	//m_overlapSolver.saveAsImg("pre.png");
	m_overlapSolver.compute();
	//m_overlapSolver.saveAsImg("after.png");

	// paint
	painter->setPen(QPen(QColor(98,98,98,200)));//243,241,222,200)));
	painter->setBrush(Qt::NoBrush);
	painter->setWorldTransform(QTransform());
	for (int i = 0; i < nTxtItem; ++i)
	{
		WordCloudItem &item = txtItemList[i];
		if(!m_overlapSolver.isShown(i))
			continue;

		font.setPointSizeF(item.m_fontSize);
		painter->setFont(font);
		painter->drawText(item.m_rect,Qt::AlignCenter,item.m_txt);
	}

	for (int ithCluster = 0; ithCluster < nCluster; ++ithCluster)
	{
		const QRectF& rect = lodMerger.getClusterRect(ithCluster);
		painter->drawRect(rect);
	}

	painter->setPen(QPen());
	painter->drawRect(0,0,55,30);
}*/

void CodeAtlas::CodeView::drawName( QPainter *painter )
{
	struct TxtItem
	{
		unsigned m_type;
		QString  m_txt;
		QRect    m_frameRect;
		LodStatus m_lodStatus;
	};

	QFont varFont("Tahoma", 7, QFont::Light);
	QFont funcFont("Tahoma", 7, QFont::Light);
	QFont classFont("Tahoma", 8, QFont::DemiBold);
	QFont projFont("Tahoma", 8, QFont::Bold);
	QFontMetrics varMetrics(varFont), funcMetrics(funcFont), classMetrics(classFont), projMetrics(projFont);

	QList<OverlapData> overlapVector;
	QList<TxtItem>     overlapNodes;
	QRect viewRect(QPoint(), m_curSize);


	SymbolTree& tree = DBManager::instance()->getSymbolTree();
	SmartDepthIterator it(tree.getRoot(), SmartDepthIterator::PREORDER,SymbolInfo::All, SymbolInfo::All & ~SymbolInfo::Block);

	const QString nextLine(QChar::LineSeparator);
	const int lineLength = 15;
	for (SymbolNode::Ptr node; node = *it; ++it)
	{
		UIElementAttr::Ptr uiAttr = node->getAttr<UIElementAttr>();
		if (uiAttr.isNull())
			continue;

		NodeUIItem::Ptr uiItem = uiAttr->getUIItem();
		if (uiItem.isNull())
			continue;

		if (uiItem->getLodStatus() == LOD_INVISIBLE)
			continue;

		bool isExpanded = uiItem->getLodStatus() == LOD_EXPANDED;
		bool isHighLighted=uiItem->getLodStatus() & LOD_HIGHLIGHTED;

		SymbolInfo info = node->getSymInfo();
		unsigned type = (unsigned)info.elementType();
		if (isExpanded && (type & (SymbolInfo::ClassStruct | SymbolInfo::Project)))
			continue;

		QString lastPart = TextProcessor::findLastPart(uiItem->name());
		QString dispName = TextProcessor::breakTextByLength(lastPart, lineLength, nextLine);

		QPointF pos;
		QSize   titleSize; 
		if (type & (SymbolInfo::Variable | SymbolInfo::Enum | SymbolInfo::Enumerator | SymbolInfo::Macro))
		{
			titleSize = varMetrics.size(0,dispName);
			pos.ry() += uiItem->getEntityRadius();
		}
		else if (type & SymbolInfo::FunctionSignalSlot)
		{
			titleSize = funcMetrics.size(0,dispName);
			pos.ry() += uiItem->getEntityRadius();
		}
		else if (type & (SymbolInfo::ClassStruct|SymbolInfo::Namespace|SymbolInfo::Struct))
		{
			titleSize = classMetrics.size(0,dispName);
			//pos.ry() += uiItem->getEntityRadius() * 0.8;
		}
		else if (type & SymbolInfo::Project)
		{
			titleSize = projMetrics.size(0,dispName);
		}
		else 
			continue;

		QPoint  posi = mapFromScene(uiItem->mapToScene(pos));
		QRect   titleRect(QPoint(), titleSize);
		titleRect.moveCenter(posi);
		if (type & (SymbolInfo::Variable | SymbolInfo::Enum | SymbolInfo::FunctionSignalSlot | SymbolInfo::Namespace))
			titleRect.translate(0, titleSize.height() / 2);

		if (!titleRect.intersects(viewRect))
			continue;

		const int padding = isHighLighted ? 3 : 35;
		QRect affectRect = UIUtility::expandRect(titleRect, padding);

		float radius = uiItem->getEntityRadius();
		float firstPriority = uiItem->isSelected() ? 3 : 2;
		float secondPriority = radius;// * radius * uiItem->getLevel() / (titleRect.width() * titleRect.height());
		if (type & (SymbolInfo::Variable))
		{
			secondPriority /= 10;
		}
		overlapVector.push_back(OverlapData(affectRect, Priority(firstPriority, secondPriority)));

		TxtItem txtItem;
		txtItem.m_txt = dispName;
		txtItem.m_type = type;
		txtItem.m_frameRect = titleRect;
		txtItem.m_lodStatus = uiItem->getLodStatus();
		overlapNodes.push_back(txtItem);
	}

	m_overlapSolver.clearInputData();
	m_overlapSolver.setOverlapData(overlapVector);
	m_overlapSolver.compute();

	const QList<OverlapData>& result = m_overlapSolver.getOverlapData();

	painter->setWorldTransform(QTransform());
	for (int i = 0; i < result.size(); ++i)
	{
		if (!result[i].m_isShown)
			continue;

		TxtItem& txtItem = overlapNodes[i];
		unsigned type = txtItem.m_type;
		const QRect& rect = txtItem.m_frameRect;

		if ( type & (SymbolInfo::Project | SymbolInfo::ClassStruct | SymbolInfo::Enum))
		{
			if (txtItem.m_lodStatus == LOD_HIGHLIGHTED)
				painter->setBrush(QBrush(QColor(200,32,2,200)));
			else
				painter->setBrush(QBrush(QColor(255,255,255,150)));
			painter->setPen(Qt::NoPen);
			const int padding = 5;
			QRect roundedRect = rect.adjusted(-padding, -padding/2, padding, padding/2);
			painter->drawRoundedRect(roundedRect, padding, padding, Qt::AbsoluteSize);
			//painter->drawRect(rect);
		}

		QFont* font;
		if (type & (SymbolInfo::Variable | SymbolInfo::Enum))
		{
			font = &varFont;
			painter->setPen(QPen(QColor(50,50,50)));
		}
		else if (type & SymbolInfo::FunctionSignalSlot)
		{
			font = &funcFont;
			painter->setPen(QPen(QColor(50,50,50)));
		}
		else if (type & SymbolInfo::ClassStruct)
		{
			font = &classFont;
			painter->setPen(QPen(QColor(0,0,0,150)));
		}
		else if (type & SymbolInfo::Project)
		{
			font = &projFont;
			painter->setPen(QPen(QColor(0,0,0)));
		}
		else
			continue;

		painter->setFont(*font);
		QString& brokenName = overlapNodes[i].m_txt;

		QRect& r = txtItem.m_frameRect;
		QPen oriPen = painter->pen();

		if (txtItem.m_lodStatus == LOD_HIGHLIGHTED)
			painter->setPen(QPen(QColor(255,220,100)));
		else
			painter->setPen(QPen(QColor(255,255,255)));
		// draw outline
		const int o = 1;
		painter->drawText(r.translated(-o,-o), Qt::AlignCenter,brokenName);
		painter->drawText(r.translated(-o, o), Qt::AlignCenter,brokenName);
		painter->drawText(r.translated( o,-o), Qt::AlignCenter,brokenName);
		painter->drawText(r.translated( o, o), Qt::AlignCenter,brokenName);

		painter->setPen(oriPen);
		painter->drawText(result[i].m_rect, Qt::AlignCenter,brokenName);

	}
}
