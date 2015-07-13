#include "StdAfx.h"
#include "LodMerger.h"

using namespace CodeAtlas;
LodMerger::LodMerger(void)
{
}

LodMerger::~LodMerger(void)
{
}

void LodMerger::addItem( const QPoint& center, float radius )
{
	Item item;
	item.m_center = QPointF(center.x(), center.y());
	item.m_radius = radius;
	float halfDim = radius / sqrt(2.f);
	item.m_rect = QRectF(	QPointF(center.x() - halfDim, center.y() - halfDim),
		QPointF(center.x() + halfDim, center.y() + halfDim));

	m_itemList.push_back(item);
}

void CodeAtlas::LodMerger::compute()
{
	computeCluster();
}

void CodeAtlas::LodMerger::computeCluster()
{
	int nInput = m_itemList.size();
	const unsigned rootTag = 0x80000000;

	QList<Item> itemTree = m_itemList;
	typedef QSet<int> IDSet;
	IDSet idxSet;
	for (int i = 0; i < nInput; ++i)
	{
		idxSet.insert(i);
		itemTree[i].m_parentID = rootTag;
	}

	// hierarchical cluster
	while(true)
	{
		// exclude too large item
		/*
		IDSet::iterator pID;
		for (pID = idxSet.begin(); pID != idxSet.end();)
		{
			int id = *pID;
			Item& item = itemTree[id];
			if (isEnoughSize(item.m_rect))
			{
				pID = idxSet.erase(pID);
			}
			else
				++pID;
		}*/
		if (idxSet.size() <= 1)
		{
			break;
		}

		// find nearest set
		float minCost = FLT_MAX;
		int ithItem = -1, jthItem = -1;
		IDSet::iterator pi,pj;
		for (pi = idxSet.begin(); pi != idxSet.end(); )
		{
			bool isValid = false;
			for (pj = idxSet.begin(); pj != idxSet.end(); ++pj)
			{
				if (pi == pj)
					continue;

				int idxI = *pi, idxJ = *pj;
				float cost = costFcn(itemTree[idxI].m_rect, itemTree[idxJ].m_rect);
				QRectF uniRect = itemTree[idxI].m_rect.united(itemTree[idxJ].m_rect);
				if (isEnoughSize(uniRect))
					continue;
				else
					isValid = true;

				if (cost < minCost)
				{
					minCost = cost;
					ithItem = idxI;
					jthItem = idxJ;
				}
			}
			if (isValid)
				++pi;
			else
				pi = idxSet.erase(pi);
		}

		if (ithItem != -1 && jthItem != -1)
		{
			// merge set
			int newItemID = itemTree.size();
			Item itm;
			itm.m_parentID = rootTag;
			itm.m_rect = itemTree[ithItem].m_rect.united(itemTree[jthItem].m_rect);
			itemTree.push_back(itm);
			itemTree[ithItem].m_parentID = newItemID;
			itemTree[jthItem].m_parentID = newItemID;

			idxSet.remove(ithItem);
			idxSet.remove(jthItem);
			idxSet.insert(newItemID);
		}
		else
			break;
	}

	// record result
	int clusterCount = 0;
	m_clusterList.clear();
	for (int i = 0; i < itemTree.size(); ++i)
	{
		if (itemTree[i].m_parentID != rootTag)
		{
			int res = i;
			for (; (itemTree[res].m_parentID & rootTag) == 0; res = itemTree[res].m_parentID)
			{}
			itemTree[i].m_parentID = res;
		}
		else
		{
			m_clusterList.push_back(itemTree[i]);
			itemTree[i].m_parentID |= clusterCount;
			clusterCount++;
		}
	}


	m_clusterIDList.clear();
	for (int i = 0; i < nInput; ++i)
	{
		int clusterID;
		if (itemTree[i].m_parentID & rootTag)
		{
			clusterID = itemTree[i].m_parentID & (~rootTag);
		}
		else
		{
			clusterID = (itemTree[itemTree[i].m_parentID].m_parentID & (~rootTag));
		}
		m_clusterIDList.push_back(clusterID);
	}
}

int CodeAtlas::LodMerger::getClusterID( int ithItem )
{
	if (ithItem < 0 || ithItem >= m_clusterIDList.size())
	{
		return -1;
	}
	return m_clusterIDList[ithItem];
}

float CodeAtlas::LodMerger::costFcn( const QRectF& r1, const QRectF& r2 )
{
	/*
	QRectF r = r1.united(r2);
	float ra = r.width() * r.height();
	float r1a= r1.width()* r1.height();
	float r2a= r2.width()* r2.height();
	return ra - r1a - r2a;*/

	QPointF c1 = r1.center();
	QPointF c2 = r2.center();
	float dx = (c1.x() - c2.x()) / m_minMergeSize.width();
	float dy = (c1.y() - c2.y()) / m_minMergeSize.height();

	//return abs(dx) + abs(dy);
	return dx*dx + dy*dy;
}
