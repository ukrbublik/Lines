#ifndef __RULES__
#define __RULES__

#include "resource.h"

class CRulesDlg : public CDialog
{
public:
	CRulesDlg(Font* pFontBroadway, Image** ppImages, UINT uImages, CWnd* pParent = NULL);
	~CRulesDlg();

	// Dialog Data
	enum { IDD = IDD_RULESBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
public:
	Font* m_pFontBroadway;
	Image** m_ppImages;
	UINT m_uImages;
	Image* m_pImagesRULES[5];

	Point ptBall;
	Size sizeBall;
	SolidBrush sbrush;

	CRect m_windowRect;
	BOOL m_bPaintFirstTime;
	CDC gdiplusDC;
};

#endif //__RULES__
