/**************************************************************************
	SelectTool.cpp

	copyright (c) 2013/07/13 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#include "StdAfx.h"
#include "resource.h"
#include "PetriSimView.h"
#include "HDrawObject.h"
#include "GDI+Helper.h"
#include "HPropertyValue.h"
#include "MainFrm.h"
#include "HRect.h"
#include "InplaceEdit.h"
#include "HUpdateObject.h"

#include "SelectTool.h"

CSelectTool::CSelectTool(CPetriSimView* pView) : CTracker(pView)
, m_pEdit(new CInplaceEdit(pView))
{

}

CSelectTool::~CSelectTool(void)
{
	m_pView->GetDoc().UnSubscribe(this);
	delete m_pEdit;
}

void CSelectTool::OnLButtonDown( UINT nFlags, const CPoint & point, CDC & dc)
{
	CPetriSimDoc& doc = m_pView->GetDoc();
	m_Handle = 0;
	m_Mode = TrackSelect;
	m_listPreview.RemoveAll();

	CPoint point2(point);
	m_pView->DPtoLP(&point2);

	if (IsLoaded())
	{
		if (HandleTest(point2))
		{
			m_Mode = TransformScale;
		}
		else
		{
			POSITION pos = m_listSelected.GetHeadPosition();
			while (pos)
			{
				CHDrawObject* pObject = m_listSelected.GetNext(pos);
				ASSERT_VALID(pObject);

				if (pObject->HitTest(point2))
				{
					m_Mode = TransformMove;
					break;
				}
			}
		}
	}
	else if (m_listSelected.GetCount() == 1)
	{

		m_lastpoint = point2;

		CHDrawObject* pObject = m_listSelected.GetHead();
		m_Handle = pObject->HitTestHandle(point2);

		if (pObject->AddPoint(m_Handle, point2))
		{
		}

		if (m_Handle > 0)
		{
			m_Mode = TrackResize;
		}
		else
		{
			if (pObject->HitTest(point2))
			{
				m_Mode = TrackMove;
			}
		}
	}

	if (m_Mode != TrackSelect)
	{
		CHConnector* pConnector = dynamic_cast<CHConnector*>(m_listSelected.GetHead());
		int Ret = 0;

		if (pConnector)
		{
			Ret = Track(nFlags, point2, (pConnector->FromHandle() != m_Handle && pConnector->ToHandle() != m_Handle));
		}
		else
		{
			Ret = Track(nFlags, point2, true);
		}

		if (Ret == TrackSucceeded)
		{
			doc.SetModifiedFlag();
		}
	}
	else
	{
		DeselectAll();

		if (!doc.SimOnline() && Track(nFlags, point2) == TrackSucceeded)
		{
			CRect rect(m_StartPoint, m_Point);
			rect.NormalizeRect();
			Select(nFlags, rect);
		}
		else
		{
			Select(nFlags, point2);
		}
	}

	UpdateUI();
}

void CSelectTool::OnUpdate( CDC * pDC, UINT nMode )
{
	ASSERT_VALID(m_pView);

	CPetriSimDoc& doc = m_pView->GetDoc();

	if (IsLoaded())
	{
		if (nMode & UpdateFirst)
		{
			m_listStoredBounds.RemoveAll();
			m_StoredPoints.SetSize(m_listSelected.GetCount());

			INT_PTR loop = 0;
			POSITION pos = m_listSelected.GetHeadPosition();
			while (pos)
			{
				CHDrawObject* pObject = m_listSelected.GetNext(pos);
				ASSERT_VALID(pObject);

				m_listStoredBounds.AddHead(pObject->GetBounds());

				pObject->StorePoints(m_StoredPoints[loop]);
				loop++;
			}
		}
		else if (nMode & UpdateDraw)
		{
			INT_PTR loop = 0;
			POSITION pos = m_listSelected.GetHeadPosition();
			POSITION pos2 = m_listStoredBounds.GetHeadPosition();
			while (pos && pos2)
			{
				CHDrawObject* pObject = m_listSelected.GetNext(pos);
				ASSERT_VALID(pObject);

				Rect bounds = m_listStoredBounds.GetNext(pos2);

				Transform(m_Transform, bounds);

				pObject->TransformPoints(m_StoredPoints[loop], m_Transform);
				loop++;
			}

		}
		else if (nMode & UpdateLast)
		{

		}
	}
	else if (m_Mode == TrackResize)
	{
		if (nMode & UpdateDraw)
		{
			Rect rect;

			CHDrawObject* pObject = m_listSelected.GetHead();
			ASSERT_VALID(pObject);

			UnionRect(rect, pObject->GetBounds(true));

			CHConnector* pConnector = dynamic_cast<CHConnector*>(pObject);

			if (pConnector && (pConnector->FromHandle() == m_Handle || pConnector->ToHandle() == m_Handle))
			{
				CHNetMember* pNetMember = doc.NetMemberHit(m_Point);

				if (pNetMember && doc.CanConnect(pConnector, pNetMember, m_Handle))
				{
					if (pConnector->ToHandle() == m_Handle)
					{	
						pConnector->To(pNetMember);
					}
					else
					{
						pConnector->From(pNetMember);
					}	

					pConnector->RecalcPoints();
					pConnector->RecalcLabelPosition();
				}
				else
				{
					if (pConnector->ToHandle() == m_Handle)
					{
						pConnector->To(0, true);

					}
					else
					{
						pConnector->From(0, true);
					}

					pObject->Resize(m_Point, m_Handle);
				}
			}
			else
			{
				pObject->Resize(m_Point, m_Handle);
			}

			UnionRect(rect, pObject->GetBounds(true));
			m_pView->InvalidateLg(rect);
		}
	}
	else if (m_Mode == TrackMove)
	{
		if (nMode & UpdateFirst)
		{
			m_StoredPoints.SetSize(1);

			CHDrawObject* pObject = m_listSelected.GetHead();
			ASSERT_VALID(pObject);

			pObject->StorePoints(m_StoredPoints[0]);

		}
		else if (nMode & UpdateDraw)
		{
			Rect rect;

			CHDrawObject* pObject = m_listSelected.GetHead();
			ASSERT_VALID(pObject);

			UnionRect(rect, pObject->GetBounds(true));
			pObject->TransformPoints(m_StoredPoints[0], m_Transform);
			UnionRect(rect, pObject->GetBounds(true));

			m_pView->InvalidateLg(rect);

		}
		else if (nMode & UpdateLast)
		{

		}
	}
	else if (m_Mode == TrackSelect)
	{	
		Rect rect;

		POSITION pos = m_listPreview.GetHeadPosition();
		while (pos)
		{
			CHDrawObject* pObject = m_listPreview.GetNext(pos);
			ASSERT_VALID(pObject);

			UnionRect(rect, pObject->GetBounds(true));
		}

		m_listPreview.RemoveAll();

		CRect tracker;
		GetTracker(tracker);

		if (tracker.IsRectNull())
		{
			return;
		}

		CHDrawObjectList& list = m_pView->GetDoc().DrawObjectList();

		pos = list.GetHeadPosition();
		while (pos)
		{
			CHDrawObject* pObject = list.GetNext(pos);
			ASSERT_VALID(pObject);

			if (m_pView->IsVisible(pObject) && pObject->HitTest(tracker, 0))
			{
				m_listPreview.AddTail(pObject);

				UnionRect(rect, pObject->GetBounds(true));
			}
		}

		m_pView->InvalidateLg(rect);
	}

	if (nMode & UpdateDraw)
	{
		//TODO: UpdateUI();
	}

	__super::OnUpdate(pDC, nMode);
}


void CSelectTool::Draw(Graphics& g)
{
	CHDrawHandle Info;

	if (m_listSelected.GetCount() > 1)
	{
		Info.m_nHint = CHDrawHandle::Hint_Preview;
	}

	POSITION pos = m_listSelected.GetHeadPosition();
	while (pos)
	{
		CHDrawObject* pObject = m_listSelected.GetNext(pos);

		if (g.IsVisible(pObject->GetBounds(true)))
		{
			pObject->DrawHandles(g, Info);
		}
	}

	if (m_listSelected.GetCount() > 1)
	{
		//DrawTracker(g);
	}

	if (IsTracking() && m_Mode == TrackSelect)
	{
		//draw preview of selection 

		Info.m_nHint |= CHDrawHandle::Hint_Preview;

		POSITION pos = m_listPreview.GetHeadPosition();
		while (pos)
		{
			m_listSelected.GetNext(pos)->DrawHandles(g, Info);
		}	
	}

	 CTracker::Draw(g);
}

CHDrawObjectList& CSelectTool::GetSelectedObjects()
{
	return m_listSelected;
}

void CSelectTool::DeselectAll(bool bUpdate /*= true*/)
{
	if (::IsWindow(m_pEdit->GetSafeHwnd()))
	{
		m_pEdit->DestroyWindow();
	}

	if (m_listSelected.GetCount() == 0)
	{
		return;
	}

	m_pView->GetPreviewTool().Reset();

	if (bUpdate)
	{
		Rect rect;
		POSITION pos = m_listSelected.GetHeadPosition();
		while (pos)
		{
			CHDrawObject* pObject = m_listSelected.GetNext(pos);
			ASSERT_VALID(pObject);

			UnionRect(rect, pObject->GetBounds(true));
		}

		m_pView->InvalidateLg(rect);
	}
	
	m_listSelected.RemoveAll();

	if (bUpdate)
	{
		OnSelectionChange();
	}
}

bool CSelectTool::Select( UINT nFlags, const CPoint & point)
{
	CPetriSimView* pView = DYNAMIC_DOWNCAST(CPetriSimView, m_pWnd);
	ASSERT_VALID(pView);

	if (pView == 0)
	{
		return false;
	}

	CPetriSimDoc& doc = pView->GetDoc();

	CHDrawObjectList& list = doc.DrawObjectList();
	//bool SimOnline = doc.Simulation().Mode();
	//bool bRunFast = doc.GetRunFast();
	//bool bMoveOnGrid =  doc.GetMoveOnGrid();
	bool bShowLabel = doc.GetShowLabel();

	CHDrawObject* pObject = 0;
	Rect rect;

	if (!doc.SimOnline())
	{
		DeselectAll();

		POSITION pos = list.GetTailPosition();
		while (pos)
		{
			CHDrawObject* pObject = list.GetPrev(pos);
			ASSERT_VALID(pObject);

			if (m_pView->IsVisible(pObject) && pObject->HitTest(point))
			{
				m_listSelected.AddTail(pObject);
				UnionRect(rect, pObject->GetBounds(true));
				break;
			}
		}
	}


	//if(SimOnline && !bRunFast)	
	//{
	//	DeselectAll();

	//	POSITION pos = list.GetTailPosition();
	//	while (pos)
	//	{
	//		pObject = list.GetPrev(pos);
	//		if((pObject->IsKindOf( RUNTIME_CLASS(CHTransition)) ||
	//			pObject->IsKindOf( RUNTIME_CLASS(CHPosition)))
	//			&& pObject->PtInObject(point))
	//		{
	//			m_listSelected.AddTail(pObject);
	//			//UpdateAllViews(NULL, 0L, pObject);
	//			break;
	//		}
	//	}
	//}
	//else if(!SimOnline)
	{
		/*if(m_listSelected.GetCount() == 1)
		{
			CHDrawObject* pObject = m_listSelected.GetHead();
			if (pObject->PtInObject(point) || pObject->PtInGripper(point))
				return;
			DeselectAll();
		}*/

		//CHDrawObject* plastobject = 0;
		//POSITION pos = list.GetTailPosition();
		//while (pos)
		//{
		//	pObject = list.GetPrev(pos);

		//	if(pObject->PtInObject(point) 
		//		&& ( ((pObject->IsKindOf( RUNTIME_CLASS(CHLabel))
		//		&& bShowLabel)) 
		//		|| !pObject->IsKindOf( RUNTIME_CLASS(CHLabel))))
		//	{
		//		// Treffer, der Punkt liegt in diesem Object.
		//		// Wir  überschreiben wieder mit dem Aktuellen.
		//		plastobject = pObject;
		//		break;
		//	}
		//}
		//if(plastobject != 0)
		//{
		//	if(m_listSelected.Find(plastobject) == 0)
		//	{
		//		m_listSelected.AddTail(plastobject);
		//		
		//		//bMoveOnGrid = false;
		//		//if(plastobject->IsPMember() /*&& m_bAlignNo*/)
		//		//{
		//		//	bMoveOnGrid = true;
		//		//}
		//		//if(!plastobject->IsPMember() /*&& m_bAlignDo*/)
		//		//{
		//		//	bMoveOnGrid = true;
		//		//}
		//	}
		//	//Der Sicht mitteilen, daß wir etwas geändert haben
		//	//UpdateAllViews(NULL,0L,plastobject);
		//}
		//else 
		//{
		//	DeselectAll();
		//}
	}

	pView->InvalidateLg(rect);

	OnSelectionChange();

	return m_listSelected.GetCount() > 0 ? true : false; 
}

bool CSelectTool::Select( CHDrawObject* pObject )
{
	ASSERT_VALID(m_pView);

	if (pObject == 0 || !m_pView->IsVisible(pObject))
	{
		return false;
	}

	DeselectAll();

	CHDrawObjectList& list = m_pView->GetDoc().DrawObjectList();

	if (list.Find(pObject) == 0)
	{
		return false;
	}

	m_listSelected.AddTail(pObject);
	m_pView->InvalidateLg(pObject->GetBounds(true));

	OnSelectionChange();

	return true;
}

bool CSelectTool::Select( UINT nFlags, const CRect & rect )
{
	ASSERT_VALID(m_pView);
	
	DeselectAll();

	CHDrawObjectList& list = m_pView->GetDoc().DrawObjectList();

	Rect rect2;

	POSITION pos = list.GetHeadPosition();
	while (pos)
	{
		CHDrawObject* pObject = list.GetNext(pos);
		ASSERT_VALID(pObject);

		if (m_pView->IsVisible(pObject) && pObject->HitTest(rect))
		{
			m_listSelected.AddTail(pObject);

			UnionRect(rect2, pObject->GetBounds(true));
		}
	}

	m_pView->InvalidateLg(rect2);

	OnSelectionChange();

	return m_listSelected.GetCount() > 0 ? true : false;
}

bool CSelectTool::Select( CHDrawObjectList& list )
{
	ASSERT_VALID(m_pView);

	Rect rect;
	POSITION pos = m_listSelected.GetHeadPosition();
	while (pos)
	{
		UnionRect(rect, m_listSelected.GetNext(pos)->GetBounds(true));
	}

	DeselectAll(false);
	
	pos = list.GetHeadPosition();
	while (pos)
	{
		CHDrawObject* pObject = list.GetNext(pos);

		if (m_pView->IsVisible(pObject))
		{
			m_listSelected.AddTail(pObject);
			UnionRect(rect, pObject->GetBounds(true));
		}
	}

	m_pView->InvalidateLg(rect);
	OnSelectionChange();
	return list.GetCount() > 0;
}

void CSelectTool::DeleteContent()
{
	m_listSelected.RemoveAll();
	m_listPreview.RemoveAll();
	OnSelectionChange();
}

void CSelectTool::SelectAll()
{
	ASSERT_VALID(m_pView);

	m_listSelected.RemoveAll();

	CHDrawObjectList& list = m_pView->GetDoc().DrawObjectList();

	Rect rect;

	POSITION pos = list.GetHeadPosition();
	while (pos)
	{
		CHDrawObject* pObject = list.GetNext(pos);
		ASSERT_VALID(pObject);

		if (m_pView->IsVisible(pObject))
		{
			m_listSelected.AddTail(pObject);
			UnionRect(rect, pObject->GetBounds(true));
		}

	}

	m_pView->InvalidateLg(rect);

	OnSelectionChange();
}

bool CSelectTool::SetSelectedProperty(const CHPropertyValue & val )
{
	ASSERT_VALID(m_pView);

	m_pView->GetPreviewTool().Reset();

	if (m_listSelected.GetCount() == 0)
	{
		return false;
	}

	CHPropertyValues vals;
	vals.Add(val);

	Rect rect;

	POSITION pos = m_listSelected.GetHeadPosition();
	while (pos)
	{
		CHDrawObject* pObject = m_listSelected.GetNext(pos);
		ASSERT_VALID(pObject);

		UnionRect(rect, pObject->GetBounds(true));
		pObject->SetProperties(vals, false);
		UnionRect(rect, pObject->GetBounds(true));
	}

	m_pView->InvalidateLg(rect);
	m_pView->GetDoc().SetModifiedFlag();
	m_pView->GetDoc().UpdateAllViews(m_pView, CPetriSimDoc::UpdateInvalidate, &CHUpdateObject(rect));

	return true;
}

BOOL CSelectTool::IsPropertyPossible( int Id )
{
	if (m_listSelected.GetCount() == 0)
	{
		return FALSE;
	}

	CArray<CHPropertyValue> arr;
	arr.Add(CHPropertyValue(Id));

	POSITION pos = m_listSelected.GetHeadPosition();
	while (pos)
	{
		if(m_listSelected.GetNext(pos)->GetProperties(arr))
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CSelectTool::IsSelected()
{
	return m_listSelected.GetCount() > 0;
}

void CSelectTool::DeleteSelected()
{
	ASSERT_VALID(m_pView);

	m_pView->GetPreviewTool().Reset();

	if (m_listSelected.GetCount() == 0)
	{
		return;
	}

	m_pView->GetDoc().StrippNet(m_listSelected);
	m_pView->GetDoc().RemoveObjects(m_listSelected);

	m_listSelected.RemoveAll();

	OnSelectionChange();
}

HCURSOR CSelectTool::GetCursor( const CPoint& point )
{
	HCURSOR hcursor = 0;

	if (m_listSelected.GetCount() == 1)
	{
		CHDrawObject* pObject = m_listSelected.GetHead();
		ASSERT_VALID(pObject);

		hcursor = pObject->GetCursor(point);

		if (hcursor == 0)
		{
			if (pObject->HitTest(point))
			{
				hcursor = AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);
			}
		}
	}
	else
	{
		hcursor = __super::GetCursor(point);

		if (hcursor == 0)
		{
			POSITION pos = m_listSelected.GetHeadPosition();
			while (pos)
			{
				CHDrawObject* pObject = m_listSelected.GetNext(pos);
				ASSERT_VALID(pObject);

				if (pObject->HitTest(point))
				{
					hcursor = AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);
					break;
				}
			}
		}
	}

	return hcursor;
}

void CSelectTool::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	switch(nChar)
	{
	//TODO:
	//case 38:// Arrow Up
	//	GetDocument()->StepMove(STEP_MOVE_UP);
	//	break;
	//case 40:// Arrow Down
	//	GetDocument()->StepMove(STEP_MOVE_DOWN);
	//	break;
	//case 37:// Arrow Left
	//	GetDocument()->StepMove(STEP_MOVE_LEFT);
	//	break;
	//case 39:// Arrow Right 
	//	GetDocument()->StepMove(STEP_MOVE_RIGHT);
	//	break;
	case VK_DELETE: DeleteSelected(); break;
	case VK_ESCAPE:
		if (IsSelected())
		{
			DeselectAll();
			break;
		}
		m_pView->SetTool();
		break;
	}
}

void CSelectTool::OnSelectionChange()
{

	UpdateObjectPropertyRibbons();

	CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	ASSERT_VALID(pFrame);

	pFrame->HideAllContextCategories();	

	if (m_listSelected.GetCount() > 0)
	{
		POSITION pos =  m_listSelected.GetHeadPosition();
		while (pos)
		{
			CHDrawObject* pObject = m_listSelected.GetNext(pos);
			ASSERT_VALID(pObject);

			if (pObject->IsKindOf(RUNTIME_CLASS(CHRect)))
			{
				pFrame->ShowRibbonContextCategory(ID_SHAPE_FORMAT_CONTEXT_TAB);
				//break;
			}
			if (pObject->IsKindOf(RUNTIME_CLASS(CHNetMember)))
			{
				pFrame->ShowRibbonContextCategory(ID_OBJECT_FORMAT_CONTEXT_TAB);
				//break;
			}
			if (pObject->IsKindOf(RUNTIME_CLASS(CHConnector)))
			{
				pFrame->ShowRibbonContextCategory(ID_OBJECT_FORMAT_CONTEXT_TAB);
				//break;
			}
		}
	}

	if ( m_listSelected.GetCount() < 2)
	{
		Clear();
	}
	else
	{
		Rect bounds;

		POSITION pos = m_listSelected.GetHeadPosition();
		while (pos)
		{
			CHDrawObject* pObject = m_listSelected.GetNext(pos);
			ASSERT_VALID(pObject);

			UnionRect(bounds, pObject->GetBounds());
		}
		
		Load(bounds);
	}
}

void CSelectTool::UpdateObjectPropertyRibbons()
{
	if (!IsSelected())
	{
		return;
	}

	CMainFrame* pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pFrame == 0)
	{
		return;
	}

	CArray<CHPropertyValue> vals;

	vals.SetSize(ID_LAST_CUSTOM_PROPERTY-ID_FIRST_CUSTOM_PROPERTY+1);

	for (INT_PTR loop=0; loop<vals.GetSize(); loop++)
	{
		vals[loop].Id =  ID_FIRST_CUSTOM_PROPERTY + loop;
	}

	POSITION pos = m_listSelected.GetHeadPosition();
	while (pos)
	{
		CHDrawObject* pObject = m_listSelected.GetNext(pos);
		ASSERT_VALID(pObject);

		pObject->GetProperties(vals);
	}

	pFrame->SetValuesToRibbon(vals);

}

bool CSelectTool::ToggleSelectedProperty( int Id )
{
	return false;
}

bool CSelectTool::GetSelectedBoolProperty( int& check, int Id )
{
	check = 2;

	if (!IsSelected())
	{
		return false;
	}

	CArray<CHPropertyValue> vals;
	vals.Add(CHPropertyValue(Id));

	bool bFind = false;
	POSITION pos = m_listSelected.GetHeadPosition();
	while (pos)
	{
		CHDrawObject* pObject = m_listSelected.GetNext(pos);
		ASSERT_VALID(pObject);

		if (pObject->GetProperties(vals))
		{
			bFind = true;
		}
	}

	if (!vals[0].IsIndet())
	{
		check = vals[0].GetBool() ? 1 : 0; 
	}

	return bFind;
}

bool CSelectTool::OnBoolCommand( int Id )
{
	CHDrawObject* pObject = GetReferenceObject();
	if (pObject)
	{
		ASSERT_VALID(pObject);

		CArray<CHPropertyValue> vals;
		vals.Add(CHPropertyValue(true, Id));

		pObject->GetProperties(vals);

		CHPropertyValue& val = vals[0];

		val.Toggle();
		return SetSelectedProperty(val);
	}
	return false;
}

CHDrawObject* CSelectTool::GetReferenceObject()
{
	if (m_listSelected.GetCount() > 0)
	{
		return m_listSelected.GetHead();
	}
	return 0;
}

void CSelectTool::OnInplaceEdit()
{
	ASSERT_VALID(m_pView);

	if (m_listSelected.GetCount() == 1)
	{
		m_pEdit->Create();
	}
}

void CSelectTool::UpdateUI()
{
	if (IsLoaded())
	{
		return;
	}

	if (m_listSelected.GetCount() != 1)
	{
		return;
	}

	CHPropertyValues vals;
	vals.Add(CHPropertyValue(ID_OBJECT_X));
	vals.Add(CHPropertyValue(ID_OBJECT_Y));
	vals.Add(CHPropertyValue(ID_OBJECT_CX));
	vals.Add(CHPropertyValue(ID_OBJECT_CY));

	//TODO: Performance Problem !!!
	if (m_listSelected.GetHead()->GetProperties(vals))
	{
		CMainFrame* pFrm = static_cast<CMainFrame*>(AfxGetMainWnd());
		ASSERT_VALID(pFrm);

		pFrm->SetValuesToRibbon(vals);
	}
}

void CSelectTool::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	ASSERT_VALID(m_pView);

	if (m_listSelected.GetCount() == 1)
	{
		CHDrawObject* pObject = m_listSelected.GetHead();
		ASSERT_VALID(pObject);

		CPoint point2(point);
		m_pView->DPtoLP(&point2);

		int handle = pObject->HitTestHandle(point2);

		if (handle)
		{
			Rect rect(pObject->GetBounds(true));
			if (pObject->RemovePoint(handle))
			{
				UnionRect(rect, pObject->GetBounds(true));
				m_pView->InvalidateLg(rect);
				UpdateUI();
				return;
			}
		}
	}

	OnInplaceEdit();
}

void CSelectTool::OnCancelEdit()
{
	if (IsSelected())
	{
		DeselectAll();
		return;
	}

	m_pView->SetTool();
}

void CSelectTool::OnEditorEvent( EditorEvent event )
{
	if (event == CHEditorEvent::EventSimOnline)
	{
		DeselectAll();
	}
}