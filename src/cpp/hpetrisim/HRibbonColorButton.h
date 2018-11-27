/**************************************************************************
	HRibbonColorButton.h

	copyright (c) 2013/07/11 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once


// CHRibbonColorButton

class CHRibbonColorButton : public CMFCRibbonColorButton
{
	DECLARE_DYNCREATE(CHRibbonColorButton)

public:
	CHRibbonColorButton();
	CHRibbonColorButton(UINT nID, LPCTSTR lpszText, BOOL bSimpleButtonLook, int nSmallImageIndex, int nLargeImageIndex, COLORREF color = RGB(0, 0, 0));
	virtual ~CHRibbonColorButton();
	static void CreateDocumentColors();
private:
	static CList<COLORREF,COLORREF> m_lstMainColors;
	static CList<COLORREF,COLORREF> m_lstAdditionalColors;
	static CList<COLORREF,COLORREF> m_lstStandardColors;

};


