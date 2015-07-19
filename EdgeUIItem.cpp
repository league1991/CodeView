#include "stdafx.h"

using namespace CodeAtlas;

float CodeAtlas::EdgeUIItem::s_edgeBaseZValue = 1;

EdgeUIItem::EdgeUIItem( const SymbolEdge::WeakPtr& edge, UIItem* parent /*= NULL*/ ):
UIItem(parent),
m_edge(edge),
m_level(GraphUtility::s_defaultBaseLevel),
m_edgeWidth(1.f),
m_edgeWidthToDraw(1.f),
m_shapeWidth(1.f),
m_displayAlpha(1.f)
{
	m_uiAttr = m_edge.toStrongRef()->getAttr<EdgeUIElementAttr>().data();
}


EdgeUIItem::Ptr CodeAtlas::EdgeUIItem::creator( const SymbolEdge::Ptr& edge, NodeUIItem* parent )
{
	EdgeUIItem::Ptr ui;
	switch (edge->type())
	{
	case SymbolEdge::EDGE_FUZZY_DEPEND:
		ui = EdgeUIItem::Ptr(new EdgeUIItem(edge.toWeakRef(), parent));
			break;
	}
	return ui;
}

void CodeAtlas::EdgeUIItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option/*=0*/, QWidget *widget /*= 0*/ )
{
	if (m_edge.isNull() || !m_uiAttr || m_lodStatus & (LOD_INVALID | LOD_INVISIBLE))
		return;

	SymbolInfo::ElementType srcType, tarType;
	if (!getSymbolType(srcType, tarType))
		return;

	int level = GraphUtility::s_defaultBaseLevel;
	UIElementAttr::Ptr uiAttr = m_edge.toStrongRef()->srcNode().toStrongRef()->getAttr<UIElementAttr>();
	if (uiAttr)
	{
		level = uiAttr->level();
	}
	unsigned flag = m_edge.toStrongRef()->flag();
	float lod = QStyleOptionGraphicsItem::levelOfDetailFromTransform(painter->worldTransform());
	computeEdgeWidth();
	float edgeWidth = m_edgeWidthToDraw;

	// determine edge color
	QColor laneClr;
	int alpha = m_displayAlpha * 255;
	bool isVarEdge = false;//srcType & SymbolInfo::Variable || tarType & SymbolInfo::Variable;
	if (isVarEdge)
	{
		alpha *= 0.4;
		laneClr = QColor(201,197,188, alpha);
	}
	else
	{
		switch(level)
		{
		case 1:		laneClr = QColor(250,250,250,alpha); break;
		case 2:		laneClr = QColor(253,252,177,alpha); break;
		case 3:		laneClr = QColor(254,214,105,alpha); break;
		case 4:		laneClr = QColor(254,195,101,alpha); break;
		default:
		case 5:		laneClr = QColor(254,177,101,alpha); break;	
		}
		if (flag & (Ref_Base|Ref_Type))
		{
			laneClr = QColor(185,122,87);
		}
		else if (flag & Ref_Call)
		{
			laneClr = QColor(255,201,14);
		}
		else if (flag & (Ref_Modify|Ref_Use|Ref_Set))
		{
			laneClr = QColor(112,146,190);
		}
		else if (flag & (Ref_Friend))
		{
			laneClr = QColor(184,206,94);
		}
	}

	if (m_lodStatus & (LOD_EXPANDED) && !isVarEdge)
	{
		QPen pen(QColor(150,150,150,255), edgeWidth * 1.5,Qt::SolidLine, Qt::FlatCap);
		pen.setColor(UIUtility::adjustLightness(laneClr, -0.2));
		painter->setPen(pen);
		painter->drawPath(m_path);
	}
	else if (m_lodStatus & LOD_HIGHLIGHTED)
	{
		//253,103,77
		QPen pen(QColor(253,103,77), edgeWidth * 1.8,Qt::SolidLine, Qt::FlatCap);
		painter->setPen(pen);
		painter->drawPath(m_path);
	}
	//laneClr.setAlphaF((m_lodStatus & (LOD_EXPANDED|LOD_HIGHLIGHTED)) ? 1 : 0.5);

	QPen lanePen(laneClr, 7,Qt::SolidLine, Qt::FlatCap);
	lanePen.setWidthF(edgeWidth);
	if (isVarEdge && 0)
	{
		QVector<qreal> dashPattern;
		float interval = 20;
		float l = interval / edgeWidth;
		dashPattern << l << l;
		lanePen.setDashPattern(dashPattern);
	}

	painter->setPen(lanePen);
	painter->drawPath(m_path);

	//QPainterPathStroker stroker;
	//stroker.setWidth(edgeWidth);
	//m_shapePath = stroker.createStroke(m_path);

	if (!isVarEdge)
		if (m_lodStatus == LOD_HIGHLIGHTED)
			drawArrow(painter, QColor(200,32,2));
		else if (m_lodStatus == LOD_EXPANDED)
			drawArrow(painter, QColor(155,155,155));
		else
		{
			MatrixXf& points = m_uiAttr->pointList();
			if (points.rows() <= 1 || points.cols() != 2)
				return;


			int begID = (points.rows()-2);//*0.66;
			int endID = begID+1;

			QPointF head(points(endID,0), points(endID,1));
			QPointF tail(points(begID,0), points(begID,1));

			QPointF dir = tail - head;
			qreal l = sqrt(dir.x()*dir.x() + dir.y()*dir.y());
			dir = dir /l  * edgeWidth*4;
			tail = head + dir;
			
			QPointF nor(dir.y(), -dir.x());
			nor *= 0.2;
			QPointF arrowHead[3] = {tail + nor, head, tail - nor};
			painter->drawPolyline(arrowHead, 3);
		}
}

void CodeAtlas::EdgeUIItem::buildUI()
{
	if (m_edge.isNull() || !m_uiAttr)
		return;

	MatrixXf& pntList = m_uiAttr->pointList();
	if (pntList.rows() <= 0)
		return; 

	m_path = QPainterPath();
	m_path.moveTo(pntList(0,0), pntList(0,1));
	for (int ithPnt = 1; ithPnt < pntList.rows(); ++ithPnt)
	{
		m_path.lineTo(pntList(ithPnt,0), pntList(ithPnt, 1));
	}

	SymbolEdge::Ptr edge = m_edge.toStrongRef();
	SymbolNode::Ptr src = edge->srcNode().toStrongRef();
	SymbolNode::Ptr tar = edge->tarNode().toStrongRef();
	if (src.isNull() || tar.isNull())
		return;

	SymbolInfo srcInfo = src->getSymInfo();
	SymbolInfo tarInfo = tar->getSymInfo();

	QString edgeAction;
	unsigned flag = edge->flag();
	if (flag & Ref_Base)
	{
		edgeAction += "inherit ";
	}
	if (flag & Ref_Call)
	{
		edgeAction += "call ";
	}
	if (flag & Ref_Exception)
	{
		edgeAction += "exception ";
	}
	if (flag & Ref_Friend)
	{
		edgeAction += "friend ";
	}
	if (flag & Ref_Modify)
	{
		edgeAction += "modify ";
	}
	if (flag & Ref_Override)
	{
		edgeAction += "override ";
	}
	if (flag & Ref_Set)
	{
		edgeAction += "set ";
	}
	if (flag & Ref_Type)
	{
		edgeAction += "typedef ";
	}
	if (flag & Ref_Use)
	{
		edgeAction += "use ";
	}
	setToolTip("[" + srcInfo.name() + "] " + edgeAction + "[" +tarInfo.name() + "]");
 
	computeEdgeWidth();
}

CodeAtlas::EdgeUIItem::~EdgeUIItem()
{
}

void CodeAtlas::EdgeUIItem::drawArrow( QPainter* painter, const QColor& color)
{
	if (!m_uiAttr)
		return;

	MatrixXf& points = m_uiAttr->pointList();
	if (points.rows() <= 1 || points.cols() != 2)
		return;

	qreal edgeWidth = m_lodStatus == LOD_HIGHLIGHTED ? m_edgeWidthToDraw * 2.8 : m_edgeWidthToDraw;
	qreal arrowSize = edgeWidth * 0.5;
	const qreal arrowLength = arrowSize * 3;
	const qreal arrowWidth  = arrowSize * 0.5;
	const qreal arrowWing   = arrowLength * 0.7;

	QPen arrowPen(color,qMax(arrowSize * 0.3,0.6), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	painter->setPen(arrowPen);

	VectorXf range = points.colwise().maxCoeff() - points.colwise().minCoeff();
	qreal approxRange = range.norm();
	QPointF thisPnt, nextPnt, dir;
	const float arrowStep = max(arrowLength * 7.f, approxRange / 100.f);
	float restLength = -arrowStep * 0.5;
	int ithPnt = 0;

	while(1)
	{
		if (restLength <= 0)
		{
			//ithPnt--;
			if (ithPnt > points.rows()-2)
				break;
			thisPnt.setX(points(ithPnt,0));
			thisPnt.setY(points(ithPnt,1));
			nextPnt.setX(points(ithPnt+1,0));
			nextPnt.setY(points(ithPnt+1,1));
			QPointF offset = nextPnt - thisPnt;
			float dl = sqrt(offset.x() * offset.x() + offset.y() * offset.y());
			dir = offset / dl;
			thisPnt += dir * -restLength;
			restLength += dl;
			ithPnt++;
		}
		else
		{
			QPointF headPnt = thisPnt + dir * arrowLength;
			UIUtility::drawArrow(painter, headPnt, thisPnt, 0.5, 0.5);
			thisPnt += dir * arrowStep;
			restLength -= arrowStep;
		}
	}
}

float CodeAtlas::EdgeUIItem::computeEdgeWidth()
{
	m_edgeWidth = 1.f;
	const float maxWidth = 5;
	const float c = 0.083f;
	const float b = 1.f / (maxWidth - 1) - c;
	if (m_uiAttr)
		m_edgeWidth = (maxWidth - 1.f / (c * m_uiAttr->edgeWeight() + b)) * 10;
	
	SymbolInfo::ElementType srcType, tarType;
	if (getSymbolType(srcType, tarType))
	{
		if (srcType & SymbolInfo::Variable ||
			tarType & SymbolInfo::Variable)
		{
			m_edgeWidth *= 0.5;
		}
	}
	return m_edgeWidth;
}

void CodeAtlas::EdgeUIItem::isNodeGlobal(bool& isSrcGlobal, bool& isTarGlobal)
{
	isSrcGlobal = isTarGlobal = false;
	SymbolEdge::Ptr edge = m_edge.toStrongRef();
	if (!edge)
		return;

	SymbolNode::Ptr src = edge->srcNode().toStrongRef();
	SymbolNode::Ptr tar = edge->tarNode().toStrongRef();
	if (src.isNull() || tar.isNull())
		return;

	if (src->getAttr<GlobalSymAttr>())
		isSrcGlobal = true;
	if (tar->getAttr<GlobalSymAttr>())
		isTarGlobal = true;
}

bool CodeAtlas::EdgeUIItem::getSymbolType( SymbolInfo::ElementType& srcType, SymbolInfo::ElementType& tarType )
{
	SymbolEdge::Ptr edge = m_edge.toStrongRef();
	if (!edge)
		return false;

	SymbolNode::Ptr src = edge->srcNode().toStrongRef();
	SymbolNode::Ptr tar = edge->tarNode().toStrongRef();
	if (src.isNull() || tar.isNull())
		return false;

	SymbolInfo srcInfo = src->getSymInfo();
	SymbolInfo tarInfo = tar->getSymInfo();

	srcType = srcInfo.elementType();
	tarType = tarInfo.elementType();
	return true;
}

QPainterPath CodeAtlas::EdgeUIItem::shape()const
{
	if (m_lodStatus == LOD_INVISIBLE)
		return QPainterPath();

	// update edge shape when necessary
	// use mutable member to avoid const constraint
	if (m_shapeWidth != m_edgeWidthToDraw)
	{
		QPainterPathStroker stroker;
		stroker.setWidth(m_edgeWidthToDraw);
		m_shapePath = stroker.createStroke(m_path);
		m_shapeWidth = m_edgeWidthToDraw;
	}
	return m_shapePath;
}

float CodeAtlas::EdgeUIItem::getInvariantEdgeViewWidth() const
{
	return MathUtility::linearInterpolate(QPointF(5,0.1), QPointF(40, 3.0), m_edgeWidth);
}

float CodeAtlas::EdgeUIItem::getHighlightedEdgeViewWidth() const
{
	return MathUtility::linearInterpolate(QPointF(5,1.5), QPointF(40, 4.0), m_edgeWidth);
}
