
#include "stdafx.h"

#include "RulesDlg.h"

CRulesDlg::CRulesDlg(Font* pFontBroadway, Image** ppImages, UINT uImages, CWnd* pParent /*=NULL*/)
	: CDialog(CRulesDlg::IDD)
	, m_pFontBroadway(pFontBroadway)
	, m_ppImages(ppImages)
	, m_uImages(uImages)
	, sbrush(Color(127, 0, 0, 0))
	, sizeBall(80, 80)
	, m_bPaintFirstTime(TRUE)
{
}

CRulesDlg::~CRulesDlg()
{
}

void CRulesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRulesDlg, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CRulesDlg::OnPaint()
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

		// Настройка текста:
		StringFormat format;
		format.SetAlignment(StringAlignmentCenter);
		format.SetLineAlignment(StringAlignmentCenter);
		graphics.SetTextRenderingHint(TextRenderingHint::TextRenderingHintAntiAlias);

		ptBall = Point(12, 8);

		// Прорисовка каринок и буковок:
		graphics.DrawImage(m_pImagesRULES[0], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("R"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush);
		ptBall = ptBall + Size(sizeBall.Width - 10, +40);
		graphics.DrawImage(m_pImagesRULES[1], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("U"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush);
		ptBall = ptBall + Size(sizeBall.Width - 10, -40);
		graphics.DrawImage(m_pImagesRULES[2], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("L"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush);
		ptBall = ptBall + Size(sizeBall.Width - 10, +40);
		graphics.DrawImage(m_pImagesRULES[3], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("E"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush);
		ptBall = ptBall + Size(sizeBall.Width - 10, -40);
		graphics.DrawImage(m_pImagesRULES[4], ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height);
		graphics.DrawString(_T("S"), 1, m_pFontBroadway, RectF(ptBall.X, ptBall.Y, sizeBall.Width, sizeBall.Height), &format, &sbrush);

		m_bPaintFirstTime = FALSE;
	}

	paintDC.BitBlt(0, 0, m_windowRect.Width(), m_windowRect.Height(), &gdiplusDC, 0, 0, SRCCOPY);
}


BOOL CRulesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// RAND:
	srand(time(NULL));
	for(int i = 0 ; i < 5 ; i++)
		m_pImagesRULES[i] = m_ppImages[rand() % m_uImages];

	GetWindowRect(m_windowRect);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

