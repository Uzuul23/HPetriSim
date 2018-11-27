///////////////////////////////////////////////////////////////////////
// PreviewWnd.cpp : Implementierungsdatei                            //
//                                                                   //
// Written by Henryk Anschuetz (s0140382@rz.fhtw-berlin.de)          //
// Copyright (c) 1999                                                //
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "PreviewWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static CHAR BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreviewWnd

CPreviewWnd::CPreviewWnd()
{
}

CPreviewWnd::~CPreviewWnd()
{
}


BEGIN_MESSAGE_MAP(CPreviewWnd, CWnd)
	//{{AFX_MSG_MAP(CPreviewWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPreviewWnd -Behandlungsroutinen

void CPreviewWnd::OnPaint() 
{
	// ZU ERLEDIGEN: Angepassten Code zum Zeichnen der Vorschau einfügen.
	// So wird hier beispielsweise eine blaue Ellipse gezeichnet.

	CPaintDC dc(this); // Gerätekontext für Zeichnen
	CRect rect;
	GetClientRect(rect);

	CBrush brushNew(RGB(0,0,255));
	CBrush* pBrushOld = dc.SelectObject(&brushNew);
	dc.Ellipse(rect);
	dc.SelectObject(pBrushOld);
}

BOOL CPreviewWnd::OnEraseBkgnd(CDC* pDC) 
{
	// Gleiche Hintergrundfarbe wie die des Dialogfelds verwenden
	//  (Eigenschaftsblatt).

	CWnd* pParentWnd = GetParent();
	HBRUSH hBrush = (HBRUSH)pParentWnd->SendMessage(WM_CTLCOLORDLG,
		(WPARAM)pDC->m_hDC, (LPARAM)pParentWnd->m_hWnd);
	CRect rect;
	GetClientRect(rect);
	pDC->FillRect(&rect, CBrush::FromHandle(hBrush));
	return TRUE;
}
