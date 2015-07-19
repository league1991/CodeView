#include "StdAfx.h"
#include "FolderUIItem.h"

using namespace CodeAtlas;

FolderUIItem::FolderUIItem( const SymbolNode::WeakPtr& node, UIItem* parent /*= 0*/ ) :ClassUIItem(node, parent)
{
}

FolderUIItem::~FolderUIItem(void)
{
}


void CodeAtlas::FolderUIItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option/*=0*/, QWidget *widget /*= 0*/ )
{
	float lod = QStyleOptionGraphicsItem::levelOfDetailFromTransform(painter->worldTransform());

	if (0 && m_uiAttr)
	{
		if (QSharedPointer<QPolygonF> visualHull = m_uiAttr->getVisualHull())
		{
			//if (m_lodStatus & (LOD_EXPANDED | LOD_HIGHLIGHTED))
			{
				float viewWidth = 1.0;
				float actualWidth = 25;
				float penWidth = actualWidth * lod < viewWidth ? viewWidth / lod : actualWidth;

				QPen pen(QColor(128,128,128,255), penWidth * 1, Qt::DashLine);
				if (m_interactionFlag & IS_FOCUSED)
				{
					pen.setColor(QColor(239,64,27,255));
				}
				else if (m_interactionFlag & IS_MOUSE_HOVER)
				{
					pen.setColor(QColor(252,185,73,255));
				}

				QColor borderColor(128,128,128,255);// = UISetting::getTerritoryColor(colorID).lighter(80);
				float width = penWidth * 1.5;

				painter->setPen(pen);
				painter->setBrush(Qt::NoBrush);
				painter->drawPolygon(*visualHull);
			}
		}
	}

	int fontSize = int((m_radius * 0.1) / 10+1) * 10;
	QFont titleFont("Arial", fontSize);
	painter->setFont(titleFont);
	float alphaW = (lod - 0.1) / (0.6 - 0.1f);
	alphaW = max(min(alphaW, 1.f), 0.f);
	painter->setPen(QPen(QColor(20,20,20,30 * (1.f - alphaW))));

	int pos = m_name.lastIndexOf('\\');
	if (pos == -1)
		return;
	QString name = m_name.mid(pos+1);
	UIUtility::drawText(painter, name, titleFont, QPointF(0, m_radius*0.4), Qt::AlignHCenter|Qt::AlignVCenter);
}
