/**************************************************************************
	NetObjectTool.cpp

	copyright (c) 2013/08/08 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#include "StdAfx.h"
#include "resource.h"
#include "PetriSimView.h"
#include "HPosition.h"
#include "HTransition.h"
#include "HConnector.h"
#include "MainFrm.h"
#include "HLabel.h"
#include "NetObjectTool.h"

INT_PTR CNetObjectTool::m_Instances = 0;
HCURSOR CNetObjectTool::m_hPositionCursor = 0;
HCURSOR CNetObjectTool::m_hTransitionCursor = 0;
HCURSOR CNetObjectTool::m_hConnectorCursor = 0;

CNetObjectTool::CNetObjectTool(CPetriSimView * pWnd) : CTracker(pWnd) , m_pNewObject(0)
{
	if (m_Instances == 0)
	{
		m_hPositionCursor = AfxGetApp()->LoadCursor(IDC_POSITION);
		m_hTransitionCursor = AfxGetApp()->LoadCursor(IDC_TRANSITION);
		m_hConnectorCursor = AfxGetApp()->LoadCursor(IDC_CONNECTOR);
	}
}

CNetObjectTool::~CNetObjectTool(void)
{
	if (--m_Instances == 0)
	{
		if (m_hPositionCursor) ::DestroyCursor(m_hPositionCursor);
		if (m_hTransitionCursor) ::DestroyCursor(m_hTransitionCursor);
		if (m_hConnectorCursor) ::DestroyCursor(m_hConnectorCursor);
	}
}

void CNetObjectTool::OnLButtonDown( UINT nFlags, const CPoint & point, CDC & dc )
{
	ASSERT_VALID(m_pView);

	CPoint point2(point);
	dc.DPtoLP(&point2);

	CSelectTool& seltool = m_pView->GetSelectTool();
	CHDrawObjectList& list = seltool.GetSelectedObjects();
	CPetriSimDoc& doc = m_pView->GetDoc();

	if (list.GetCount() == 1)
	{
		CHDrawObject* pObject = list.GetHead();
		ASSERT_VALID(pObject);

		if (pObject->HitTest(point2) || pObject->HitTestHandle(point2))
		{
			seltool.OnLButtonDown(nFlags, point, dc);
			return;
		}
	}

	CPoint point3(point2);

	CHDrawObjectList newlist;

	switch (m_pView->GetTool())
	{
	case ID_POSITION_MODE: 
		{
			doc.ToGrid(point3);

			CHPosition* pPosition = new CHPosition(point3, doc.NextPositionName());
			newlist.AddTail(new CHLabel(pPosition, 0));
			newlist.AddTail(new CHLabel(pPosition, 1, false));

			pPosition->UpdateLabelBounds();
			pPosition->RecalcLabelPosition();

			m_pNewObject = pPosition;

			break;
		}
	case ID_TRANSITION_MODE: 
		{
			
			doc.ToGrid(point3);

			CHTransition* pTransition = new CHTransition(point3, doc.NextTransitionName());
			newlist.AddTail(new CHLabel(pTransition, 0));
			newlist.AddTail(new CHLabel(pTransition, 1, false));

			pTransition->UpdateLabelBounds();
			pTransition->RecalcLabelPosition();

			m_pNewObject = pTransition;
		
			break;
		}
	case ID_CONNECTOR_MODE:
		{
			CHConnector* pConnector = new CHConnector(point3, doc.NextConnectorName());
			newlist.AddTail(new CHLabel(pConnector, 0, false));
			newlist.AddTail(new CHLabel(pConnector, 1, false));

			pConnector->UpdateLabelBounds();
			pConnector->RecalcLabelPosition();

			CHNetMember* pMember = doc.NetMemberHit(point2);

			if (pMember)
			{
				if (doc.CanConnect(pConnector, pMember, pConnector->FromHandle()))
				{
					pConnector->From(pMember);
					pConnector->RecalcPoints();
					pConnector->RecalcLabelPosition();
				}
			}

			m_pNewObject = pConnector;
			break;
		}
	}

	newlist.AddTail(m_pNewObject);
	doc.AddObjects(newlist, m_pView);
	m_pView->GetSelectTool().Select(m_pNewObject);

	m_lastpoint = point2;

	CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	ASSERT_VALID(pFrame);

	pFrame->ActivateRibbonContextCategory(ID_OBJECT_FORMAT_CONTEXT_TAB);

	if (m_pView->GetTool() == ID_CONNECTOR_MODE)
	{
		int Ret = Track(nFlags, point3);

		CHConnector* pConnector = static_cast<CHConnector*>(m_pNewObject);
		Rect rect(pConnector->GetBounds(true));

		if (Ret == TrackNoMove)
		{
			m_pNewObject->SetMinSize();
			m_pView->GetDoc().UpdateAllViews(0, CPetriSimDoc::UpdateInvalidate, m_pNewObject);
		}
		else
		{
			CHNetMember* pMember = DYNAMIC_DOWNCAST(CHNetMember, doc.ObjectHit(m_Point));

			if (pMember)
			{
				if (doc.CanConnect(pConnector, pMember, pConnector->ToHandle()))
				{
					pConnector->To(pMember);
				}
			}

			m_pView->GetDoc().UpdateAllViews(m_pView, CPetriSimDoc::UpdateInvalidate, m_pNewObject);
		}

		pConnector->RecalcPoints();
		UnionRect(rect, pConnector->GetBounds(true));

		m_pView->InvalidateLg(rect);
	}

	doc.UpdateAllViews(0, CPetriSimDoc::UpdateInvalidate, m_pNewObject);

	m_pNewObject = 0;
}

void CNetObjectTool::OnUpdate( CDC * pDC, UINT nMode )
{
	ASSERT_VALID(m_pNewObject);
	ASSERT_VALID(m_pView);

	Rect rect;
	UnionRect(rect, m_pNewObject->GetBounds(true));

	CHConnector* pConnector = dynamic_cast<CHConnector*>(m_pNewObject);

	if (pConnector)
	{
		CPetriSimDoc& doc = m_pView->GetDoc();

		CHNetMember* pNetMember = doc.NetMemberHit(m_Point);

		if (pNetMember && doc.CanConnect(pConnector, pNetMember, pConnector->ToHandle()))
		{
			pConnector->To(pNetMember);
			pConnector->RecalcPoints();
			pConnector->RecalcLabelPosition();
		}
		else
		{
			pConnector->To(0, true);
			pConnector->Resize(m_Point, 0);
		}
	}
	else
	{
		m_pNewObject->Resize(m_Point, 0);
	}

	UnionRect(rect, m_pNewObject->GetBounds(true));
	m_pView->InvalidateLg(rect);
}

HCURSOR CNetObjectTool::GetCursor( const CPoint& point )
{
	switch (m_pView->GetTool())
	{
	case ID_POSITION_MODE: return m_hPositionCursor; 
	case ID_TRANSITION_MODE: return m_hTransitionCursor;
	case ID_CONNECTOR_MODE: return m_hConnectorCursor;
	default: return 0;
	}
}