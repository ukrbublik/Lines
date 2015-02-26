#pragma once
#include "afxwin.h"


// CEnterNameDialog dialog

class CEnterNameDialog : public CDialog
{
	DECLARE_DYNAMIC(CEnterNameDialog)

public:
	CEnterNameDialog(CWnd* pParent = NULL, UINT uLimit = 20);   // standard constructor
	virtual ~CEnterNameDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_ENTER_NAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editName;
	CString m_strName;
	UINT m_uLimitTextName;
	virtual BOOL OnInitDialog();
	CString m_strCongratulations;
};
