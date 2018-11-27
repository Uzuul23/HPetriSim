/**************************************************************************
	RibbonEdit.h

	copyright (c) 2013/08/14 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once


// CRibbonEdit

class CRibbonEdit : public CMFCRibbonEdit
{
	DECLARE_DYNAMIC(CRibbonEdit)

public:
	CRibbonEdit(UINT nID, int nWidth, LPCTSTR lpszLabel = NULL, int nImage = -1);
	virtual ~CRibbonEdit();

	void SetEditText(CString strText, bool NotifyOther = true);

};


