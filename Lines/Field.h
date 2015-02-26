#ifndef __FILELD__
#define __FILELD__

#include "Skin.h"
class CMainDlg;

// Cell -------------------------------------------------------------------------

struct Cell
{
	enum CellState {FREE = 0, BUSY = 1, NEXT = 2} state;
	INT color;
};

// Field ------------------------------------------------------------------------

class Field
{
public:
	Field(Skin* _pSkin, UINT _numBalls = 3);
	~Field();

private:
	Cell** m_ppCells;
	CPoint* m_pNextBalls; //это не настоящие точки!!! а индексы!
	INT* m_pNextColors;

	Skin* m_pSkin;
	UINT m_byNextBalls;

public:

// Restart
	void Restart();

// Direct pointers
	Cell* GetPtrCells(INT iNum) {return m_ppCells[iNum];}
	CPoint* GetPtrNextBalls() {return m_pNextBalls;}
	INT* GetPtrNextColors() {return m_pNextColors;}
	UINT& GetRefNextCount() {return m_byNextBalls;}

// Gets & Sets
	Cell::CellState GetCellState(UINT uX, UINT uY);
	Cell::CellState GetCellState(CPoint point)  { return GetCellState(point.x, point.y); }
	BOOL SetCellState(UINT uX, UINT uY, Cell::CellState state);
	BOOL SetCellState(CPoint point, Cell::CellState state)  { return SetCellState(point.x, point.y, state); }

	INT GetCellColor(UINT uX, UINT uY);
	INT GetCellColor(CPoint point)  { return GetCellColor(point.x, point.y); }
	BOOL SetCellColor(UINT uX, UINT uY, INT color);
	BOOL SetCellColor(CPoint point, INT color)  { return SetCellColor(point.x, point.y, color); }

	CPoint GetNextBallCoord(UINT _number);
	BOOL SetNextBallCoord(UINT _number, CPoint point);

	INT GetNextBallColor(UINT _number);
	BOOL SetNextBallColor(UINT _number, INT color);

	UINT GetNumberOfNext();
	BOOL SetNumberOfNext(UINT _numBalls);

// Utilities
	CPoint GetPointFromIndexes(UINT uX, UINT uY);
	CPoint GetPointFromIndexes(CPoint point)  { return GetPointFromIndexes(point.x, point.y); }

	CPoint GetRealPointFromIndexes(UINT uX, UINT uY);
	CPoint GetRealPointFromIndexes(CPoint point)  { return GetRealPointFromIndexes(point.x, point.y); }

	CPoint GetIndexesFromRealPoint(UINT uX, UINT uY);
	CPoint GetIndexesFromRealPoint(CPoint point)  { return GetIndexesFromRealPoint(point.x, point.y); };

	UINT GetTotalColors();

// Modify field
	BOOL PrepareNextBalls(CArray<CPoint, CPoint>& arrInvalidate, BOOL bInvalidate = TRUE);
	BOOL ConvertNextBallsToCurrent(CArray<CPoint, CPoint>& arrAppearence/*, CArray<CPoint, CPoint>& arrInvalidate*/);
	BOOL Move(UINT uPrevX, UINT uPrevY, UINT uNextX, UINT uNextY, CArray<CPoint, CPoint>& arrInvalidate, BOOL bInvalidate = TRUE);
	BOOL Move(CPoint _ptPrev, CPoint _ptNext, CArray<CPoint, CPoint>& arrInvalidate, BOOL bInvalidate = TRUE)  { return Move(_ptPrev.x, _ptPrev.y, _ptNext.x, _ptNext.y, arrInvalidate, bInvalidate); }
	void Destroy(CArray<CPoint, CPoint>& arrDestroyBalls);
	BOOL FindWay(CArray<CPoint, CPoint>& arrMoveBalls, UINT uPrevX, UINT uPrevY, UINT uNextX, UINT uNextY);
	BOOL FindWay(CArray<CPoint, CPoint>& arrMoveBalls, CPoint _ptPrev, CPoint _ptNext)  { return FindWay(arrMoveBalls, _ptPrev.x, _ptPrev.y, _ptNext.x, _ptNext.y); }
	BOOL FindDestroy(CArray<CPoint, CPoint>& arrDestroyBalls);
};

// ------------------------------------------------------------------------------


#endif //__FILELD__