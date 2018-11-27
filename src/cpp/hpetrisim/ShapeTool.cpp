/**************************************************************************
	ShapeTool.cpp

	copyright (c) 2013/07/13 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#include "StdAfx.h"
#include "resource.h"
#include "PetriSimView.h"
#include "HRect.h"
#include "HPoly.h"
#include "HText.h"
#include "GDI+Helper.h"
#include "MainFrm.h"
#include "ShapeTool.h"

INT_PTR CShapeTool::m_Instances = 0;
HCURSOR CShapeTool::m_hRectCursor = 0;
HCURSOR CShapeTool::m_hLineCursor = 0;
HCURSOR CShapeTool::m_hEllipseCursor = 0;
HCURSOR CShapeTool::m_hRoundedCursor = 0;
HCURSOR CShapeTool::m_hPolyCursor = 0;
HCURSOR CShapeTool::m_hTextCursor = 0;

CShapeTool::CShapeTool(CPetriSimView * pWnd) : CTracker(pWnd) , m_pNewObject(0)
{
	if (m_Instances == 0)
	{
		m_hRectCursor = AfxGetApp()->LoadCursor(IDC_RECT);
		m_hLineCursor = AfxGetApp()->LoadCursor(IDC_LINE);
		m_hEllipseCursor = AfxGetApp()->LoadCursor(IDC_ELLIPSE);
		m_hRoundedCursor = AfxGetApp()->LoadCursor(IDC_ROUND);
		m_hPolyCursor = AfxGetApp()->LoadCursor(IDC_POLY);
		m_hTextCursor = AfxGetApp()->LoadCursor(IDC_TEXT);
	}
	m_Instances++;
}

CShapeTool::~CShapeTool(void)
{
	
	if (--m_Instances == 0)
	{
		if (m_hRectCursor) ::DestroyCursor(m_hRectCursor);
		if (m_hLineCursor) ::DestroyCursor(m_hLineCursor);
		if (m_hEllipseCursor) ::DestroyCursor(m_hEllipseCursor);
		if (m_hRoundedCursor) ::DestroyCursor(m_hRoundedCursor);
		if (m_hPolyCursor) ::DestroyCursor(m_hPolyCursor);
	}
}

void CShapeTool::OnLButtonDown( UINT nFlags, const CPoint & point, CDC & dc )
{
	ASSERT_VALID(m_pView);

	CPoint point2(point);
	dc.DPtoLP(&point2);

	CSelectTool& seltool = m_pView->GetSelectTool();
	CHDrawObjectList& list = seltool.GetSelectedObjects();

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

	m_pView->GetDoc().ToGrid(point2);

	switch (m_pView->GetTool())
	{
	case ID_RECT_MODE: m_pNewObject = new CHRect(point2, CHRect::Rectangle); break;
	case ID_ELLIPSE_MODE: m_pNewObject = new CHRect(point2, CHRect::Ellipse); break;
	case ID_ROUNDRECT_MODE: m_pNewObject = new CHRect(point2, CHRect::Rounded); break;
	case ID_LINE_MODE: m_pNewObject = new CHPoly(point2, false); break;
	case ID_POLYGON_MODE: m_pNewObject = new CHPoly(point2); break;
	case ID_TEXT_MODE: m_pNewObject = new CHText(point2); break;
	}

	CHDrawObjectList newlist;

	newlist.AddTail(m_pNewObject);
	m_pView->GetDoc().AddObjects(newlist, m_pView);
	m_pView->GetSelectTool().Select(m_pNewObject);

	CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	ASSERT_VALID(pFrame);

	pFrame->ActivateRibbonContextCategory(ID_SHAPE_FORMAT_CONTEXT_TAB);

	m_lastpoint = point2;

	int Ret = Track(nFlags, point2, true);

	if (Ret == TrackNoMove)
	{
		m_pNewObject->SetMinSize();
		m_pView->GetDoc().UpdateAllViews(0, CPetriSimDoc::UpdateInvalidate, m_pNewObject);
	}
	else
	{
		m_pView->GetDoc().UpdateAllViews(m_pView, CPetriSimDoc::UpdateInvalidate, m_pNewObject);
	}
	
	m_pNewObject = 0;
}

void CShapeTool::OnUpdate( CDC * pDC, UINT nMode )
{
	ASSERT_VALID(m_pNewObject);
	ASSERT_VALID(m_pView);

	if (nMode & UpdateDraw)
	{
		Rect rect;
		UnionRect(rect, m_pNewObject->GetBounds(true));
		m_pNewObject->Resize(m_Point, 0);
		UnionRect(rect, m_pNewObject->GetBounds(true));
		m_pView->InvalidateLg(rect);

		CHPropertyValues vals;
		vals.Add(CHPropertyValue(ID_OBJECT_CX));
		vals.Add(CHPropertyValue(ID_OBJECT_CY));

		if (m_pNewObject->GetProperties(vals))
		{
			CMainFrame* pFrame = dynamic_cast<CMainFrame*>(AfxGetMainWnd());

			if (pFrame)
			{
				ASSERT_VALID(pFrame);
				pFrame->SetValuesToRibbon(vals);
			}
		}
	}	
}

HCURSOR CShapeTool::GetCursor( const CPoint& point )
{
	CPetriSimView* pView = (CPetriSimView*) m_pWnd;
	ASSERT_VALID(pView);

	switch (pView->GetTool())
	{
	case ID_RECT_MODE: return m_hRectCursor; 
	case ID_LINE_MODE: return m_hLineCursor;
	case ID_ELLIPSE_MODE: return m_hEllipseCursor;
	case ID_ROUNDRECT_MODE: return m_hRoundedCursor;
	case ID_POLYGON_MODE: return m_hPolyCursor;
	case ID_TEXT_MODE: return m_hTextCursor;
	default: return 0;
	}
}