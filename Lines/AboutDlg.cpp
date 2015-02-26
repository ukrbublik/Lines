// AboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Lines.h"
#include "AboutDlg.h"


// CAboutDlg dialog

IMPLEMENT_DYNAMIC(CAboutDlg, CDialog)

CAboutDlg::CAboutDlg(Font* pFontBroadway, Font* pFontBroadway_small, Image** ppImages, UINT uImages, CString strSkinCopyrights, CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDlg::IDD, pParent)
	, m_pFontBroadway(pFontBroadway)
	, m_pFontBroadway_small(pFontBroadway_small)
	, m_ppImages(ppImages)
	, m_uImages(uImages)
	, sizeBall(80, 80)
	, sbrush(Color(127, 0, 0, 0))
	, m_strSkinCopyrights(strSkinCopyrights)
	, m_bPaintFirstTime(TRUE)
{

}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_SKIN_COPYRIGHT, m_strSkinCopyrights);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CAboutDlg message handlers

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// rand:
	srand(time(NULL));
	for(int i = 0 ; i < 5 ; i++)
	{
		m_pImagesLINES[i] = m_ppImages[rand() % m_uImages];
	}
	for(int j = 0 ; j < 6 ; j++)
	{
		m_pImagesBUBLIK[j] = m_ppImages[rand() % m_uImages];
	}

	GetWindowRect(m_windowRect);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnPaint()
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
		
		ptBall = Point(46, 15);
		
		// Фон:
		DWORD dwColorWinBk = GetSysColor(COLOR_3DFACE);
		Color colorWinBk(GetRValue(dwColorWinBk), GetGValue(dwColorWinBk), GetBValue(dwColorWinBk));
		SolidBrush sbr(colorWinBk);
		graphics.FillRectangle(&sbr, 0, 0, m_windowRect.Width(), m_windowRect.Height());
		
		// Настройка текста:
		StringFormat format;
		format.SetAlignment(StringAlignmentCenter);
		format.SetLineAlignment(StringAlignmentCenter);
		graphics.SetTextRenderingHint(TextRenderingHint::TextRenderingHintAntiAlias);
		
		// Прорисовка каринок и буковок слова LINES:
		graphics.DrawImage(m_pImagesLINES[0], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("L"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush);
		ptBall = ptBall + Size(sizeBall.Width - 7, +20);
		graphics.DrawImage(m_pImagesLINES[1], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("i"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush);
		ptBall = ptBall + Size(sizeBall.Width - 5, +15);
		graphics.DrawImage(m_pImagesLINES[2], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("N"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush);
		ptBall = ptBall + Size(sizeBall.Width - 5, -15);
		graphics.DrawImage(m_pImagesLINES[3], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("E"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush);
		ptBall = ptBall + Size(sizeBall.Width - 7, -20);
		graphics.DrawImage(m_pImagesLINES[4], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("S"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush);

		// by
		graphics.DrawString(_T("by"), 2, m_pFontBroadway_small, RectF(0, 90, 7 + 80 * 6 - 7 * 2 - 5 * 2 - 3, 110), &format, &sbrush);

		// Прорисовка каринок и буковок слова BUBLIK:
		ptBall = Point(7, 200);
		graphics.DrawImage(m_pImagesBUBLIK[0], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("B"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush);
		ptBall = ptBall + Size(sizeBall.Width - 7, -25);
		graphics.DrawImage(m_pImagesBUBLIK[1], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("U"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush);
		ptBall = ptBall + Size(sizeBall.Width - 5, -20);
		graphics.DrawImage(m_pImagesBUBLIK[2], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("B"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush);
		ptBall = ptBall + Size(sizeBall.Width - 3 , 0);
		graphics.DrawImage(m_pImagesBUBLIK[3], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("L"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush);
		ptBall = ptBall + Size(sizeBall.Width - 5, +20);
		graphics.DrawImage(m_pImagesBUBLIK[4], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("i"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush);
		ptBall = ptBall + Size(sizeBall.Width - 7, +25);
		graphics.DrawImage(m_pImagesBUBLIK[5], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("K"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush);

		m_bPaintFirstTime = FALSE;
	}

	paintDC.BitBlt(0, 0, m_windowRect.Width(), m_windowRect.Height(), &gdiplusDC, 0, 0, SRCCOPY);
}

void CAboutDlg::OnDestroy()
{
	CDialog::OnDestroy();
}