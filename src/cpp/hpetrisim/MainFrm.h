/**************************************************************************
	MainFrm.h
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#pragma once

#include "HSimToolbar.h"
#include "HPropertyValue.h"
#include "OutputWnd.h"

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNCREATE(CMainFrame)

public:
	CMainFrame();
	virtual ~CMainFrame();

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	COutputWnd& GetOutputWnd() { return m_wndOutput; };
	bool GetValueFromRibbon(CHPropertyValue& val);
	bool SetValuesToRibbon(const CHPropertyValues& vals);
	void ShowRibbonContextCategory(UINT Id);
	void ActivateRibbonContextCategory(UINT Id);
	void HideAllContextCategories();
	void UpdateStatusBar(int nId, const CString& text);

private:
	void CreateDockingWindows();
	void InitializeStatusBar();
	void InitializeHomeRibbon();
	void InitializeMainRibbon();
	void InitializeViewRibbon();
	void InitializePropertiesRibbon();
	void InitializeShapeFormatContextRibbon();
	void InitializeObjectFormatContextRibbon();
	virtual BOOL OnShowMDITabContextMenu(CPoint point, DWORD dwAllowedItems, BOOL bDrop);
	virtual BOOL OnShowPopupMenu(CMFCPopupMenu* pMenuPopup);

	CMFCRibbonBar m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	typedef CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> RibbonArray;
	CMap<long, long, RibbonArray*, RibbonArray*&> m_mapRibbons;

	CMFCMenuBar m_wndMenuBar;
	CMFCToolBar m_wndToolBar;
	CMFCToolBar m_wndEditBar;
	CHSimToolbar m_wndSimBar;
	COutputWnd m_wndOutput;

protected:

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg void OnTimer(UINT_PTR nIDEvent );
	afx_msg LRESULT OnSimThreadMessage(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnHighlightRibbonListItem(WPARAM wp, LPARAM lp);
	afx_msg void OnMdiMoveToNextGroup();
	afx_msg void OnMdiMoveToPrevGroup();
	afx_msg void OnMdiNewHorzTabGroup();
	afx_msg void OnMdiNewVertGroup();
	afx_msg void OnMdiCancel();
	afx_msg void OnDummy();
	afx_msg void OnOutputWindow();
	afx_msg void OnUpdateOutputWindow(CCmdUI* pCmdUI);
	afx_msg void OnViewFullscreen();

	DECLARE_MESSAGE_MAP()

};
