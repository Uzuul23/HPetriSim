// ==========================================================================
// 						Class Specification : COXZoomView
// ==========================================================================

// Header file : OXZoomView.h

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.                      
                         
// //////////////////////////////////////////////////////////////////////////
// 
// Properties:
//	YES	Abstract class (does not have any objects)
//	YES	Derived from CView

//	YES	Is a Cwnd.                     
//	YES	Two stage creation (constructor & Create())
//	YES Has a message map
//	NO  Needs a resource (template)

//	YES	Persistent objects (saveable on disk)      
//	YES Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
//	
//		COXZoomView - a virtual base class for views that need zooming and scrolling.
//		It's based on CScrollView, but significantly modified and enhanced
//		Additions/Alterations to CScrollView: 
//		- Handles MM_ANISOTROPIC.
//		- Scrollsizes now definable in percent of the client area, updated on resizing.
//		- ScrollToPosition allows the anchor to be the upper left corner, or additionally
//		  the lower left corner or the center of the window. 
//		- Accepts a CRect for the document size, so the origin can be located in the 
//		  lower left corner or the center or any arbitrary point of the document. You don't
//		  need to handle negative y-coords any more ! 
//		- Zoom level can vary between 10% and 1000%
//		- When zooming, either keeps the upper left window corner, the lower left window
//		  corner or keeps the center.
//		- Allows zooming up to a rectangle (in client coords).
//		- After zooming, the rectangle can be justified to either the upper left or
//		  lower left corner or to the window center.
//		- Allows zooming to the window size (so the scrollbars just disappear).
//		- Output can be aligned to the bottom of the view, when the view's height is 
//		  bigger than the document's.
//
	

// Remark:
//		MM_SCALETOFIT not supported (no need, use CScrollView).
//		Placement in splitter windows not supported (how to handle different zoom levels?).
//		RESTRICTION: the users display shouldn't have more than 3200 pixels in either
//		direction, iow. the viewport extents cannot exceed some 32000 units.

// Prerequisites (necessary conditions):
//		

/////////////////////////////////////////////////////////////////////////////

#ifndef __ZOOMVIEW_H__
#define __ZOOMVIEW_H__ 

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "OXDllExt.h"

#ifdef OXZOOMVIEW_USE_RULER
#include "OXRulerOrganizer.h"
#endif	//	OXZOOMVIEW_USE_RULER

#define ID_MULTICOEF		10

class COXZoomView : public CView
{
	DECLARE_DYNAMIC(COXZoomView)

protected:
	COXZoomView();
	virtual ~COXZoomView();

// Data Members
public:

	#ifdef WIN32
		static AFX_DATA const SIZE sizeDefault;
	#else
		static const SIZE AFXAPI_DATA sizeDefault;
	#endif

	// used to specify default calculated page and line sizes
		
    enum ZoomAlignment { ZV_DEFAULT, ZV_TOPLEFT, ZV_BOTTOMLEFT, ZV_CENTER };
    enum { MINZOOM = 10, MAXZOOM = 1000 };

protected:
	int				m_nMapMode;
	CSize			m_totalLog;
	CRect			m_rectLog;
	CSize			m_totalDev;
	CSize			m_pageDev;
	CSize			m_lineDev;
	CSize			m_sizeDev;
	BOOL			m_bCenter;
	BOOL			m_bInsideUpdate;
	int				m_nZoomLevel;
	BOOL			m_bAlignToBottom;
	int				m_nPagePercent;
	int				m_nLinePercent;
	BOOL			m_bUseRelativeScrollSizes;
	ZoomAlignment	m_align;

#ifdef OXZOOMVIEW_USE_RULER
	// ruller object
	COXRulerOrganizer m_ruler;
#endif	//	OXZOOMVIEW_USE_RULER

// Member Functions
public:

	// --- In: pDC: device context in which the filling is to be done
	//		   pBrush: brush with which the area is to be filled
	// --- Out: none
	// --- Returns: none
	// --- Effect: fills the area of the view that appears outside of the scrolling
	//			   area. Use FillOutsideRect in your scroll view’s OnEraseBkgnd handler
	//			   function to prevent excessive background repainting.
	void FillOutsideRect(CDC* pDC, CBrush* pBrush);


	// --- In: none
	// --- Out: none
	// --- Returns: whether alignment to bottom is active or not
	// --- Effect: none
    BOOL GetAlignToBottom() const;

	// --- In: 
	// --- Out:
	// --- Returns: whether centering of the contents is active or not
	// --- Effect: none
    BOOL IsCentered() const;

	
	// --- In: none
	// --- Out: none
	// --- Returns: The horizontal and vertical positions (in device units) of the
	//				scroll boxes as a CPoint object. (in device units)
	// --- Effect: Call GetDeviceScrollPosition when you need the current horizontal and
	//			   vertical positions of the scroll boxes in the scroll bars. This coordinate pair
	//			   corresponds to the location in the document to which the upper-left corner of
	//			   the view has been scrolled. This is useful for offsetting mouse-device positions
	//			   to scroll-view device positions.
	CPoint GetDeviceScrollPosition() const;


	// --- In: none
	// --- Out: nMapMode: the current mapping mode for this view. For a list of possible values,
	//					 see SetScrollSizes.
	//		    sizeTotal: the current total size of the scroll view in device units.
	//		    sizePage: the current horizontal and vertical amounts to scroll in each direction in response
	//					  to a mouse click in a scroll-bar shaft.
	//		    sizeLine: the current horizontal and vertical amounts to scroll in each direction in
	//					  response to a mouse click in a scroll arrow.
	// --- Returns: none
	// --- Effect: Gets the current mapping mode, the total size, and the line and page sizes of the
	//			   scrollable view. Sizes are in device units. This member function is rarely called.
	void GetDeviceScrollSizes(int& nMapMode, SIZE& sizeTotal, SIZE& sizePage, SIZE& sizeLine) const;


	// --- In : align: the alignement of 
	// --- Out : none
	// --- Returns : The horizontal and vertical positions (in device units) of the
	//				scroll boxes as a CPoint object. (logical units)
	// --- Effect :  This call takes an alignment argument, in order to get back the
	//				 position of one of the alignment reference points.
	CPoint GetScrollPosition(ZoomAlignment Align = ZV_DEFAULT) const; 

	
	// --- In : none
	// --- Out : none
	// --- Returns : the logical size
	// --- Effect :
	CSize GetTotalSize() const;


	// --- In: none
	// --- Out: none
	// --- Returns: the state of the zoom alignment
	// --- Effect: none
    ZoomAlignment GetZoomAlign();


	// --- In : none
	// --- Out : none
	// --- Returns : the current zoom level in percent.
	// --- Effect :
	int GetZoomLevel() const;

// maintained for backward compatibility with CScrollView

	// --- In  : nMapMode: the mapping mode to set for this view
	//			 sizeTotal: The total size of the zoom view. The cx member contains
	//						the horizontal extent. The cy member contains the vertical
	//						extent. Sizes are in logical units. Both cx and cy must be
	//						greater than or equal to 0.
	//			 sizePage:	The horizontal and vertical amounts to scroll in each
	//						direction in response to a mouse click in a scroll-bar shaft.
	//			 sizeLine:  The horizontal and vertical amounts to scroll in each
	//						direction in response to a mouse click in a scroll arrow.
	// --- Out : none
	// --- Returns : none
	// --- Effect : Use this to set TEXT, ENGLISH or METRIC mapping modes. You have
	//				to specify the mode and the size of the document. If you are not
	//				using MM_TEXT make the rectangles top member bigger than the bottom
	//				member, because these modes assume that the y coords are getting
	//				bigger downside up. If you make top 0 and bottom negative you'll get
	//				normal CScrollView behaviour, having y always negative ore 0. If you
	//				make bottom 0 and top positive, you y coords are always positive.
	//				You can even shift the origin to the center of your doc by
	//				specifying the proper rectangle.
	void SetScrollSizes(int nMapMode, SIZE sizeTotal,
					    const SIZE& sizePage = sizeDefault,
						const SIZE& sizeLine = sizeDefault);


// three overloaded enhanced Versions to set the sizes
// allows control of logical origin

	// --- In  : nMapMode: the mapping mode to set for this view
	//			 rectDoc: the total size of the document.
	//			 nPagePercent: the amount to scroll in response to a mouse click in a
	//						   scroll-bar shaft
	//			 nLinePercent: the amount to scroll in response to a mouse click in a
	//						   scroll arrow
	// --- Out : none
	// --- Returns : none
	// --- Effect : Use this to set TEXT, ENGLISH or METRIC mapping modes. You have
	//				to specify the mode and the size of the document. If you are not
	//				using MM_TEXT make the rectangles top member bigger than the bottom
	//				member, because these modes assume that the y coords are getting
	//				bigger downside up. If you make top 0 and bottom negative you'll get
	//				normal CScrollView behaviour, having y always negative ore 0. If you
	//				make bottom 0 and top positive, you y coords are always positive.
	//				You can even shift the origin to the center of your doc by
	//				specifying the proper rectangle.
	void SetDeviceScrollSizesRelative(int nMapMode, const CRect& rectDoc, int nPagePercent = 25, int nLinePercent = 5);

	// --- In  : sizeDevice: size of the view in pixels
	//			 rectDoc: the total size of the document.
	//			 nPagePercent: the amount to scroll in response to a mouse click in a
	//						   scroll-bar shaft
	//			 nLinePercent: the amount to scroll in response to a mouse click in a
	//						   scroll arrow
	// --- Out : none
	// --- Returns : none
	// --- Effect : Use this for MM_ANISOTROPIC. You specify the document rectangle
	//				as usual and the corresponding size of the view in pixels. cx and
	//				cy of the size have to be positive, COXZoomView will invert cy if
	//				necessary.
	void SetDeviceScrollSizesRelative(SIZE sizeDevice, const CRect& rectDoc, int nPagePercent = 25, int nLinePercent = 5);

	// --- In  : nMapMode: the mapping mode to set for this view
	//			 sizeTotal: The total size of the zoom view. The cx member contains
	//						the horizontal extent. The cy member contains the vertical
	//						extent. Sizes are in logical units. Both cx and cy must be
	//						greater than or equal to 0.
	//			 nPagePercent: The amount to scroll in response to a mouse click in a
	//						   scroll-bar shaft.
	//			 nLinePercent: The amount to scroll in response to a mouse click in a
	//						   scroll arrow.
	// --- Out : none
	// --- Returns : none
	// --- Effect : this is the most compatible version to CScrollView. This Call will
	//				synthesize a rect based on the passed SIZE and the mapping mode.
	void SetDeviceScrollSizesRelative(int nMapMode, SIZE sizeTotal,	int nPagePercent, int nLinePercent);


	// --- In : bAlignToBottom:
	// --- Out : none
	// --- Returns : none
	// --- Effect : If you call this API with TRUE, COXZoomView will display your data
	//				aligned to the bottom, if the size of the view is bigger than it
	//				has to be to display the whole document.
	void SetAlignToBottom(BOOL bAlignToBottom);

	// --- In : bCenter:
	// --- Out : none
	// --- Returns : none
	// --- Effect : If you call this API with TRUE, COXZoomView will display your data
	//				centered in the screen, if the size of the view is bigger than it
	//				has to be to display the whole document.
	void Center( BOOL bCenter );



	// --- In : align:
	// --- Out : none
	// --- Returns : none
	// --- Effect : This sets the default ZoomAlignment for your view, you can specify
	//				one of the enumerated values: ZV_TOPLEFT, ZV_BOTTOMLEFT, ZV_CENTER.
	void SetZoomAlign(ZoomAlignment align);


	// --- In : nNewLevel: the new zoom level in percent.
	//			align: the new default ZoomAlignment for your view (ZV_TOPLEFT,
	//				   ZV_BOTTOMLEFT, ZV_CENTER)
	// --- Out : none
	// --- Returns : the previous zoom level inpercent
	// --- Effect : Sets the new zoom level and updates the window, using one of the alignment
	//				reference points.
	int SetZoomLevel(int nNewLevel, ZoomAlignment Align = ZV_DEFAULT);


	// --- In: pt: the point to scroll to, in logical units; the cx member must be
	//			   a positive value (greater than or equal to 0, up to the total size
	//			   of the view); the same is true for the cy member when the mapping
	//			   mode is MM_TEXT;
	//			   the cy member is negative in mapping modes other than MM_TEXT.
	//		   align: the new default ZoomAlignment for your view (ZV_TOPLEFT,
	//				  ZV_BOTTOMLEFT, ZV_CENTER)
	// --- Out: none
	// --- Returns: none
	// --- Effect: Scrolls one of the alignment reference points to the desired POINT
	//			   position. If you specify ZV_DEFAULT it uses the value set up with
	//			  SetZoomAlignment.
	void ScrollToPosition(POINT pt, ZoomAlignment Align = ZV_DEFAULT);    

	// --- In: none
	// --- Out: none
	// --- Returns: none
	// --- Effect: Fits the view into the current window and updates the zoom
	//			   level accordingly.
	void ZoomToWindow();


	// --- In: rectZoom: the rectangle you want to zoom out
	//		   align: the new default ZoomAlignment for your view (ZV_TOPLEFT,
	//				   ZV_BOTTOMLEFT, ZV_CENTER)
	// --- Out: none
	// --- Returns: none
	// --- Effect: Zooms the view up, so that the passed rectangle in client coords fills
	//			   the window as much as possible, using one of the reference points.
	void ZoomToRectangle(CRect rectZoom, ZoomAlignment Align = ZV_DEFAULT);


	// helper for mouse wheel support
	BOOL DoMouseWheel(UINT fFlags, short zDelta, CPoint point);


#ifdef OXZOOMVIEW_USE_RULER
	// --- In  :	bHorzRuler	-	horizontal ruler band will be displayed
	//				bVertRuler	-	vertical ruler band will be displayed
	// --- Out : 
	// --- Returns:	TRUE if COXRulerOrganizer object was successfully attached
	// --- Effect:	attaches to COXRulerOrganizer object which is responsible
	//				for displaying ruler bars
	inline BOOL AttachRuler(BOOL bHorzRuler=TRUE, BOOL bVertRuler=TRUE) { 
		if(m_ruler.IsAttached())
		{
			SetShowHorzRulerBar(bHorzRuler);
			SetShowVertRulerBar(bVertRuler);
			return TRUE;
		}
		return m_ruler.Attach(this,bHorzRuler,bVertRuler);
	}

	// --- In  :	
	// --- Out : 
	// --- Returns:	
	// --- Effect:	detaches the attached COXRulerOrganizer object which is responsible
	//				for displaying ruler bars
	inline void DetachRuler() { m_ruler.Detach(); }


	// --- In  :
	// --- Out : 
	// --- Returns:	pointer to the internal COXRulerOrganizer object that is used to 
	//				draw ruler bars
	// --- Effect:	retrieves the pointer to the internal COXRulerOrganizer object 
	//				that is used to draw ruler bars
	inline COXRulerOrganizer* GetRulerOrganizer() { return &m_ruler; }


	// --- In  :	bShowVertRulerBar	-	if TRUE then vertical ruler bar will
	//										be displayed
	// --- Out : 
	// --- Returns:	
	// --- Effect:	sets the flag that specifies whether to display the 
	//				vertical ruler bar or not
 	inline void SetShowVertRulerBar(BOOL bShowVertRulerBar) {
		m_ruler.SetShowVertRulerBar(bShowVertRulerBar);
	}

	// --- In  :	
	// --- Out : 
	// --- Returns:	TRUE if vertical ruler bar is displayed or FALSE otherwise
	// --- Effect:	retrieves the flag that specifies whether to display the 
	//				vertical ruler bar or not
	inline BOOL GetShowVertRulerBar() const { return m_ruler.GetShowVertRulerBar(); }


	// --- In  :	bShowHorzRulerBar	-	if TRUE then horizontal ruler bar will
	//										be displayed
	// --- Out : 
	// --- Returns:	
	// --- Effect:	sets the flag that specifies whether to display the 
	//				horizontal ruler bar or not
	inline void SetShowHorzRulerBar(BOOL bShowHorzRulerBar) {
		m_ruler.SetShowHorzRulerBar(bShowHorzRulerBar);
	}

	// --- In  :	
	// --- Out : 
	// --- Returns:	TRUE if horizontal ruler bar is displayed or FALSE otherwise
	// --- Effect:	retrieves the flag that specifies whether to display the 
	//				horizontal ruler bar or not
	inline BOOL GetShowHorzRulerBar() const { return m_ruler.GetShowHorzRulerBar(); }

#endif	//	OXZOOMVIEW_USE_RULER


#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

protected:

	// virtual function, used to initialize the COXScrollWnd object after 
	// it was created or was used to subclass created window
	virtual BOOL Initialize();

	void ScrollToDevicePosition(POINT ptDev); // explicit scrolling no checking
	
	// helper functions for zooming
	void SetMapMode(CDC* pDC) const;
	void ScaleViewport(CDC* pDC) const { pDC->ScaleViewportExt(m_nZoomLevel, 100, m_nZoomLevel, 100); }

	virtual void OnDraw(CDC* pDC) = 0;      // pass on pure virtual

	void UpdateBars(BOOL bSendRecalc = TRUE); // adjust scrollbars etc
	BOOL GetTrueClientSize(CSize& size, CSize& sizeSb);
		// size with no bars
	void GetScrollBarSizes(CSize& sizeSb);
	void GetScrollBarState(CSize sizeClient, CSize& needSb,
		CSize& sizeRange, CPoint& ptMove, BOOL bInsideClient);

	virtual void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);

	// scrolling implementation support for OLE 2.0
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE);

	static UINT GetMouseScrollLines();

	//{{AFX_MSG(COXZoomView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT fFlags, short zDelta, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
};

// little helper class for zooming to a rectangle
// DON'T comment on this !!
class CZoomRect
{
public:
	BOOL TrackTheMouse(UINT uMessageEnd, CWnd *pWnd, CRect& rectZoom, CPoint ptAnchor);
protected:
	void RetrieveMsg(MSG* pmsg);
	BOOL ProcessMsg(MSG* pmsg, CWnd* pWnd);
	void UpdateRectangle(CWnd* pWnd, CPoint& pt);
	void DrawZoomRect(CDC* pDC);
	BOOL m_bFirstMove;
	BOOL m_bSuccess;
	UINT m_uMessageEnd;
	CPoint m_ptOrig;
	CPoint m_ptLast;
};

#include "OXZoomVw.inl"

#endif // __ZOOMVIEW_H__
