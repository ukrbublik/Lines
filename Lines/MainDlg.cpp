// MainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Lines.h"
#include "MainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <math.h>


CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent)
	, m_Field(&m_Skin)
	, m_pBtnStartNewGame(NULL)
	, m_pBtnStopGame(NULL)
	, m_pBtnSaveGameToFile(NULL)
	, m_pBtnLoadGameFromFile(NULL)
	, m_pBtnSettings(NULL)
	, m_pBtnSkins(NULL)
	, m_pBtnTableOfBestResults(NULL)
	, m_pBtnHelp(NULL)
	, m_pBtnAbout(NULL)
	, m_pfontMonotypeCorsiva(NULL)
	, m_pFontBroadway(NULL)
	, m_pFontBroadway_small(NULL)
	, m_pFontBroadway_small_italic(NULL)
	, m_ppImagesBalls(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_uTimerID_Main = 1;
	m_uTimerID_Jump = 2;
	m_uTimerID_Appearence = 3;
	m_uTimerID_Destruction = 4;
	m_uTimerID_Move = 5;

	m_nTimerIDEvent_Main = 0;
	m_nTimerIDEvent_Jump = 0;
	m_nTimerIDEvent_Appearence = 0;
	m_nTimerIDEvent_Destruction = 0;
	m_nTimerIDEvent_Move = 0;

	m_iCurrentFrame_Jump = -1;
	m_iCurrentFrame_Appearence = -1;
	m_iCurrentFrame_Destruction = -1;

	m_ptSelStart = CPoint(-1, -1);
	m_ptMoving = CPoint(-1, -1);

	m_nScores = 0;
	m_nHighScores = 0;

	m_Time = CTimeSpan(0, 0, 0, 0);

	bPrepareNextAfterDestroy = FALSE;

	bTimerStopped = FALSE;
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
	ON_WM_NCLBUTTONDOWN()
	ON_BN_CLICKED(IDC_BTN_START, OnBnClicked_Start)
	ON_BN_CLICKED(IDC_BTN_STOP, OnBnClicked_Stop)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBnClicked_Save)
	ON_BN_CLICKED(IDC_BTN_LOAD, OnBnClicked_Load)
	ON_BN_CLICKED(IDC_BTN_SETTINGS, OnBnClicked_Settings)
	ON_BN_CLICKED(IDC_BTN_SKINS, OnBnClicked_Skins)
	ON_BN_CLICKED(IDC_BTN_BEST, OnBnClicked_Best)
	ON_BN_CLICKED(IDC_BTN_HELP, OnBnClicked_Help)
	ON_BN_CLICKED(IDC_BTN_ABOUT, OnBnClicked_About)
	ON_WM_SIZE()
END_MESSAGE_MAP()


BOOL CMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);	
	SetIcon(m_hIcon, FALSE);


	if(InitializeSkinValues(FALSE) && LoadImages(FALSE))
	{
		// Skin loaded success!
	}
	else
	{
		if(AfxMessageBox(_T("Не удалось загрузить скин!\nВыберите skin-файл и загрузите его, иначе игра будет закрыта!"), MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK)
		{
			if(!LoadSkin(TRUE))
			{
				EndDialog(IDCANCEL);
				return FALSE;
			}
		}
		else
		{
			EndDialog(IDCANCEL);
			return FALSE;
		}
	}

	// Подстроить размеры окна под картинку background
	CRect rectClient, rectWindow;
	GetClientRect(rectClient);
	GetWindowRect(rectWindow);
	rectWindow.right = rectWindow.right - rectClient.Width() + m_Skin.images.imgBackground.GetWidth() + RIGHT_PANEL_WIDTH;
	rectWindow.bottom = rectWindow.bottom - rectClient.Height() + m_Skin.images.imgBackground.GetHeight();
	MoveWindow(rectWindow);

	// ToolTip
	m_tooltip.Create(this);
	m_tooltip.SetDelayTime(PPTOOLTIP_TIME_INITIAL, 1000);
	m_tooltip.SetDelayTime(PPTOOLTIP_TIME_AUTOPOP, 5000);
	m_tooltip.SetDelayTime(PPTOOLTIP_TIME_FADEIN, 25);
	m_tooltip.SetDelayTime(PPTOOLTIP_TIME_FADEOUT, 25);
	m_tooltip.SetBorder(RGB(114, 125, 210));
	m_tooltip.SetColorBk(RGB(255, 255, 255), RGB(193, 200, 236), RGB(114, 125, 207));
	m_tooltip.SetEffectBk( CPPDrawManager::EFFECT_3VGRADIENT );
	m_tooltip.SetTooltipShadow(5, 5, 50, TRUE, 7, 7);
	
	// Создать кнопочки:
	m_pBtnStartNewGame = new CButtonST;
	m_pBtnStopGame = new CButtonST;
	m_pBtnSaveGameToFile = new CButtonST;
	m_pBtnLoadGameFromFile = new CButtonST;
	m_pBtnSettings = new CButtonST;
	m_pBtnSkins = new CButtonST;
	m_pBtnTableOfBestResults = new CButtonST;
	m_pBtnHelp = new CButtonST;
	m_pBtnAbout = new CButtonST;

	GetClientRect(rectClient);
	UINT uBetweenBtns = (rectClient.Height() - 48 * 8) / (2 * 2 + 7);
	CPoint ptBtn = CPoint(rectClient.right - RIGHT_PANEL_WIDTH + (RIGHT_PANEL_WIDTH - 48) / 2, rectClient.top + uBetweenBtns * 2);
	CSize szBtn = CSize(48, 48);

	m_pBtnStartNewGame->Create(NULL, WS_CHILD | WS_VISIBLE, CRect(ptBtn, szBtn), this, IDC_BTN_START);
	m_pBtnStartNewGame->SetIcon(IDR_ICON_START_1, IDR_ICON_START_0);
	m_pBtnStartNewGame->SizeToContent();
	m_pBtnStartNewGame->DrawBorder(FALSE);
	m_tooltip.AddTool(GetDlgItem(IDC_BTN_START), _T("Нажмите эту кнопку, чтобы \nначать новую игру"), LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ICON_START_2)));

	ptBtn.y += (uBetweenBtns + szBtn.cy);
	m_pBtnStopGame->Create(NULL, WS_CHILD | WS_VISIBLE, CRect(ptBtn, szBtn), this, IDC_BTN_STOP);
	m_pBtnStopGame->SetIcon(IDR_ICON_STOP_1, IDR_ICON_STOP_0);
	m_pBtnStopGame->SizeToContent();
	m_pBtnStopGame->DrawBorder(FALSE);
	m_tooltip.AddTool(GetDlgItem(IDC_BTN_STOP), _T("Нажмите эту кнопку, чтобы \nзавершить текущую игру"), LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ICON_STOP_2)));

	ptBtn.y += (uBetweenBtns + szBtn.cy);
	m_pBtnSaveGameToFile->Create(NULL, WS_CHILD | WS_VISIBLE, CRect(ptBtn, szBtn), this, IDC_BTN_SAVE);
	m_pBtnSaveGameToFile->SetIcon(IDR_ICON_SAVE_1, IDR_ICON_SAVE_0);
	m_pBtnSaveGameToFile->SizeToContent();
	m_pBtnSaveGameToFile->DrawBorder(FALSE);
	m_tooltip.AddTool(GetDlgItem(IDC_BTN_SAVE), _T("Нажмите эту кнопку, чтобы \nсохранить игру в файл"), LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ICON_SAVE_2)));

	ptBtn.y += (uBetweenBtns + szBtn.cy);
	m_pBtnLoadGameFromFile->Create(NULL, WS_CHILD | WS_VISIBLE, CRect(ptBtn, szBtn), this, IDC_BTN_LOAD);
	m_pBtnLoadGameFromFile->SetIcon(IDR_ICON_LOAD_1, IDR_ICON_LOAD_0);
	m_pBtnLoadGameFromFile->SizeToContent();
	m_pBtnLoadGameFromFile->DrawBorder(FALSE);
	m_tooltip.AddTool(GetDlgItem(IDC_BTN_LOAD), _T("Нажмите эту кнопку, чтобы \nзагрузить игру из файла"), LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ICON_LOAD_2)));

// 	ptBtn.y += (uBetweenBtns + szBtn.cy);
// 	m_pBtnSettings->Create(NULL, WS_CHILD | WS_VISIBLE, CRect(ptBtn, szBtn), this, IDC_BTN_SETTINGS);
// 	m_pBtnSettings->SetIcon(IDR_ICON_SETTINGS_1, IDR_ICON_SETTINGS_0);
// 	m_pBtnSettings->SizeToContent();
// 	m_pBtnSettings->DrawBorder(FALSE);
//	m_tooltip.AddTool(GetDlgItem(IDC_BTN_SETTINGS), _T("Изменить настройки игры"));

	ptBtn.y += (uBetweenBtns + szBtn.cy);
	m_pBtnSkins->Create(NULL, WS_CHILD | WS_VISIBLE, CRect(ptBtn, szBtn), this, IDC_BTN_SKINS);
	m_pBtnSkins->SetIcon(IDR_ICON_SKINS_1, IDR_ICON_SKINS_0);
	m_pBtnSkins->SizeToContent();
	m_pBtnSkins->DrawBorder(FALSE);
	m_tooltip.AddTool(GetDlgItem(IDC_BTN_SKINS), _T("Нажмите эту кнопку, чтобы \nизменить скин"), LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ICON_SKINS_2)));

	ptBtn.y += (uBetweenBtns + szBtn.cy);
	m_pBtnTableOfBestResults->Create(NULL, WS_CHILD | WS_VISIBLE, CRect(ptBtn, szBtn), this, IDC_BTN_BEST);
	m_pBtnTableOfBestResults->SetIcon(IDR_ICON_BEST_1, IDR_ICON_BEST_0);
	m_pBtnTableOfBestResults->SizeToContent();
	m_pBtnTableOfBestResults->DrawBorder(FALSE);
	m_tooltip.AddTool(GetDlgItem(IDC_BTN_BEST), _T("Нажмите эту кнопку, чтобы \nпосмотреть таблицу рекордов"), LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ICON_BEST_2)));

	ptBtn.y += (uBetweenBtns + szBtn.cy);
	m_pBtnHelp->Create(NULL, WS_CHILD | WS_VISIBLE, CRect(ptBtn, szBtn), this, IDC_BTN_HELP);
	m_pBtnHelp->SetIcon(IDR_ICON_HELP_1, IDR_ICON_HELP_0);
	m_pBtnHelp->SizeToContent();
	m_pBtnHelp->DrawBorder(FALSE);
	m_tooltip.AddTool(GetDlgItem(IDC_BTN_HELP), _T("Нажмите эту кнопку, чтобы \nпрочитать правила игры"), LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ICON_HELP_2)));

	ptBtn.y += (uBetweenBtns + szBtn.cy);
	m_pBtnAbout->Create(NULL, WS_CHILD | WS_VISIBLE, CRect(ptBtn, szBtn), this, IDC_BTN_ABOUT);
	m_pBtnAbout->SetIcon(IDR_ICON_ABOUT_1, IDR_ICON_ABOUT_0);
	m_pBtnAbout->SizeToContent();
	m_pBtnAbout->DrawBorder(FALSE);
	m_tooltip.AddTool(GetDlgItem(IDC_BTN_ABOUT), _T("Нажмите эту кнопку, чтобы \nполучить информацию об игре"), LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ICON_ABOUT_2)));
	
	// Загрузить дополнительные ресурсы:
	LoadFonts();
	LoadImagesBalls();

	// Загрузить таблицу:
	LoadBestResults();

	return TRUE;
}


// Инициализация скина:
BOOL CMainDlg::InitializeSkinValues(BOOL bErrMsg)
{
	CSimpleIni m_ini;
	SI_Error sierr = m_ini.LoadFile(GetProgramDir() + _T("\\CurSkin\\Skin.ini"));
	if(sierr < 0)
	{
		CString strError;
		strError = _T("Произошла ошибка при открытии файла CurSkin\\Skin.ini: ");
		if(sierr == SI_FAIL)
			strError += _T("Generic failure");
		else if (sierr == SI_NOMEM)
			strError += _T("Out of memory error");
		else if(sierr == SI_FILE)
			strError += _T("File error");
		if(bErrMsg)
			AfxMessageBox(strError);
		return FALSE;
	}

	CWordArray arr;

	// [Files]
	m_Skin.files.strBackground	= GetProgramDir() + CString(_T("\\CurSkin\\")) + m_ini.GetValue(_T("Files"), _T("BKG"));
	m_Skin.files.strBalls		= GetProgramDir() + CString(_T("\\CurSkin\\")) + m_ini.GetValue(_T("Files"), _T("BALLS"));
	m_Skin.files.strNext		= GetProgramDir() + CString(_T("\\CurSkin\\")) + m_ini.GetValue(_T("Files"), _T("NEXT"));
	m_Skin.files.strScore		= GetProgramDir() + CString(_T("\\CurSkin\\")) + m_ini.GetValue(_T("Files"), _T("YSCORE"));
	m_Skin.files.strTime		= GetProgramDir() + CString(_T("\\CurSkin\\")) + m_ini.GetValue(_T("Files"), _T("TIME"));
	m_Skin.files.strType		= GetProgramDir() + CString(_T("\\CurSkin\\")) + m_ini.GetValue(_T("Files"), _T("GTYPE"));

	// [SizesOfElements]
	GetMultiIniValues(m_ini, _T("SizesOfElements"), _T("BallSize"), &arr);
	m_Skin.sizesOfElements.sizeBall = CSize(arr[0], arr[1]);
	GetMultiIniValues(m_ini, _T("SizesOfElements"), _T("NextColorSize"), &arr);
	m_Skin.sizesOfElements.sizeNext = CSize(arr[0], arr[1]);
	GetMultiIniValues(m_ini, _T("SizesOfElements"), _T("YScoreDigitSize"), &arr);
	m_Skin.sizesOfElements.sizeScore = CSize(arr[0], arr[1]);
	GetMultiIniValues(m_ini, _T("SizesOfElements"), _T("TimeDigitSize"), &arr);
	m_Skin.sizesOfElements.sizeTime = CSize(arr[0], arr[1]);
	GetMultiIniValues(m_ini, _T("SizesOfElements"), _T("GameTypeSize"), &arr);
	m_Skin.sizesOfElements.sizeType = CSize(arr[0], arr[1]);

	// [PlacementOfElements]
	GetMultiIniValues(m_ini, _T("PlacementOfElements"), _T("UpperLeftBall"), &arr);
	m_Skin.placementOfElements.pointUpperLeftBall = CPoint(arr[0], arr[1]);
	GetMultiIniValues(m_ini, _T("PlacementOfElements"), _T("BetweenBalls"), &arr);
	m_Skin.placementOfElements.sizeBetweenBalls = CSize(arr[0], arr[1]);
	GetMultiIniValues(m_ini, _T("PlacementOfElements"), _T("YourScorePos"), &arr);
	m_Skin.placementOfElements.pointMyScore = CPoint(arr[0], arr[1]);
	GetMultiIniValues(m_ini, _T("PlacementOfElements"), _T("YourScoreBetweenX"), &arr);
	m_Skin.placementOfElements.uBetweenMyScore = arr[0];
	GetMultiIniValues(m_ini, _T("PlacementOfElements"), _T("HighScorePos"), &arr);
	m_Skin.placementOfElements.pointHighScore = CPoint(arr[0], arr[1]);
	GetMultiIniValues(m_ini, _T("PlacementOfElements"), _T("HighScoreBetweenX"), &arr);
	m_Skin.placementOfElements.uBetweenHighScore = arr[0];
	GetMultiIniValues(m_ini, _T("PlacementOfElements"), _T("TimePos"), &arr);
	m_Skin.placementOfElements.pointTime = CPoint(arr[0], arr[1]);
	GetMultiIniValues(m_ini, _T("PlacementOfElements"), _T("TimeBetweenAndComma"), &arr);
	m_Skin.placementOfElements.uBetweenTime = arr[0];
	m_Skin.placementOfElements.uTimeComma = arr[1];
	GetMultiIniValues(m_ini, _T("PlacementOfElements"), _T("GameTypePos"), &arr);
	m_Skin.placementOfElements.pointType = CPoint(arr[0], arr[1]);
	GetMultiIniValues(m_ini, _T("PlacementOfElements"), _T("NextColor1"), &arr);
	m_Skin.placementOfElements.pointNext1 = CPoint(arr[0], arr[1]);
	GetMultiIniValues(m_ini, _T("PlacementOfElements"), _T("NextColor2"), &arr);
	m_Skin.placementOfElements.pointNext2 = CPoint(arr[0], arr[1]);
	GetMultiIniValues(m_ini, _T("PlacementOfElements"), _T("NextColor3"), &arr);
	m_Skin.placementOfElements.pointNext3 = CPoint(arr[0], arr[1]);

	// [AnimationOfBalls]
	GetMultiIniValues(m_ini, _T("AnimationOfBalls"), _T("BallFrames"), &arr);
	m_Skin.animationOfBalls.uFramesBall_Total = arr[0];

	GetMultiIniValues(m_ini, _T("AnimationOfBalls"), _T("BallSelected"), &arr);
	m_Skin.animationOfBalls.nFramesBall_Jump = arr.GetCount();
	if(m_Skin.animationOfBalls.pFramesBall_Jump)
		delete [] m_Skin.animationOfBalls.pFramesBall_Jump;
	m_Skin.animationOfBalls.pFramesBall_Jump = new UINT[m_Skin.animationOfBalls.nFramesBall_Jump];
	for(int i = 0 ; i < arr.GetCount() ; i++)
		m_Skin.animationOfBalls.pFramesBall_Jump[i] = arr[i];

	GetMultiIniValues(m_ini, _T("AnimationOfBalls"), _T("BallAppearence"), &arr);
	m_Skin.animationOfBalls.nFramesBall_Appearence = arr.GetCount();
	if(m_Skin.animationOfBalls.pFramesBall_Appearence)
		delete [] m_Skin.animationOfBalls.pFramesBall_Appearence;
	m_Skin.animationOfBalls.pFramesBall_Appearence = new UINT[m_Skin.animationOfBalls.nFramesBall_Appearence];
	for(int i = 0 ; i < arr.GetCount() ; i++)
		m_Skin.animationOfBalls.pFramesBall_Appearence[i] = arr[i];

	GetMultiIniValues(m_ini, _T("AnimationOfBalls"), _T("BallDestruction"), &arr);
	m_Skin.animationOfBalls.nFramesBall_Destruction = arr.GetCount();
	if(m_Skin.animationOfBalls.pFramesBall_Destruction)
		delete [] m_Skin.animationOfBalls.pFramesBall_Destruction;
	m_Skin.animationOfBalls.pFramesBall_Destruction = new UINT[m_Skin.animationOfBalls.nFramesBall_Destruction];
	for(int i = 0 ; i < arr.GetCount() ; i++)
		m_Skin.animationOfBalls.pFramesBall_Destruction[i] = arr[i];

	GetMultiIniValues(m_ini, _T("AnimationOfBalls"), _T("BallHint"), &arr);
	m_Skin.animationOfBalls.uFrameBall_Hint = arr[0];

	// [Delays]
	GetMultiIniValues(m_ini, _T("Delays"), _T("DelayJump"), &arr);
	m_Skin.delays.uDelayJump = arr[0];
	GetMultiIniValues(m_ini, _T("Delays"), _T("DelayAppearence"), &arr);
	m_Skin.delays.uDelayAppearence = arr[0];
	GetMultiIniValues(m_ini, _T("Delays"), _T("DelayDestruction"), &arr);
	m_Skin.delays.uDelayDestruction = arr[0];
	GetMultiIniValues(m_ini, _T("Delays"), _T("DelayMove"), &arr);
	m_Skin.delays.uDelayMove = arr[0];

	// [Copyright]
	m_Skin.copyrights.strCopyrightLine1 = m_ini.GetValue(_T("Copyright"), _T("CopyrightLine1"));
	m_Skin.copyrights.strCopyrightLine2 = m_ini.GetValue(_T("Copyright"), _T("CopyrightLine2"));
	m_Skin.copyrights.strDeveloperContactInfo = m_ini.GetValue(_T("Copyright"), _T("DeveloperContactInfo"));

	return TRUE;
}


// Загрузить изображения
BOOL CMainDlg::LoadImages(BOOL bErrMsg)
{
	BOOL bRes = TRUE;
	HRESULT hRes;

	if(!m_Skin.images.imgBackground.IsNull())
		m_Skin.images.imgBackground.Destroy();
	hRes = m_Skin.images.imgBackground.Load(m_Skin.files.strBackground);
	if(hRes)
	{
		if(bErrMsg)
			AfxMessageBox(_T("Ошибка при загрузке изображения ") + m_Skin.files.strBackground);
		bRes = FALSE;
	}

	if(!m_Skin.images.imgBalls.IsNull())
		m_Skin.images.imgBalls.Destroy();
	hRes = m_Skin.images.imgBalls.Load(m_Skin.files.strBalls);
	if(hRes)
	{
		if(bErrMsg)
			AfxMessageBox(_T("Ошибка при загрузке изображения ") + m_Skin.files.strBalls);
		bRes = FALSE;
	}

	if(!m_Skin.images.imgNext.IsNull())
		m_Skin.images.imgNext.Destroy();
	hRes = m_Skin.images.imgNext.Load(m_Skin.files.strNext);
	if(hRes)
	{
		if(bErrMsg)
			AfxMessageBox(_T("Ошибка при загрузке изображения ") + m_Skin.files.strNext);
		bRes = FALSE;
	}

	if(!m_Skin.images.imgScore.IsNull())
		m_Skin.images.imgScore.Destroy();
	hRes = m_Skin.images.imgScore.Load(m_Skin.files.strScore);
	if(hRes)
	{
		if(bErrMsg)
			AfxMessageBox(_T("Ошибка при загрузке изображения ") + m_Skin.files.strScore);
		bRes = FALSE;
	}

	if(!m_Skin.images.imgTime.IsNull())
		m_Skin.images.imgTime.Destroy();
	hRes = m_Skin.images.imgTime.Load(m_Skin.files.strTime);
	if(hRes)
	{
		if(bErrMsg)
			AfxMessageBox(_T("Ошибка при загрузке изображения ") + m_Skin.files.strTime);
		bRes = FALSE;
	}

	if(!m_Skin.images.imgType.IsNull())
		m_Skin.images.imgType.Destroy();
	hRes = m_Skin.images.imgType.Load(m_Skin.files.strType);
	if(hRes)
	{
		if(bErrMsg)
			AfxMessageBox(_T("Ошибка при загрузке изображения ") + m_Skin.files.strType);
		bRes = FALSE;
	}

	return bRes;
}


void CMainDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
//----------------------------------------------- PAINT [>>> ----------------------------------------------------
		CPaintDC dc(this);

		// Прорисовка фона
		m_Skin.images.imgBackground.Draw(dc.GetSafeHdc(), 0, 0);
		
		// Прорисовка шариков на поле (больших и маленьких)
		for( int i = 0 ; i < 9 ; i++ )
		{
			for( int j = 0 ; j < 9 ; j++ )
			{
				if(CPoint(i, j) != m_ptMoving)
				{
					if(m_Field.GetCellState(i, j) == Cell::CellState::BUSY)
						m_Skin.images.imgBalls.Draw(dc.GetSafeHdc(), m_Field.GetRealPointFromIndexes(i, j).x, m_Field.GetRealPointFromIndexes(i, j).y, m_Skin.sizesOfElements.sizeBall.cx, m_Skin.sizesOfElements.sizeBall.cy, /*(m_Skin.animationOfBalls.pFramesBall_Jump[0] - 1) * m_Skin.sizesOfElements.sizeBall.cx*/ 0, m_Field.GetCellColor(i, j) * m_Skin.sizesOfElements.sizeBall.cy, m_Skin.sizesOfElements.sizeBall.cx, m_Skin.sizesOfElements.sizeBall.cy);
					else if(m_Field.GetCellState(i, j) == Cell::CellState::NEXT)
						m_Skin.images.imgBalls.Draw(dc.GetSafeHdc(), m_Field.GetRealPointFromIndexes(i, j).x, m_Field.GetRealPointFromIndexes(i, j).y, m_Skin.sizesOfElements.sizeBall.cx, m_Skin.sizesOfElements.sizeBall.cy, /*(m_Skin.animationOfBalls.pFramesBall_Appearence[0] - 1)*/ m_Skin.animationOfBalls.uFrameBall_Hint * m_Skin.sizesOfElements.sizeBall.cx, m_Field.GetCellColor(i, j) * m_Skin.sizesOfElements.sizeBall.cy, m_Skin.sizesOfElements.sizeBall.cx, m_Skin.sizesOfElements.sizeBall.cy);
				}
			}
		}

		// Прорисовка лучшего счёта
		int digit;
		for( int i = 0 ; i < 5 ; i++ )
		{
			digit = m_nHighScores % (int)pow(10.0, 5 - i) / (int)pow(10.0, 4 - i);
			m_Skin.images.imgScore.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointHighScore.x + (m_Skin.sizesOfElements.sizeScore.cx + m_Skin.placementOfElements.uBetweenHighScore) * i, m_Skin.placementOfElements.pointHighScore.y, m_Skin.sizesOfElements.sizeScore.cx, m_Skin.sizesOfElements.sizeScore.cy, m_Skin.sizesOfElements.sizeScore.cx * digit, 0, m_Skin.sizesOfElements.sizeScore.cx, m_Skin.sizesOfElements.sizeScore.cy);
		}

		// Прорисовка моего счёта
		for( int i = 0 ; i < 5 ; i++ )
		{
			digit = m_nScores % (int)pow(10.0, 5 - i) / (int)pow(10.0, 4 - i);
			m_Skin.images.imgScore.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointMyScore.x + (m_Skin.sizesOfElements.sizeScore.cx + m_Skin.placementOfElements.uBetweenMyScore) * i, m_Skin.placementOfElements.pointMyScore.y, m_Skin.sizesOfElements.sizeScore.cx, m_Skin.sizesOfElements.sizeScore.cy, m_Skin.sizesOfElements.sizeScore.cx * digit, 0, m_Skin.sizesOfElements.sizeScore.cx, m_Skin.sizesOfElements.sizeScore.cy);
		}

		// Прорисовка 3 цветов новых шаров
		m_Skin.images.imgNext.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointNext1.x, m_Skin.placementOfElements.pointNext1.y, m_Skin.sizesOfElements.sizeNext.cx, m_Skin.sizesOfElements.sizeNext.cy, m_Skin.sizesOfElements.sizeNext.cx * m_Field.GetNextBallColor(0), 0, m_Skin.sizesOfElements.sizeNext.cx, m_Skin.sizesOfElements.sizeNext.cy);
		m_Skin.images.imgNext.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointNext2.x, m_Skin.placementOfElements.pointNext2.y, m_Skin.sizesOfElements.sizeNext.cx, m_Skin.sizesOfElements.sizeNext.cy, m_Skin.sizesOfElements.sizeNext.cx * m_Field.GetNextBallColor(1), 0, m_Skin.sizesOfElements.sizeNext.cx, m_Skin.sizesOfElements.sizeNext.cy);
		m_Skin.images.imgNext.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointNext3.x, m_Skin.placementOfElements.pointNext3.y, m_Skin.sizesOfElements.sizeNext.cx, m_Skin.sizesOfElements.sizeNext.cy, m_Skin.sizesOfElements.sizeNext.cx * m_Field.GetNextBallColor(2), 0, m_Skin.sizesOfElements.sizeNext.cx, m_Skin.sizesOfElements.sizeNext.cy);

		// Прорисовка времени
		m_Skin.images.imgTime.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointTime.x, m_Skin.placementOfElements.pointTime.y, m_Skin.sizesOfElements.sizeTime.cx, m_Skin.sizesOfElements.sizeTime.cy, m_Skin.sizesOfElements.sizeTime.cx * int (m_Time.GetHours() % 10), 0, m_Skin.sizesOfElements.sizeTime.cx, m_Skin.sizesOfElements.sizeTime.cy);
		m_Skin.images.imgTime.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointTime.x + m_Skin.sizesOfElements.sizeTime.cx * 1 + m_Skin.placementOfElements.uBetweenTime * 1 + m_Skin.placementOfElements.uTimeComma * 1, m_Skin.placementOfElements.pointTime.y, m_Skin.sizesOfElements.sizeTime.cx, m_Skin.sizesOfElements.sizeTime.cy, m_Skin.sizesOfElements.sizeTime.cx * int (m_Time.GetMinutes() / 10), 0, m_Skin.sizesOfElements.sizeTime.cx, m_Skin.sizesOfElements.sizeTime.cy);
		m_Skin.images.imgTime.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointTime.x + m_Skin.sizesOfElements.sizeTime.cx * 2 + m_Skin.placementOfElements.uBetweenTime * 2 + m_Skin.placementOfElements.uTimeComma * 1, m_Skin.placementOfElements.pointTime.y, m_Skin.sizesOfElements.sizeTime.cx, m_Skin.sizesOfElements.sizeTime.cy, m_Skin.sizesOfElements.sizeTime.cx * int (m_Time.GetMinutes() % 10), 0, m_Skin.sizesOfElements.sizeTime.cx, m_Skin.sizesOfElements.sizeTime.cy);
		m_Skin.images.imgTime.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointTime.x + m_Skin.sizesOfElements.sizeTime.cx * 3 + m_Skin.placementOfElements.uBetweenTime * 3 + m_Skin.placementOfElements.uTimeComma * 2, m_Skin.placementOfElements.pointTime.y, m_Skin.sizesOfElements.sizeTime.cx, m_Skin.sizesOfElements.sizeTime.cy, m_Skin.sizesOfElements.sizeTime.cx * int (m_Time.GetSeconds() / 10), 0, m_Skin.sizesOfElements.sizeTime.cx, m_Skin.sizesOfElements.sizeTime.cy);
		m_Skin.images.imgTime.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointTime.x + m_Skin.sizesOfElements.sizeTime.cx * 4 + m_Skin.placementOfElements.uBetweenTime * 4 + m_Skin.placementOfElements.uTimeComma * 2, m_Skin.placementOfElements.pointTime.y, m_Skin.sizesOfElements.sizeTime.cx, m_Skin.sizesOfElements.sizeTime.cy, m_Skin.sizesOfElements.sizeTime.cx * int (m_Time.GetSeconds() % 10), 0, m_Skin.sizesOfElements.sizeTime.cx, m_Skin.sizesOfElements.sizeTime.cy);

		// Прорисовка типа игры
		m_Skin.images.imgType.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointType.x, m_Skin.placementOfElements.pointType.y, m_Skin.sizesOfElements.sizeType.cx, m_Skin.sizesOfElements.sizeType.cy, m_Skin.sizesOfElements.sizeType.cx * 0, 0, m_Skin.sizesOfElements.sizeType.cx, m_Skin.sizesOfElements.sizeType.cy);
//-------------------------------------------- <<<] PAINT ------------------------------------------------------
	}
}

HCURSOR CMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMainDlg::OnTimer(UINT_PTR nIDEvent)
{
//----------------------------------------------- MAIN TIMER ---------------------------------------------------
	if(nIDEvent == m_nTimerIDEvent_Main)
	{
		m_Time += CTimeSpan(0, 0, 0, 1);
		InvalidateTime();
	}
//----------------------------------------------- JUMP TIMER ----------------------------------------------------
	else if(nIDEvent == m_nTimerIDEvent_Jump)
	{
		if(m_ptSelStart != CPoint(-1, -1))
		{
			// Следующий кадр:
			m_iCurrentFrame_Jump++;

			// Зацикливание кадров:
			if(m_iCurrentFrame_Jump >= m_Skin.animationOfBalls.nFramesBall_Jump)
				m_iCurrentFrame_Jump = 0;

			// Отрисовка текущего кадра:
			CClientDC dc(this);
			m_Skin.images.imgBalls.Draw(dc.GetSafeHdc(), m_Field.GetRealPointFromIndexes(m_ptSelStart).x, m_Field.GetRealPointFromIndexes(m_ptSelStart).y, m_Skin.sizesOfElements.sizeBall.cx, m_Skin.sizesOfElements.sizeBall.cy, (m_Skin.animationOfBalls.pFramesBall_Jump[m_iCurrentFrame_Jump] - 1) * m_Skin.sizesOfElements.sizeBall.cx, m_Field.GetCellColor(m_ptSelStart) * m_Skin.sizesOfElements.sizeBall.cy, m_Skin.sizesOfElements.sizeBall.cx, m_Skin.sizesOfElements.sizeBall.cy);
		}
	}
//----------------------------------------------- MOVE TIMER ----------------------------------------------------
	else if(nIDEvent == m_nTimerIDEvent_Move)
	{
		if(m_arrMove.GetCount() > 1)
		{
			// Стереть [0]
			m_arrInvalidate.Add(m_arrMove[0]);
			InvalidateQueue();

			// Нарисовать [1]
			CClientDC dc(this);
			m_Skin.images.imgBalls.Draw(dc.GetSafeHdc(), m_Field.GetRealPointFromIndexes(m_arrMove[1]).x, m_Field.GetRealPointFromIndexes(m_arrMove[1]).y, m_Skin.sizesOfElements.sizeBall.cx, m_Skin.sizesOfElements.sizeBall.cy, (m_Skin.animationOfBalls.pFramesBall_Jump[0] - 1) * m_Skin.sizesOfElements.sizeBall.cx, m_Field.GetCellColor(m_ptSelStart) * m_Skin.sizesOfElements.sizeBall.cy, m_Skin.sizesOfElements.sizeBall.cx, m_Skin.sizesOfElements.sizeBall.cy);

			// Удалить [0]
			m_arrMove.RemoveAt(0);
		}
		else if(m_arrMove.GetCount() == 1)
		{
			// Сразу убить таймер перемещений:
			KillTimer(m_nTimerIDEvent_Move);
			m_nTimerIDEvent_Move = 0;

			// Выполнить окончательное перемещение
			// и убрать ненужные заглушку и захват начального шарика.
			// * Заметка:
			// Возможно, при перемещении потеряется 1 next шарик, 
			// но его обновлять НЕ НАДО, т.к. он сразу же вырастит!!! 
			// * Поправка v1.0.1: НАДО!!!
			// Т.к. он может вырасти не сразу, 
			// если на его старом месте соберётся линия, и перехода хода не будет
			m_Field.Move(m_ptSelStart, m_arrMove[0], m_arrInvalidate, /*v1.0.1*/ /*FALSE*/ TRUE);
			m_ptMoving = CPoint(-1, -1);
			m_ptSelStart = CPoint(-1, -1);

			// Обновить наконец шар назначения
			// и очистить массив перемещений:
			m_arrInvalidate.Add(m_arrMove[0]);
			m_arrMove.RemoveAll();
			InvalidateQueue();


			// Когда всё устаканилось, надо попробовать собрать линию:
			INT nDestroyed = 0;
			if(nDestroyed = m_Field.FindDestroy(m_arrDestruction))
			{
				// Запустить таймер уничтожения,
				// отменив переход хода:
				bPrepareNextAfterDestroy = FALSE;
				m_nTimerIDEvent_Destruction = SetTimer(m_uTimerID_Destruction, m_Skin.delays.uDelayDestruction, NULL);
				m_nScores += nDestroyed * (nDestroyed - 4);
			}
			else
			{
				Next();
			}
		}
	}
//-------------------------------------------- APPEARANCE TIMER -------------------------------------------------
	else if(nIDEvent == m_nTimerIDEvent_Appearence)
	{
		m_iCurrentFrame_Appearence++;

		if(m_iCurrentFrame_Appearence < m_Skin.animationOfBalls.nFramesBall_Appearence)
		{
			// Отрисовка текущего кадра анимации роста:
			for(INT i = 0 ; i < m_arrAppearence.GetCount() ; i++)
			{
				CClientDC dc(this);
				m_Skin.images.imgBalls.Draw(dc.GetSafeHdc(), m_Field.GetRealPointFromIndexes(m_arrAppearence[i]).x, m_Field.GetRealPointFromIndexes(m_arrAppearence[i]).y, m_Skin.sizesOfElements.sizeBall.cx, m_Skin.sizesOfElements.sizeBall.cy, (m_Skin.animationOfBalls.pFramesBall_Appearence[m_iCurrentFrame_Appearence] - 1) * m_Skin.sizesOfElements.sizeBall.cx, m_Field.GetCellColor(m_arrAppearence[i]) * m_Skin.sizesOfElements.sizeBall.cy, m_Skin.sizesOfElements.sizeBall.cx, m_Skin.sizesOfElements.sizeBall.cy);
			}
		}
		else
		{
			// Сразу убить таймер появления и сбросить кадр:
			m_iCurrentFrame_Appearence = -1;
			KillTimer(m_nTimerIDEvent_Appearence);
			m_nTimerIDEvent_Appearence = 0;

			// Обновить выросшие шарики на поле:
			m_arrInvalidate.Append(m_arrAppearence);
			m_arrAppearence.RemoveAll();
			InvalidateQueue();

			// Попробовать собрать линии:
			INT nBallsDestroyed = 0;
			if(nBallsDestroyed = m_Field.FindDestroy(m_arrDestruction))
			{
				// Запустить таймер уничтожения,
				// по окончании появится новая партия next:
				bPrepareNextAfterDestroy = TRUE;
				m_nTimerIDEvent_Destruction = SetTimer(m_uTimerID_Destruction, m_Skin.delays.uDelayDestruction, NULL);
				m_nScores += nBallsDestroyed * (nBallsDestroyed - 4);
			}
			else if(m_Field.PrepareNextBalls(m_arrInvalidate))
			{
				// Обновить новоподготовленные next шары:
				InvalidateQueue();
				InvalidateNext();
			}
			else
			{
				EndGame();
			}
		}
	}
//--------------------------------------------- DESTROY TIMER ---------------------------------------------------
	else if(nIDEvent == m_nTimerIDEvent_Destruction)
	{
		m_iCurrentFrame_Destruction++;

		if(m_iCurrentFrame_Destruction < m_Skin.animationOfBalls.nFramesBall_Destruction)
		{
			// Отрисовка текущего кадра анимации уничтожения:
			for(INT i = 0 ; i < m_arrDestruction.GetCount() ; i++)
			{
				CClientDC dc(this);
				m_Skin.images.imgBalls.Draw(dc.GetSafeHdc(), m_Field.GetRealPointFromIndexes(m_arrDestruction[i]).x, m_Field.GetRealPointFromIndexes(m_arrDestruction[i]).y, m_Skin.sizesOfElements.sizeBall.cx, m_Skin.sizesOfElements.sizeBall.cy, (m_Skin.animationOfBalls.pFramesBall_Destruction[m_iCurrentFrame_Destruction] - 1) * m_Skin.sizesOfElements.sizeBall.cx, m_Field.GetCellColor(m_arrDestruction[i]) * m_Skin.sizesOfElements.sizeBall.cy, m_Skin.sizesOfElements.sizeBall.cx, m_Skin.sizesOfElements.sizeBall.cy);
			}
		}
		else
		{
			// Сразу убить таймер уничтожения и сбросить кадр:
			m_iCurrentFrame_Destruction = -1;
			KillTimer(m_nTimerIDEvent_Destruction);
			m_nTimerIDEvent_Destruction = 0;

			// Теперь шарики нужно удалить и стереть с поля:
			m_Field.Destroy(m_arrDestruction);
			m_arrInvalidate.Append(m_arrDestruction);
			m_arrDestruction.RemoveAll();
			InvalidateQueue();

			// Заработанные очки тоже нужно обновить:
			InvalidateMyScore();

			if(bPrepareNextAfterDestroy)
			{
				if(m_Field.PrepareNextBalls(m_arrInvalidate))
				{
					// Обновить новоподготовленные next шары:
					InvalidateQueue();
					InvalidateNext();
				}
				else
				{
					EndGame();
				}

				bPrepareNextAfterDestroy = FALSE;
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}


BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	m_tooltip.RelayEvent(pMsg);
	if( ( pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP ) && ( pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN ) )
		return TRUE;
	else
		return CDialog::PreTranslateMessage(pMsg);
}


void CMainDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	TRACE(_T("WM_LBUTTONDOWN:  (%d, %d)  <%d, %d>\n"), m_Field.GetIndexesFromRealPoint(point).x, m_Field.GetIndexesFromRealPoint(point).y, m_ptSelStart.x, m_ptSelStart.y);

	// Не попал на поле (или штанга)!
	if(m_Field.GetIndexesFromRealPoint(point) == CPoint(-1, -1))
	{
		TRACE(_T("A click must be on the field!\n"));
		return;
	}

	// Кликнул на уже выбранном шарике!
	if(m_Field.GetIndexesFromRealPoint(point) == m_ptSelStart)
	{
		TRACE(_T("The ball (%d, %d) already selected!\n"), m_ptSelStart.x, m_ptSelStart.y);
		return;
	}


	// Клика раньше не было
	if(m_ptSelStart == CPoint(-1, -1))
	{
		// выбор #1
		if(m_Field.GetCellState(m_Field.GetIndexesFromRealPoint(point)) == Cell::CellState::BUSY)
		{
			m_ptSelStart = m_Field.GetIndexesFromRealPoint(point);
			TRACE(_T("You selected the ball: (%d, %d).\n"), m_ptSelStart.x, m_ptSelStart.y);

			// Этот шарик начинает прыгать:
			m_nTimerIDEvent_Jump = SetTimer(m_uTimerID_Jump, m_Skin.delays.uDelayJump, NULL);
		}

		// нечего выбирать!
		else
		{
			TRACE(_T("You must select a ball!\n"));
		}
	}

	// Клик уже был
	else
	{
		// Re: выбор #1
		if(m_Field.GetCellState(m_Field.GetIndexesFromRealPoint(point)) == Cell::CellState::BUSY)
		{
			// Остановить прыгание выбранного ранее шарика + сбросить кадр анимации:
			m_iCurrentFrame_Jump = -1;
			KillTimer(m_nTimerIDEvent_Jump);
			m_nTimerIDEvent_Jump = 0;

			// Обновить выбранный ранее шарик:
			m_arrInvalidate.Add(m_ptSelStart);
			InvalidateQueue();

			m_ptSelStart = m_Field.GetIndexesFromRealPoint(point);
			TRACE(_T("You re-selected the ball: (%d, %d).\n"), m_ptSelStart.x, m_ptSelStart.y);

			// Новоизбранный шарик начинает прыгать:
			m_nTimerIDEvent_Jump = SetTimer(m_uTimerID_Jump, m_Skin.delays.uDelayJump, NULL);
		}

		// выбор #2
		else
		{
			if(m_Field.FindWay(m_arrMove, m_ptSelStart, m_Field.GetIndexesFromRealPoint(point)))
			{
				// Выбранный шарик уже не прыгает + сбросить кадр анимации:
				m_iCurrentFrame_Jump = -1;
				KillTimer(m_nTimerIDEvent_Jump);
				m_nTimerIDEvent_Jump = 0;

				// Заглушить стартовую точку
				m_ptMoving = m_ptSelStart;

				// Вставить в начало пути стартовую точку (особенности анимации)
				m_arrMove.InsertAt(0, m_ptSelStart);

				// Запуск анимации передвижения:
				m_nTimerIDEvent_Move = SetTimer(m_uTimerID_Move, m_Skin.delays.uDelayMove, NULL);

				// По окончании анимации передвижения
				// таймер сам себя завершит, 
				// уберёт заглушку
				// и переместит шарик из начала в конец.
			}
		}
	}

}

// * Функция изменена в v1.0.1:
// В v1.0 для каждой ячейки поля массива m_arrInvalidate[] просто вызывалась ф-ия InvalidateRect(), и перерисовка осуществлялась в ф-ии OnPaint()
// В v1.0.1 каждая ячейка поля массива m_arrInvalidate[] перерисовывается прямо здесь функцией Draw(), без перенаправления в OnPaint()
// (Благодаря этому устранились блики при стирании)
void CMainDlg::InvalidateQueue()
{
	if(!m_arrInvalidate.IsEmpty())
	{
		CClientDC dc(this);

		for(INT i = 0 ; i < m_arrInvalidate.GetCount() ; i++)
		{
			TRACE(_T("Updating cell (%d, %d)\n"), m_arrInvalidate[i].x, m_arrInvalidate[i].y);
			if(m_arrInvalidate[i] == m_ptMoving)
			{
				m_Skin.images.imgBackground.Draw(dc.GetSafeHdc(), m_Field.GetRealPointFromIndexes(m_arrInvalidate[i]).x, m_Field.GetRealPointFromIndexes(m_arrInvalidate[i]).y, m_Skin.sizesOfElements.sizeBall.cx, m_Skin.sizesOfElements.sizeBall.cy, m_Field.GetRealPointFromIndexes(m_arrInvalidate[i]).x, m_Field.GetRealPointFromIndexes(m_arrInvalidate[i]).y, m_Skin.sizesOfElements.sizeBall.cx, m_Skin.sizesOfElements.sizeBall.cy);
			}
			else
			{
 				if(m_Field.GetCellState(m_arrInvalidate[i]) == Cell::CellState::BUSY)
 					m_Skin.images.imgBalls.Draw(dc.GetSafeHdc(), m_Field.GetRealPointFromIndexes(m_arrInvalidate[i]).x, m_Field.GetRealPointFromIndexes(m_arrInvalidate[i]).y, m_Skin.sizesOfElements.sizeBall.cx, m_Skin.sizesOfElements.sizeBall.cy, 0, m_Field.GetCellColor(m_arrInvalidate[i]) * m_Skin.sizesOfElements.sizeBall.cy, m_Skin.sizesOfElements.sizeBall.cx, m_Skin.sizesOfElements.sizeBall.cy);
 				else if(m_Field.GetCellState(m_arrInvalidate[i]) == Cell::CellState::NEXT)
 					m_Skin.images.imgBalls.Draw(dc.GetSafeHdc(), m_Field.GetRealPointFromIndexes(m_arrInvalidate[i]).x, m_Field.GetRealPointFromIndexes(m_arrInvalidate[i]).y, m_Skin.sizesOfElements.sizeBall.cx, m_Skin.sizesOfElements.sizeBall.cy, m_Skin.animationOfBalls.uFrameBall_Hint * m_Skin.sizesOfElements.sizeBall.cx, m_Field.GetCellColor(m_arrInvalidate[i]) * m_Skin.sizesOfElements.sizeBall.cy, m_Skin.sizesOfElements.sizeBall.cx, m_Skin.sizesOfElements.sizeBall.cy);
				else
					m_Skin.images.imgBackground.Draw(dc.GetSafeHdc(), m_Field.GetRealPointFromIndexes(m_arrInvalidate[i]).x, m_Field.GetRealPointFromIndexes(m_arrInvalidate[i]).y, m_Skin.sizesOfElements.sizeBall.cx, m_Skin.sizesOfElements.sizeBall.cy, m_Field.GetRealPointFromIndexes(m_arrInvalidate[i]).x, m_Field.GetRealPointFromIndexes(m_arrInvalidate[i]).y, m_Skin.sizesOfElements.sizeBall.cx, m_Skin.sizesOfElements.sizeBall.cy);
			}
		}

		m_arrInvalidate.RemoveAll();
	}
}

// * Функция изменена в v1.0.1:
// В v1.0 для каждого next шарика из 3-ёх просто вызывалась ф-ия InvalidateRect(), и перерисовка осуществлялась в ф-ии OnPaint()
// В v1.0.1 каждый next шарик из 3-ёх перерисовывается прямо здесь функцией Draw(), без перенаправления в OnPaint()
// (Благодаря этому устранились блики при стирании)
void CMainDlg::InvalidateNext()
{
	CClientDC dc(this);
	
	if(m_Field.GetNextBallColor(0) == -1)
		m_Skin.images.imgBackground.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointNext1.x, m_Skin.placementOfElements.pointNext1.y, m_Skin.sizesOfElements.sizeNext.cx, m_Skin.sizesOfElements.sizeNext.cy, m_Skin.placementOfElements.pointNext1.x, m_Skin.placementOfElements.pointNext1.y, m_Skin.sizesOfElements.sizeNext.cx, m_Skin.sizesOfElements.sizeNext.cy);
	else
		m_Skin.images.imgNext.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointNext1.x, m_Skin.placementOfElements.pointNext1.y, m_Skin.sizesOfElements.sizeNext.cx, m_Skin.sizesOfElements.sizeNext.cy, m_Skin.sizesOfElements.sizeNext.cx * m_Field.GetNextBallColor(0), 0, m_Skin.sizesOfElements.sizeNext.cx, m_Skin.sizesOfElements.sizeNext.cy);
	
	if(m_Field.GetNextBallColor(1) == -1)
		m_Skin.images.imgBackground.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointNext2.x, m_Skin.placementOfElements.pointNext2.y, m_Skin.sizesOfElements.sizeNext.cx, m_Skin.sizesOfElements.sizeNext.cy, m_Skin.placementOfElements.pointNext2.x, m_Skin.placementOfElements.pointNext2.y, m_Skin.sizesOfElements.sizeNext.cx, m_Skin.sizesOfElements.sizeNext.cy);
	else
		m_Skin.images.imgNext.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointNext2.x, m_Skin.placementOfElements.pointNext2.y, m_Skin.sizesOfElements.sizeNext.cx, m_Skin.sizesOfElements.sizeNext.cy, m_Skin.sizesOfElements.sizeNext.cx * m_Field.GetNextBallColor(1), 0, m_Skin.sizesOfElements.sizeNext.cx, m_Skin.sizesOfElements.sizeNext.cy);

	if(m_Field.GetNextBallColor(2) == -1)
		m_Skin.images.imgBackground.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointNext3.x, m_Skin.placementOfElements.pointNext3.y, m_Skin.sizesOfElements.sizeNext.cx, m_Skin.sizesOfElements.sizeNext.cy, m_Skin.placementOfElements.pointNext3.x, m_Skin.placementOfElements.pointNext3.y, m_Skin.sizesOfElements.sizeNext.cx, m_Skin.sizesOfElements.sizeNext.cy);
	else
		m_Skin.images.imgNext.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointNext3.x, m_Skin.placementOfElements.pointNext3.y, m_Skin.sizesOfElements.sizeNext.cx, m_Skin.sizesOfElements.sizeNext.cy, m_Skin.sizesOfElements.sizeNext.cx * m_Field.GetNextBallColor(2), 0, m_Skin.sizesOfElements.sizeNext.cx, m_Skin.sizesOfElements.sizeNext.cy);
}

// * Функция изменена в v1.0.1:
// В v1.0 для каждой цифры просто вызывалась ф-ия InvalidateRect(), и перерисовка осуществлялась в ф-ии OnPaint()
// В v1.0.1 каждая цифра перерисовывается прямо здесь функцией Draw(), без перенаправления в OnPaint()
// (Благодаря этому устранились блики при стирании)
void CMainDlg::InvalidateMyScore()
{
	CClientDC dc(this);
	int digit;
	for( int i = 0 ; i < 5 ; i++ )
	{
		digit = m_nScores % (int) pow(10.0, 5 - i) / (int) pow(10.0, 4 - i);
		m_Skin.images.imgScore.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointMyScore.x + (m_Skin.sizesOfElements.sizeScore.cx + m_Skin.placementOfElements.uBetweenMyScore) * i, m_Skin.placementOfElements.pointMyScore.y, m_Skin.sizesOfElements.sizeScore.cx, m_Skin.sizesOfElements.sizeScore.cy, m_Skin.sizesOfElements.sizeScore.cx * digit, 0, m_Skin.sizesOfElements.sizeScore.cx, m_Skin.sizesOfElements.sizeScore.cy);
	}
}

// * Функция изменена в v1.0.1:
// В v1.0 для каждой цифры просто вызывалась ф-ия InvalidateRect(), и перерисовка осуществлялась в ф-ии OnPaint()
// В v1.0.1 каждая цифра перерисовывается прямо здесь функцией Draw(), без перенаправления в OnPaint()
// (Благодаря этому устранились блики при стирании)
void CMainDlg::InvalidateHighScore()
{
	CClientDC dc(this);
	int digit;
	for( int i = 0 ; i < 5 ; i++ )
	{
		digit = m_nHighScores % (int)pow(10.0, 5 - i) / (int)pow(10.0, 4 - i);
		m_Skin.images.imgScore.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointHighScore.x + (m_Skin.sizesOfElements.sizeScore.cx + m_Skin.placementOfElements.uBetweenHighScore) * i, m_Skin.placementOfElements.pointHighScore.y, m_Skin.sizesOfElements.sizeScore.cx, m_Skin.sizesOfElements.sizeScore.cy, m_Skin.sizesOfElements.sizeScore.cx * digit, 0, m_Skin.sizesOfElements.sizeScore.cx, m_Skin.sizesOfElements.sizeScore.cy);
	}
}

// * Функция изменена в v1.0.1:
// В v1.0 для каждой цифры просто вызывалась ф-ия InvalidateRect(), и перерисовка осуществлялась в ф-ии OnPaint()
// В v1.0.1 каждая цифра перерисовывается прямо здесь функцией Draw(), без перенаправления в OnPaint()
// (Благодаря этому устранились блики при стирании)
void CMainDlg::InvalidateTime()
{
	CClientDC dc(this);
	m_Skin.images.imgTime.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointTime.x, m_Skin.placementOfElements.pointTime.y, m_Skin.sizesOfElements.sizeTime.cx, m_Skin.sizesOfElements.sizeTime.cy, m_Skin.sizesOfElements.sizeTime.cx * int (m_Time.GetHours() % 10), 0, m_Skin.sizesOfElements.sizeTime.cx, m_Skin.sizesOfElements.sizeTime.cy);
	m_Skin.images.imgTime.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointTime.x + m_Skin.sizesOfElements.sizeTime.cx * 1 + m_Skin.placementOfElements.uBetweenTime * 1 + m_Skin.placementOfElements.uTimeComma * 1, m_Skin.placementOfElements.pointTime.y, m_Skin.sizesOfElements.sizeTime.cx, m_Skin.sizesOfElements.sizeTime.cy, m_Skin.sizesOfElements.sizeTime.cx * int (m_Time.GetMinutes() / 10), 0, m_Skin.sizesOfElements.sizeTime.cx, m_Skin.sizesOfElements.sizeTime.cy);
	m_Skin.images.imgTime.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointTime.x + m_Skin.sizesOfElements.sizeTime.cx * 2 + m_Skin.placementOfElements.uBetweenTime * 2 + m_Skin.placementOfElements.uTimeComma * 1, m_Skin.placementOfElements.pointTime.y, m_Skin.sizesOfElements.sizeTime.cx, m_Skin.sizesOfElements.sizeTime.cy, m_Skin.sizesOfElements.sizeTime.cx * int (m_Time.GetMinutes() % 10), 0, m_Skin.sizesOfElements.sizeTime.cx, m_Skin.sizesOfElements.sizeTime.cy);
	m_Skin.images.imgTime.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointTime.x + m_Skin.sizesOfElements.sizeTime.cx * 3 + m_Skin.placementOfElements.uBetweenTime * 3 + m_Skin.placementOfElements.uTimeComma * 2, m_Skin.placementOfElements.pointTime.y, m_Skin.sizesOfElements.sizeTime.cx, m_Skin.sizesOfElements.sizeTime.cy, m_Skin.sizesOfElements.sizeTime.cx * int (m_Time.GetSeconds() / 10), 0, m_Skin.sizesOfElements.sizeTime.cx, m_Skin.sizesOfElements.sizeTime.cy);
	m_Skin.images.imgTime.Draw(dc.GetSafeHdc(), m_Skin.placementOfElements.pointTime.x + m_Skin.sizesOfElements.sizeTime.cx * 4 + m_Skin.placementOfElements.uBetweenTime * 4 + m_Skin.placementOfElements.uTimeComma * 2, m_Skin.placementOfElements.pointTime.y, m_Skin.sizesOfElements.sizeTime.cx, m_Skin.sizesOfElements.sizeTime.cy, m_Skin.sizesOfElements.sizeTime.cx * int (m_Time.GetSeconds() % 10), 0, m_Skin.sizesOfElements.sizeTime.cx, m_Skin.sizesOfElements.sizeTime.cy);
}

void CMainDlg::OnDestroy()
{
	if (m_pBtnStartNewGame)
		delete m_pBtnStartNewGame;
	if (m_pBtnStopGame)
		delete m_pBtnStopGame;
	if (m_pBtnSaveGameToFile)
		delete m_pBtnSaveGameToFile;
	if (m_pBtnLoadGameFromFile)
		delete m_pBtnLoadGameFromFile;
	if (m_pBtnSettings)
		delete m_pBtnSettings;
	if (m_pBtnSkins)
		delete m_pBtnSkins;
	if (m_pBtnTableOfBestResults)
		delete m_pBtnTableOfBestResults;
	if (m_pBtnHelp)
		delete m_pBtnHelp;
	if (m_pBtnAbout)
		delete m_pBtnAbout;

	DeleteFonts();
	DeleteImagesBalls();

	CDialog::OnDestroy();
}


void CMainDlg::Next(BOOL bGrow/* = TRUE*/)
{
	TRACE(_T("\n***************************************************************************\n\n"));

	// Next шарики должны вырасти:
	m_Field.ConvertNextBallsToCurrent(m_arrAppearence);

	// Если не надо анимации роста:
	if(!bGrow)
		m_iCurrentFrame_Appearence = m_Skin.animationOfBalls.nFramesBall_Appearence - 1;

	// Next шарики начинают расти:
	m_nTimerIDEvent_Appearence = SetTimer(m_uTimerID_Appearence, m_Skin.delays.uDelayAppearence, NULL);

	// По окончании анимации роста
	// таймер сам себя завершит
	// и подготовит новую партию next шариков...
	// (не забывая про уничтожение линий)
}


void CMainDlg::StartNewGame()
{
	TRACE(_T("\n############################### NEW GAME ###################################\n\n"));

	// СТЕРЕТЬ ВСЁ !!!
	KillAll();

	// Старт таймера:
	m_nTimerIDEvent_Main = SetTimer(m_uTimerID_Main, 1000, NULL);

	// Сначала подготавливается 1-ая партия next шариков, 
	// НО не обновляется на экране, 
	// т.к. сразу после этого они вырастут - и вот тогда обновятся!
	m_Field.PrepareNextBalls(m_arrInvalidate, FALSE);
	
	// Следующая партия:
	Next(FALSE);
}


void CMainDlg::EndGame()
{
	// Перед "стиркой" запомнить время и очки:
	INT tempScores = m_nScores;
	CTimeSpan tempTime = m_Time;

	// СТЕРЕТЬ ВСЁ !!!
	KillAll();

	// Записать результат:
	SaveResult(tempScores, tempTime);
}

void CMainDlg::KillAll()
{
	// Убрать заглушку для обработчика таймера уничтожения:
	bPrepareNextAfterDestroy = FALSE;

	// Очистка координат выбранного и перемещаемого шарика
	m_ptSelStart = CPoint(-1, -1);
	m_ptMoving = CPoint(-1, -1);

	// Очистка всех кадров анимации:
	m_iCurrentFrame_Jump = -1;
	m_iCurrentFrame_Appearence = -1;
	m_iCurrentFrame_Destruction = -1;

	// Остановка всех таймеров:
	KillTimer(m_nTimerIDEvent_Jump);
	KillTimer(m_nTimerIDEvent_Move);
	KillTimer(m_nTimerIDEvent_Appearence);
	KillTimer(m_nTimerIDEvent_Destruction);

	// Очистка ID всех таймеров:
	m_nTimerIDEvent_Jump = 0;
	m_nTimerIDEvent_Appearence = 0;
	m_nTimerIDEvent_Destruction = 0;
	m_nTimerIDEvent_Move = 0;

	// Чтобы корректно остановить перемещение шарика,
	// нужно очистить последний нарисованный кадр:
	if(!m_arrMove.IsEmpty())
	{
		m_arrInvalidate.Add(m_arrMove[0]);
		InvalidateQueue();
	}

	// Очистка всех массивов:
	m_arrInvalidate.RemoveAll();
	m_arrAppearence.RemoveAll();
	m_arrDestruction.RemoveAll();
	m_arrMove.RemoveAll();

	// Обнулить и стереть все шарики, включая next:
	for(INT i = 0 ; i < 9 ; i++)
		for(INT j = 0 ; j < 9 ; j++)
			if(m_Field.GetCellState(i, j) != Cell::CellState::FREE)
				m_arrInvalidate.Add(CPoint(i, j));
	m_Field.Restart();
	InvalidateQueue();
	InvalidateNext();

	// Остановка главного таймера:
	KillTimer(m_nTimerIDEvent_Main);
	m_nTimerIDEvent_Main = 0;
	m_Time = CTimeSpan(0, 0, 0, 0);
	InvalidateTime();

	// Стереть свои очки:
	m_nScores = 0;
	InvalidateMyScore();
}


BOOL CMainDlg::SaveGame(CString strFile, CString* pstrError/* = NULL*/)
{
	// Токрыть файл для записи:
	CFileException ex;
	CFile file;
	if(!file.Open(strFile, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary, &ex))
	{
		if(pstrError)
			ex.GetErrorMessage(pstrError->GetBufferSetLength(1023), 1024);
		return FALSE;
	}

	// Записать преамбулу:
	char sPreambula[] = "Lines_by_BUBLiK__Save";
	file.Write(sPreambula, sizeof(sPreambula));

	// Записать время:
	LONG lHours = m_Time.GetHours();
	LONG lMinutes = m_Time.GetMinutes();
	LONG lSeconds = m_Time.GetSeconds();
	file.Write(&lHours, sizeof(LONG));
	file.Write(&lMinutes, sizeof(LONG));
	file.Write(&lSeconds, sizeof(LONG));

	// Записать поле:
	for(int i = 0 ; i < 9 ; i++)
		file.Write(m_Field.GetPtrCells(i), sizeof(Cell) * 9);

	// Записать next:
	file.Write(&m_Field.GetRefNextCount(), sizeof(UINT));
	file.Write(m_Field.GetPtrNextBalls(), sizeof(CPoint) * m_Field.GetNumberOfNext());
	file.Write(m_Field.GetPtrNextColors(), sizeof(INT) * m_Field.GetNumberOfNext());

	// Записать набранные очки:
	file.Write(&m_nScores, sizeof(INT));

	return TRUE;
}

BOOL CMainDlg::LoadGame(CString strFile, CString* pstrError/* = NULL*/)
{
	// Токрыть файл для чтения:
	CFileException ex;
	CFile file;
	if(!file.Open(strFile, CFile::modeRead | CFile::typeBinary, &ex))
	{
		if(pstrError)
			ex.GetErrorMessage(pstrError->GetBufferSetLength(1023), 1024);
		return FALSE;
	}

	// Проверить преамбулу:
	char sPreambula[sizeof("Lines_by_BUBLiK__Save")];
	file.Read(sPreambula, sizeof("Lines_by_BUBLiK__Save"));
	if (strcmp(sPreambula, "Lines_by_BUBLiK__Save"))
	{
		if(pstrError)
			*pstrError = _T("Это не файл сохранения!");
		return FALSE;
	}

	TRACE(_T("\n############################### GAME LOADED ################################\n\n"));

	// СТЕРЕТЬ ВСЁ !!!
	KillAll();

	// Загрузить время:
	LONG lHours;
	LONG lMinutes;
	LONG lSeconds;
	file.Read(&lHours, sizeof(LONG));
	file.Read(&lMinutes, sizeof(LONG));
	file.Read(&lSeconds, sizeof(LONG));
	m_Time = CTimeSpan(0, lHours, lMinutes, lSeconds);

	// Загрузить поле:
	for(int i = 0 ; i < 9 ; i++)
		file.Read(m_Field.GetPtrCells(i), sizeof(Cell) * 9);

	// Загрузить next:
	file.Read(&m_Field.GetRefNextCount(), sizeof(UINT));
	file.Read(m_Field.GetPtrNextBalls(), sizeof(CPoint) * m_Field.GetNumberOfNext());
	file.Read(m_Field.GetPtrNextColors(), sizeof(INT) * m_Field.GetNumberOfNext());

	// Загрузить набранные очки:
	file.Read(&m_nScores, sizeof(INT));

	// Обновить поле:
	for(INT i = 0 ; i < 9 ; i++)
		for(INT j = 0 ; j < 9 ; j++)
			if(m_Field.GetCellState(i, j) != Cell::CellState::FREE)
				m_arrInvalidate.Add(CPoint(i, j));
	InvalidateQueue();

	// Обновить всё остальное:
	InvalidateNext();
	InvalidateMyScore();
	InvalidateTime();

	// Продолжение работы таймера:
	m_nTimerIDEvent_Main = SetTimer(m_uTimerID_Main, 1000, NULL);

	return TRUE;
}


BOOL CMainDlg::LoadBestResults()
{
	// Открыть файл для чтения:
	CFileException ex;
	CFile file;
	if(!file.Open(GetProgramDir() + _T("\\Records.dat"), CFile::modeRead | CFile::typeBinary, &ex))
	{
		TCHAR* szError = new TCHAR[1024];
		ex.GetErrorMessage(szError, 1024);
		AfxMessageBox( /*szError*/ _T("Таблица рекордов не загружена!\nНе найден файл Records.dat!") );
		delete [] szError;
		return FALSE;
	}

	// Проверить преамбулу:
	char sPreambula[sizeof("Lines_by_BUBLiK__Best_Results")];
	file.Read(sPreambula, sizeof("Lines_by_BUBLiK__Best_Results"));
	if (strcmp(sPreambula, "Lines_by_BUBLiK__Best_Results"))
	{
		AfxMessageBox(_T("Файл Records.dat не является файлом результатов!"));
		file.Close();
		return FALSE;
	}

	// Считать размер массива лучших результатов:
	UINT uSizeOfResultsArray;
	file.Read(&uSizeOfResultsArray, sizeof(UINT));
	m_arrResults.RemoveAll();
	m_arrResults.SetSize(min(uSizeOfResultsArray, MAX_RESULTS));

	// Считать последовательно каждый рез-т в массив:
	UINT uSizeBuffer;
	LONG lHours;
	LONG lMinutes;
	LONG lSeconds;
	for(int i = 0 ; i < m_arrResults.GetCount() ; i++)
	{
		file.Read(&uSizeBuffer, sizeof(UINT));
		file.Read(m_arrResults[i].name.GetBufferSetLength(uSizeBuffer), sizeof(TCHAR) * (uSizeBuffer + 1));
		file.Read(&lHours, sizeof(LONG));
		file.Read(&lMinutes, sizeof(LONG));
		file.Read(&lSeconds, sizeof(LONG));
		m_arrResults[i].time = CTimeSpan(0, lHours, lMinutes, lSeconds);
		file.Read(&m_arrResults[i].scores, sizeof(UINT));
	}

	// Загрузка HIGH SCORES:
	if(!m_arrResults.IsEmpty())
		m_nHighScores = m_arrResults[0].scores;
	InvalidateHighScore();

	file.Close();
	return TRUE;
}

INT CMainDlg::WhatResult(UINT nScores)
{
	INT iRecord = -1;

	if(!nScores)
		return -1;

	if (m_arrResults.IsEmpty())
	{
		iRecord = 0;
	}
	else
	{
		for(int i = 0 ; i < m_arrResults.GetCount() ; i++)
		{
			if(nScores >= m_arrResults[i].scores)
			{
				iRecord = i;
				break;
			}
		}

		if(iRecord == -1 && m_arrResults.GetCount() < MAX_RESULTS)
			iRecord = m_arrResults.GetCount();
	}

	TRACE(_T("Result = %d\n"), iRecord);
	return iRecord;
}

BOOL CMainDlg::SaveResult(UINT nScores, CTimeSpan Time)
{
	// Найти заработанную позицию в списке рекордов:
	INT iRecord = WhatResult(nScores);

	// Не попал в TOP10:
	if(iRecord == -1)
		return FALSE;

	// Диалог ввода имени:
	CEnterNameDialog dlgEnterName;
	CString strScores, strTime, strPosition;
	strScores.Format(_T("%d"), nScores);
	strTime = Time.Format(_T("%H:%M:%S"));
	strPosition.Format(_T("%d"), iRecord + 1);
	dlgEnterName.m_strCongratulations.Replace(_T("xxx"), strScores);
	dlgEnterName.m_strCongratulations.Replace(_T("yyy"), strTime);
	dlgEnterName.m_strCongratulations.Replace(_T("zzz"), strPosition);

	// Если юзер ввёл имя:
	if(dlgEnterName.DoModal() == IDOK)
	{
		// Запись рекорда:
		Result result;
		result.name = dlgEnterName.m_strName;
		result.scores = nScores;
		result.time = Time;
		m_arrResults.InsertAt(iRecord, result);

		// Удалить побитые рекорды:
		if(m_arrResults.GetCount() > MAX_RESULTS)
			for(int i = m_arrResults.GetCount() - 1 ; i >= MAX_RESULTS ; i--)
				m_arrResults.RemoveAt(i);

		// Открыть файл для записи:
		CFileException ex;
		CFile file;
		if(!file.Open(GetProgramDir() + _T("\\Records.dat"), CFile::modeWrite | CFile::modeCreate | CFile::typeBinary, &ex))
		{
			TCHAR* szError = new TCHAR[1024];
			ex.GetErrorMessage(szError, 1024);
			AfxMessageBox(szError);
			delete [] szError;
			return FALSE;
		}

		// Вписать в начало преамбулу:
		char sPreambula[] = "Lines_by_BUBLiK__Best_Results";
		file.Write(sPreambula, sizeof(sPreambula));

		// Вписать все рекорды:
		UINT uSizeOfResultsArray = m_arrResults.GetCount();
		file.Write(&uSizeOfResultsArray, sizeof(UINT));

		LONG lHours;
		LONG lMinutes;
		LONG lSeconds;
		UINT uSizeBuffer;
		for(int i = 0 ; i < m_arrResults.GetCount() ; i++)
 		{
			uSizeBuffer = m_arrResults[i].name.GetLength();
			file.Write(&uSizeBuffer, sizeof(UINT));
			file.Write(m_arrResults[i].name.GetBufferSetLength(uSizeBuffer), sizeof(TCHAR) * (uSizeBuffer + 1));
			lHours = m_arrResults[i].time.GetHours();
			lMinutes = m_arrResults[i].time.GetMinutes();
			lSeconds = m_arrResults[i].time.GetSeconds();
			file.Write(&lHours, sizeof(LONG));
			file.Write(&lMinutes, sizeof(LONG));
			file.Write(&lSeconds, sizeof(LONG));
			file.Write(&m_arrResults[i].scores, sizeof(UINT));
		}

		file.Close();

		// Подняться на вершину:
		if(iRecord == 0)
		{
			m_nHighScores = nScores;
			InvalidateHighScore();
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}



void CMainDlg::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	if(nHitTest == HTSYSMENU)
		TRACE(_T("Non-client mouse click = HTSYSMENU\n"));

	CDialog::OnNcLButtonDown(nHitTest, point);
}




// --------------------------------- Обработчики нажатия кнопок ----------------------------

void CMainDlg::OnBnClicked_Start()
{
	if(m_nTimerIDEvent_Main)
		EndGame();
	StartNewGame();
}

void CMainDlg::OnBnClicked_Stop()
{
	if(m_nTimerIDEvent_Main)
		EndGame();
}

void CMainDlg::OnBnClicked_Save()
{
	if(!m_nTimerIDEvent_Main)
	{
		AfxMessageBox(_T("Нельзя сохранить неначатую игру!"));
		return;
	}

	CFileDialog dlgSaveFile(FALSE, _T("sav"), GetProgramDir() + _T("\\Saves\\*.sav"), OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY /* | OFN_EXTENSIONDIFFERENT*/ | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, _T("Save Files for Lines by BIBLiK (*.sav)|*.sav||"), this);
	dlgSaveFile.m_ofn.lpstrTitle = _T("Сохранить игру в файл");
	INT_PTR iResult;

	if(m_nTimerIDEvent_Main)
	{
		KillTimer(m_nTimerIDEvent_Main);
		iResult = dlgSaveFile.DoModal();
		m_nTimerIDEvent_Main = SetTimer(m_uTimerID_Main, 1000, NULL);
	}
	else
	{
		iResult = dlgSaveFile.DoModal();
	}
	
	if(iResult == IDOK)
	{
		CString strError;
		CString strFullPath = dlgSaveFile.GetPathName();
		if(!SaveGame(strFullPath, &strError))
		{
			CString strMsg;
			strMsg.Format(_T("Произошла ошибка при сохранении игры в файл %s:\n%s"), strFullPath, strError);
			if(m_nTimerIDEvent_Main)
			{
				KillTimer(m_nTimerIDEvent_Main);
				AfxMessageBox(strMsg, MB_OK | MB_ICONEXCLAMATION);
				m_nTimerIDEvent_Main = SetTimer(m_uTimerID_Main, 1000, NULL);
			}
			else
			{
				AfxMessageBox(strMsg, MB_OK | MB_ICONEXCLAMATION);
			}
		}
		else
		{
			if(m_nTimerIDEvent_Main)
			{
				KillTimer(m_nTimerIDEvent_Main);
				AfxMessageBox(CString(_T("Игра успешно сохранена в файл \n")) + strFullPath, MB_OK | MB_ICONINFORMATION);
				m_nTimerIDEvent_Main = SetTimer(m_uTimerID_Main, 1000, NULL);
			}
			else
			{
				AfxMessageBox(CString(_T("Игра успешно сохранена в файл \n")) + strFullPath, MB_OK | MB_ICONINFORMATION);
			}
		}
	}
}

void CMainDlg::OnBnClicked_Load()
{
	CFileDialog dlgLoadFile(TRUE, _T("sav"), GetProgramDir() + _T("\\Saves\\*.sav"), OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY /* | OFN_EXTENSIONDIFFERENT*/ | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, _T("Save Files for Lines by BIBLiK (*.sav)|*.sav||"), this);
	dlgLoadFile.m_ofn.lpstrTitle = _T("Загрузить игру из файла");
	INT_PTR iResult;

	if(m_nTimerIDEvent_Main)
	{
		KillTimer(m_nTimerIDEvent_Main);
		iResult = dlgLoadFile.DoModal();
		m_nTimerIDEvent_Main = SetTimer(m_uTimerID_Main, 1000, NULL);
	}
	else
	{
		iResult = dlgLoadFile.DoModal();
	}

	if(iResult == IDOK)
	{
		CString strError;
		CString strFullPath = dlgLoadFile.GetPathName();
		if(!LoadGame(strFullPath, &strError))
		{
			CString strMsg;
			strMsg.Format(_T("Произошла ошибка при загрузки игры из файла %s:\n%s"), strFullPath, strError);
			
			if(m_nTimerIDEvent_Main)
			{
				KillTimer(m_nTimerIDEvent_Main);
				AfxMessageBox(strMsg, MB_OK | MB_ICONEXCLAMATION);
				m_nTimerIDEvent_Main = SetTimer(m_uTimerID_Main, 1000, NULL);
			}
			else
			{
				AfxMessageBox(strMsg, MB_OK | MB_ICONEXCLAMATION);
			}
		}
// 		else
// 		{
// 			if(m_nTimerIDEvent_Main)
// 			{
// 				KillTimer(m_nTimerIDEvent_Main);
// 				AfxMessageBox(CString(_T("Игра успешно загружена из файла \n")) + strFullPath, MB_OK | MB_ICONINFORMATION);
// 				m_nTimerIDEvent_Main = SetTimer(m_uTimerID_Main, 1000, NULL);
// 			}
// 			else
// 			{
// 				AfxMessageBox(CString(_T("Игра успешно загружена из файла \n")) + strFullPath, MB_OK | MB_ICONINFORMATION);
// 			}
// 		}
	}
}

void CMainDlg::OnBnClicked_Settings()
{

}

void CMainDlg::OnBnClicked_Skins()
{
	LoadSkin();
}

void CMainDlg::OnBnClicked_Best()
{
	CBestScoresDialog dlgBestScores(&m_arrResults, MAX_RESULTS, m_pFontBroadway_small, m_pfontMonotypeCorsiva, m_ppImagesBalls, 8);
	dlgBestScores.DoModal();
}

void CMainDlg::OnBnClicked_Help()
{
	CRulesDlg dlgRules(m_pFontBroadway, m_ppImagesBalls, 8);
	dlgRules.DoModal();
}

void CMainDlg::OnBnClicked_About()
{
	CAboutDlg dlgAbout(m_pFontBroadway, m_pFontBroadway_small_italic, m_ppImagesBalls, 8, m_Skin.copyrights.strCopyrightLine1 + _T("\n") + m_Skin.copyrights.strCopyrightLine2 + _T("\n") + m_Skin.copyrights.strDeveloperContactInfo);
	dlgAbout.DoModal();
}

BOOL CMainDlg::LoadSkin(BOOL bIsFirstLoad/* = FALSE*/)
{
	CFileDialog dlgSkin(TRUE, _T("skn"), GetProgramDir() + _T("\\Skins\\*.skn"), OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY /* | OFN_EXTENSIONDIFFERENT*/ | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, _T("Skins for Lines by BIBLiK (*.skn)|*.skn||"), this);
	dlgSkin.m_ofn.lpstrTitle = _T("Загрузить скин");
	INT_PTR iResult;

	if(m_nTimerIDEvent_Main)
	{
		KillTimer(m_nTimerIDEvent_Main);
		iResult = dlgSkin.DoModal();
		m_nTimerIDEvent_Main = SetTimer(m_uTimerID_Main, 1000, NULL);
	}
	else
	{
		iResult = dlgSkin.DoModal();
	}

	if(iResult == IDOK)
	{
	// ---------------------------------------------------------------------------------------------------
	// Удалить файлы старого скина:
		try	{ CFile::Remove(GetProgramDir() + _T("\\CurSkin\\Skin.ini")); }
		catch(CFileException* e)
		{
			if( e->m_cause == CFileException::fileNotFound || e->m_cause == CFileException::accessDenied )
			{
//				LPTSTR szError = new TCHAR[1000];
//				e->GetErrorMessage(szError, 1000);
//				AfxMessageBox(szError);
			}
			e->Delete();
		}

		try	{ CFile::Remove(m_Skin.files.strBackground); }
		catch(CFileException* e)
		{
			if( e->m_cause == CFileException::fileNotFound || e->m_cause == CFileException::accessDenied )
			{
//				LPTSTR szError = new TCHAR[1000];
//				e->GetErrorMessage(szError, 1000);
//				AfxMessageBox(szError);
			}
			e->Delete();
		}

		try	{ CFile::Remove(m_Skin.files.strBalls); }
		catch(CFileException* e)
		{
			if( e->m_cause == CFileException::fileNotFound || e->m_cause == CFileException::accessDenied )
			{
//				LPTSTR szError = new TCHAR[1000];
//				e->GetErrorMessage(szError, 1000);
//				AfxMessageBox(szError);
			}
			e->Delete();
		}

		try	{ CFile::Remove(m_Skin.files.strNext); }
		catch(CFileException* e)
		{
			if( e->m_cause == CFileException::fileNotFound || e->m_cause == CFileException::accessDenied )
			{
//				LPTSTR szError = new TCHAR[1000];
//				e->GetErrorMessage(szError, 1000);
//				AfxMessageBox(szError);
			}
			e->Delete();
		}

		try	{ CFile::Remove(m_Skin.files.strScore); }
		catch(CFileException* e)
		{
			if( e->m_cause == CFileException::fileNotFound || e->m_cause == CFileException::accessDenied )
			{
//				LPTSTR szError = new TCHAR[1000];
//				e->GetErrorMessage(szError, 1000);
//				AfxMessageBox(szError);
			}
			e->Delete();
		}

		try	{ CFile::Remove(m_Skin.files.strTime); }
		catch(CFileException* e)
		{
			if( e->m_cause == CFileException::fileNotFound || e->m_cause == CFileException::accessDenied )
			{
//				LPTSTR szError = new TCHAR[1000];
//				e->GetErrorMessage(szError, 1000);
//				AfxMessageBox(szError);
			}
			e->Delete();
		}

		try	{ CFile::Remove(m_Skin.files.strType); }
		catch(CFileException* e)
		{
			if( e->m_cause == CFileException::fileNotFound || e->m_cause == CFileException::accessDenied )
			{
//				LPTSTR szError = new TCHAR[1000];
//				e->GetErrorMessage(szError, 1000);
//				AfxMessageBox(szError);
			}
			e->Delete();
		}
	// ---------------------------------------------------------------------------------------------------

		CString strFullPath = dlgSkin.GetPathName();
		CW2A pBuffer(strFullPath);

		Cabinet::CExtract cabSkin;
		if(!cabSkin.CreateFDIContext())
		{
			CString strMsg;
			strMsg.Format(_T("Ошибка создания FDI контекста для распаковки архива\n%s"), strFullPath);
			if(m_nTimerIDEvent_Main)
			{
				KillTimer(m_nTimerIDEvent_Main);
				AfxMessageBox(strMsg, MB_OK | MB_ICONEXCLAMATION);
				m_nTimerIDEvent_Main = SetTimer(m_uTimerID_Main, 1000, NULL);
			}
			else
			{
				AfxMessageBox(strMsg, MB_OK | MB_ICONEXCLAMATION);
			}
			return FALSE;
		}
		else if(!cabSkin.IsCabinet(pBuffer, NULL))
		{
			CString strMsg;
			strMsg.Format(_T("Файл %s не является CAB-архивом!"), strFullPath);
			if(m_nTimerIDEvent_Main)
			{
				KillTimer(m_nTimerIDEvent_Main);
				AfxMessageBox(strMsg, MB_OK | MB_ICONEXCLAMATION);
				m_nTimerIDEvent_Main = SetTimer(m_uTimerID_Main, 1000, NULL);
			}
			else
			{
				AfxMessageBox(strMsg, MB_OK | MB_ICONEXCLAMATION);
			}
			return FALSE;
		}
		else if(!cabSkin.ExtractFile(pBuffer, CW2A(GetProgramDir() + _T("\\CurSkin"))))
		{
			CString strMsg;
			strMsg.Format(_T("Ошибка распаковки архива\n%s"), strFullPath);
			if(m_nTimerIDEvent_Main)
			{
				KillTimer(m_nTimerIDEvent_Main);
				AfxMessageBox(strMsg, MB_OK | MB_ICONEXCLAMATION);
				m_nTimerIDEvent_Main = SetTimer(m_uTimerID_Main, 1000, NULL);
			}
			else
			{
				AfxMessageBox(strMsg, MB_OK | MB_ICONEXCLAMATION);
			}
			return FALSE;
		}
		else
		{
			if(!InitializeSkinValues(TRUE))
				return FALSE;

			if(!LoadImages(TRUE))
				return FALSE;

			if(!bIsFirstLoad)
			{
				ResizeWindowAndMoveButtons();
				Invalidate();
			}

			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}
}


void CMainDlg::ResizeWindowAndMoveButtons()
{
	// Подстроить размеры окна под картинку background
	CRect rectClient, rectWindow;
	GetClientRect(rectClient);
	GetWindowRect(rectWindow);
	rectWindow.right = rectWindow.right - rectClient.Width() + m_Skin.images.imgBackground.GetWidth() + RIGHT_PANEL_WIDTH;
	rectWindow.bottom = rectWindow.bottom - rectClient.Height() + m_Skin.images.imgBackground.GetHeight();
	MoveWindow(rectWindow);

	// Переместить кнопки
	GetClientRect(rectClient);
	UINT uBetweenBtns = (rectClient.Height() - 48 * 8) / (2 * 2 + 7);
	CPoint ptBtn = CPoint(rectClient.right - RIGHT_PANEL_WIDTH + (RIGHT_PANEL_WIDTH - 48) / 2, rectClient.top + uBetweenBtns * 2);
	CSize szBtn = CSize(48, 48);
	
	m_pBtnStartNewGame->MoveWindow(CRect(ptBtn, szBtn));
	ptBtn.y += (uBetweenBtns + szBtn.cy);
	m_pBtnStopGame->MoveWindow(CRect(ptBtn, szBtn));
	ptBtn.y += (uBetweenBtns + szBtn.cy);
	m_pBtnSaveGameToFile->MoveWindow(CRect(ptBtn, szBtn));
	ptBtn.y += (uBetweenBtns + szBtn.cy);
	m_pBtnLoadGameFromFile->MoveWindow(CRect(ptBtn, szBtn));
	ptBtn.y += (uBetweenBtns + szBtn.cy);
	m_pBtnSkins->MoveWindow(CRect(ptBtn, szBtn));
	ptBtn.y += (uBetweenBtns + szBtn.cy);
	m_pBtnTableOfBestResults->MoveWindow(CRect(ptBtn, szBtn));
	ptBtn.y += (uBetweenBtns + szBtn.cy);
	m_pBtnHelp->MoveWindow(CRect(ptBtn, szBtn));
	ptBtn.y += (uBetweenBtns + szBtn.cy);
	m_pBtnAbout->MoveWindow(CRect(ptBtn, szBtn));
	ptBtn.y += (uBetweenBtns + szBtn.cy);

}
void CMainDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if(nType == SIZE_MINIMIZED && !bTimerStopped && m_nTimerIDEvent_Main)
	{
		KillTimer(m_nTimerIDEvent_Main);
		m_nTimerIDEvent_Main = 0;
		bTimerStopped = TRUE;
	}
	else if(nType == SIZE_RESTORED && bTimerStopped && !m_nTimerIDEvent_Main)
	{
		m_nTimerIDEvent_Main = SetTimer(m_uTimerID_Main, 1000, NULL);
		bTimerStopped = FALSE;
	}
}
