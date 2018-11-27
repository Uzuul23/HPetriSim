/**************************************************************************
	ChildFrm.cpp
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#include "stdafx.h"
#include "PetriSim.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
END_MESSAGE_MAP()

CChildFrame::CChildFrame()
{

}

CChildFrame::~CChildFrame()
{
}

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	__super::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}

#endif //_DEBUG

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs
								 , CCreateContext* pContext)
{
	//create splitter
	return m_wndSplitter.Create(this, 2, 2, CSize(50, 50), pContext);

	// __super::OnCreateClient( lpcs, pContext);
}



