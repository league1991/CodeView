#pragma once

namespace CodeAtlas
{
	class LodMerger
	{
	public:
		LodMerger(void);
		~LodMerger(void);

		void				setMinMergeSize(const QSizeF& s){m_minMergeSize = s;}
		void				addItem(const QPoint& center, float radius);

		void				compute();
		int					getClusterID(int ithItem);
		int					getClusterNum(){return m_clusterList.size();}

		const QRectF&       getClusterRect(int ithCluster)const
		{
			return m_clusterList[ithCluster].m_rect;
		}
	private:
		void				computeCluster();
		struct Item
		{
			QRectF			m_rect;
			QPointF			m_center;
			float			m_radius;

			unsigned		m_parentID;
		};
		struct ResultItem
		{
			int				m_oriItemID;
		};

		inline bool			isEnoughSize(const QRectF& rect)
		{
			float w = rect.width(), h = rect.height();
			return w > m_minMergeSize.width() || h > m_minMergeSize.height();
		}
		inline float		normalizeDistanceSq(float p0[2], float p1[2])
		{
			float dx = (p1[0] - p0[0])/m_minMergeSize.width();
			float dy = (p1[1] - p0[1])/m_minMergeSize.height();

			return dx*dx + dy*dy;
		}
		float		costFcn(const QRectF& r1, const QRectF& r2);

		QList<Item>			m_itemList;					// input data
		QList<int>			m_clusterIDList;			// cluster id of input data

		QList<Item>			m_clusterList;				// information of each root clusters

		QSizeF				m_minMergeSize;
	};
}

