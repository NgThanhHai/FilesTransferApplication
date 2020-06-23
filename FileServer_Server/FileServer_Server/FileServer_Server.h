
// FileServer_Server.h : main header file for the PROJECT_NAME application
//

#pragma once



#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "afxwin.h"
#include "stdlib.h"
#include "time.h"
#include <vector>
#include "afxcmn.h"
#include "Resource.h"

// CFileServerServerApp:
// See FileServer_Server.cpp for the implementation of this class
//

class CFileServerServerApp : public CWinApp
{
public:
	CFileServerServerApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CFileServerServerApp theApp;
