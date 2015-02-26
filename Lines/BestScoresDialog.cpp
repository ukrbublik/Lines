// BestScoresDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Lines.h"
#include "BestScoresDialog.h"


// CBestScoresDialog dialog

IMPLEMENT_DYNAMIC(CBestScoresDialog, CDialog)

CBestScoresDialog::CBestScoresDialog(CArray<Result, Result>* pArrBestScores, UINT uMax, Font *pfontBroadway, Font *pfontMonotypeCorsiva, Image** ppImages, UINT uImages, CWnd* pParent/* = NULL*/)
	: CDialog(CBestScoresDialog::IDD, pParent)
	, m_pArrBestScores(pArrBestScores)
	, m_uMax(uMax)
	, m_pFontBroadway(pfontBroadway)
	, m_pfontMonotypeCorsiva(pfontMonotypeCorsiva)
	, m_ppImages(ppImages)
	, m_uImages(uImages)
	, pen(Color(127, 0, 0, 0), 2)
	, sbrush(Color(191, 0, 0, 0))
	, sbrush_balls(Color(127, 0, 0, 0))
	, rectbrush(Color(63, 127, 127, 127))
	, sizeBall(60, 60)
	, m_bPaintFirstTime(TRUE)
{
	uHeigth_All = 25;
	uWidth_Number = 30;
	uWidth_Name = 300;
	uWidth_Time = 85;
	uWidth_Scores = 90;
	iRadius = 5;

	for(int i = 0 ; i < 4 ; i++)
		pathes[i] = NULL;

	for(int i = 0 ; i < 4 ; i++)
		frects[i] = NULL;
}

CBestScoresDialog::~CBestScoresDialog()
{
}

void CBestScoresDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBestScoresDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CBestScoresDialog message handlers

BOOL CBestScoresDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetWindowRect(m_windowRect);

	// rand:
	srand(time(NULL));
	for(int i = 0 ; i < 4 ; i++)
	{
		m_pImagesHIGH[i] = m_ppImages[rand() % m_uImages];
	}
	for(int i = 0 ; i < 6 ; i++)
	{
		m_pImagesSCORES[i] = m_ppImages[rand() % m_uImages];
	}

	// Настройка текста:
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);

	// Строки для таблицы:
	CString strTemp;
 	for(int i = 0 ; i < m_pArrBestScores->GetCount() ; i++)
	{
		m_arrTimes.Add((*m_pArrBestScores)[i].time.Format(_T("%H:%M:%S")));
		strTemp.Format(_T("%d"), (*m_pArrBestScores)[i].scores);
		m_arrScores.Add(strTemp);
	}


	// Таблица с закруглёнными краями:
	for(int i = 0 ; i < 4 ; i++)
	{
		pathes[i] = new GraphicsPath[m_uMax + 1];
		frects[i] = new RectF[m_uMax + 1];
	}

	Rect rect;
	Point pt = Point(14, 110);
	Size sizeBetweenRects = Size(5, 5);

	for(int i = 0 ; i <= m_uMax ; i++)
	{
		rect.X = pt.X;
		rect.Y = pt.Y;
		rect.Height = uHeigth_All;
		rect.Offset ( 0, i * ( uHeigth_All + sizeBetweenRects.Height ) );

 		rect.Width = uWidth_Number;
 		frects[0][i] = RectF(rect.X, rect.Y, rect.Width, rect.Height + 5);
 		CreateRoundRectangle(&pathes[0][i], rect, iRadius);
 
		rect.Width = uWidth_Name;
		rect.Offset(uWidth_Number + sizeBetweenRects.Width, 0);
		frects[1][i] = RectF(rect.X, rect.Y, rect.Width, rect.Height + 5);
		CreateRoundRectangle(&pathes[1][i], rect, iRadius);

		rect.Width = uWidth_Time;
		rect.Offset(uWidth_Name + sizeBetweenRects.Width, 0);
		frects[2][i] = RectF(rect.X, rect.Y, rect.Width, rect.Height + 5);
		CreateRoundRectangle(&pathes[2][i], rect, iRadius);

		rect.Width = uWidth_Scores;
		rect.Offset(uWidth_Time + sizeBetweenRects.Width, 0);
		frects[3][i] = RectF(rect.X, rect.Y, rect.Width, rect.Height + 5);
		CreateRoundRectangle(&pathes[3][i], rect, iRadius);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CBestScoresDialog::OnPaint()
{
	CPaintDC paintDC(this);

	if(m_bPaintFirstTime)
	{
		HDC gdiplusHDC = CreateCompatibleDC(paintDC.m_hDC);
		gdiplusDC.Attach(gdiplusHDC);

		CBitmap gdiplusBMP;
		gdiplusBMP.CreateCompatibleBitmap(&paintDC, m_windowRect.Width(), m_windowRect.Height());
		gdiplusDC.SelectObject(&gdiplusBMP);

		Graphics graphics(gdiplusHDC);

		// Фон:
		DWORD dwColorWinBk = GetSysColor(COLOR_3DFACE);
		Color colorWinBk(GetRValue(dwColorWinBk), GetGValue(dwColorWinBk), GetBValue(dwColorWinBk));
		SolidBrush sbr(colorWinBk);
		graphics.FillRectangle(&sbr, 0, 0, m_windowRect.Width(), m_windowRect.Height());

		graphics.SetSmoothingMode(SmoothingModeAntiAlias);
		graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
		graphics.SetTextRenderingHint(TextRenderingHint::TextRenderingHintAntiAlias);

		// Прорисовка каринок и буковок:
		Point ptBall = Point(5, 8);

		graphics.DrawImage(m_pImagesHIGH[0], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("H"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush_balls);
		ptBall = ptBall + Size(sizeBall.Width - 8, +25);
		graphics.DrawImage(m_pImagesHIGH[1], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("I"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush_balls);
		ptBall = ptBall + Size(sizeBall.Width - 8, -25);
		graphics.DrawImage(m_pImagesHIGH[2], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("G"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush_balls);
		ptBall = ptBall + Size(sizeBall.Width - 8, +25);
		graphics.DrawImage(m_pImagesHIGH[3], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("H"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush_balls);

		ptBall = Point(224, 8);
		graphics.DrawImage(m_pImagesSCORES[0], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("S"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush_balls);
		ptBall = ptBall + Size(sizeBall.Width - 8, +25);
		graphics.DrawImage(m_pImagesSCORES[1], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("C"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush_balls);
		ptBall = ptBall + Size(sizeBall.Width - 8, -25);
		graphics.DrawImage(m_pImagesSCORES[2], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("O"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush_balls);
		ptBall = ptBall + Size(sizeBall.Width - 8, +25);
		graphics.DrawImage(m_pImagesSCORES[3], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("R"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush_balls);
		ptBall = ptBall + Size(sizeBall.Width - 8, -25);
		graphics.DrawImage(m_pImagesSCORES[4], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("E"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush_balls);
		ptBall = ptBall + Size(sizeBall.Width - 8, +25);
		graphics.DrawImage(m_pImagesSCORES[5], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("S"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush_balls);



		// Таблица:
		CString strTemp;
 		for(int i = 0 ; i <= m_uMax ; i++)
 		{
 			graphics.DrawPath(&pen, &pathes[0][i]);
			if(!i)
			{
				graphics.FillPath(&rectbrush, &pathes[0][i]);
				graphics.DrawString(_T("#"), CString(_T("#")).GetLength(), m_pfontMonotypeCorsiva, frects[0][i], &format, &sbrush);
			}
			else
			{
				strTemp.Format(_T("%d"), i);
				graphics.DrawString(strTemp, strTemp.GetLength(), m_pfontMonotypeCorsiva, frects[0][i], &format, &sbrush);
			}

			graphics.DrawPath(&pen, &pathes[1][i]);
			if(!i)
			{
				graphics.FillPath(&rectbrush, &pathes[1][i]);
				graphics.DrawString(_T("Имя игрока"), CString(_T("Имя игрока")).GetLength(), m_pfontMonotypeCorsiva, frects[1][i], &format, &sbrush);
			}
			else if(m_arrScores.GetCount() >= i)
			{
				graphics.DrawString((*m_pArrBestScores)[i-1].name, (*m_pArrBestScores)[i-1].name.GetLength(), m_pfontMonotypeCorsiva, frects[1][i], &format, &sbrush);
			}

			graphics.DrawPath(&pen, &pathes[2][i]);
			if(!i)
			{
				graphics.FillPath(&rectbrush, &pathes[2][i]);
				graphics.DrawString(_T("Время"), CString(_T("Время")).GetLength(), m_pfontMonotypeCorsiva, frects[2][i], &format, &sbrush);
			}
			else if(m_arrTimes.GetCount() >= i)
			{
				graphics.DrawString(m_arrTimes[i-1], m_arrTimes[i-1].GetLength(), m_pfontMonotypeCorsiva, frects[2][i], &format, &sbrush);
			}

			graphics.DrawPath(&pen, &pathes[3][i]);
			if(!i)
			{
				graphics.FillPath(&rectbrush, &pathes[3][i]);
				graphics.DrawString(_T("Очки"), CString(_T("Очки")).GetLength(), m_pfontMonotypeCorsiva, frects[3][i], &format, &sbrush);
			}
			else if(m_arrScores.GetCount() >= i)
			{
				graphics.DrawString(m_arrScores[i-1], m_arrScores[i-1].GetLength(), m_pfontMonotypeCorsiva, frects[3][i], &format, &sbrush);
			}
		}

		m_bPaintFirstTime = FALSE;
	}
	
	paintDC.BitBlt(0, 0, m_windowRect.Width(), m_windowRect.Height(), &gdiplusDC, 0, 0, SRCCOPY);
}

void CBestScoresDialog::CreateRoundRectangle(GraphicsPath* ppath, Rect rectangle, int radius)
{
	int l = rectangle.GetLeft();
	int t = rectangle.GetTop();
	int w = rectangle.Width;
	int h = rectangle.Height;
	int d = radius << 1;
	ppath->Reset();
	ppath->AddArc(l, t, d, d, 180, 90);
	ppath->AddLine(l + radius, t, l + w - radius, t);
	ppath->AddArc(l + w - d, t, d, d, 270, 90);
	ppath->AddLine(l + w, t + radius, l + w, t + h - radius);
	ppath->AddArc(l + w - d, t + h - d, d, d, 0, 90);
	ppath->AddLine(l + w - radius, t + h, l + radius, t + h);
	ppath->AddArc(l, t + h - d, d, d, 90, 90);
	ppath->AddLine(l, t + h - radius, l, t + radius);
	ppath->CloseFigure();
}

void CBestScoresDialog::OnDestroy()
{
	for(int i = 0 ; i < 4 ; i++)
	{
		if(pathes[i])
			delete [] pathes[i];
	}

	for(int i = 0 ; i < 4 ; i++)
	{
		if(frects[i])
			delete [] frects[i];
	}

	CDialog::OnDestroy();
}
