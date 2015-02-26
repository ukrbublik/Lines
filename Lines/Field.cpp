#include "stdafx.h"
#include "Field.h"


//////////////////////////////////////////////// Конструктор ///////////////////////////////////////////

Field::Field (Skin* _pSkin, UINT _numBalls /* = 3*/)
{
	srand(time(NULL));

	TRACE(_T("Creating the field object...\n"));

	// Инициализация переданных параметров...
	m_pSkin = _pSkin;
	if(m_pSkin == NULL)
	{
		TRACE(_T("ERROR! Pointer to 0!!! m_pSkin = NULL\nCan't create the field object!!!\n"));
		this->~Field();
		return;
	}
	m_byNextBalls = _numBalls;
	if(_numBalls < 3 || _numBalls > 5)	// только 3..5
	{
		TRACE(_T("WARNING! Trying to wrong initialize m_byNextBalls = %d, corrected to 3.\n"), _numBalls);
		m_byNextBalls = 3;
	}

	// Создание массива ячеек...
	TRACE(_T("Creating the array of cells...\n"));
	m_ppCells = new Cell*[9];
	for (INT i = 0 ; i < 9 ; i++)
		m_ppCells[i] = new Cell[9];

	// Создание массива next шариков...
	TRACE(_T("Creating the array of balls...\n"));
	m_pNextBalls = new CPoint[m_byNextBalls];
	m_pNextColors = new INT[m_byNextBalls];

	// Стирание массива ячеек и массива next шариков
	Restart();

	TRACE(_T("Field object successfully created!\n"));
}


//////////////////////////////////////////////// Очистка значений ///////////////////////////////////////////

void Field::Restart()
{
	// стирание массива ячеек...
	TRACE(_T("Clearing the array of cells...\n"));
	for (INT i = 0 ; i < 9 ; i++)
	{
		for (INT j = 0 ; j < 9 ; j++)
		{
			SetCellState(i, j, Cell::CellState::FREE);
			SetCellColor(i, j, -1);
		}
	}

	// Стирание массива next шариков...
	TRACE(_T("Clearing the array of balls...\n"));
	for (INT i = 0 ; i < m_byNextBalls ; i++)
	{
		SetNextBallCoord(i, CPoint(-1, -1));
		SetNextBallColor(i, -1);
	}
}


//////////////////////////////////////////////// Деструктор ///////////////////////////////////////////

Field::~Field()
{
	TRACE(_T("Deleting field object...\n"));

	for (INT i = 0 ; i < 9 ; i++)
		delete [] m_ppCells[i];
	delete [] m_ppCells;
	delete [] m_pNextBalls;
	delete [] m_pNextColors;

	TRACE(_T("Field object successfully deleted!\n"));
}

//////////////////////////////////////////// Гетеры и сетеры /////////////////////////////////////////////////

Cell::CellState Field::GetCellState(UINT uX, UINT uY)
{
	if(uX < 0 || uY < 0 || uX >= 9 || uY >= 9)
	{
		TRACE(_T("[Field::GetCellState !] Wrong cell indexes: %d, %d. Must be [0 ; 8]!\n"), uX, uY);
		return (Cell::CellState) -1;
	}
	else
		return m_ppCells[uX][uY].state;
}

BOOL Field::SetCellState(UINT uX, UINT uY, Cell::CellState state)
{
	if(uX < 0 || uY < 0 || uX >= 9 || uY >= 9)
	{
		TRACE(_T("[Field::SetCellState !] Wrong cell indexes: %d, %d. Must be [0 ; 8]!\n"), uX, uY);
		return FALSE;
	}
	else
	{
		m_ppCells[uX][uY].state = state;
		return TRUE;
	}
}

INT Field::GetCellColor(UINT uX, UINT uY)
{
	if(uX < 0 || uY < 0 || uX >= 9 || uY >= 9)
	{
		TRACE(_T("[Field::GetCellColor !] Wrong cell indexes: %d, %d. Must be [0 ; 8]!\n"), uX, uY);
		return -1;
	}
	else
		return m_ppCells[uX][uY].color;
}

BOOL Field::SetCellColor(UINT uX, UINT uY, INT color)
{
	if(uX < 0 || uY < 0 || uX >= 9 || uY >= 9)
	{
		TRACE(_T("[Field::SetCellColor !] Wrong cell indexes: %d, %d. Must be [0 ; 8]!\n"), uX, uY);
		return FALSE;
	}
	else
	{
		m_ppCells[uX][uY].color = color;
		return TRUE;
	}
}

CPoint Field::GetNextBallCoord(UINT _number)
{
	if(_number < 0 || _number >= m_byNextBalls)
	{
		TRACE(_T("[Field::GetNextBallCoord !] Wrong next ball index: %d. Must be [0 ; %d]!\n"), _number, m_byNextBalls - 1);
		return CPoint(-1, -1);
	}
	else
	{
		return m_pNextBalls[_number];
	}
}

BOOL Field::SetNextBallCoord(UINT _number, CPoint point)
{
	if(_number < 0 || _number >= m_byNextBalls)
	{
		TRACE(_T("[Field::SetNextBallCoord !] Wrong next ball index: %d. Must be [0 ; %d]!\n"), _number, m_byNextBalls - 1);
		return FALSE;
	}
	else
	{
		m_pNextBalls[_number] = point;
		return TRUE;
	}
}

INT Field::GetNextBallColor(UINT _number)
{
	if(_number < 0 || _number >= m_byNextBalls)
	{
		TRACE(_T("[Field::GetNextBallColor !] Wrong next ball index: %d. Must be [0 ; %d]!\n"), _number, m_byNextBalls - 1);
		return -1;
	}
	else
	{
		return m_pNextColors[_number];
	}
}

BOOL Field::SetNextBallColor(UINT _number, INT color)
{
	if(_number < 0 || _number >= m_byNextBalls)
	{
		TRACE(_T("[Field::SetNextBallColor !] Wrong next ball index: %d. Must be [0 ; %d]!\n"), _number, m_byNextBalls - 1);
		return FALSE;
	}
	else
	{
		m_pNextColors[_number] = color;
		return TRUE;
	}
}

UINT Field::GetNumberOfNext()
{ 
	return m_byNextBalls; 
}

BOOL Field::SetNumberOfNext(UINT _numBalls)
{
	if(_numBalls < 3 || _numBalls > 5)	// только 3..5
	{
		TRACE(_T("WARNING! Trying to wrong set m_byNextBalls = %d.\n"), _numBalls);
		return FALSE;
	}
	else
	{
		m_byNextBalls = _numBalls;
		return TRUE;
	}
}

/////////////////// Преобразоваtель индексов шарика на поле -> в относителные координаты (пикселы) /////////////////

CPoint Field::GetPointFromIndexes(UINT uX, UINT uY)
{
	if(uX < 0 || uY < 0 || uX >= 9 || uY >= 9)
	{
		TRACE(_T("[Field::GetPointFromIndexes !] Wrong cell indexes: %d, %d. Must be [0 ; 8]!\n"), uX, uY);
		return CPoint(-1, -1);
	}
	else
	{
		return CPoint(
			(m_pSkin->placementOfElements.sizeBetweenBalls.cx + m_pSkin->sizesOfElements.sizeBall.cx) * uX,
			(m_pSkin->placementOfElements.sizeBetweenBalls.cy + m_pSkin->sizesOfElements.sizeBall.cy) * uY
		);
	}
}

/////////////////// Преобразоваtель индексов шарика на поле -> в абсолютные координаты (пикселы) /////////////////

CPoint Field::GetRealPointFromIndexes(UINT uX, UINT uY)
{
	if(uX < 0 || uY < 0 || uX >= 9 || uY >= 9)
	{
		TRACE(_T("[Field::GetRealPointFromIndexes !] Wrong cell indexes: %d, %d. Must be [0 ; 8]!\n"), uX, uY);
		return CPoint(-1, -1);
	}
	else
	{
		return CPoint(
			m_pSkin->placementOfElements.pointUpperLeftBall.x + GetPointFromIndexes(uX, uY).x,
			m_pSkin->placementOfElements.pointUpperLeftBall.y + GetPointFromIndexes(uX, uY).y
		);
	}

}

/////////////////// Преобразоваtель реальных координат поля (в пикселях) -> в индекс шарика /////////////////

CPoint Field::GetIndexesFromRealPoint(UINT uX, UINT uY)
{
	for ( INT i = 0 ; i < 9 ; i++ )
		for ( INT j = 0 ; j < 9 ; j++ )
			if ( uX >= GetRealPointFromIndexes(i, j).x && uX < (GetRealPointFromIndexes(i, j).x + m_pSkin->sizesOfElements.sizeBall.cx)
				&&
				uY >= GetRealPointFromIndexes(i, j).y && uY < (GetRealPointFromIndexes(i, j).y + m_pSkin->sizesOfElements.sizeBall.cy)
			)
				return CPoint(i, j);
	return CPoint(-1, -1);
}

////////////////////////////////////////////// Сколько цветов? ////////////////////////////////////////////////

UINT Field::GetTotalColors()
{
	return (m_pSkin->images.imgBalls.GetHeight() / m_pSkin->sizesOfElements.sizeBall.cy);
}


////////////////////////////////// Подготовить следующую партию next шариков ////////////////////////////////////

BOOL Field::PrepareNextBalls(CArray<CPoint, CPoint>& arrInvalidate, BOOL bInvalidate/* = TRUE*/)
{
	TRACE(_T("Generating next balls:\n"));

	UINT nFreeCells = 0;
	UINT nNextBallsToGenerate = 0;
	INT iFreeCell = -1;
	INT iRand = -1;

	// Сколько next шариков надо сгенерировать?
	for (INT iNext = 0 ; iNext < m_byNextBalls ; iNext++)
		if(GetNextBallCoord(iNext) == CPoint(-1, -1) || GetNextBallColor(iNext) == -1)
			nNextBallsToGenerate++;

	// Если уже всё готово...
	if(nNextBallsToGenerate == 0)
	{
		TRACE(_T("WARNING! nothing to generate!\n"));
		return TRUE;
	}

	// Сколько свободных ячеек?
	for(INT i = 0 ; i < 9 ; i++)
		for(INT j = 0 ; j < 9 ; j++)
			if(GetCellState(i, j) == Cell::CellState::FREE)
				nFreeCells++;

	// Если свободных ячеек меньше, чем надо...
	if(nFreeCells < nNextBallsToGenerate)
	{
		TRACE(_T("ERROR! not enough free cells!\n"));
		return FALSE;
	}

	// Генерация:
	for (INT iNext = 0 ; iNext < m_byNextBalls ; iNext++)
	{
		if(GetNextBallCoord(iNext) != CPoint(-1, -1) && GetNextBallColor(iNext) != -1)
			continue;

		iRand = rand() % nFreeCells;
		iFreeCell = -1;

		for(INT i = 0 ; i < 9 ; i++)
		{
			for(INT j = 0 ; j < 9 ; j++)
			{
				if(GetCellState(i, j) == Cell::CellState::FREE)
					iFreeCell++;

				if(iFreeCell == iRand)
				{
					// Заполнение next шарика
					if(GetNextBallCoord(iNext) == CPoint(-1, -1))
						SetNextBallCoord(iNext, CPoint(i, j));
					if(GetNextBallColor(iNext) == -1)
						SetNextBallColor(iNext, rand() % GetTotalColors());

					// Заполнение ячейки
					SetCellState(GetNextBallCoord(iNext), Cell::CellState::NEXT);
					SetCellColor(GetNextBallCoord(iNext), GetNextBallColor(iNext));

					if(bInvalidate)
					{
						// Пометить ячейку для дальнейшего обновления
						arrInvalidate.Add(GetNextBallCoord(iNext));
					}

					// Свободных ячеек стало на 1 меньше
					nFreeCells--;

					TRACE(_T("Next ball is generated: (%d, %d), color is %d\n"), GetNextBallCoord(iNext).x, GetNextBallCoord(iNext).y, GetNextBallColor(iNext));
					break;
				}
			}
			if(iFreeCell == iRand)
				break;
		}
	}

	// Узнать, все ли next шарики сгенерированы?
	nNextBallsToGenerate = 0;
	for (INT iNext = 0 ; iNext < m_byNextBalls ; iNext++)
		if(GetNextBallCoord(iNext) == CPoint(-1, -1) || GetNextBallColor(iNext) == -1)
			nNextBallsToGenerate++;

	// Вывод:
	if(nNextBallsToGenerate == 0)
	{
		TRACE(_T("OK!\n"));
		return TRUE;
	}
	else
	{
		TRACE(_T("ERROR! %d balls were not generated!\n"), nNextBallsToGenerate);
		return FALSE;
	}
}


//////////////////////////////////////////////// Заполнить NEXT ////////////////////////////////////////////////

BOOL Field::ConvertNextBallsToCurrent(CArray<CPoint, CPoint>& arrAppearence/*, CArray<CPoint, CPoint>& arrInvalidate*/)
{
	TRACE(_T("Birth of new balls (from next balls):\n"));
	BOOL bIsAllRight = TRUE;

	for (INT iNext = 0 ; iNext < m_byNextBalls ; iNext++)
	{
		TRACE(_T("%d. "), iNext);

		// Проверка...
		if(GetNextBallCoord(iNext) == CPoint(-1, -1))
		{
			TRACE(_T("ERROR! no coordinates!\n"));
			bIsAllRight = FALSE;
			continue;
		}
		else
			TRACE(_T("(%d, %d) ... "), GetNextBallCoord(iNext).x, GetNextBallCoord(iNext).y);

		if(GetNextBallColor(iNext) == -1)
		{
			TRACE(_T("ERROR! no color!\n"));
			bIsAllRight = FALSE;
			continue;
		}
		if(GetNextBallColor(iNext) != GetCellColor(GetNextBallCoord(iNext)))
		{
			TRACE(_T("ERROR! colors are not equal!\n"));
			bIsAllRight = FALSE;
			continue;
		}

		// Установка State & Color ячейки
		SetCellState(GetNextBallCoord(iNext), Cell::CellState::BUSY);
		SetCellColor(GetNextBallCoord(iNext), GetNextBallColor(iNext));
		
		// Пометить ячейку для АНИМАЦИИ РОСТА
		arrAppearence.Add(GetNextBallCoord(iNext));

		// Стирание State & Color next шарика
		SetNextBallCoord(iNext, CPoint(-1, -1));
		SetNextBallColor(iNext, -1);

		TRACE(_T("OK.\n"));
	}

	return bIsAllRight;
}


///////////////////////////////////////////////// Перемещение ///////////////////////////////////////////////////

BOOL Field::Move(UINT uPrevX, UINT uPrevY, UINT uNextX, UINT uNextY, CArray<CPoint, CPoint>& arrInvalidate, BOOL bInvalidate/* = TRUE*/)
{
	TRACE(_T("Moving the ball: (%d, %d) -> (%d, %d)\n"), uPrevX, uPrevY, uNextX, uNextY);

	BOOL bMustReGenerate = FALSE;
	if(GetCellState(uNextX, uNextY) == Cell::CellState::NEXT)
	{
		TRACE(_T("Next ball (%d, %d) must be regenerated!\n"), uNextX, uNextY);
		bMustReGenerate = TRUE;
	}

	// Установка State & Color ячейки #2
	SetCellState(uNextX, uNextY, Cell::CellState::BUSY);
	SetCellColor(uNextX, uNextY, GetCellColor(uPrevX, uPrevY));

	// Стирание State & Color ячейки #1
	SetCellState(uPrevX, uPrevY, Cell::CellState::FREE);
	SetCellColor(uPrevX, uPrevY, -1);

	if(bMustReGenerate)
	{
		for (INT iNext = 0 ; iNext < m_byNextBalls ; iNext++)
			if(GetNextBallCoord(iNext) == CPoint(uNextX, uNextY))
			{
				// Стирание State next шарика, который был на месте ячейки #2
				SetNextBallCoord(iNext, CPoint(-1, -1));

				// Color НЕ СТИРАТЬ !!!
				//SetNextBallColor(iNext, -1);

				break;
			}
		
		PrepareNextBalls(arrInvalidate, bInvalidate);

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

///////////////////////////////////////////////// Уничтожение ///////////////////////////////////////////////////

void Field::Destroy(CArray<CPoint, CPoint>& arrDestroyBalls)
{
	for(INT i = 0 ; i < arrDestroyBalls.GetSize() ; i++)
	{
		SetCellState(arrDestroyBalls[i], Cell::CellState::FREE);
		SetCellColor(arrDestroyBalls[i], -1);

		TRACE(_T("Destroying the ball (%d, %d)\n"), arrDestroyBalls[i].x, arrDestroyBalls[i].y);
	}
}

///////////////////////////////////////// Поик пути перемещения /////////////////////////////////////////////////

BOOL Field::FindWay(CArray<CPoint, CPoint>& arrMoveBalls, UINT uPrevX, UINT uPrevY, UINT uNextX, UINT uNextY)
{
	TRACE(_T("Finding a way to move ball (%d, %d) -> (%d, %d):\n"), uPrevX, uPrevY, uNextX, uNextY);

	INT F = 9 * 9;
	INT Ni = 0;

	INT **ppWork = new INT*[9];
	for(INT i = 0 ; i < 9 ; i++)
		ppWork[i] = new INT[9];
	
	for(INT i = 0 ; i < 9 ; i++)
		for(INT j = 0 ; j < 9 ; j++)
			if(i == uNextX && j == uNextY)
				ppWork[i][j] = 0;
			else if(i == uPrevX && j == uPrevY)
				ppWork[i][j] = F;
			else if(GetCellState(i, j) != Cell::CellState::BUSY)
				ppWork[i][j] = F + 1;
			else
				ppWork[i][j] = F + 2;

	BOOL bGotoPathGen = FALSE;
	while(TRUE)
	{
		for(INT i = 0 ; i < 9 ; i++)
		{
			for(INT j = 0 ; j < 9 ; j++)
			{
				if(ppWork[i][j] == Ni)
				{
					if(j < 8 && ppWork[i][j+1] == F)
					{
						bGotoPathGen = TRUE; break;
					}
					else if(j < 8 && ppWork[i][j+1] == F + 1)
					{
						ppWork[i][j+1] = Ni + 1;
					}

					if(j > 0 && ppWork[i][j-1] == F)
					{
						bGotoPathGen = TRUE; break;
					}
					else if(j > 0 && ppWork[i][j-1] == F + 1)
					{
						ppWork[i][j-1] = Ni + 1;
					}

					if(i < 8 && ppWork[i+1][j] == F)
					{
						bGotoPathGen = TRUE; break;
					}
					else if(i < 8 && ppWork[i+1][j] == F + 1)
					{
						ppWork[i+1][j] = Ni + 1;
					}

					if(i > 0 && ppWork[i-1][j] == F)
					{
						bGotoPathGen = TRUE; break;
					}
					else if(i > 0 && ppWork[i-1][j] == F + 1)
					{
						ppWork[i-1][j] = Ni + 1;
					}
				}
			}

			if(bGotoPathGen) break;
		}

		if(bGotoPathGen) break;

		Ni++;

		if(Ni > F)
		{
			TRACE(_T("Moving impossible!\n"));

			for(INT i = 0 ; i < 9 ; i++)
				delete [] ppWork[i];
			delete [] ppWork;
			
			return FALSE;
		}
	}

	// Построение мaршрутa перемещения
	INT iX = uPrevX, iY = uPrevY;
	INT iX_, iY_;
	
	while(TRUE)
	{
		if(iX > 0) //если шарик НЕ на левой границе
		{
			if(iX < 8) //если шарик НЕ на правой границе
			{
				if(ppWork[iX - 1][iY] < ppWork[iX + 1][iY])
				{
					iX_ = iX - 1;
					iY_ = iY;
				}
				else
				{
					iX_ = iX + 1;
					iY_ = iY;
				}
			}
			else //если шарик на правой границе
			{
				iX_ = iX - 1;
				iY_ = iY;
			}
		}
		else //если шарик на левой границе
		{
			iX_ = iX + 1;
			iY_ = iY;
		}

		if(iY < 8 && ppWork[iX][iY + 1] < ppWork[iX_][iY_])
		{
			iX_ = iX;
			iY_ = iY + 1;
		}

		if(iY > 0 && ppWork[iX][iY - 1] < ppWork[iX_][iY_])
		{
			iX_ = iX;
			iY_ = iY - 1;
		}

		// вот оно!
		arrMoveBalls.Add(CPoint(iX_, iY_));
		TRACE(_T(" -> (%d, %d)"), iX_, iY_);
		
		if(ppWork[iX_][iY_] == 0)
			break;

		iX = iX_;
		iY = iY_;
	}

	TRACE(_T(" -> FINISH!\n"));

	for(INT i = 0 ; i < 9 ; i++)
		delete [] ppWork[i];
	delete [] ppWork;

	return TRUE;
}

/////////////////////////////////////// Поиск сложенных шаров /////////////////////////////////////////////////

BOOL Field::FindDestroy(CArray<CPoint, CPoint>& arrDestroyBalls)
{
	INT iCount = 0;
	INT iStart = -1;

	// По вертикали:
	for(INT i = 0 ; i < 9 ; i++)
	{
		iStart = -1;
		iCount = 0;

		for(INT j = 0 ; j < 9 ; j++)
		{
			if(GetCellState(i, j) == Cell::CellState::BUSY)
			{
				if(iStart == -1)
				{
					iStart = j;
					iCount = 1;
				}
				else
				{
					if(GetCellColor(i, j) == GetCellColor(i, j - 1))
					{
						iCount++;
					}
					else
					{
						if(iCount >= 5)
							for(INT jj = iStart ; jj < iStart + iCount ; jj++)
								arrDestroyBalls.Add(CPoint(i, jj));

						iStart = j;
						iCount = 1;
					}
				}
			}
			else 
			{
				if (iStart != -1)
				{
					if(iCount >= 5)
						for(INT jj = iStart ; jj < iStart + iCount ; jj++)
							arrDestroyBalls.Add(CPoint(i, jj));

					iStart = -1;
					iCount = 0;
				}
			}
		}

		if(iCount >= 5)
			for(INT jj = iStart ; jj < iStart + iCount ; jj++)
				arrDestroyBalls.Add(CPoint(i, jj));
	}


	// По горизонтали:
	for(INT j = 0 ; j < 9 ; j++)
	{
		iStart = -1;
		iCount = 0;

		for(INT i = 0 ; i < 9 ; i++)
		{
			if(GetCellState(i, j) == Cell::CellState::BUSY)
			{
				if(iStart == -1)
				{
					iStart = i;
					iCount = 1;
				}
				else
				{
					if(GetCellColor(i, j) == GetCellColor(i - 1, j))
					{
						iCount++;
					}
					else
					{
						if(iCount >= 5)
							for(INT ii = iStart ; ii < iStart + iCount ; ii++)
								arrDestroyBalls.Add(CPoint(ii, j));

						iStart = i;
						iCount = 1;
					}
				}
			}
			else 
			{
				if (iStart != -1)
				{
					if(iCount >= 5)
						for(INT ii = iStart ; ii < iStart + iCount ; ii++)
							arrDestroyBalls.Add(CPoint(ii, j));

					iStart = -1;
					iCount = 0;
				}
			}
		}

		if(iCount >= 5)
			for(INT ii = iStart ; ii < iStart + iCount ; ii++)
				arrDestroyBalls.Add(CPoint(ii, j));
	}
	

	// По диагонали:

	// 1. ///\\\ (правая сторона \\\)
	for(INT x = 0 ; x < 9 ; x++)
	{
		iStart = -1;
		iCount = 0;

		for(INT i = x ; i < 9 ; i++)
		{
			if(GetCellState(i, i - x) == Cell::CellState::BUSY)
			{
				if(iStart == -1)
				{
					iStart = i;
					iCount = 1;
				}
				else
				{
					if(GetCellColor(i, i - x) == GetCellColor(i - 1, (i - 1) - x))
					{
						iCount++;
					}
					else
					{
						if(iCount >= 5)
							for(INT ii = iStart ; ii < iStart + iCount ; ii++)
								arrDestroyBalls.Add(CPoint(ii, ii - x));

						iStart = i;
						iCount = 1;
					}
				}
			}
			else 
			{
				if (iStart != -1)
				{
					if(iCount >= 5)
						for(INT ii = iStart ; ii < iStart + iCount ; ii++)
							arrDestroyBalls.Add(CPoint(ii, ii - x));

					iStart = -1;
					iCount = 0;
				}
			}
		}

		if(iCount >= 5)
			for(INT ii = iStart ; ii < iStart + iCount ; ii++)
				arrDestroyBalls.Add(CPoint(ii, ii - x));
	}

	// 1. ///\\\ (левая сторона ///)
	for(INT x = 0 ; x < 9 ; x++)
	{
		iStart = -1;
		iCount = 0;

		for(INT i = x ; i >= 0 ; i--)
		{
			if(GetCellState(i, x - i) == Cell::CellState::BUSY)
			{
				if(iStart == -1)
				{
					iStart = i;
					iCount = 1;
				}
				else
				{
					if(GetCellColor(i, x - i) == GetCellColor(i + 1, x - (i + 1)))
					{
						iCount++;
					}
					else
					{
						if(iCount >= 5)
							for(INT ii = iStart ; ii > iStart - iCount ; ii--)
								arrDestroyBalls.Add(CPoint(ii, x - ii));

						iStart = i;
						iCount = 1;
					}
				}
			}
			else 
			{
				if (iStart != -1)
				{
					if(iCount >= 5)
						for(INT ii = iStart ; ii > iStart - iCount ; ii--)
							arrDestroyBalls.Add(CPoint(ii, x - ii));

					iStart = -1;
					iCount = 0;
				}
			}
		}

		if(iCount >= 5)
			for(INT ii = iStart ; ii > iStart - iCount ; ii--)
				arrDestroyBalls.Add(CPoint(ii, x - ii));
	}

	// 2. \\\/// (правая сторона ///)
	for(INT x = 0 ; x < 9 ; x++)
	{
		iStart = -1;
		iCount = 0;

		for(INT i = x ; i < 9 ; i++)
		{
			if(GetCellState(i, 8 - (i - x)) == Cell::CellState::BUSY)
			{
				if(iStart == -1)
				{
					iStart = i;
					iCount = 1;
				}
				else
				{
					if(GetCellColor(i, 8 - (i - x)) == GetCellColor(i - 1, 8 - ((i - 1) - x)))
					{
						iCount++;
					}
					else
					{
						if(iCount >= 5)
							for(INT ii = iStart ; ii < iStart + iCount ; ii++)
								arrDestroyBalls.Add(CPoint(ii, 8 - (ii - x)));

						iStart = i;
						iCount = 1;
					}
				}
			}
			else 
			{
				if (iStart != -1)
				{
					if(iCount >= 5)
						for(INT ii = iStart ; ii < iStart + iCount ; ii++)
							arrDestroyBalls.Add(CPoint(ii, 8 - (ii - x)));

					iStart = -1;
					iCount = 0;
				}
			}
		}

		if(iCount >= 5)
			for(INT ii = iStart ; ii < iStart + iCount ; ii++)
				arrDestroyBalls.Add(CPoint(ii, 8 - (ii - x)));
	}

	// 2. \\\/// (левая сторона \\\)
	for(INT x = 0 ; x < 9 ; x++)
	{
		iStart = -1;
		iCount = 0;

		for(INT i = x ; i >= 0 ; i--)
		{
			if(GetCellState(i, 8 - (x - i)) == Cell::CellState::BUSY)
			{
				if(iStart == -1)
				{
					iStart = i;
					iCount = 1;
				}
				else
				{
					if(GetCellColor(i, 8 - (x - i)) == GetCellColor(i + 1, 8 - (x - (i + 1))))
					{
						iCount++;
					}
					else
					{
						if(iCount >= 5)
							for(INT ii = iStart ; ii > iStart - iCount ; ii--)
								arrDestroyBalls.Add(CPoint(ii, 8 - (x - ii)));

						iStart = i;
						iCount = 1;
					}
				}
			}
			else 
			{
				if (iStart != -1)
				{
					if(iCount >= 5)
						for(INT ii = iStart ; ii > iStart - iCount ; ii--)
							arrDestroyBalls.Add(CPoint(ii, 8 - (x - ii)));

					iStart = -1;
					iCount = 0;
				}
			}
		}

		if(iCount >= 5)
			for(INT ii = iStart ; ii > iStart - iCount ; ii--)
				arrDestroyBalls.Add(CPoint(ii, 8 - (x - ii)));
	}


	// Удалить одинаковые элементы:
 	for(INT i = 0 ; i < arrDestroyBalls.GetCount() ; i++)
 	{
 		for(INT j = i + 1 ; j < arrDestroyBalls.GetCount() ; j++)
 		{
 			if(arrDestroyBalls[i] == arrDestroyBalls[j])
 			{
 				arrDestroyBalls.RemoveAt(j);
 				j--;
 			}
 		}
 	}

	// Вернуть количество уничтоженных шаров...
	return arrDestroyBalls.GetCount();
}

