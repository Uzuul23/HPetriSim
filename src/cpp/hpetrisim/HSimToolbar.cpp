/**************************************************************************
	HSimToolbar.cpp
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#include "stdafx.h"
#include "PetriSimDoc.h"
#include "HSimToolbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CHSimToolbar, CMFCToolBar)

CHSimToolbar::CHSimToolbar()
{
}

CHSimToolbar::~CHSimToolbar()
{
}

BEGIN_MESSAGE_MAP(CHSimToolbar, CMFCToolBar)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

void CHSimToolbar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(nSBCode == SB_THUMBPOSITION)
	{
		CMDIFrameWnd* pFrame = DYNAMIC_DOWNCAST(CMDIFrameWnd, AfxGetMainWnd());
		ASSERT(pFrame);

		CPetriSimDoc* pDoc = DYNAMIC_DOWNCAST(CPetriSimDoc,
			pFrame->MDIGetActive()->GetActiveDocument());

//		if(pDoc)pDoc->SetSimSpeed(nPos);
	}

	CMFCToolBar::OnHScroll(nSBCode, nPos, pScrollBar);
}
