// Lines.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CLinesApp:
// See Lines.cpp for the implementation of this class
//

class CLinesApp : public CWinApp
{
public:
	CLinesApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

// GDI+
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	virtual int ExitInstance();
};

extern CLinesApp theApp;