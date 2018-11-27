// QTracker.cpp
//
//===============================
// Version 1.0, August 20, 2003
// (c) Sjaak Priester, Amsterdam
// www.sjaakpriester.nl

#include "StdAfx.h"
#include "QTracker.h"
#include "QBufferDC.h"

#include "afxpriv.h"

QTracker::QTracker(CWnd * pWnd)
: m_pWnd(pWnd)
, m_Point(0)
, m_StartPoint(0)
, m_bTracking(false)
, m_bDirty(false)
{
	ASSERT_VALID(pWnd);
}

QTracker::~QTracker(void)
{
}

//  Track mouse, starting at positiom point.
int QTracker::Track(CDC * pDC, UINT nFlags, CPoint point, bool bClipCursor /*= false*/)
{
	if (::GetCapture()) return TrackFailed;	// Someone else has captured the mouse, cancel

	// Give derived class the opportunity to process starting message
	int r = OnBeginTrack(nFlags, point);
	if (r != TrackContinue) return r;		// cancel or success, even before tracking starts...

	m_bDirty = false;

	m_Point = point;

	m_PreviousPoint = m_Point;
	m_StartPoint = m_Point;
	nFlags &= UpdateMouseFlags;	// to be sure

	m_bTracking = true;

	// Undocumented; don't know what this does, but MFC's CRectTracker calls it too.
	::AfxLockTempMaps();

	int oldROP2(R2_COPYPEN);
	COLORREF oldBkColor(RGB(0, 0, 0));

	if (pDC)
	{
		// Prepare the dc for NOT-XOR drawing on white.
		oldROP2 = pDC->SetROP2(R2_NOTXORPEN);
		oldBkColor = pDC->SetBkColor(RGB(255, 255, 255));
	}

	// Let user draw first track object
	OnUpdate(pDC, nFlags | UpdateDraw | UpdateFirst);

	if (bClipCursor)
	{
		CRect rcClient;
		m_pWnd->GetClientRect(rcClient);
		m_pWnd->ClientToScreen(rcClient);
		VERIFY(::ClipCursor(rcClient));
	}

	m_pWnd->SetCapture();
	ASSERT(CWnd::GetCapture() == m_pWnd);
	TRACE("SetCapture\r\n");

	ASSERT(r == TrackContinue);

	while (r == TrackContinue)
	{
		MSG msg;

		int cnt = 0;
		while (! ::PeekMessage(& msg, NULL, 0, 0, PM_REMOVE))
		{
			// Give the statusbar the opportunity to update itself
			// by sending a private MFC message (see <afxpriv.h>).
			if (cnt == 0)
				::AfxGetMainWnd()->SendMessageToDescendants(WM_IDLEUPDATECMDUI, TRUE);
			cnt++;
		}

		if (CWnd::GetCapture() != m_pWnd || msg.message == WM_CANCELMODE) r = TrackFailed;

		else if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST)
		{
			// We use our own class QBufferDC for double buffered drawing.
			// We accumulate the removing of the old tracking rectangle
			// and the drawing of the new one, and update the screen with
			// the result. Thus, screen flicker is avoided.
			QBufferDC * pXDC = NULL;
			if (pDC)
			{
				pXDC = new QBufferDC(pDC, NOTSRCINVERT);

				// Prepare for NOT-XOR drawing (background color is white by default).
				pXDC->SetROP2(R2_NOTXORPEN);
			}

			// Remove old display feedback
			OnUpdate(pXDC, nFlags | UpdateRemove | UpdateEnter);

			// These casts ensure that sign is preserved
			int x = (int)(short) LOWORD(msg.lParam);
			int y = (int)(short) HIWORD(msg.lParam);

			// Convert to logical coordinates
			CPoint pnt(x, y);
			if (pDC) pDC->DPtoLP(& pnt);

			m_PreviousPoint = m_Point;
			m_Point = pnt;
			if (pnt != m_PreviousPoint) m_bDirty = true;

			nFlags = (UINT) msg.wParam & UpdateMouseFlags;

			// Let user change state
			r = OnMouseMessage(msg.message, nFlags, pnt);

			// QTracker's state is updated, now draw new track objects.
			OnUpdate(pXDC, nFlags | UpdateDraw | UpdateLeave);

			// QBufferDC's destructor will update the screen.
			if (pDC) delete pXDC;
		}

		else if (msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST)
			r = OnKeyMessage(msg.message, (UINT) msg.wParam,
				(UINT) LOWORD(msg.lParam), (UINT) HIWORD(msg.lParam));

		else r = OnMessage(msg);
	}
	VERIFY(::ReleaseCapture());
	TRACE("ReleaseCapture\r\n");
	if (bClipCursor) VERIFY(::ClipCursor(NULL));

	::AfxUnlockTempMaps();
	m_bTracking = false;

	// Let user clean up
	OnUpdate(pDC, nFlags | UpdateRemove | UpdateLast);

	if (r > 0 && !m_bDirty) r = TrackNoMove;

	r = OnEndTrack(r);

	if (pDC)
	{
		// Clean up pDC
		pDC->SetROP2(oldROP2);
		pDC->SetBkColor(oldBkColor);
	}
	
	return r;
}

int QTracker::OnBeginTrack(UINT /*nFlags*/, CPoint /*point*/)
{
	return TrackContinue;
}

int QTracker::OnEndTrack(int trackResult)
{
	return trackResult;
}

// Update the state of QTracker; should be overridden.
int QTracker::OnMouseMessage(UINT msg, UINT nFlags, CPoint /*point*/)
{
	// Default just checks for end of tracking operation.
	// return TrackSucceeded if msg == WM_LBUTTONUP,
	// return TrackCopy if msg == WM_LBUTTONUP and Ctrl pressed,
	// cancel tracking if msg == WM_RBUTTONDOWN,
	// otherwise, continue tracking.
	if (msg == WM_LBUTTONUP) return (nFlags & MK_CONTROL) ? TrackCopy : TrackSucceeded;
	if (msg == WM_RBUTTONDOWN) return TrackCancelled;
	return TrackContinue;
}

// Called after pressing or releasing a key during track. May be overridden.
int QTracker::OnKeyMessage(UINT msg, UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/)
{
	// Default: cancel tracking if escape key pressed.
	if (msg == WM_KEYDOWN && nChar == VK_ESCAPE) return TrackCancelled;
	return TrackContinue;
}

// Called for any other message during track. May be overridden.
int QTracker::OnMessage(MSG& msg)
{
	::DispatchMessage(& msg);
	return TrackContinue;
}

// Called during tracking when screen should be updated. Should be overridden.
void QTracker::OnUpdate(CDC * pDC, UINT /*nMode*/)
{
	// Default draws line in debug build, does nothing in release build.
#ifdef _DEBUG
	if (pDC)
	{
		pDC->MoveTo(m_StartPoint);
		pDC->LineTo(m_Point);
	}
#endif
}
