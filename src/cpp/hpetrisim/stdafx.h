/**************************************************************************
	StdAfx.h
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#include "targetver.h"

#include <afxwin.h>
#include <afxext.h>
#include <afxdisp.h> 
#include <afxtempl.h>
#include <afxcontrolbars.h>
#include <afxpriv.h>
#include <strsafe.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>

#include "mmsystem.h"	// for mm timer
#pragma comment(lib, "winmm.lib")

#include <math.h>

#include "HConst.h" // some defines for PetriSim

// DrawClient (MFC Sample)
#include "DrawClient/RibbonListButton.h"

// QTracker/ GDI+
#define _winmajor 5
#include "QTransformTracker/QGdiPlus.h"
#include "QTransformTracker/QBufferDC.h"
#include "QTransformTracker/QTracker.h"
#include "QTransformTracker/QTransformTracker.h"

// ZoomView
#include "OXZoomView/OXZOOMVW.h"
#include <afx.h>

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
