#include "stdafx.h"
#include "OverlapSolver.h"

using namespace CodeAtlas;
using namespace std;

void CodeAtlas::OverlapSolver::clearAll()
{
	clearViewBuf();
	clearInputData();
}

void CodeAtlas::OverlapSolver::setViewRect( const QRect& viewRect )
{
	m_viewRect = viewRect;
	allocateViewBuf();
}

bool CodeAtlas::OverlapSolver::isLookupBufValid()
{
	return (m_cellHeightCount > 0 && m_cellWidthCount > 0 && m_lookupBuf);
}

void CodeAtlas::OverlapSolver::clearInputData()
{
	m_overlapData.clear();
}

void CodeAtlas::OverlapSolver::resetViewBuf()
{
	if (m_cellHeightCount <= 0 || m_cellWidthCount <= 0 || !m_lookupBuf)
		return;
	int nBufSize = m_cellHeightCount * m_cellWidthCount;
	memset(m_lookupBuf, 0, nBufSize * sizeof(char));
}
void CodeAtlas::OverlapSolver::allocateViewBuf()
{
	clearViewBuf();
	m_cellWidth = (float)m_minCellSize;
	m_cellHeight= (float)m_minCellSize;

	m_cellWidthCount  = m_viewRect.width() / m_cellWidth + 1;
	m_cellHeightCount = m_viewRect.height() / m_cellHeight + 1;
	int nBufSize = m_cellHeightCount * m_cellWidthCount;
	if (nBufSize <= 0)
		return;
	m_lookupBuf = new char[nBufSize];
	memset(m_lookupBuf, 0, nBufSize * sizeof(char));
}

void CodeAtlas::OverlapSolver::clearViewBuf()
{
	delete[] m_lookupBuf;
	m_lookupBuf = NULL;
	m_cellWidthCount = m_cellHeightCount = 0;
}

const QList<OverlapData>& CodeAtlas::OverlapSolver::getOverlapData()
{
	return m_overlapData;
}

void CodeAtlas::OverlapSolver::setOverlapData( const QList<OverlapData>& data )
{
	m_overlapData = data;
}

void CodeAtlas::OverlapSolver::compute()
{
	if (!isLookupBufValid())
		return;

	std::priority_queue<OverlapPriority> elementQueue;
	for (QList<OverlapData>::iterator pO = m_overlapData.begin(); 
		 pO != m_overlapData.end(); ++pO)
	{
		OverlapData& data = *pO;
		elementQueue.push(OverlapPriority(&data));
	}

	while (!elementQueue.empty())
	{
		OverlapPriority topData = elementQueue.top();
		elementQueue.pop();
		//printf("%f %f %f\n", topData.m_data->m_priority.m_firstPriority,topData.m_data->m_priority.m_secondPriority,topData.m_data->m_priority.m_thirdPriority);
		bool isShown = tryToAddToBuf(*topData.m_data);
		topData.m_data->m_isShown = isShown;
	}
}

CodeAtlas::OverlapSolver::~OverlapSolver()
{
	clearViewBuf();
}

bool CodeAtlas::OverlapSolver::tryToAddToBuf(const OverlapData& data )
{
	QRect intersection = m_viewRect.intersected(data.m_rect);
	if (intersection.isEmpty())
	{
		return false;
	}

	int rangeW[2] = {intersection.left() - m_viewRect.left(), intersection.right() - m_viewRect.left()};
	int rangeH[2] = {intersection.top() - m_viewRect.top(), intersection.bottom() - m_viewRect.top()};

	int cellW[2] = {rangeW[0]/m_cellWidth,  min(rangeW[1]/m_cellWidth +1, m_cellWidthCount -1)};
	int cellH[2] = {rangeH[0]/m_cellHeight, min(rangeH[1]/m_cellHeight+1, m_cellHeightCount-1)};

	// check interval range
	bool isOccupied = false;
	char* hBeg = m_lookupBuf + m_cellWidthCount * cellH[0];
	char* hEnd = m_lookupBuf + m_cellWidthCount * cellH[1];
	for (char* pRow = hBeg; pRow <= hEnd; pRow += m_cellWidthCount * sizeof(char))
	{
		for (char* pCol = pRow + cellW[0] * sizeof(char);
			pCol <= pRow + cellW[1]*sizeof(char); ++pCol)
		{
			if (*pCol != 0)
			{
				return false;
			}
		}
	}

	// occupy corresponding area
	for (char* pRow = hBeg; pRow <= hEnd; pRow += m_cellWidthCount * sizeof(char))
	{
		for (char* pCol = pRow + cellW[0] * sizeof(char);
			pCol <= pRow + cellW[1]*sizeof(char); ++pCol)
		{
			*pCol = 1;
		}
	}
	return true;
}

void CodeAtlas::OverlapSolver::setMinCellSize( int minCellSize )
{
	m_minCellSize = minCellSize;
	allocateViewBuf();
}

void CodeAtlas::OverlapSolver::addOverlapData( const OverlapData& data )
{
	m_overlapData.push_back(data);
}

bool CodeAtlas::OverlapSolver::isShown( int ithItem ) const
{
	return m_overlapData[ithItem].m_isShown;
}

void CodeAtlas::OverlapSolver::saveAsImg( char* fileName )
{
	QImage img(m_cellWidthCount, m_cellHeightCount, QImage::Format_ARGB32);
	unsigned* pchar = (unsigned*)img.bits();

	for (int i =0; i < m_cellWidthCount * m_cellHeightCount; ++i, ++pchar)
	{
		if (m_lookupBuf[i])
		{
			*pchar = 0xffffffff;
		}
		else
			*pchar = 0xff00ffff;
	}
	img.save(fileName);
}


