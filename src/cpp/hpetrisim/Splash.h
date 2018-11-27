/**************************************************************************
	Splash.h
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#ifndef _SPLASH_SCRN_
#define _SPLASH_SCRN_

// Splash.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen Klasse

class CSplashWnd : public CWnd
{
// Konstruktion
protected:
	CSplashWnd();

// Attribute:
public:
	CBitmap m_bitmap;

// Operationen
public:
	static void EnableSplashScreen(BOOL bEnable = TRUE);
	static void ShowSplashScreen(CWnd* pParentWnd = NULL);
	static BOOL PreTranslateAppMessage(MSG* pMsg);

// Überschreibungen
	// Vom Klassen-Assistenten generierte Überschreibungen virtueller Funktionen
	//{{AFX_VIRTUAL(CSplashWnd)
	//}}AFX_VIRTUAL

// Implementierung
public:
	~CSplashWnd();
	virtual void PostNcDestroy();

protected:
	BOOL Create(CWnd* pParentWnd = NULL);
	void HideSplashScreen();
	static BOOL c_bShowSplashWnd;
	static CSplashWnd* c_pSplashWnd;

// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CSplashWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
