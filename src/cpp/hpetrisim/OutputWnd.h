/**************************************************************************
	OutputWnd.h
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#pragma once

// COutputWndTab

class COutputWndTab : public CEdit
{

public:
	COutputWndTab();
	virtual ~COutputWndTab();

	void SetText(const CString& text);
	void Show();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);

	DECLARE_MESSAGE_MAP()
	
};


// COutputWnd

class COutputWnd : public CDockablePane
{
	DECLARE_DYNAMIC(COutputWnd)

public:
	COutputWnd();
	virtual ~COutputWnd();

	COutputWndTab* Add(const CString & name);
	void Remove(COutputWndTab* pWnd);
	void Label(COutputWndTab* pWnd, const CString & label);

private:
	CFont m_Font;
	CMFCTabCtrl	m_wndTabs;

	void AdjustHorzScroll(CListBox& wndListBox);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};
