#pragma once

using namespace std;
namespace CodeAtlas
{

	struct Priority
	{
		Priority(float first = 0.f, float second = 0.f, float third = 0.f):m_firstPriority(first), m_secondPriority(second), m_thirdPriority(third){}
		inline bool operator<(Priority& other)const
		{
			if (m_firstPriority != other.m_firstPriority)
				return m_firstPriority < other.m_firstPriority;
			if (m_secondPriority != other.m_secondPriority)
				return m_secondPriority < other.m_secondPriority;
			return m_thirdPriority < other.m_thirdPriority;
		}
		float m_firstPriority;
		float m_secondPriority;
		float m_thirdPriority;
	};
	struct OverlapData
	{
		OverlapData(const QRect& rect, const Priority& priority = Priority()):m_rect(rect),m_priority(priority), m_isShown(false){}
		QRect m_rect;
		Priority m_priority;
		bool  m_isShown;
	};
	class OverlapSolver
	{
	public:
		OverlapSolver(float cellSizeFactor = 0.5, int minCellSize = 3):
			  m_lookupBuf(NULL), 
			  m_cellSizeFactor(cellSizeFactor), 
			  m_minCellSize(minCellSize), 
			  m_cellWidth(0), m_cellHeight(0), 
			  m_cellHeightCount(0), m_cellWidthCount(0){}
		
		~OverlapSolver();

		void clearAll();
		void clearInputData();
		void resetViewBuf();
		void setViewRect(const QRect& viewRect);

		void setMinCellSize(int minCellSize);
		void setOverlapData(const QList<OverlapData>& data);
		void addOverlapData(const OverlapData& data);
		const QList<OverlapData>& getOverlapData();
		bool isShown(int ithItem)const;
		void compute();

		void saveAsImg(char* fileName);

	private:
		struct OverlapPriority
		{
			OverlapPriority(OverlapData* data):m_data(data){}
			bool operator<(const OverlapPriority& other)const
			{return m_data->m_priority < other.m_data->m_priority;}
			OverlapData* m_data;
		};
		void allocateViewBuf();
		void clearViewBuf();

		bool tryToAddToBuf(const OverlapData& data);
		bool isLookupBufValid();

		float				m_cellSizeFactor;
		int					m_minCellSize;
		int					m_cellWidth, m_cellHeight;
		int					m_cellWidthCount, m_cellHeightCount;

		QList<OverlapData> m_overlapData;
		QRect				m_viewRect;
		char*				m_lookupBuf;
	};
}