// ZoomView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "PetriSim.h"
#include "ZoomView.h"

// CZoomView

IMPLEMENT_DYNCREATE(CZoomView, COXZoomView)

CZoomView::CZoomView()	: COXZoomView(), m_ZoomMode(MODE_ZOOMOFF)
{
	m_hZoomInCursor = ::LoadCursor(AfxGetInstanceHandle()
		, MAKEINTRESOURCE(IDC_ZOOM_IN_CURSOR));

	m_hZoomOutCursor = ::LoadCursor(AfxGetInstanceHandle()
		, MAKEINTRESOURCE(IDC_ZOOM_OUT_CURSOR));

	Center(TRUE);
}

CZoomView::~CZoomView()
{
	if (m_hZoomInCursor)
		DestroyCursor(m_hZoomInCursor);

	if (m_hZoomOutCursor)
		DestroyCursor(m_hZoomOutCursor);
}

BEGIN_MESSAGE_MAP(CZoomView, COXZoomView)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_ZOOM_NORM, &CZoomView::OnZoomNorm)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_NORM, &CZoomView::OnUpdateZoomNorm)
	ON_COMMAND(ID_ZOOM_IN, &CZoomView::OnZoomIn)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_IN, &CZoomView::OnUpdateZoomIn)
	ON_COMMAND(ID_ZOOM_OUT, &CZoomView::OnZoomOut)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_OUT, &CZoomView::OnUpdateZoomOut)
	ON_COMMAND(ID_ZOOM_FULL, &CZoomView::OnZoomFull)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_FULL, &CZoomView::OnUpdateZoomFull)
END_MESSAGE_MAP()

void CZoomView::OnInitialUpdate()
{
	__super::OnInitialUpdate();
}

void CZoomView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
}

// CZoomView diagnostics

#ifdef _DEBUG
void CZoomView::AssertValid() const
{
	__super::AssertValid();
}

#ifndef _WIN32_WCE
void CZoomView::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}
#endif
#endif //_DEBUG

void CZoomView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	ASSERT_VALID(pDC);

	ASSERT(m_totalDev.cx >= 0 && m_totalDev.cy >= 0);
	SetMapMode(pDC);

	CPoint ptVpOrg(0, 0);       // assume no shift for printing

	if (!pDC->IsPrinting())
	{
		ASSERT(pDC->GetWindowOrg() == CPoint(0,0));

		ScaleViewport(pDC);
		// by default shift viewport origin in negative direction of scroll
		ptVpOrg = -GetDeviceScrollPosition();

		if (m_bCenter)
		{
			CRect rect;
			GetClientRect(&rect);

			// if client area is larger than total device size,
			// override scroll positions to place origin such that
			// output is centered in the window
			if (m_totalDev.cx < rect.Width())
				ptVpOrg.x = (rect.Width() - m_totalDev.cx) / 2;
			if (m_totalDev.cy < rect.Height())
				ptVpOrg.y = (rect.Height() - m_totalDev.cy) / 2;
		}
		else
		{
			// if no vertical SB, eventually align to bottom 
			if(m_bAlignToBottom)
			{
				CRect rect;
				int cy;
				GetClientRect(&rect);
				cy = rect.Height() - m_totalDev.cy;
				if(cy > 0)
					ptVpOrg.y = cy;
			}
		}
		// Set the logical origin, do it yourself when printing !!!
		pDC->SetWindowOrg(m_rectLog.TopLeft());

		pDC->SetViewportOrg(ptVpOrg);
	}
	else
	{
		// Special case for printing
		CSize  printSize;
		printSize.cx = pDC->GetDeviceCaps(HORZRES);
		printSize.cy = pDC->GetDeviceCaps(VERTRES);

		// Maintain the original ratio, setup origin shift
		PersistRatio(m_totalLog, printSize, ptVpOrg);

		// Zoom completely out
		pDC->SetViewportExt(printSize);
		
		CRect rect;
		GetClientRect(&rect);

		if (m_totalDev.cx < rect.Width())
			ptVpOrg.x = (rect.Width() - m_totalDev.cx) / 2;
		if (m_totalDev.cy < rect.Height())
			ptVpOrg.y = (rect.Height() - m_totalDev.cy) / 2;
		
		pDC->SetWindowOrg(m_rectLog.TopLeft());
	}

	CView::OnPrepareDC(pDC, pInfo);     // For default Printing behavior
}

void CZoomView::PersistRatio(const CSize& orig, CSize& dest, CPoint& remainder)
{
	float ratio1 = (float) orig.cx / orig.cy;
	float ratio2 = (float) dest.cx / dest.cy;
	int   newSize;

	// Do nothing if they are the same
	if (ratio1 > ratio2) 
	{
		// Shrink height
		newSize = (int)(dest.cx / ratio1);
		remainder.x = 0;
		remainder.y = dest.cy - newSize;
		dest.cy = newSize;
	} 
	else if (ratio2 > ratio1) 
	{
		// Shrink width
		newSize = (int)(dest.cy * ratio1);
		remainder.x = dest.cx - newSize;
		remainder.y = 0;
		dest.cx = newSize;
	}
}

// CZoomView message handlers

BOOL CZoomView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest != HTCLIENT)
	{
		return __super::OnSetCursor(pWnd, nHitTest, message);
	}

	switch (m_ZoomMode)
	{
	case MODE_ZOOMIN:
		::SetCursor(m_hZoomInCursor);
		break;
	case MODE_ZOOMOUT:
		::SetCursor(m_hZoomOutCursor);
		break;
	default:
		return __super::OnSetCursor(pWnd, nHitTest, message);
	}

	return TRUE;
} 

void CZoomView::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);

	if (m_ZoomMode == MODE_ZOOMIN)
	{
		CRectTracker tracker;

		if (tracker.TrackRubberBand(this, point, FALSE))
		{
			CRect rect;

			tracker.GetTrueRect(rect);
			ZoomToRectangle(rect);
		}
	}
	else if (m_ZoomMode == MODE_ZOOMOUT)
	{
		SetZoomLevel(GetZoomLevel()/2);
	}
}

BOOL CZoomView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (nFlags == MK_CONTROL)
	{
		UINT uWheelScrollLines = GetMouseScrollLines();
		int nToScroll = ::MulDiv(-zDelta, uWheelScrollLines, WHEEL_DELTA);

		SetZoomLevel(GetZoomLevel()-10*nToScroll);

		return TRUE;
	}

	return __super::OnMouseWheel(nFlags, zDelta, pt);
}

void CZoomView::OnZoomNorm()
{
	m_ZoomMode = MODE_ZOOMOFF;
	SetZoomLevel(100);
}

void CZoomView::OnUpdateZoomNorm(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetZoomLevel() != 100);
}

void CZoomView::OnZoomIn()
{
	m_ZoomMode = m_ZoomMode == MODE_ZOOMIN ? MODE_ZOOMOFF : MODE_ZOOMIN;
}

void CZoomView::OnUpdateZoomIn(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_ZoomMode == MODE_ZOOMIN);
}

void CZoomView::OnZoomOut()
{
	m_ZoomMode = m_ZoomMode == MODE_ZOOMOUT ? MODE_ZOOMOFF : MODE_ZOOMOUT;
}

void CZoomView::OnUpdateZoomOut(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_ZoomMode == MODE_ZOOMOUT);
}

void CZoomView::OnZoomFull()
{
	m_ZoomMode = MODE_ZOOMOFF;
	ZoomToWindow();
}

void CZoomView::OnUpdateZoomFull(CCmdUI *pCmdUI)
{
}

void CZoomView::DPtoLP( CRect* pRect)
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(pRect);
}

void CZoomView::DPtoLP( CPoint* pPoint, int count /*= 1*/ )
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(pPoint, count);
}

void CZoomView::LPtoDP( CPoint* pPoint, int count /*= 1*/ )
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.LPtoDP(pPoint, count);
}

void CZoomView::LPtoDP( CRect* pRect)
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.LPtoDP(pRect);
}