// MainDlg.h : header file
//

#pragma once

#include "Skin.h"				// структура Skin (параметры скина для игры)
#include "Field.h"				// класс Filed (поле для игры)
#include "EnterNameDialog.h"	// диалог ввода имени
#include "BtnST.h"				// навороченные кнопочки
#include "ExtractT.hpp"			// библиотека cab
#include "RulesDlg.h"			// диалог "Правила игры в LiNES"
#include "AboutDlg.h"			// диалог "О порграмме LiNES by BUBLiK"
#include "BestScoresDialog.h"	// диалог лучших результатов
#include "SimpleIni.h"			// класс для ini-files
#include "PPTooltip.h"			// ToolTip

#define RIGHT_PANEL_WIDTH 60
#define MAX_RESULTS 10



// CMainDlg dialog
class CMainDlg : public CDialog
{
// Construction
public:
	CMainDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_LINES_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:

	CString GetProgramDir()
	{
		CString strProgramDir;
		TCHAR szFileName[MAX_PATH + 1];
		GetModuleFileName(AfxGetInstanceHandle(), szFileName, MAX_PATH);
		strProgramDir = szFileName;
		strProgramDir = strProgramDir.Left(strProgramDir.ReverseFind('\\'));
		return strProgramDir;
	}

// Is Minimized?
	BOOL bTimerStopped;

// Доска
	Field m_Field;

// Параметры скина
	Skin m_Skin;

// Загрузить параметры из SKIN.INI:
	BOOL InitializeSkinValues(BOOL bErrMsg);

// Загрузить изображения:
	BOOL LoadImages(BOOL bErrMsg);

// Переопределить размеры окна и передвинуть кнопочки
	void ResizeWindowAndMoveButtons();

// Загрузить скин из файла *.skn
	BOOL LoadSkin(BOOL bIsFirstLoad = FALSE);

// Время и таймеры
	CTimeSpan m_Time;
	void InvalidateTime();

	UINT		m_uTimerID_Main;
	UINT_PTR	m_nTimerIDEvent_Main;

	UINT		m_uTimerID_Jump;
	UINT_PTR	m_nTimerIDEvent_Jump;

	UINT		m_uTimerID_Appearence;
	UINT_PTR	m_nTimerIDEvent_Appearence;

	UINT		m_uTimerID_Destruction;
	UINT_PTR	m_nTimerIDEvent_Destruction;

	UINT		m_uTimerID_Move;
	UINT_PTR	m_nTimerIDEvent_Move;

// Очки
	INT m_nScores;
	INT m_nHighScores;
	void InvalidateMyScore();
	void InvalidateHighScore();

// Масивы шаров для анимации:
	CArray<CPoint, CPoint> m_arrInvalidate;
	CArray<CPoint, CPoint> m_arrAppearence;
	CArray<CPoint, CPoint> m_arrDestruction;
	CArray<CPoint, CPoint> m_arrMove;

// Текущие кадры анимации:
	INT m_iCurrentFrame_Jump;
	INT m_iCurrentFrame_Appearence;
	INT m_iCurrentFrame_Destruction;

// Для анимации:
	// Заглушка (используется в OnPaint) для процесса перемещения шарика
	CPoint m_ptMoving;
	// Выбранные шарики
	CPoint m_ptSelStart;
	// Фигня одна:
	BOOL bPrepareNextAfterDestroy;

// Вызвать Invalidate() для каждой клетки массива m_arrInvalidate
	void InvalidateQueue();

// Показать следующие шары
	void InvalidateNext();

// Управление игрой:
	void StartNewGame();
	void EndGame();
	void KillAll();
	void Next(BOOL bGrow = TRUE);
	BOOL SaveGame(CString strFile, CString* pstrError = NULL);
	BOOL LoadGame(CString strFile, CString* pstrError = NULL);

// Результаты
	CArray<Result, Result> m_arrResults;
	BOOL LoadBestResults();
	BOOL SaveResult(UINT nScores, CTimeSpan Time);
	INT WhatResult(UINT nScores);

// Кнопки
	CButtonST* m_pBtnStartNewGame;
	CButtonST* m_pBtnStopGame;
	CButtonST* m_pBtnSaveGameToFile;
	CButtonST* m_pBtnLoadGameFromFile;
	CButtonST* m_pBtnSettings;
	CButtonST* m_pBtnSkins;
	CButtonST* m_pBtnTableOfBestResults;
	CButtonST* m_pBtnHelp;
	CButtonST* m_pBtnAbout;

// Обработчики сообщений:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);

	afx_msg void OnBnClicked_Start();
	afx_msg void OnBnClicked_Stop();
	afx_msg void OnBnClicked_Save();
	afx_msg void OnBnClicked_Load();
	afx_msg void OnBnClicked_Settings();
	afx_msg void OnBnClicked_Skins();
	afx_msg void OnBnClicked_Best();
	afx_msg void OnBnClicked_Help();
	afx_msg void OnBnClicked_About();

// Функция получения массива int-значений из 1 ключа ini-файла
	void GetMultiIniValues(CSimpleIni& ini, CString strSection, CString strKey, CWordArray* arr)
	{
		arr->RemoveAll();
		CString strTemp = ini.GetValue(strSection, strKey);

		if(strTemp.Find(_T(',')) == -1)
		{
			arr->Add(StrToInt(strTemp));
			return;
		}

		int iCurPos = 0;
		CString strResult = strTemp.Tokenize(_T(","), iCurPos);
		while (!strResult.IsEmpty())
		{
			arr->Add(StrToInt(strResult));
			strResult = strTemp.Tokenize(_T(","), iCurPos);
		}
	}

	// Всплывающие подсказки
	CPPToolTip m_tooltip;

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	PrivateFontCollection m_fontCollectionBroadway;
	PrivateFontCollection m_fontCollectionCorsiva;

	Font* m_pfontMonotypeCorsiva;
	Font* m_pFontBroadway;
	Font* m_pFontBroadway_small;
	Font* m_pFontBroadway_small_italic;

	Image **m_ppImagesBalls;

	void LoadFonts()
	{
		int numFamilies = 1;
		FontFamily fontFamilies[1];

		m_fontCollectionBroadway.AddFontFile(GetProgramDir() + _T("\\Fonts\\BROADW.TTF"));
		m_fontCollectionBroadway.GetFamilies(1, fontFamilies, &numFamilies);
		m_pFontBroadway = new Font(&fontFamilies[0], 32, FontStyleBold, UnitPoint);
		m_pFontBroadway_small = new Font(&fontFamilies[0], 26, FontStyleBold, UnitPoint);
		m_pFontBroadway_small_italic = new Font(&fontFamilies[0], 20, FontStyleItalic, UnitPoint);

		m_fontCollectionCorsiva.AddFontFile(GetProgramDir() + _T("\\Fonts\\HEINRICH.TTF"));
		m_fontCollectionCorsiva.GetFamilies(1, fontFamilies, &numFamilies);
		m_pfontMonotypeCorsiva = new Font(&fontFamilies[0], 16, FontStyleRegular, UnitPoint);
	}

	void DeleteFonts()
	{
		if(m_pfontMonotypeCorsiva)
			delete m_pfontMonotypeCorsiva;
		if(m_pFontBroadway)
			delete m_pFontBroadway;
		if(m_pFontBroadway_small)
			delete m_pFontBroadway_small;
		if(m_pFontBroadway_small_italic)
			delete m_pFontBroadway_small_italic;
	}

	void LoadImagesBalls()
	{
		m_ppImagesBalls = new Image*[8];
 		m_ppImagesBalls[0] = new Image(GetProgramDir() + _T("\\Balls\\ball_blue.png"));
 		m_ppImagesBalls[1] = new Image(GetProgramDir() + _T("\\Balls\\ball_red.png"));
 		m_ppImagesBalls[2] = new Image(GetProgramDir() + _T("\\Balls\\ball_green.png"));
 		m_ppImagesBalls[3] = new Image(GetProgramDir() + _T("\\Balls\\ball_yellow.png"));
 		m_ppImagesBalls[4] = new Image(GetProgramDir() + _T("\\Balls\\ball_white_light.png"));
 		m_ppImagesBalls[5] = new Image(GetProgramDir() + _T("\\Balls\\ball_red_light.png"));
 		m_ppImagesBalls[6] = new Image(GetProgramDir() + _T("\\Balls\\ball_green_light.png"));
 		m_ppImagesBalls[7] = new Image(GetProgramDir() + _T("\\Balls\\ball_blue_light.png"));
	}

	void DeleteImagesBalls()
	{
		if(m_ppImagesBalls)
		{
			for(int i = 0 ; i < 8 ; i++)
				delete [] m_ppImagesBalls[i];
			delete [] m_ppImagesBalls;
		}
	}
};
