//===============================
// QTransformTracker.cpp
//===============================
// Version 1.03, August 31, 2005:
// Resolved MS KB 208856 issue (cursors disappearing under static link)
//
// Version 1.02, August 14, 2005:
// Resolved ambiguous call to atan2, to make it compile with VC++ 7.1
//
// Version 1.01, August 30, 2003:
// Added check for Windows version at pen creation (PS_ALTERNATE only on Win2000 and later)
//
// Version 1.0, August 20, 2003:
// (c) Sjaak Priester, Amsterdam
// www.sjaakpriester.nl

#include "StdAfx.h"
#include "QTransformTracker.h"

#include <math.h>

namespace
{
	const double pi = 4.0 * atan(1.0);
	// Note: Do _not_ use const REAL pi... etc.
	// For some reason beyond my comprehension, VC++ 7.1 then makes pi = 0.0000f!

	enum Handle
	{
		HandleTopLeft = 0,
		HandleTop = 1,
		HandleTopRight = 2,
		HandleRight = 3,
		HandleBottomRight = 4,
		HandleBottom = 5,
		HandleBottomLeft = 6,
		HandleLeft = 7,
		HandleCenter = 8, 
		HandleBody = 9,
		HandleNothing = -1
	};
}

HCURSOR QTransformTracker::g_hCursor[10] = { 0 };

QTransformTracker::QTransformTracker(CWnd * pWnd)
: QTracker(pWnd)
, m_Options(OptionDefault)
, m_Mode(TransformNone)
, m_Handle(HandleNothing)
, m_bLoaded(false)
, m_bSpace(false)
, m_bMapFlip(false)
, m_bAlt(false)
, m_colorMark(RGB(192, 192, 192))
, m_colorHandles(RGB(0, 0, 0))
, m_colorCenter(RGB(0, 0, 0))
, m_colorTrack(RGB(0, 0, 0))
, m_colorPath(RGB(0, 0, 0))
, m_HandleSize(4)
, m_InnerMargin(4)
, m_OuterMargin(2)
, m_FixedSrc(0)
, m_FixedDest(0)
, m_StartPhi(0.0f)
, m_pGraphicsPath(NULL)
, m_pPathPoints(NULL)
, m_pPathTypes(NULL)
, m_IndicatorString(_T(""))
{
	if (g_hCursor[0] == NULL)
	{
		// Load static default cursors
		// Note: if linking to a static MFC-library,
		// do not define _AFX_NO_TRACKER_RESOURCES in the resource file.
		// Bug in Visual Studio. See: KB-article 208856 on MSDN.

		HINSTANCE hInst = ::AfxFindResourceHandle(
			MAKEINTRESOURCE(AFX_IDC_TRACKNWSE), RT_GROUP_CURSOR);

		UINT DefaultCursorIDs[] =
		{
			AFX_IDC_TRACKNWSE, AFX_IDC_TRACKNESW, AFX_IDC_TRACKNS, AFX_IDC_TRACKWE,
			AFX_IDC_TRACK4WAY,
			AFX_IDC_MOVE4WAY, AFX_IDC_MOVE4WAY,
			AFX_IDC_TRACK4WAY, AFX_IDC_TRACK4WAY, AFX_IDC_TRACK4WAY
		};
		for (int i = 0; i < sizeof(DefaultCursorIDs)/sizeof(UINT); i++)
		{
			if (g_hCursor[i] != NULL) continue;	// already loaded by user
			LoadCursor(i, DefaultCursorIDs[i], hInst);
			ASSERT(g_hCursor[i]);
			// If you get an assert here, there's probably something wrong
			// with your resources.
		}
	}
}

QTransformTracker::~QTransformTracker(void)
{
	delete m_pGraphicsPath;
	delete[] m_pPathPoints;
	delete[] m_pPathTypes;
}

int QTransformTracker::OnBeginTrack(UINT nFlags, CPoint point)
{
	m_Transform.Reset();
	if (! m_bLoaded) return TrackFailed;

	m_Handle = HandleTest(point);
	if (m_Handle == HandleNothing) return TrackFailed;
	
	else if (m_Handle == HandleCenter) m_Mode = TransformCenter;
	else if (m_Handle == HandleBody) m_Mode = TransformMove;
	else
	{
		m_Mode = TransformScale;
		if (nFlags & MK_CONTROL)
		{
			if ((m_Handle & 1) && (m_Options & OptionShear)) m_Mode = TransformShear;
			if (! (m_Handle & 1) && (m_Options & OptionRotate)) m_Mode = TransformRotate;
		}

		m_bAlt = false;
		if (m_Options & OptionCenter) m_bAlt = ::GetKeyState(VK_MENU) < 0;
		if (m_Mode == TransformRotate && (m_Options & OptionRotateReverseAlt)) m_bAlt ^= true;

		SetFixedPoints(m_bAlt);
	}
	SetCursor(m_Handle);
	return TrackContinue;
}

int QTransformTracker::OnEndTrack(int trackResult)
{
	if (m_Mode == TransformCenter && trackResult == TrackCopy) trackResult = TrackSucceeded;
	if (trackResult < 0)
	{
		m_Transform.Reset();
		::CopyMemory(m_PointDest, m_PointSrc, 5 * sizeof(POINT));
	}
	m_Mode = TransformNone;
	m_IndicatorString.Empty();
	return trackResult;
}

int QTransformTracker::OnMouseMessage(UINT msg, UINT nFlags, CPoint point)
{
	bool bPrevAlt = m_bAlt;
	m_bAlt = false;
	if (m_Options & OptionCenter) m_bAlt = ::GetKeyState(VK_MENU) < 0;
	if (m_Mode == TransformRotate && (m_Options & OptionRotateReverseAlt)) m_bAlt ^= true;

	// Alt key changed; change fixed points
	if (m_bAlt != bPrevAlt) SetFixedPoints(m_bAlt);

	if (::GetKeyState(VK_SPACE) < 0)
	{
		// Space bar pressed, accumulate move in m_Transform
		CSize sz = point - m_PreviousPoint;
		m_Transform.Translate((REAL) sz.cx, (REAL) sz.cy, MatrixOrderAppend);

		SetIndicatorString(TransformMove, (REAL) point.x, (REAL) point.y);

		m_bSpace = true;
		return QTracker::OnMouseMessage(msg, nFlags, point);
	}

	if (m_bSpace)	// Space bar released
	{
		SetFixedPoints(m_bAlt);
		m_bSpace = false;
	}

	switch (m_Mode)
	{
	case TransformMove:
		{
			if (nFlags & MK_SHIFT) point = RestrictPoint(point, m_StartPoint);
			CSize sz = point - m_StartPoint;
			m_Transform.Reset();
			m_Transform.Translate((REAL) sz.cx, (REAL) sz.cy);

			SetIndicatorString(TransformMove, (REAL) point.x, (REAL) point.y);
		}
		break;

	case TransformCenter:
		{
			if (nFlags & MK_SHIFT) point = RestrictPoint(point, m_StartPoint);
			m_PointSrc[4] = m_PointDest[4] = point;
			// Center point is in m_PointSrc[4], no transformation, so Dest == Src

			SetIndicatorString(TransformCenter, (REAL) point.x, (REAL) point.y);
		}
		break;

	case TransformScale:
		{
			CSize sz = point - m_FixedDest;
			CSize szSrc = m_StartPoint - m_FixedSrc;

			REAL scaleX = 1.0f;
			REAL scaleY = 1.0f;

			if ((m_Handle & 1) == 0)	// corner, scale both
			{
				if (szSrc.cx != 0) scaleX = (REAL) sz.cx / (REAL) szSrc.cx;
				if (! (m_Options & OptionAllowMirror) && scaleX < 0.0f) scaleX = 0.0f;

				if (szSrc.cy != 0) scaleY = (REAL) sz.cy / (REAL) szSrc.cy;
				if (! (m_Options & OptionAllowMirror) && scaleY < 0.0f) scaleY = 0.0f;

				if (nFlags & MK_SHIFT)	// maintain proportions
				{
					REAL scaleXabs = fabsf(scaleX);
					REAL scaleYabs = fabsf(scaleY);

					if (scaleXabs > scaleYabs) scaleX = (scaleX < 0) ? - scaleYabs : scaleYabs;
					else scaleY = (scaleY < 0) ? - scaleXabs : scaleXabs;
				}

				// Set cursor; might be changed after flipping
				int curs = (m_Handle & 2) / 2;				// 0 or 1
				if (m_bMapFlip) curs ^= 1;
				curs ^= scaleX < 0;
				curs ^= scaleY < 0;

				HCURSOR h = g_hCursor[curs];
				if (h) ::SetCursor(h);
			}

			else if ((m_Handle & 3) == 1)	 // horizontal edge, scale vertical
			{
				if (szSrc.cy != 0) scaleY = (REAL) sz.cy / (REAL) szSrc.cy;
				if (! (m_Options & OptionAllowMirror) && scaleY < 0.0f) scaleY = 0.0f;
				if (nFlags & MK_SHIFT) scaleX = scaleY;
			}

			else if ((m_Handle & 3) == 3)	 // vertical edge, scale horizontal
			{
				if (szSrc.cx != 0) scaleX = (REAL) sz.cx / (REAL) szSrc.cx;
				if (! (m_Options & OptionAllowMirror) && scaleX < 0.0f) scaleX = 0.0f;
				if (nFlags & MK_SHIFT) scaleY = scaleX;
			}

			m_Transform.Reset();

			// Translate the fixed point to the origin.
			m_Transform.Translate((REAL) - m_FixedSrc.x, (REAL) - m_FixedSrc.y, MatrixOrderAppend);

			// Scale the object.
			m_Transform.Scale(scaleX, scaleY, MatrixOrderAppend);

			// Translate back to fixed point (which may be different).
			m_Transform.Translate((REAL) m_FixedDest.x, (REAL) m_FixedDest.y, MatrixOrderAppend);

			SetIndicatorString(TransformScale, (REAL) m_StartRect.Width() * scaleX,
				(REAL) m_StartRect.Height() * scaleY);
		}
		break;

	case TransformRotate:
		{
			CSize sz = point - m_FixedDest;

			REAL phi = 180.0f * atan2((REAL) sz.cy, (REAL) sz.cx) / (REAL) pi - m_StartPhi;
			while (phi <= -180.0f) phi += 360.0f;	// (-180, 180]

			if (nFlags & MK_SHIFT)
			{
				int a = 15 * (int)((phi + 367.5f) / 15.0f);	// multiple of 15 degrees
				while (a > 180) a -= 360;	// (-180, 180]
				phi = (REAL) a;
			}

			m_Transform.Reset();
			m_Transform.Translate((REAL) - m_FixedSrc.x, (REAL) - m_FixedSrc.y);
			m_Transform.Rotate(phi, MatrixOrderAppend);
			m_Transform.Translate((REAL) m_FixedDest.x, (REAL) m_FixedDest.y, MatrixOrderAppend);

			SetIndicatorString(TransformRotate, m_bMapFlip ? phi : -phi);
		}
		break;

	case TransformShear:
		{
			CSize sz = point - m_FixedDest;
			CSize szSrc = m_StartPoint - m_FixedSrc;

			REAL shearX = 0.0f;
			REAL shearY = 0.0f;
			REAL scaleX = 1.0f;
			REAL scaleY = 1.0f;

			if (m_Handle & 2)		// vertical edge
			{
				if (sz.cx != 0) shearY = (REAL) sz.cy / (REAL) sz.cx;
				if (szSrc.cx != 0) scaleX = (REAL) sz.cx / (REAL) szSrc.cx;
				if (! (m_Options & OptionAllowMirror) && scaleX < 0.0f) scaleX = 0.0f;

				SetIndicatorString(TransformShear, shearY * -100.0f);
			}
			else		// horizontal edge
			{
				if (sz.cy != 0) shearX = (REAL) sz.cx / (REAL) sz.cy;
				if (szSrc.cy != 0) scaleY = (REAL) sz.cy / (REAL) szSrc.cy;
				if (! (m_Options & OptionAllowMirror) && scaleY < 0.0f) scaleY = 0.0f;

				SetIndicatorString(TransformShear, shearX * -100.0f);
			}

			m_Transform.Reset();
			m_Transform.Translate((REAL) - m_FixedSrc.x, (REAL) - m_FixedSrc.y);

			// Don't scale the other direction if Shift is pressed
			if ((nFlags & MK_SHIFT) == 0) m_Transform.Scale(scaleX, scaleY, MatrixOrderAppend);

			m_Transform.Shear(shearX, shearY, MatrixOrderAppend);
			m_Transform.Translate((REAL) m_FixedDest.x, (REAL) m_FixedDest.y, MatrixOrderAppend);
		}
		break;

	default:
		break;
	}

	return QTracker::OnMouseMessage(msg, nFlags, point);
}

void QTransformTracker::OnUpdate(CDC * pDC, UINT nMode)
{
	if (m_Handle == HandleCenter)
	{
		DrawCenter(pDC, m_PointSrc[4], TRUE);
		return;
	}

	int nPathPoints = 0;
	if (m_pGraphicsPath) nPathPoints = m_pGraphicsPath->GetPointCount();

	if (nMode & UpdateDraw)	// Apply transformation
	{
		// The transformed Track Rectangle and the path are cached, so they won't
		// have to be transformed again when OnUpdate() is called with UpdateRemove.

		// Copy four untransformed points of the Track Rectangle, plus center point
		::CopyMemory(m_PointDest, m_PointSrc, 5 * sizeof(POINT));

		// Transform them
		m_Transform.TransformPoints((Point *) m_PointDest, 5);

		if (nPathPoints > 0) // If a path is loaded, and it has some points...
		{
			// ...refresh the points...
			m_pGraphicsPath->GetPathPoints((Point *) m_pPathPoints, nPathPoints);

			// ... and transform them too.
			m_Transform.TransformPoints((Point *) m_pPathPoints, nPathPoints);
		}
	}

	// Draw the Track Rectangle. QTracker has set the ROP-mode to NOT-XOR.
	CPen * pTrackPen = NULL;

	if (_winmajor > 4 && m_Options & OptionTrackDotted)
	{
		LOGBRUSH lb;
		lb.lbColor = m_colorTrack;
		lb.lbHatch = 0;
		lb.lbStyle = BS_SOLID;
		
		pTrackPen = new CPen(PS_COSMETIC | PS_ALTERNATE, 1, & lb);
	}
	else pTrackPen = new CPen(PS_SOLID, 0, m_colorTrack);

	CGdiObject * pOldPen = NULL;
	if (pTrackPen) pOldPen = pDC->SelectObject(pTrackPen);
	// TODO: solve draw problem
	//pDC->Polygon(m_PointDest, 4);
	if (pOldPen) pDC->SelectObject(pOldPen);
	delete pTrackPen;

	if (nPathPoints > 0)
	{
		// Draw the path. We do this in 'ordinary' gdi, because GDI+ doesn't
		// understand NOT-XOR.

		CPen * pPathPen = NULL;

		if (_winmajor > 4 && m_Options & OptionPathDotted)
		{
			LOGBRUSH lb;
			lb.lbColor = m_colorPath;
			lb.lbHatch = 0;
			lb.lbStyle = BS_SOLID;
			
			pPathPen = new CPen(PS_COSMETIC | PS_ALTERNATE, 1, & lb);
		}
		else pPathPen = new CPen(PS_SOLID, 0, m_colorPath);

		CGdiObject * pOldPen = NULL;
		if (pPathPen) pOldPen = pDC->SelectObject(pPathPen);

		// Next is a variation of code in the MFC documentation
		// for CDC::BeginPath(). Draw a GDI+ GraphicsPath in gdi.
		// We could have used CDC::PolyDraw(), but it isn't supported on Win98 and WinME.
		int i;
		LPPOINT pLastMoveTo = NULL;

		for (i = 0; i < nPathPoints; i++)
		{
			BYTE type = m_pPathTypes[i];
			type &= ~(PathPointTypeDashMode | PathPointTypePathMarker);

			switch(type)
			{
			case PathPointTypeStart:
				if (pLastMoveTo && i > 0) pDC->LineTo(* pLastMoveTo);
				pDC->MoveTo(m_pPathPoints[i]);
				pLastMoveTo = & m_pPathPoints[i];
				break;

			case PathPointTypeLine | PathPointTypeCloseSubpath:
				pDC->LineTo(m_pPathPoints[i]);
				if (pLastMoveTo) pDC->LineTo(* pLastMoveTo);
				pLastMoveTo = NULL;
				break;

			case PathPointTypeLine:
				pDC->LineTo(m_pPathPoints[i]);
				break;

			case PathPointTypeBezier | PathPointTypeCloseSubpath:
				pDC->PolyBezierTo(& m_pPathPoints[i], 3);
				i += 2;
				if (pLastMoveTo) pDC->LineTo(* pLastMoveTo);
				pLastMoveTo = NULL;
				break;

			case PathPointTypeBezier:
				pDC->PolyBezierTo(&m_pPathPoints[i], 3);
				i += 2;
				break;

			default:
				break;
			}
		}
		if (pLastMoveTo && i > 1) pDC->LineTo(* pLastMoveTo);

		if (pOldPen) pDC->SelectObject(pOldPen);
		delete pPathPen;
	}

	DrawCenter(pDC, m_PointDest[4], TRUE);
}

int QTransformTracker::OnKeyMessage(UINT msg, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	ASSERT(m_Handle != HandleNothing);

	if (nChar == VK_CONTROL || nChar == VK_SPACE) SetCursor(m_Handle);
	return QTracker::OnKeyMessage(msg, nChar, nRepCnt, nFlags);
}

void QTransformTracker::Draw(CDC * pDC)
{
	if (! m_bLoaded) return;

	// All drawing is done in NOT-XOR mode.
	int nOldRop = pDC->SetROP2(R2_NOTXORPEN);
	DrawMarkRect(pDC);
	DrawCenter(pDC, m_PointSrc[4], FALSE);
	pDC->SetROP2(nOldRop);
}

void QTransformTracker::DrawMarkRect(CDC * pDC)
{
	ASSERT_VALID(pDC);

	CRect rc(m_StartRect);

	CGdiObject * pOldBrush = pDC->SelectStockObject(NULL_BRUSH);

	CPen * pMarkPen = NULL;
	if (_winmajor > 4 && m_Options & OptionMarkDotted)
	{
		LOGBRUSH lb;
		lb.lbColor = m_colorMark;
		lb.lbHatch = 0;
		lb.lbStyle = BS_SOLID;
		
		pMarkPen = new CPen(PS_COSMETIC | PS_ALTERNATE, 1, & lb);
	}
	else pMarkPen = new CPen(PS_SOLID, 0, m_colorMark);

	CGdiObject * pOldPen = NULL;
	if (pMarkPen) pOldPen = pDC->SelectObject(pMarkPen);
	pDC->Rectangle(& rc);
	
	CPen penHandles(PS_SOLID, 0, m_colorHandles);
	pDC->SelectObject(& penHandles);
	for (int i = 0; i < 8; i++) pDC->Rectangle(GetHandleRect(i));

	if (pOldPen) pDC->SelectObject(pOldPen);
	if (pOldBrush) pDC->SelectObject(pOldBrush);

	delete pMarkPen;
}

void QTransformTracker::DrawCenter(CDC * pDC, POINT center, bool bTrack)
{
	ASSERT_VALID(pDC);

	if (! (m_Options & OptionCenter)) return;

	CPen pen(PS_SOLID, 0, m_colorCenter);
	CGdiObject * pOldPen = pDC->SelectObject(& pen);

	CRect rcInside(center, CSize(0, 0));
	int d = m_HandleSize / 2;
	if (bTrack) d++;

	rcInside.InflateRect(d, d);
	pDC->Ellipse(& rcInside);
	rcInside.InflateRect(d, d);
	CRect rcOutside(rcInside);

	rcOutside.InflateRect(m_HandleSize, m_HandleSize);

	if (bTrack && (m_Options & OptionCenterMove))
	{
		pDC->MoveTo(rcInside.TopLeft());
		pDC->LineTo(rcOutside.TopLeft());

		pDC->MoveTo(rcInside.right, rcInside.top);
		pDC->LineTo(rcOutside.right, rcOutside.top);

		pDC->MoveTo(rcInside.BottomRight());
		pDC->LineTo(rcOutside.BottomRight());

		pDC->MoveTo(rcInside.left, rcInside.bottom);
		pDC->LineTo(rcOutside.left, rcOutside.bottom);
	}
	else
	{
		pDC->MoveTo(center.x, rcInside.top);
		pDC->LineTo(center.x, rcOutside.top);

		pDC->MoveTo(rcInside.right, center.y);
		pDC->LineTo(rcOutside.right, center.y);

		pDC->MoveTo(center.x, rcInside.bottom);
		pDC->LineTo(center.x, rcOutside.bottom);

		pDC->MoveTo(rcInside.left, center.y);
		pDC->LineTo(rcOutside.left, center.y);
	}

	if (pOldPen) pDC->SelectObject(pOldPen);
}

void QTransformTracker::Load(CRect& rc, bool bSetCenter, CDC * pDC)
{
	// Remove possible old
	if (pDC) Draw(pDC);

	m_StartRect = rc;

	m_StartRect.NormalizeRect();
	m_bLoaded = ! m_StartRect.IsRectEmpty();

	if (! m_bLoaded) return;

	m_StartRect.InflateRect(m_InnerMargin, m_InnerMargin);
	SetPoints(bSetCenter);

	if (pDC) Draw(pDC);
}

void QTransformTracker::Load(Rect& rect, bool bSetCenter, CDC * pDC)
{
	CRect rc(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
	Load(rc, bSetCenter, pDC);
}

void QTransformTracker::Load(GraphicsPath& path, bool bSetCenter, CDC * pDC)
{
	delete m_pGraphicsPath;
	m_pGraphicsPath = NULL;
	delete[] m_pPathPoints;
	m_pPathPoints = NULL;
	delete[] m_pPathTypes;
	m_pPathTypes = NULL;

	m_pGraphicsPath = path.Clone();
	if (! m_pGraphicsPath) return;

	int n = path.GetPointCount();
	if (n > 0)
	{
		// Reserve space for path points...
		m_pPathPoints = new POINT[n];
		if (! m_pPathPoints)
		{
			delete m_pGraphicsPath;
			m_pGraphicsPath = NULL;
			return;
		}

		// ... and for point types
		m_pPathTypes = new BYTE[n];
		// Get the point types. We'll retrieve the points in OnUpdate().
		if (m_pPathTypes) path.GetPathTypes(m_pPathTypes, n);
		else
		{
			delete m_pGraphicsPath;
			m_pGraphicsPath = NULL;
			return;
		}
	}

	Rect rc;
	path.GetBounds(& rc);
	Load(rc, bSetCenter, pDC);
}

void QTransformTracker::Clear(CDC * pDC)
{
	if (pDC) Draw(pDC);
	m_bLoaded = FALSE;
}

void QTransformTracker::SetMetrics(UINT handleSize, UINT innerMargin, UINT outerMargin, CDC * pDC)
{
	// Remove possible old
	if (pDC) Draw(pDC);

	m_StartRect.DeflateRect(m_InnerMargin, m_InnerMargin);

	m_HandleSize = handleSize;
	m_InnerMargin = innerMargin;
	m_OuterMargin = outerMargin;

	m_StartRect.InflateRect(m_InnerMargin, m_InnerMargin);
	SetPoints(FALSE);

	if (pDC) Draw(pDC);
}

void QTransformTracker::GetMetrics(UINT& handleSize, UINT& innerMargin, UINT& outerMargin) const
{
	handleSize = m_HandleSize;
	innerMargin = m_InnerMargin;
	outerMargin = m_OuterMargin;
}

BOOL QTransformTracker::OnSetCursor(CDC * pDC)
{
	if (! m_bLoaded || ! pDC) return FALSE;

	CPoint point;
	::GetCursorPos(& point);
	m_pWnd->ScreenToClient(& point);
	pDC->DPtoLP(& point);

	CSize szVPExt = pDC->GetViewportExt();
	m_bMapFlip = (szVPExt.cx ^ szVPExt.cy) < 0;

	int iHandle = HandleTest(point);
	if (iHandle == HandleNothing) return FALSE;

	return SetCursor(iHandle);
}

void QTransformTracker::SetPoints(bool bSetCenter)
{
	CRect rc(m_StartRect);
	rc.InflateRect(m_OuterMargin, m_OuterMargin);

	m_PointSrc[0].x = m_PointSrc[3].x = rc.left;
	m_PointSrc[1].x = m_PointSrc[2].x = rc.right;
	m_PointSrc[0].y = m_PointSrc[1].y = rc.top;
	m_PointSrc[2].y = m_PointSrc[3].y = rc.bottom;

	if (bSetCenter) m_PointSrc[4] = m_StartRect.CenterPoint();
	else m_PointSrc[4] = m_PointDest[4];

	::CopyMemory(m_PointDest, m_PointSrc, 5 * sizeof(POINT));
}

CRect QTransformTracker::GetHandleRect(int iHandle)
{
	if (iHandle == HandleBody) return m_StartRect;

	CRect rc;
	rc.SetRectEmpty();
	rc.OffsetRect(GetHandlePoint(m_PointSrc, iHandle));
	int d = m_HandleSize / 2;
	rc.InflateRect(d, d);
	return rc;
}

CPoint QTransformTracker::GetHandlePoint(LPPOINT pPoints, int iHandle)
{
	// Handles are numbered clockwise, like this:
	//
	//		0		   1		  2
	//		*----------*----------*
	//		|					  |
	//		|					  |
	//	  7 *		   * 8		  * 3
	//		|					  |
	//		|					  |
	//		*----------*----------*
	//		6		   5		  4
	//
	// The center point is handle 8, the body is handle 9.

	if (iHandle >= 8) return pPoints[4];	// center

	int i = iHandle / 2;
	if ((iHandle & 1) == 0) return pPoints[i];	// corner

	int j = i + 1;
	if (j > 3) j = 0;

	CPoint pnt;
	pnt.x = (pPoints[i].x + pPoints[j].x) / 2;
	pnt.y = (pPoints[i].y + pPoints[j].y) / 2;
	return pnt;		// edge
}

// logical coordinates
int QTransformTracker::HandleTest(CPoint point)
{
	int i;

	for (i = 0; i < 10; i++)
		if (GetHandleRect(i).PtInRect(point)) break;

	if (i > HandleBody) i = HandleNothing;

	else if (i == HandleCenter && !(m_Options & OptionCenterMove)) i = HandleBody;
	
	if (i == HandleBody && m_pGraphicsPath
		&& ! m_pGraphicsPath->IsVisible(point.x, point.y)) i = HandleNothing;
	return i;
}

BOOL QTransformTracker::SetCursor(int iHandle)
{
	bool bCtrl = ::GetKeyState(VK_CONTROL) < 0;

	int curs = CursorMove;	// body

	if (iHandle == HandleCenter	&& (m_Options & OptionCenterMove))
		curs = CursorCenter;	// center

	else if (::GetKeyState(VK_SPACE) < 0) iHandle = HandleBody;
			// if space pressed, move rectangle

	if (iHandle < 8)
	{
		curs = (iHandle & 2) / 2;							// 0 or 1
		if ((iHandle & 1) == NULL)	// corner
		{
			if ((bCtrl && (m_Options & OptionRotate)) || m_Mode == TransformRotate)
				curs = CursorRotate;	// rotate
			else if (m_bMapFlip) curs ^= 1;
		}
		else
		{
			curs += 2;	// 2 or 3
			if ((bCtrl && (m_Options & OptionShear)) || m_Mode == TransformShear)
				curs += 6;	// shear, 8 or 9
		}
	}

	// If Ctrl is pressed, change CursorMove to CursorCopy
	if (bCtrl && iHandle == HandleBody) curs = CursorCopy;	// copy

	HCURSOR h = g_hCursor[curs];
	if (h) ::SetCursor(h);

	return h != 0;
}

void QTransformTracker::SetFixedPoints(bool bAlt)
{
	// If bAlt is true, fixed point is center point, otherwise it is
	// the handle opposite to m_Handle.
	int hFixed = bAlt ? HandleCenter : (m_Handle ^ 4);

	m_FixedSrc = GetHandlePoint(m_PointSrc, hFixed);
	m_FixedDest = GetHandlePoint(m_PointDest, hFixed);

	if (m_Mode == TransformRotate)
	{
		CSize sz = CPoint(m_PointSrc[m_Handle / 2]) - m_FixedSrc;
		m_StartPhi = 180.0f * (REAL) atan2((REAL) sz.cy, (REAL) sz.cx) / (REAL) pi;
	}
}

CPoint QTransformTracker::RestrictPoint(CPoint point, CPoint pntBase)
{
	// Restrict point to horizontal, vertical or diagonal with respect to pntBase
	CSize d = point - pntBase;
	int cxAbs = abs(d.cx);
	int cyAbs = abs(d.cy);

	bool bHandled = FALSE;

	if (cxAbs > 2 * cyAbs)			// 0 degrees
	{
		point.y = pntBase.y;
		bHandled = true;
	}
	else if (cyAbs > 2 * cxAbs)		// 90 degrees
	{
		point.x = pntBase.x;
		bHandled = true;
	}
	
	if (! bHandled)					// 45 degrees
	{
		if (cxAbs > cyAbs)
			point.x = pntBase.x + ((d.cx < 0) ? -cyAbs : cyAbs);
		else point.y = pntBase.y + ((d.cy < 0) ? -cxAbs : cxAbs);
	}
	return point;
}

void QTransformTracker::SetIndicatorString(Mode mode, REAL x, REAL y)
{
	int xi = (int) x;
	int yi = (int) y;

	switch (mode)
	{
	case TransformMove:
	case TransformCenter:
		m_IndicatorString.Format(_T("x:%d  y:%d"), xi, yi);
		break;
	case TransformScale:
		m_IndicatorString.Format(_T("%d \xd7 %d"), xi, yi);
		break;
	case TransformRotate:
		m_IndicatorString.Format(_T("%d\xb0"), xi);
		break;
	case TransformShear:
		m_IndicatorString.Format(_T("%d%%"), xi);
		break;
	default:
		ASSERT(0);
		break;
	}
}

// Load one of the cursors QTransformTracker will display
/* static */
bool QTransformTracker::LoadCursor(int type, UINT nResourceID, HINSTANCE hInst)
{
	ASSERT(type >= 0 && type <= 10);

	HCURSOR h(0);
	if (nResourceID)
	{
		if (hInst) h = ::LoadCursor(hInst, MAKEINTRESOURCE(nResourceID));
		else h = ::AfxGetApp()->LoadCursor(nResourceID);
	}

	g_hCursor[type] = h;

	return h != 0;
}
