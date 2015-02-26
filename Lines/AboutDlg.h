#pragma once


// CAboutDlg dialog

class CMainDlg;

class CAboutDlg : public CDialog
{
	DECLARE_DYNAMIC(CAboutDlg)

public:
	CAboutDlg(Font* pFontBroadway, Font* pFontBroadway_small, Image** ppImages, UINT uImages, CString strSkinCopyrights, CWnd* pParent = NULL);   // standard constructor
	virtual ~CAboutDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ABOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();

public:
	Size sizeBall;
	Point ptBall;
	SolidBrush sbrush;

	Font* m_pFontBroadway;
	Font* m_pFontBroadway_small;
	Image** m_ppImages;
	UINT m_uImages;

	Image* m_pImagesLINES[5];
	Image* m_pImagesBUBLIK[6];
public:
	CString m_strSkinCopyrights;

	CRect m_windowRect;
	BOOL m_bPaintFirstTime;
	CDC gdiplusDC;
};
