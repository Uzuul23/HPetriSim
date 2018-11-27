// ==========================================================================
// 							Class Implementation : COXZoomView
// ==========================================================================

// Source file : OXZoomView.cpp

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.                      
                          
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXZoomVw.h"
#include <limits.h>
#include <afxpriv.h>

#ifndef __OXMFCIMPL_H__
#if _MFC_VER >= 0x0700
#if _MFC_VER >= 1400
#include <afxtempl.h>
#endif
#include <..\src\mfc\afximpl.h>
#else
#include <..\src\afximpl.h>
#endif
#define __OXMFCIMPL_H__
#endif

#ifndef SPI_GETWHEELSCROLLLINES
	#define SPI_GETWHEELSCROLLLINES   104
#endif

/*#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif     */   

#ifndef WIN32
	#ifdef _DEBUG
		#define UNUSED(x)
	#else
		#define UNUSED(x) x
	#endif
#endif


/////////////////////////////////////////////////////////////////////////////
// COXZoomView

/////////////////////////////////////////////////////////////////////////////
// Definition of static members

#ifdef WIN32
	AFX_DATA const SIZE COXZoomView::sizeDefault = {0,0};
#else
	const SIZE AFXAPI_DATA COXZoomView::sizeDefault = {0,0};
#endif

// Data members -------------------------------------------------------------
// protected:
	// CSize m_totalLog;
	// --- total size in logical units (no rounding)
	
	// CRect m_rectLog;
	// --- bounding rectangle of document in logical units
	
	// CSize m_totalDev;
	// --- total size in device units, scaled

	// CSize m_pageDev;
	// --- per page scroll size in device units
	
	// CSize m_lineDev;
	// --- per line scroll size in device units

	// CSize m_sizeDev;
	// --- total size in device units, unscaled for MM_ANISOTROPIC

	// BOOL	m_bCenter;
	// --- Center output if larger than total size

	// BOOL	m_bInsideUpdate;
	// --- internal state for OnSize callback
	
	// int m_nZoomLevel;
	// --- percentage of zooming

	// BOOL m_bAlignToBottom;
	// --- align to bottom of window if it's bigger than necessary

	// int m_nPagePercent;
	// --- percentage of window to scroll in case of mouse click in a scroll-bar shaft
	
	// int m_nLinePercent;
	// --- percentage of window to scroll in case of mouse click in a scroll arrow

IMPLEMENT_DYNAMIC(COXZoomView, CView)

BEGIN_MESSAGE_MAP(COXZoomView, CView)
	//{{AFX_MSG_MAP(COXZoomView)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Special mapping modes just for COXZoomView implementation
#define MM_NONE             0


// Member functions ---------------------------------------------------------

// public:

COXZoomView::COXZoomView() :
	m_nMapMode(MM_NONE),
	m_nZoomLevel(100),		// 100%
	m_align(ZV_TOPLEFT),
	m_totalLog(0,0),
	m_rectLog(0,0,0,0),
	m_totalDev(0,0),
	m_pageDev(0,0),
	m_lineDev(0,0),
	m_sizeDev(0,0),
	m_nPagePercent(0),
	m_nLinePercent(0),
	m_bUseRelativeScrollSizes(FALSE),
	m_bInsideUpdate(FALSE),
	m_bCenter(FALSE),
	m_bAlignToBottom(FALSE)
{
}

COXZoomView::~COXZoomView()
{
}

int COXZoomView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CView::OnCreate(lpCreateStruct)==-1)
		return -1;

	if(!Initialize())
	{
		TRACE(_T("COXZoomView::OnCreate: failed to initialize the object!\n"));
		return -1;
	}

	return 0;
}

BOOL COXZoomView::Initialize()
{
	BOOL bResult=FALSE;

#ifdef OXZOOMVIEW_USE_RULER
	bResult=AttachRuler();
#else
	bResult=TRUE;
#endif	//	OXZOOMVIEW_USE_RULER

	return bResult;
}


void COXZoomView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	ASSERT_VALID(pDC);

	ASSERT(m_totalDev.cx >= 0 && m_totalDev.cx >= 0);
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
	}
	pDC->SetViewportOrg(ptVpOrg);

	CView::OnPrepareDC(pDC, pInfo);     // For default Printing behavior
}

/////////////////////////////////////////////////////////////////////////////
// Set mode and scaling/scrolling sizes

void COXZoomView::SetDeviceScrollSizesRelative(int nMapMode, SIZE sizeTotal,
	int nPagePercent, int nLinePercent)
{
	// synthesize a rectangle from the size
	CRect rectLog(CPoint(0,0), sizeTotal);
	if(nMapMode != MM_TEXT)
		rectLog.bottom = -rectLog.bottom;
	// call overloaded function for real work
	SetDeviceScrollSizesRelative(nMapMode, rectLog, nPagePercent, nLinePercent);
}

void COXZoomView::SetDeviceScrollSizesRelative(SIZE sizeDevice, const CRect& rectDoc,
	int nPagePercent, int nLinePercent)
{
	ASSERT(sizeDevice.cy > 0);
	if(rectDoc.Height() < 0)
		sizeDevice.cy = -sizeDevice.cy;
	m_sizeDev = sizeDevice;
	SetDeviceScrollSizesRelative(MM_ANISOTROPIC, rectDoc, nPagePercent, nLinePercent);
}


void COXZoomView::SetDeviceScrollSizesRelative(int nMapMode, const CRect& rectDoc,
	int nPagePercent, int nLinePercent)
{
	ASSERT(nLinePercent > 0);
	ASSERT(nPagePercent >= nLinePercent);
	ASSERT(nLinePercent <= 100);
	ASSERT(nPagePercent <= 100);
	m_bUseRelativeScrollSizes = TRUE;
	m_nPagePercent = nPagePercent;
	m_nLinePercent = nLinePercent;
	m_rectLog = rectDoc;
	m_totalLog.cx = m_rectLog.Width();
	m_totalLog.cy = m_rectLog.Height();
	if(m_totalLog.cy < 0)
		m_totalLog.cy = -m_totalLog.cy;
	ASSERT(nMapMode > 0);
	ASSERT(nMapMode != MM_ISOTROPIC);
    
	int nOldMapMode = m_nMapMode;
	m_nMapMode = nMapMode;

	//BLOCK: convert logical coordinate space to device coordinates
	{
		CWindowDC dc(NULL);
		SetMapMode(&dc);
		ScaleViewport(&dc);
		m_totalDev = m_totalLog;
		dc.LPtoDP((LPPOINT)&m_totalDev);
		if (m_totalDev.cy < 0)
			m_totalDev.cy = -m_totalDev.cy;
	} // release DC here

	// now adjust device specific sizes
	ASSERT(m_totalDev.cx >= 0 && m_totalDev.cy >= 0);

	if (m_hWnd != NULL)
	{
		// window has been created, invalidate now
		UpdateBars();
		if (nOldMapMode != m_nMapMode)
			Invalidate(TRUE);
	}
}

// maintained for backward compatibility
void COXZoomView::SetScrollSizes(int nMapMode, SIZE sizeTotal,
	const SIZE& sizePage, const SIZE& sizeLine)
{
	m_bUseRelativeScrollSizes = FALSE;
	ASSERT(sizeTotal.cx >= 0 && sizeTotal.cy >= 0);
	ASSERT(nMapMode > 0);
	ASSERT(nMapMode != MM_ISOTROPIC && nMapMode != MM_ANISOTROPIC);
    
	int nOldMapMode = m_nMapMode;
	m_nMapMode = nMapMode;
	m_totalLog = sizeTotal;
	// synthesize a rectangle from the size
	m_rectLog = CRect(CPoint(0,0), sizeTotal);
	if(m_nMapMode != MM_TEXT)
		m_rectLog.bottom = -m_rectLog.bottom;

	//BLOCK: convert logical coordinate space to device coordinates
	{
		CWindowDC dc(NULL);
		ASSERT(m_nMapMode > 0);
		SetMapMode(&dc);
        // leave page and line sizes unaffected from zooming
		m_pageDev = sizePage;
		dc.LPtoDP((LPPOINT)&m_pageDev);
		m_lineDev = sizeLine;
		dc.LPtoDP((LPPOINT)&m_lineDev);
		// total size
		ScaleViewport(&dc);
		m_totalDev = m_totalLog;
		dc.LPtoDP((LPPOINT)&m_totalDev);
		if (m_totalDev.cy < 0)
			m_totalDev.cy = -m_totalDev.cy;
		if (m_pageDev.cy < 0)
			m_pageDev.cy = -m_pageDev.cy;
		if (m_lineDev.cy < 0)
			m_lineDev.cy = -m_lineDev.cy;
	} // release DC here

	// now adjust device specific sizes
	ASSERT(m_totalDev.cx >= 0 && m_totalDev.cy >= 0);
	if (m_pageDev.cx == 0)
		m_pageDev.cx = m_totalDev.cx / 10;
	if (m_pageDev.cy == 0)
		m_pageDev.cy = m_totalDev.cy / 10;
	if (m_lineDev.cx == 0)
		m_lineDev.cx = m_pageDev.cx / 10;
	if (m_lineDev.cy == 0)
		m_lineDev.cy = m_pageDev.cy / 10;

	if (m_hWnd != NULL)
	{
		// window has been created, invalidate now
		UpdateBars();
		if (nOldMapMode != m_nMapMode)
			Invalidate(TRUE);
	}
}

// Zoom Ops

void COXZoomView::ZoomToWindow()
{
	int nZoomLevel=GetZoomLevel();
	{
		CClientDC dc(this);
		ASSERT(m_nMapMode > 0);
		SetMapMode(&dc);

		CSize	sizeSb, sizeZoom, sizeClient;
		long temp;
		GetTrueClientSize(sizeClient,sizeSb);
    
		CRect rectClient;
		GetClientRect(rectClient);
		rectClient.right=rectClient.left+sizeClient.cx;
		rectClient.bottom=rectClient.top+sizeClient.cy;
		dc.DPtoLP(rectClient);
		sizeClient=rectClient.Size();
		if(sizeClient.cy < 0) 
    		sizeClient.cy = -sizeClient.cy;

		temp = (long)((float)100 * (float)sizeClient.cx / 
			(float)m_totalLog.cx);
		if(temp > (long)MAXZOOM)
    		sizeZoom.cx = MAXZOOM;
		else
    		sizeZoom.cx = (int)temp;
		temp = (long)((float)100 * (float)sizeClient.cy / 
			(float)m_totalLog.cy);
		if(temp > (long)MAXZOOM)
    		sizeZoom.cy = MAXZOOM;
		else
    		sizeZoom.cy = (int)temp;
			
		// use the lower value
		if(sizeZoom.cy < sizeZoom.cx)
    		nZoomLevel = sizeZoom.cy;
		else
    		nZoomLevel = sizeZoom.cx;
		if (nZoomLevel < MINZOOM)
    		nZoomLevel = MINZOOM;
	}

	SetZoomLevel(nZoomLevel);
}


void COXZoomView::ZoomToRectangle(CRect rectZoom, ZoomAlignment Align)
{
	int nZoomLevel=GetZoomLevel();
	{
		CClientDC dc(this);
		OnPrepareDC(&dc);

		// clip zoom rectangle to document size
		CRect rectDevDoc = m_rectLog;
		dc.LPtoDP(rectDevDoc);
		if(!rectZoom.IntersectRect(rectZoom, rectDevDoc))
			return;		// competely outside of the document, ignore
		dc.DPtoLP(rectZoom);

		// check again in logical coords
		if(0 == rectZoom.Width())
			return;
		if(0 == rectZoom.Height())
			return;

		CSize sizeZoom=rectZoom.Size();
		if(sizeZoom.cy < 0) 
    		sizeZoom.cy = -sizeZoom.cy;

		CSize	sizeSb, sizeClient;
		GetTrueClientSize(sizeClient,sizeSb);
    
		CRect rectClient;
		GetClientRect(rectClient);
		rectClient.right=rectClient.left+sizeClient.cx;
		rectClient.bottom=rectClient.top+sizeClient.cy;
	    SetMapMode(&dc);	// assume 100%
		dc.DPtoLP(rectClient);
		sizeClient=rectClient.Size();
		if(sizeClient.cy < 0) 
    		sizeClient.cy = -sizeClient.cy;

		long temp = (long)((float)100 * (float)sizeClient.cx / 
			(float)sizeZoom.cx);
		if(temp > (long)MAXZOOM)
    		sizeZoom.cx = MAXZOOM;
		else
    		sizeZoom.cx = (int)temp;
		temp = (long)((float)100 * (float)sizeClient.cy / 
			(float)sizeZoom.cy);
		if(temp > (long)MAXZOOM)
    		sizeZoom.cy = MAXZOOM;
		else
    		sizeZoom.cy = (int)temp;
		// use the lower value
		if(sizeZoom.cy < sizeZoom.cx)
    		nZoomLevel = sizeZoom.cy;
		else
    		nZoomLevel = sizeZoom.cx;
		if (nZoomLevel < MINZOOM)
    		nZoomLevel = MINZOOM;
	}

	SetZoomLevel(nZoomLevel);

	CPoint	ptAlign;
	if (m_hWnd != NULL)
	{
		if(Align == ZV_DEFAULT)
			Align = m_align;
		switch(Align)
		{
			case ZV_TOPLEFT:
				ptAlign.x = rectZoom.left;
				ptAlign.y = rectZoom.top;
				break;
					
			case ZV_BOTTOMLEFT:
				ptAlign.x = rectZoom.left;
				ptAlign.y = rectZoom.bottom;
				break;
					
			case ZV_CENTER:
				ptAlign.x = (rectZoom.left + rectZoom.right)/2;
				ptAlign.y = (rectZoom.top + rectZoom.bottom)/2;
				break;
		}
		ScrollToPosition(ptAlign, Align);
		UpdateBars();
		Invalidate(TRUE);
	}
}


int COXZoomView::SetZoomLevel(int nNewLevel, ZoomAlignment Align)
{
	int nOldLevel = m_nZoomLevel;
	if(Align == ZV_DEFAULT)
		Align = m_align;
	CPoint ptCurrent = GetScrollPosition(Align);
	if(nNewLevel < MINZOOM)
		nNewLevel = MINZOOM;
	if(nNewLevel > MAXZOOM)
		nNewLevel = MAXZOOM;
	m_nZoomLevel = nNewLevel;
	if(nOldLevel != nNewLevel)
	{
		//BLOCK: convert logical coordinate space to device coordinates
		{
			CWindowDC dc(NULL);
			ASSERT(m_nMapMode > 0);
			SetMapMode(&dc);
			// total size
			ScaleViewport(&dc);
			m_totalDev = m_totalLog;
			dc.LPtoDP((LPPOINT)&m_totalDev);
			if (m_totalDev.cy < 0)
				m_totalDev.cy = -m_totalDev.cy;
		} // release DC here
	
		ASSERT(m_totalDev.cx >= 0 && m_totalDev.cy >= 0);
	
		if (m_hWnd != NULL)
		{
			// window exists, invalidate now
			ScrollToPosition(ptCurrent, Align);
			UpdateBars(FALSE);
			Invalidate(TRUE);
		}
	}

#ifdef OXZOOMVIEW_USE_RULER
	m_ruler.ZoomRuler(m_nZoomLevel,TRUE);
	m_ruler.ZoomRuler(m_nZoomLevel,FALSE);
#endif	//	OXZOOMVIEW_USE_RULER

	return(nOldLevel);
}

// zoom helper functions

void COXZoomView::SetMapMode(CDC* pDC) const
{
	ASSERT(m_nMapMode > 0);
	pDC->SetMapMode(m_nMapMode);
    if(m_nMapMode == MM_ANISOTROPIC)
    {                     
    	ASSERT(m_sizeDev.cx >= 10);
    	ASSERT(m_sizeDev.cy >= 10 || m_sizeDev.cy <= -10);
    	ASSERT(m_sizeDev.cx < 32000);
    	ASSERT(m_sizeDev.cy < 32000 || m_sizeDev.cy >= -32000);
    	pDC->SetWindowExt(m_totalLog);
    	pDC->SetViewportExt(m_sizeDev);
    }
    else
    {
		// unlock mapping mode for zooming, extents of previous map mode are inherited
		pDC->SetMapMode(MM_ANISOTROPIC);
		if(m_nMapMode == MM_TEXT)
		{
			// current extents are 1,1 which are too small to zoom down
			pDC->ScaleWindowExt(1000, 1, 1000, 1);
			pDC->ScaleViewportExt(1000, 1, 1000, 1);
		}
    }
}

void COXZoomView::SetZoomAlign(ZoomAlignment align)
{
#ifdef _DEBUG
	if(align == ZV_DEFAULT)
		TRACE(_T("You cannot set the ZoomAlignment to ZV_DEFAULT"));
#endif
	if(align != ZV_DEFAULT)
		m_align = align;
}

/////////////////////////////////////////////////////////////////////////////
// Getting information

CPoint COXZoomView::GetScrollPosition(ZoomAlignment Align) const   // logical coordinates
{
	CPoint pt = GetDeviceScrollPosition();
	// pt may be negative if m_bCenter is set
	
	CRect	rectClient;
	GetClientRect(rectClient);
	DWORD dwStyle = GetStyle();
	BOOL bHasV, bHasH;
    bHasV = (dwStyle & WS_VSCROLL) != 0 || GetScrollBarCtrl(SB_VERT) != NULL;
    bHasH = (dwStyle & WS_HSCROLL) != 0 || GetScrollBarCtrl(SB_HORZ) != NULL;
    
	if(Align == ZV_DEFAULT)
		Align = m_align;
	switch (Align)
	{
		case ZV_BOTTOMLEFT:
			if(bHasV)
				pt.y += rectClient.Height();
			else
				pt.y = m_totalDev.cy - 1;
			break;
			    
		case ZV_CENTER:
			if(bHasV)
				pt.y += rectClient.Height()/2;
			else
				pt.y = m_totalDev.cy/2;
				
			if(bHasH)
				pt.x += rectClient.Width()/2;
			else
				pt.x = m_totalDev.cx/2;
			break;
	}
	
	ASSERT(m_nMapMode > 0); // must be set
	CWindowDC dc(NULL);
	SetMapMode(&dc);
	ScaleViewport(&dc);
	// CRect::TopLeft() doesn't work with const CRects
	dc.SetWindowOrg(m_rectLog.left, m_rectLog.top);
	dc.DPtoLP((LPPOINT)&pt);
	return pt;
}

void COXZoomView::ScrollToPosition(POINT pt, ZoomAlignment Align)    // logical coordinates
{
	ASSERT(m_nMapMode > 0);     // not allowed
	CWindowDC dc(NULL);
	SetMapMode(&dc);
	ScaleViewport(&dc);
	dc.SetWindowOrg(m_rectLog.TopLeft());
	dc.LPtoDP((LPPOINT)&pt);
	
	CRect	rectClient;
	GetClientRect(rectClient);

	// now in device coordinates
	if(Align == ZV_DEFAULT)
		Align = m_align;
	switch (Align)
	{
		case ZV_BOTTOMLEFT:
			pt.y -= rectClient.Height();
			break;
		case ZV_CENTER:
			pt.y -= rectClient.Height()/2;
			pt.x -= rectClient.Width()/2;
			break;
	}
	
	//limit if out of range
	int xMin, xMax, yMin, yMax;
	GetScrollRange(SB_HORZ, &xMin, &xMax);
	GetScrollRange(SB_VERT, &yMin, &yMax);
	ASSERT(xMin == 0 && yMin == 0);
	if (pt.x < 0)
		pt.x = 0;
	else if (pt.x > xMax)
		pt.x = xMax;
	if (pt.y < 0)
		pt.y = 0;
	else if (pt.y > yMax)
		pt.y = yMax;

	ScrollToDevicePosition(pt);
}

CPoint COXZoomView::GetDeviceScrollPosition() const
{
	CPoint pt(GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT));
	ASSERT(pt.x >= 0 && pt.y >= 0);

	if (m_bCenter)
	{
		CRect rect;
		GetClientRect(&rect);

		// if client area is larger than total device size,
		// the scroll positions are overridden to place origin such that
		// output is centered in the window
		// GetDeviceScrollPosition() must reflect this

		if (m_totalDev.cx < rect.Width())
			pt.x = -((rect.Width() - m_totalDev.cx) / 2);
		if (m_totalDev.cy < rect.Height())
			pt.y = -((rect.Height() - m_totalDev.cy) / 2);
	}

	return pt;
}

void COXZoomView::GetDeviceScrollSizes(int& nMapMode, SIZE& sizeTotal,
			SIZE& sizePage, SIZE& sizeLine) const
{
	if (m_nMapMode <= 0)
		TRACE(_T("Warning: COXZoomView::GetDeviceScrollSizes returning invalid mapping mode\n"));
	nMapMode = m_nMapMode;
	sizeTotal = m_totalDev;
	sizePage = m_pageDev;
	sizeLine = m_lineDev;
}

void COXZoomView::ScrollToDevicePosition(POINT ptDev)
{
	ASSERT(ptDev.x >= 0);
	ASSERT(ptDev.y >= 0);
	int dx, dy;

	// Note: ScrollToDevicePosition can and is used to scroll out-of-range
	//  areas as far as COXZoomView is concerned -- specifically in
	//  the print-preview code.  Since OnScrollBy makes sure the range is
	//  valid, ScrollToDevicePosition does not vector through OnScrollBy.

	int xOrig = SetScrollPos(SB_HORZ, ptDev.x);
	int yOrig = SetScrollPos(SB_VERT, ptDev.y);
	dx = xOrig - ptDev.x;
	dy = yOrig - ptDev.y;
	ScrollWindow(dx, dy);
}

/////////////////////////////////////////////////////////////////////////////
// Other helpers
// retained for compatibility
void COXZoomView::FillOutsideRect(CDC* pDC, CBrush* pBrush)
{
	ASSERT_VALID(pDC);
	ASSERT_VALID(pBrush);
	// Fill Rect outside the image
	CRect rect;
	GetClientRect(rect);
	ASSERT(rect.left == 0 && rect.top == 0);
	rect.left = m_totalDev.cx;
	if (!rect.IsRectEmpty())
		pDC->FillRect(rect, pBrush);    // vertical strip along the side
	rect.left = 0;
	rect.right = m_totalDev.cx;
	rect.top = m_totalDev.cy;
	if (!rect.IsRectEmpty())
		pDC->FillRect(rect, pBrush);    // horizontal strip along the bottom
}


/////////////////////////////////////////////////////////////////////////////

void COXZoomView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	UpdateBars(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// Tie to scrollbars and CWnd behaviour

void COXZoomView::GetScrollBarSizes(CSize& sizeSb)
{
	sizeSb.cx = sizeSb.cy = 0;
	DWORD dwStyle = GetStyle();

	if (GetScrollBarCtrl(SB_VERT) == NULL)
	{
		// vert scrollbars will impact client area of this window
		sizeSb.cx = GetSystemMetrics(SM_CXVSCROLL);
		if (dwStyle & WS_BORDER)
			sizeSb.cx -= GetSystemMetrics(SM_CXBORDER);
	}
	if (GetScrollBarCtrl(SB_HORZ) == NULL)
	{
		// horz scrollbars will impact client area of this window
		sizeSb.cy = GetSystemMetrics(SM_CYHSCROLL);
		if (dwStyle & WS_BORDER)
			sizeSb.cy -= GetSystemMetrics(SM_CYBORDER);
	}
}

BOOL COXZoomView::GetTrueClientSize(CSize& size, CSize& sizeSb)
	// return TRUE if enough room to add scrollbars if needed
{
	CRect rect;
	GetClientRect(&rect);
	ASSERT(rect.top == 0 && rect.left == 0);
	size.cx = rect.right;
	size.cy = rect.bottom;
	DWORD dwStyle = GetStyle();

	// first get the size of the scrollbars for this window
	GetScrollBarSizes(sizeSb);

	// first calculate the size of a potential scrollbar
	// (scroll bar controls do not get turned on/off)
	if (sizeSb.cx != 0 && (dwStyle & WS_VSCROLL))
	{
		// vert scrollbars will impact client area of this window
		size.cx += sizeSb.cx;   // currently on - adjust now
	}
	if (sizeSb.cy != 0 && (dwStyle & WS_HSCROLL))
	{
		// horz scrollbars will impact client area of this window
		size.cy += sizeSb.cy;   // currently on - adjust now
	}

	// return TRUE if enough room
	return (size.cx > sizeSb.cx && size.cy > sizeSb.cy);
}

// helper to return the state of the scrollbars without actually changing
//  the state of the scrollbars
void COXZoomView::GetScrollBarState(CSize sizeClient, CSize& needSb,
	CSize& sizeRange, CPoint& ptMove, BOOL bInsideClient)
{
	// get scroll bar sizes (the part that is in the client area)
	CSize sizeSb;
	GetScrollBarSizes(sizeSb);

	// enough room to add scrollbars
	sizeRange = m_totalDev - sizeClient;
		// > 0 => need to scroll
	ptMove = GetDeviceScrollPosition();
		// point to move to (start at current scroll pos)

	BOOL bNeedH = sizeRange.cx > 0;
	if (!bNeedH)
		ptMove.x = 0;                       // jump back to origin
	else if (bInsideClient)
		sizeRange.cy += sizeSb.cy;          // need room for a scroll bar

	BOOL bNeedV = sizeRange.cy > 0;
	if (!bNeedV)
		ptMove.y = 0;                       // jump back to origin 
		//ptMove.y =  m_totalDev.cy - sizeClient.cy;
	else if (bInsideClient)
		sizeRange.cx += sizeSb.cx;          // need room for a scroll bar

	if (bNeedV && !bNeedH && sizeRange.cx > 0)
	{
		ASSERT(bInsideClient);
		// need a horizontal scrollbar after all
		bNeedH = TRUE;
		sizeRange.cy += sizeSb.cy;
	}

	// if current scroll position will be past the limit, scroll to limit
	if (sizeRange.cx > 0 && ptMove.x >= sizeRange.cx)
		ptMove.x = sizeRange.cx;
	if (sizeRange.cy > 0 && ptMove.y >= sizeRange.cy)
		ptMove.y = sizeRange.cy;

	// now update the bars as appropriate
	needSb.cx = bNeedH;
	needSb.cy = bNeedV;

	// needSb, sizeRange, and ptMove area now all updated
}

void COXZoomView::UpdateBars(BOOL /*bSendRecalc*/)
{
	// UpdateBars may cause window to be resized - ignore those resizings
	if (m_bInsideUpdate)
		return;         // Do not allow recursive calls

	// Lock out recursion
	m_bInsideUpdate = TRUE;

	// update the horizontal to reflect reality
	// NOTE: turning on/off the scrollbars will cause 'OnSize' callbacks
	ASSERT(m_totalDev.cx >= 0 && m_totalDev.cy >= 0);

	CRect rectClient;
	BOOL bCalcClient = TRUE;

	// allow parent to do inside-out layout first
	CWnd* pParentWnd = GetParent();
	if (pParentWnd != NULL)
	{
		// if parent window responds to this message, use just
		//  client area for scroll bar calc -- not "true" client area
		if ((BOOL)pParentWnd->SendMessage(WM_RECALCPARENT, 0,
			(LPARAM)(LPCRECT)&rectClient) != 0)
		{
			// use rectClient instead of GetTrueClientSize for
			//  client size calculation.
			bCalcClient = FALSE;
		}
	}

	CSize sizeClient;
	CSize sizeSb;

	if (bCalcClient)
	{
		// get client rect
		if (!GetTrueClientSize(sizeClient, sizeSb))
		{
			// no room for scroll bars (common for zero sized elements)
			CRect rect;
			GetClientRect(&rect);
			if (rect.right > 0 && rect.bottom > 0)
			{
				// if entire client area is not invisible, assume we have
				//  control over our scrollbars
				EnableScrollBarCtrl(SB_BOTH, FALSE);
			}
			m_bInsideUpdate = FALSE;
			return;
		}
	}
	else
	{
		// let parent window determine the "client" rect
		sizeClient.cx = rectClient.right - rectClient.left;
		sizeClient.cy = rectClient.bottom - rectClient.top;
	}

	// enough room to add scrollbars
	CSize sizeRange;
	CPoint ptMove;
	CSize needSb;
    
    // if scrolling-sizes are relative to client area update them NOW
    if(m_bUseRelativeScrollSizes)
    {
    	m_pageDev.cx = MulDiv(m_nPagePercent, sizeClient.cx, 100);
    	m_pageDev.cy = MulDiv(m_nPagePercent, sizeClient.cy, 100);
    	m_lineDev.cx = MulDiv(m_nLinePercent, sizeClient.cx, 100);
    	m_lineDev.cy = MulDiv(m_nLinePercent, sizeClient.cy, 100);
    }
    
	// get the current scroll bar state given the true client area
	GetScrollBarState(sizeClient, needSb, sizeRange, ptMove, bCalcClient);
	if (needSb.cx)
		sizeClient.cy -= sizeSb.cy;
	if (needSb.cy)
		sizeClient.cx -= sizeSb.cx;

	// first scroll the window as needed
	ScrollToDevicePosition(ptMove); // will set the scroll bar positions too

	// this structure needed to update the scrollbar page range
	SCROLLINFO info;
	info.fMask = SIF_PAGE|SIF_RANGE;
	info.nMin = 0;

	// now update the bars as appropriate
	EnableScrollBarCtrl(SB_HORZ, needSb.cx);
	if (needSb.cx)
	{
		info.nPage = sizeClient.cx;
		info.nMax = m_totalDev.cx-1;
		if(!SetScrollInfo(SB_HORZ, &info, TRUE))
			SetScrollRange(SB_HORZ, 0, sizeRange.cx, TRUE);
	}
	EnableScrollBarCtrl(SB_VERT, needSb.cy);
	if (needSb.cy)
	{
		info.nPage = sizeClient.cy;
		info.nMax = m_totalDev.cy-1;
		if(!SetScrollInfo(SB_VERT, &info, TRUE))
			SetScrollRange(SB_VERT, 0, sizeRange.cy, TRUE);
	}

	// Remove recursion lockout
	m_bInsideUpdate = FALSE;
}

void COXZoomView::CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType)
{
	if (nAdjustType == adjustOutside)
	{
		// if the view is being used in-place, add scrollbar sizes
		//  (scollbars should appear on the outside when in-place editing)
		CSize sizeClient(
			lpClientRect->right - lpClientRect->left,
			lpClientRect->bottom - lpClientRect->top);

		CSize sizeRange = m_totalDev - sizeClient;
			// > 0 => need to scroll

		// get scroll bar sizes (used to adjust the window)
		CSize sizeSb;
		GetScrollBarSizes(sizeSb);

		// adjust the window size based on the state
		if (sizeRange.cy > 0)
		{   // vertical scroll bars take up horizontal space
			lpClientRect->right += sizeSb.cx;
		}
		if (sizeRange.cx > 0)
		{   // horizontal scroll bars take up vertical space
			lpClientRect->bottom += sizeSb.cy;
		}
	}
	else
	{
		// call default to handle other non-client areas
		::AdjustWindowRect(lpClientRect, GetStyle(), FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// COXZoomView scrolling

void COXZoomView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	ASSERT(pScrollBar == GetScrollBarCtrl(SB_HORZ));    // may be null
	UNUSED(pScrollBar);

	OnScroll(MAKEWORD(nSBCode, -1), nPos);
}

void COXZoomView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	ASSERT(pScrollBar == GetScrollBarCtrl(SB_VERT));    // may be null
	UNUSED(pScrollBar);

	OnScroll(MAKEWORD(-1, nSBCode), nPos);
}

BOOL COXZoomView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll)
{
	// calc new x position
	int x = GetScrollPos(SB_HORZ);
	int xOrig = x;

	switch (LOBYTE(nScrollCode))
	{
	case SB_TOP:
		x = 0;
		break;
	case SB_BOTTOM:
		x = INT_MAX;
		break;
	case SB_LINEUP:
		x -= m_lineDev.cx;
		break;
	case SB_LINEDOWN:
		x += m_lineDev.cx;
		break;
	case SB_PAGEUP:
		x -= m_pageDev.cx;
		break;
	case SB_PAGEDOWN:
		x += m_pageDev.cx;
		break;
	case SB_THUMBTRACK:
		x = nPos;
		break;
	}

	// calc new y position
	int y = GetScrollPos(SB_VERT);
	int yOrig = y;

	switch (HIBYTE(nScrollCode))
	{
	case SB_TOP:
		y = 0;
		break;
	case SB_BOTTOM:
		y = INT_MAX;
		break;
	case SB_LINEUP:
		y -= m_lineDev.cy;
		break;
	case SB_LINEDOWN:
		y += m_lineDev.cy;
		break;
	case SB_PAGEUP:
		y -= m_pageDev.cy;
		break;
	case SB_PAGEDOWN:
		y += m_pageDev.cy;
		break;
	case SB_THUMBTRACK:
		y = nPos;
		break;
	}

	BOOL bResult = OnScrollBy(CSize(x - xOrig, y - yOrig), bDoScroll);
	if (bResult && bDoScroll)
		UpdateWindow();

	return bResult;
}

BOOL COXZoomView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
{
	int xOrig, x;
	int yOrig, y;

	// don't scroll if there is no valid scroll range (ie. no scroll bar)
	CScrollBar* pBar;
	DWORD dwStyle = GetStyle();
	pBar = GetScrollBarCtrl(SB_VERT);
	if ((pBar != NULL && !pBar->IsWindowEnabled()) ||
		(pBar == NULL && !(dwStyle & WS_VSCROLL)))
	{
		// vertical scroll bar not enabled
		sizeScroll.cy = 0;
	}
	pBar = GetScrollBarCtrl(SB_HORZ);
	if ((pBar != NULL && !pBar->IsWindowEnabled()) ||
		(pBar == NULL && !(dwStyle & WS_HSCROLL)))
	{
		// horizontal scroll bar not enabled
		sizeScroll.cx = 0;
	}

	// adjust current x position
	xOrig = x = GetScrollPos(SB_HORZ);
	int xMax = GetScrollLimit(SB_HORZ);
	x += sizeScroll.cx;
	if (x < 0)
		x = 0;
	else if (x > xMax)
		x = xMax;

	// adjust current y position
	yOrig = y = GetScrollPos(SB_VERT);
	int yMax = GetScrollLimit(SB_VERT);
	y += sizeScroll.cy;
	if (y < 0)
		y = 0;
	else if (y > yMax)
		y = yMax;

	// did anything change?
	if (x == xOrig && y == yOrig)
		return FALSE;

	if (bDoScroll)
	{
		// do scroll and update scroll positions
		ScrollWindow(-(x-xOrig), -(y-yOrig));
		if (x != xOrig)
			SetScrollPos(SB_HORZ, x);
		if (y != yOrig)
			SetScrollPos(SB_VERT, y);
	}
	return TRUE;
}

BOOL COXZoomView::OnMouseWheel(UINT fFlags, short zDelta, CPoint point)
{
	// we don't handle anything but scrolling just now
	if (fFlags & (MK_SHIFT | MK_CONTROL))
		return FALSE;

	// if the parent is a splitter, it will handle the message
	if (GetParentSplitter(this, TRUE))
		return FALSE;

	// we can't get out of it--perform the scroll ourselves
	return DoMouseWheel(fFlags, zDelta, point);
}

// This function isn't virtual. If you need to override it,
// you really need to override OnMouseWheel() here or in
// CSplitterWnd.
BOOL COXZoomView::DoMouseWheel(UINT fFlags, short zDelta, CPoint point)
{
	UNUSED_ALWAYS(point);
	UNUSED_ALWAYS(fFlags);

	// if we have a vertical scroll bar, the wheel scrolls that
	// if we have _only_ a horizontal scroll bar, the wheel scrolls that
	// otherwise, don't do any work at all

	DWORD dwStyle = GetStyle();
	CScrollBar* pBar = GetScrollBarCtrl(SB_VERT);
	BOOL bHasVertBar = ((pBar != NULL) && pBar->IsWindowEnabled()) ||
					(dwStyle & WS_VSCROLL);

	pBar = GetScrollBarCtrl(SB_HORZ);
	BOOL bHasHorzBar = ((pBar != NULL) && pBar->IsWindowEnabled()) ||
					(dwStyle & WS_HSCROLL);

	if (!bHasVertBar && !bHasHorzBar)
		return FALSE;

	BOOL bResult = FALSE;
	UINT uWheelScrollLines=GetMouseScrollLines();
	int nToScroll;
	int nDisplacement;

	if (bHasVertBar)
	{
		nToScroll = ::MulDiv(-zDelta, uWheelScrollLines, WHEEL_DELTA);
		if (nToScroll == -1 || uWheelScrollLines == WHEEL_PAGESCROLL)
		{
			nDisplacement = m_pageDev.cy;
			if (zDelta > 0)
				nDisplacement = -nDisplacement;
		}
		else
		{
			nDisplacement = nToScroll * m_lineDev.cy;
			nDisplacement = min(nDisplacement, m_pageDev.cy);
		}
		bResult = OnScrollBy(CSize(0, nDisplacement), TRUE);
	}
	else if (bHasHorzBar)
	{
		nToScroll = ::MulDiv(-zDelta, uWheelScrollLines, WHEEL_DELTA);
		if (nToScroll == -1 || uWheelScrollLines == WHEEL_PAGESCROLL)
		{
			nDisplacement = m_pageDev.cx;
			if (zDelta > 0)
				nDisplacement = -nDisplacement;
		}
		else
		{
			nDisplacement = nToScroll * m_lineDev.cx;
			nDisplacement = min(nDisplacement, m_pageDev.cx);
		}
		bResult = OnScrollBy(CSize(nDisplacement, 0), TRUE);
	}

	if (bResult)
		UpdateWindow();

	return bResult;
}


static BOOL g_bGotScrollLines = FALSE;

UINT COXZoomView::GetMouseScrollLines()
{
	static UINT uCachedScrollLines;

	// if we've already got it and we're not refreshing,
	// return what we've already got

	if (g_bGotScrollLines)
		return uCachedScrollLines;

	// see if we can find the mouse window

	g_bGotScrollLines = TRUE;

	static UINT msgGetScrollLines;
	static WORD nRegisteredMessage;

	if (nRegisteredMessage == 0)
	{
		msgGetScrollLines = ::RegisterWindowMessage(MSH_SCROLL_LINES);
		if (msgGetScrollLines == 0)
			nRegisteredMessage = 1;     // couldn't register!  never try again
		else
			nRegisteredMessage = 2;     // it worked: use it
	}

	if (nRegisteredMessage == 2)
	{
		HWND hwMouseWheel = NULL;
		hwMouseWheel = ::FindWindow(MSH_WHEELMODULE_CLASS, MSH_WHEELMODULE_TITLE);
		if (hwMouseWheel && msgGetScrollLines)
		{
			uCachedScrollLines = (UINT)
				::SendMessage(hwMouseWheel, msgGetScrollLines, 0, 0);
			return uCachedScrollLines;
		}
	}

	// couldn't use the window -- try system settings
	uCachedScrollLines = 3; // reasonable default
	DWORD dwVersion = GetVersion();
	if (dwVersion < 0x80000000)              // Windows NT/2000/XP
	{
		HKEY hKey;
		if (RegOpenKeyEx(HKEY_CURRENT_USER,  _T("Control Panel\\Desktop"),
				0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
		{
			TCHAR szData[128];
			DWORD dwKeyDataType;
			DWORD dwDataBufSize = _countof(szData);

			if (RegQueryValueEx(hKey, _T("WheelScrollLines"), NULL, &dwKeyDataType,
					(LPBYTE) &szData, &dwDataBufSize) == ERROR_SUCCESS)
			{
				uCachedScrollLines = _tcstoul(szData, NULL, 10);
			}
			RegCloseKey(hKey);
		}
	}
#if _MFC_VER > 0x0421
//	else if (!afxData.bWin95)
#else
//	else if (!afxData.bWin32s)
#endif
	{
		::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &uCachedScrollLines, 0);
	}

	return uCachedScrollLines;
}


/////////////////////////////////////////////////////////////////////////////
// COXZoomView diagnostics

#ifdef _DEBUG
void COXZoomView::Dump(CDumpContext& dc) const
{
	ASSERT_VALID(this);

	CView::Dump(dc);

	AFX_DUMP1(dc, "\nm_totalLog = ", m_totalLog);
	AFX_DUMP1(dc, "\nm_totalDev = ", m_totalDev);
	AFX_DUMP1(dc, "\nm_pageDev = ", m_pageDev);
	AFX_DUMP1(dc, "\nm_lineDev = ", m_lineDev);
	AFX_DUMP1(dc, "\nm_bCenter = ", m_bCenter);
	AFX_DUMP1(dc, "\nm_bInsideUpdate = ", m_bInsideUpdate);
	AFX_DUMP0(dc, "\nm_nMapMode = ");
	switch (m_nMapMode)
	{
	case MM_NONE:
		AFX_DUMP0(dc, "MM_NONE");
		break;
	case MM_TEXT:
		AFX_DUMP0(dc, "MM_TEXT");
		break;
	case MM_LOMETRIC:
		AFX_DUMP0(dc, "MM_LOMETRIC");
		break;
	case MM_HIMETRIC:
		AFX_DUMP0(dc, "MM_HIMETRIC");
		break;
	case MM_LOENGLISH:
		AFX_DUMP0(dc, "MM_LOENGLISH");
		break;
	case MM_HIENGLISH:
		AFX_DUMP0(dc, "MM_HIENGLISH");
		break;
	case MM_TWIPS:
		AFX_DUMP0(dc, "MM_TWIPS");
		break;
	case MM_ANISOTROPIC:
		AFX_DUMP0(dc, "MM_ANISOTROPIC");
		AFX_DUMP1(dc, "\nm_sizeDev = ", m_sizeDev);
		break;
	default:
		AFX_DUMP0(dc, "*unknown*");
		break;
	}
	AFX_DUMP1(dc, "\nm_nZoomLevel = ", m_nZoomLevel);
	AFX_DUMP1(dc, "\nm_bAlignToBottom = ", m_bAlignToBottom);
	AFX_DUMP1(dc, "\nm_bUseRelativeScrollSizes = ", m_bUseRelativeScrollSizes);
	if(m_bUseRelativeScrollSizes)
	{
		AFX_DUMP1(dc, "\nm_nPagePercent = ", m_nPagePercent);
		AFX_DUMP1(dc, "\nm_nLinePercent = ", m_nLinePercent);
	}
	AFX_DUMP0(dc, "\nm_align = ");
	switch (m_align)
	{
	case ZV_TOPLEFT:
		AFX_DUMP0(dc, "TOPLEFT");
		break;
	case ZV_BOTTOMLEFT:
		AFX_DUMP0(dc, "BOTTOMLEFT");
		break;
	case ZV_CENTER:
		AFX_DUMP0(dc, "CENTER");
		break;
	default:
		AFX_DUMP0(dc, "*illegal*");
		break;
    }
}

void COXZoomView::AssertValid() const
{
	CView::AssertValid();

	switch (m_nMapMode)
	{
	case MM_NONE:
	case MM_TEXT:
	case MM_LOMETRIC:
	case MM_HIMETRIC:
	case MM_LOENGLISH:
	case MM_HIENGLISH:
	case MM_TWIPS:
	case MM_ANISOTROPIC:
		break;
	case MM_ISOTROPIC:
		ASSERT(FALSE); // illegal mapping mode
	default:
		ASSERT(FALSE); // unknown mapping mode
	}
}
#endif //_DEBUG



/////////////////////////////////////////////////////////////////////////////
// CZoomRect
// little helper class for zooming to a rectangle
// DON'T comment om this !!

BOOL CZoomRect::TrackTheMouse(UINT uMessageEnd, CWnd *pWnd, CRect& rectZoom, CPoint ptAnchor)
{
	// return FALSE if mouse is already captured
	if(NULL != CWnd::GetCapture())
		return(FALSE);
	CRect	rectClip;
	MSG		msg;
	// capture the mouse and clip it to the client area
	pWnd->SetCapture();
	pWnd->GetClientRect(rectClip);
	pWnd->ClientToScreen(rectClip);
	::ClipCursor(rectClip);
	// preset member vars
	m_bSuccess = FALSE;
	m_bFirstMove = TRUE;
	m_ptOrig = ptAnchor;
	m_uMessageEnd = uMessageEnd;
	msg.message = WM_NULL;	// safe start value 
	// looks weird, but works real nice
	while(ProcessMsg(&msg, pWnd))
		RetrieveMsg(&msg); 
	// free mouse and clipping
	::ClipCursor(NULL);
	::ReleaseCapture();
	if(!m_bFirstMove)	// at least one WM_MOUSEMOVE processed
	{
		CClientDC dc(pWnd);
        DrawZoomRect(&dc);
	}
	// setup return rect and normalize it
	rectZoom.TopLeft() = m_ptOrig;
	rectZoom.BottomRight() = m_ptLast;
	int ntemp;
	if(rectZoom.left > rectZoom.right)
	{
		ntemp = rectZoom.left;
		rectZoom.left = rectZoom.right;
		rectZoom.right = ntemp;
	}
	if(rectZoom.top > rectZoom.bottom)
	{
		ntemp = rectZoom.top;
		rectZoom.top = rectZoom.bottom;
		rectZoom.bottom = ntemp;
	}
	return(m_bSuccess);
}

void CZoomRect::RetrieveMsg(MSG* pmsg)
{
	// get a message out of the queue
	while(!::PeekMessage(pmsg, 0, 0, 0, PM_REMOVE));
	return;
}

BOOL CZoomRect::ProcessMsg(MSG* pmsg, CWnd* pWnd)
{
	// ending message ?
	if(m_uMessageEnd == pmsg->message)
	{
		m_bSuccess = TRUE;
		CPoint pt(pmsg->lParam);
		UpdateRectangle(pWnd, pt);
		return(FALSE);
	}
	switch(pmsg->message)
	{
		case WM_MOUSEMOVE:
		{
			CPoint pt(pmsg->lParam);
			UpdateRectangle(pWnd, pt);
		}
		    break;
		case WM_CANCELMODE:
			return(FALSE);
		case WM_KEYDOWN:
			if(VK_ESCAPE == pmsg->wParam)
				return(FALSE);
			// else fall through
		default:
			::TranslateMessage(pmsg);
			::DispatchMessage(pmsg);
	}
	return(TRUE);		
}

void CZoomRect::UpdateRectangle(CWnd* pWnd, CPoint& pt)
{
	CClientDC dc(pWnd);
	// if this is the first move, there is no old rect to remove
	if(m_bFirstMove)
		m_bFirstMove = FALSE;
	else	// erase old rect
		DrawZoomRect(&dc);
	m_ptLast = pt;
	DrawZoomRect(&dc);
}

void CZoomRect::DrawZoomRect(CDC* pDC)
{
	int nOldRop = pDC->SetROP2(R2_NOTXORPEN);
	pDC->MoveTo(m_ptOrig);
	pDC->LineTo(m_ptLast.x, m_ptOrig.y);
	pDC->LineTo(m_ptLast);
	pDC->LineTo(m_ptOrig.x, m_ptLast.y);
	pDC->LineTo(m_ptOrig);
	pDC->SetROP2(nOldRop);
}

