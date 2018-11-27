/**************************************************************************
	SelectTool.h

	copyright (c) 2013/07/13 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/
#pragma once

#include "HDrawObject.h"
#include "Tracker.h"
#include "DrawHandle.h"
#include "HEditorEvent.h"

class CHPropertyValue;
class CInplaceEdit;
class CSelectTool : public CTracker, public CHEditorEvent
{
public:
	CSelectTool(CPetriSimView* pView);
	virtual ~CSelectTool(void);

	void OnCancelEdit();
	void OnInplaceEdit();
	void OnLButtonDown(UINT nFlags, const CPoint & point, CDC & dc);
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags); 
	void DeselectAll(bool bUpdate = true);
	void SelectAll();
	bool Select(UINT nFlags, const CPoint & point);
	bool Select(UINT nFlags, const CRect & rect);
	bool Select(CHDrawObject* pObject);
	bool Select(CHDrawObjectList& list);
	BOOL IsSelected();
	void DeleteSelected();
	void Draw(Graphics& g);
	CHDrawObjectList& GetSelectedObjects();
	bool SetSelectedProperty( const CHPropertyValue & val );
	bool ToggleSelectedProperty( int Id);
	BOOL IsPropertyPossible( int Id );
	bool GetSelectedBoolProperty(int& check, int Id);
	bool OnBoolCommand(int Id);
	void DeleteContent();
	HCURSOR GetCursor(const CPoint& point);
	void UpdateObjectPropertyRibbons();
	CHDrawObject* GetReferenceObject();

	//CHEditorEvent
	virtual void OnEditorEvent(EditorEvent event);
	
private:
	virtual void OnUpdate(CDC * pDC, UINT nMode);
	void OnSelectionChange();
	void UpdateUI();

	CList<Rect> m_listStoredBounds;
	CArray<CArray<Point>> m_StoredPoints;
	CHDrawObjectList m_listSelected;
	CHDrawObjectList m_listPreview;
	CInplaceEdit* m_pEdit;

};
