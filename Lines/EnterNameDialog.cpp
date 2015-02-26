// EnterNameDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Lines.h"
#include "EnterNameDialog.h"


// CEnterNameDialog dialog

IMPLEMENT_DYNAMIC(CEnterNameDialog, CDialog)

CEnterNameDialog::CEnterNameDialog(CWnd* pParent /*=NULL*/, UINT uLimit/* = 20*/)
	: CDialog(CEnterNameDialog::IDD, pParent)
	, m_uLimitTextName(uLimit)
	, m_strName(_T(""))
	, m_strCongratulations(_T("Поздравляю!\nВы набрали xxx очков за yyy\nи попали в таблицу рекордов под #zzz!\n\nВведите своё имя:"))
{
}

CEnterNameDialog::~CEnterNameDialog()
{
}

void CEnterNameDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NAME, m_editName);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDX_Text(pDX, IDC_STATIC_CONGRATULATIONS, m_strCongratulations);
}


BEGIN_MESSAGE_MAP(CEnterNameDialog, CDialog)
END_MESSAGE_MAP()


// CEnterNameDialog message handlers

BOOL CEnterNameDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_editName.SetLimitText(m_uLimitTextName);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
