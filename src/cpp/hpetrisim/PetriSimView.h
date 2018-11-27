/**************************************************************************
	PetriSimView.h
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#pragma once
#include "resource.h"

#include "ZoomView.h"
#include "SelectTool.h"
#include "ShapeTool.h"
#include "NetObjectTool.h"
#include "PreviewTool.h"
#include "PetriSimDoc.h"
#include "HClipboardTool.h"
#include "HDrawObject.h"
#include "HArrangeTool.h"


class CPetriSimView : public CZoomView
{
protected:
	CPetriSimView();
	DECLARE_DYNCREATE(CPetriSimView)

public:
	virtual ~CPetriSimView();
	CPetriSimDoc* GetDocument();
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnPrint( CDC* pDC, CPrintInfo* pInfo );

	void StoreScreenToBitmap(bool all);
	void InvalidateDcRect(CRect & crect);
	BOOL IsSelected();

	//new
	CPetriSimDoc& GetDoc();
	int GetTool();
	void SetTool(int Tool = ID_SELECT_TOOL);
	CSelectTool& GetSelectTool();
	CPreviewTool& GetPreviewTool();
	CHClipboardTool& ClipboardTool() { return m_ClipboardTool; };
	void InvalidateLg(Rect& rect); //rect in logicals coordinates
	bool IsVisible(CHDrawObject* pObject);
	HCURSOR GetCursor();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
protected:
	afx_msg void OnUpdateEditCustomProperty(CCmdUI* pCmdUI);
	afx_msg void OnEditCustomProperty(UINT nID);
	afx_msg void OnUpdateEditBoolProperty(CCmdUI* pCmdUI);
	afx_msg void OnEditBoolProperty(UINT nID);
	afx_msg void OnUpdateMode(CCmdUI* pCmdUI);
	afx_msg void OnMode(UINT nID);
	afx_msg void OnFilePrintPreview();
	afx_msg void OnUpdateEditCopyCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnContextPopup();
	afx_msg void OnContextPushback();
	afx_msg void OnGroup();
	afx_msg void OnUngroup();
	afx_msg void OnUpdateContextPopup(CCmdUI* pCmdUI);
	afx_msg void OnUpdateContextPushback(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGroup(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUngroup(CCmdUI* pCmdUI);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnStoreScreen();
	afx_msg void OnStoreClient();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnUpdateIndicatorMousePos(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIndicatorZoomScale(CCmdUI* pCmdUI);
	afx_msg void OnContextCommand( UINT nID );
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	afx_msg void OnCancelEdit();
	afx_msg void OnUpdateArrangeCommand(CCmdUI* pCmdUI);
	afx_msg void OnArrangeCommand(UINT nID);

	DECLARE_MESSAGE_MAP()

private:

	int m_Tool;
	UINT m_uiItemID;
	CPoint m_cpLine;
	BOOL m_bPolyLineAddPoint;
	
	CRect m_crOldRect;
	CHDrawObject* object;
	CPoint m_cpFirstPoint;
	CPoint m_MousePosition;

	CList<CHPropertyValue> m_listStorePreviewState;
	CSelectTool m_SelectTool;
	CShapeTool m_ShapeTool;
	CNetObjectTool m_NetObjectTool;
	CPreviewTool m_PreviewTool;
	CHClipboardTool m_ClipboardTool;
	CHArrangeTool m_ArrangeTool;
	CMFCToolTipCtrl m_ToolTip;
	
public:
	afx_msg void OnRenderFormat(UINT nFormat);
	afx_msg void OnRenderAllFormats();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#ifndef _DEBUG
inline CPetriSimDoc* CPetriSimView::GetDocument()
   { return (CPetriSimDoc*)m_pDocument; }
#endif

inline int CPetriSimView::GetTool()
{
	return m_Tool;
}

inline CSelectTool& CPetriSimView::GetSelectTool()
{
	return m_SelectTool;
}

inline CPreviewTool& CPetriSimView::GetPreviewTool()
{
	return m_PreviewTool;
}

inline bool CPetriSimView::IsVisible(CHDrawObject* pObject)
{
	CHLabel* pLabel = dynamic_cast<CHLabel*>(pObject);

	if (pLabel)
	{
		return static_cast<CPetriSimDoc*>(m_pDocument)->GetShowLabel();
	}

	return true;
}