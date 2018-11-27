/**************************************************************************
	PreviewWnd.h
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// CPreviewWnd Fenster

#ifndef __PREVIEWWND_H__
#define __PREVIEWWND_H__

class CPreviewWnd : public CWnd
{
// Konstruktion
public:
	CPreviewWnd();

// Attribute
public:

// Operationen
public:

// Überschreibungen
	// Vom Klassen-Assistenten generierte Überschreibungen virtueller Funktionen
	//{{AFX_VIRTUAL(CPreviewWnd)
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CPreviewWnd();

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CPreviewWnd)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif		// __PREVIEWWND_H__