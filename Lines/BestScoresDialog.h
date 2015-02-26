#pragma once

struct Result
{
	CString name;
	CTimeSpan time;
	UINT scores;
};

// CBestScoresDialog dialog

class CBestScoresDialog : public CDialog
{
	DECLARE_DYNAMIC(CBestScoresDialog)

public:
	CBestScoresDialog(CArray<Result, Result>* pArrBestScores, UINT uMax, Font *pfontMonotypeBroadway, Font *pfontMonotypeCorsiva, Image** ppImages, UINT uImages, CWnd* pParent = NULL);   // standard constructor
	virtual ~CBestScoresDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_BEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CArray<Result, Result>* m_pArrBestScores;
	UINT m_uMax;
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();

private:
	void CreateRoundRectangle(GraphicsPath* ppath, Rect rectangle, int radius);

public:
	Font* m_pfontMonotypeCorsiva;
	Font* m_pFontBroadway;
	Image** m_ppImages;
	UINT m_uImages;

	Image* m_pImagesHIGH[4];
	Image* m_pImagesSCORES[6];
	GraphicsPath *pathes[4];
	RectF *frects[4];

	CStringArray m_arrTimes;
	CStringArray m_arrScores;

	StringFormat format;
	SolidBrush sbrush_balls;

	Pen pen;
	SolidBrush rectbrush;  
	INT iRadius;

	UINT uHeigth_All;
	UINT uWidth_Number;
	UINT uWidth_Name;
	UINT uWidth_Time;
	UINT uWidth_Scores;
	
	Size sizeBall;
	SolidBrush sbrush;

	CRect m_windowRect;
	BOOL m_bPaintFirstTime;
	CDC gdiplusDC;
};
