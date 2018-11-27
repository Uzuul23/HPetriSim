/**************************************************************************
	RibbonEdit.cpp

	copyright (c) 2013/08/14 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#include "stdafx.h"
#include "RibbonEdit.h"


// CRibbonEdit

IMPLEMENT_DYNAMIC(CRibbonEdit, CMFCRibbonEdit)

CRibbonEdit::CRibbonEdit(UINT nID, int nWidth, LPCTSTR lpszLabel /*= NULL*/, int nImage /*= -1*/)
: CMFCRibbonEdit(nID, nWidth, lpszLabel, nImage)
{

}

CRibbonEdit::~CRibbonEdit()
{
}

void CRibbonEdit::SetEditText( CString strText, bool NotifyOther /*= true*/ )
{
	if (NotifyOther)
	{
		CMFCRibbonEdit::SetEditText(strText);
		return;
	}

	BOOL bDontNotify = m_bDontNotify;

	m_bDontNotify = TRUE;
	CMFCRibbonEdit::SetEditText(strText);
	m_bDontNotify = bDontNotify;
}


