/**************************************************************************
	Tracker.cpp

	copyright (c) 2013/07/21 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#include "StdAfx.h"
#include "Tracker.h"
#include "GDI+Helper.h"
#include "PetriSimView.h"

CTracker::CTracker(CPetriSimView* pView) 
: QTracker(pView)
, m_pView(pView)
, m_Mode(TrackSelect)
, m_bLoaded(false)
, m_Handle(0)
, m_FixedSrc(0)
{
	ASSERT_VALID(m_pView);
}

CTracker::~CTracker(void)
{
}

int CTracker::Track(UINT nFlags, CPoint point, bool OnGrid)
{
	if (::GetCapture())
	{ 
		return TrackFailed;
	}

	// Give derived class the opportunity to process starting message
	int r = OnBeginTrack(nFlags, point);

	if (r != TrackContinue) return r;		// cancel or success, even before tracking starts...

	m_bDirty = false;

	if (OnGrid)
	{
		m_pView->GetDoc().ToGrid(point);
	}

	m_Point = point;
	m_PreviousPoint = point;
	m_StartPoint = point;

	m_FixedSrc = GetHandlePoint(m_Handle, true);

	nFlags &= UpdateMouseFlags;	// to be sure

	m_bTracking = true;

	// Undocumented; don't know what this does, but MFC's CRectTracker calls it too.
	::AfxLockTempMaps();

	// Let user draw first track object
	OnUpdate(0, nFlags | UpdateDraw | UpdateFirst);

	m_pWnd->SetCapture();
	ASSERT(CWnd::GetCapture() == m_pWnd);

	while (r == TrackContinue)
	{
		MSG msg;

		int cnt = 0;
		while (! ::GetMessage(& msg, NULL, 0, 0))
		/*while (! ::PeekMessage(& msg, NULL, 0, 0, PM_REMOVE))*/
		{
			// Give the status bar the opportunity to update itself
			// by sending a private MFC message (see <afxpriv.h>).
			if (cnt == 0)
				::AfxGetMainWnd()->SendMessageToDescendants(WM_IDLEUPDATECMDUI, TRUE);
			cnt++;
		}

		if (CWnd::GetCapture() != m_pWnd || msg.message == WM_CANCELMODE)
		{
			r = TrackFailed;
		}
		else if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST)
		{

			// These casts ensure that sign is preserved
			int x = (int)(short) LOWORD(msg.lParam);
			int y = (int)(short) HIWORD(msg.lParam);

			// Convert to logical coordinates
			CPoint point2(x, y);
			m_pView->DPtoLP(&point2);

			if (OnGrid)
			{
				m_pView->GetDoc().ToGrid(point2);
			}

			m_PreviousPoint = m_Point;
			m_Point = point2;

			nFlags = (UINT) msg.wParam & UpdateMouseFlags;

			// Let user change state
			r = OnMouseMessage(msg.message, nFlags, point2);

			if (point != m_PreviousPoint) 
			{
				m_bDirty = true;
				OnUpdate(0, nFlags | UpdateDraw | UpdateLeave);

				m_lastpoint = m_Point;
			}
		}
		else if (msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST)
		{
			r = OnKeyMessage(msg.message, (UINT) msg.wParam,
			(UINT) LOWORD(msg.lParam), (UINT) HIWORD(msg.lParam));
		}
		else
		{
			r = OnMessage(msg);
		}
	}

	VERIFY(::ReleaseCapture());

	::AfxUnlockTempMaps();
	m_bTracking = false;

	// Let user clean up
	OnUpdate(0, nFlags | UpdateRemove | UpdateLast);

	if (r > 0 && !m_bDirty) r = TrackNoMove;

	r = OnEndTrack(r);

	return r;
}

void CTracker::OnUpdate( CDC * pDC, UINT nMode )
{
	if (IsTracking() && m_Mode == TrackSelect)
	{
		if (nMode & UpdateEnter)
		{
			m_UpdateRect = CRect(m_StartPoint, m_PreviousPoint);
			m_UpdateRect.NormalizeRect();
		}

		else if (nMode & UpdateDraw)
		{
			CRect rect(m_StartPoint, m_Point);
			rect.NormalizeRect();

			m_UpdateRect.UnionRect(m_UpdateRect, rect);
			m_UpdateRect.InflateRect(1, 1);

			m_pView->LPtoDP(&m_UpdateRect);
			m_pView->InvalidateRect(&m_UpdateRect);
		}
	}
	else if (IsLoaded())
	{
		if (nMode & UpdateEnter)
		{
			m_UpdateRect = FromRect(m_NowTracker);
			m_UpdateRect.NormalizeRect();
		}
		
		else if (nMode & UpdateDraw)
		{
			m_NowTracker = m_StartTracker;

			Transform(m_Transform, m_NowTracker);

			CRect rect(FromRect(m_NowTracker));
			rect.NormalizeRect();

			m_UpdateRect.UnionRect(m_UpdateRect, rect);
			m_UpdateRect.InflateRect(5, 5);

			m_pView->LPtoDP(&m_UpdateRect);
			m_pView->InvalidateRect(&m_UpdateRect);
		}
		
		else if (nMode & UpdateLast)
		{
			m_NowTracker = m_StartTracker;

			Transform(m_Transform, m_NowTracker);

			CRect rect(FromRect(m_NowTracker));
			rect.NormalizeRect();

			m_UpdateRect.InflateRect(5, 5);

			m_pView->LPtoDP(&rect);
			m_pView->InvalidateRect(&rect);

			m_StartTracker = m_NowTracker;
			m_Transform.Reset();
		}
	}
}

int CTracker::OnMouseMessage(UINT msg, UINT nFlags, CPoint point)
{
	switch (m_Mode)
	{
		case TrackMove:
		case TransformMove:
			{
				CSize sz = point - m_StartPoint;

				m_Transform.Reset();
				m_Transform.Translate(static_cast<REAL>(sz.cx), static_cast<REAL>(sz.cy));
			}
			break;
		case TransformScale:
			{
				
				CSize sz = point - m_FixedSrc;
				CSize szSrc = m_StartPoint - m_FixedSrc;

				REAL scaleX = 1.0f;
				REAL scaleY = 1.0f;

				if (m_Handle & 1) // corner, scale both
				{
					if (szSrc.cx != 0) 
						scaleX = (REAL) sz.cx / (REAL) szSrc.cx;

					if(scaleX < 0.05f) 
						scaleX = 0.05f;

					if (szSrc.cy != 0) 
						scaleY = (REAL) sz.cy / (REAL) szSrc.cy;

					if(scaleY < 0.05f) 
						scaleY = 0.05f;
				}

				else if (m_Handle == 2 || m_Handle == 6) // horizontal edge, scale vertical
				{
					if (szSrc.cy != 0) 
						scaleY = (REAL) sz.cy / (REAL) szSrc.cy;

					if(scaleY < 0.05f) 
						scaleY = 0.05f;
				}

				else if (m_Handle == 4 || m_Handle == 8) // vertical edge, scale horizontal
				{
					if (szSrc.cx != 0) 
						scaleX = (REAL) sz.cx / (REAL) szSrc.cx;

					if(scaleX < 0.05f) 
						scaleX = 0.05f;
				}

				m_Transform.Reset();

				// Translate the fixed point to the origin.
				m_Transform.Translate((REAL) - m_FixedSrc.x, (REAL) - m_FixedSrc.y, MatrixOrderAppend);

				// Scale the object.
				m_Transform.Scale(scaleX, scaleY, MatrixOrderAppend);

				// Translate back to fixed point (which may be different).
				m_Transform.Translate((REAL) m_FixedSrc.x, (REAL) m_FixedSrc.y, MatrixOrderAppend);
			
			}
			break;
	}

	return __super::OnMouseMessage(msg, nFlags, point);
}

void CTracker::Draw( Graphics& g )
{
	if (IsTracking() && m_Mode == TrackSelect)
	{
		CRect rect(m_StartPoint, m_Point);
		rect.NormalizeRect();

		Rect rect2 = ToRect(rect);

		SolidBrush brush(Color(20, 0, 0, 255));
		g.FillRectangle(&brush, rect2);

		Pen pen(Color::Blue);
		g.DrawRectangle(&pen, rect2);
	}
	else if (IsLoaded())
	{
		Pen pen(Color::Blue);
		SolidBrush brush(Color(50,0,0,0));

		Rect rcs[8];

		for (int loop = 0; loop<8; loop++)
		{
			GetHandle(rcs[loop], loop+1);
		}

		g.DrawRectangle(&pen, m_NowTracker);
		g.FillRectangles(&brush, rcs, 8);
		g.DrawRectangles(&pen, rcs, 8);
	}
}

void CTracker::GetTracker( CRect & rect )
{
	rect = CRect(m_StartPoint, m_Point);
	rect.NormalizeRect();
}

void CTracker::Load( const Rect & rect )
{
	if (rect.IsEmptyArea())
	{
		Clear();
		return;
	}

	m_StartTracker = rect;
	m_NowTracker = m_StartTracker;
	m_bLoaded = true;
}

void CTracker::Clear()
{
	m_bLoaded = false;
}

CPoint CTracker::GetHandlePoint(int Handle, bool bOpposite)
{
	CPoint pt(0);

	switch (Handle)
	{
	case 1: pt.x = m_NowTracker.GetRight(); pt.y = m_NowTracker.GetBottom(); break;
	case 2: pt.x = m_NowTracker.GetLeft()+m_NowTracker.Width/2; pt.y = m_NowTracker.GetBottom(); break;
	case 3: pt.x = m_NowTracker.GetLeft(); pt.y = m_NowTracker.GetBottom(); break;
	case 4: pt.x = m_NowTracker.GetLeft(); pt.y = m_NowTracker.GetTop()+m_NowTracker.Height/2; break;
	case 5: pt.x = m_NowTracker.GetLeft(); pt.y = m_NowTracker.GetTop(); break;
	case 6: pt.x = m_NowTracker.GetLeft()+m_NowTracker.Width/2; pt.y = m_NowTracker.GetTop(); break;
	case 7: pt.x = m_NowTracker.GetRight(); pt.y = m_NowTracker.GetTop(); break;
	case 8: pt.x = m_NowTracker.GetRight(); pt.y = m_NowTracker.GetTop()+m_NowTracker.Height/2; break;
	default: pt.x = m_NowTracker.GetRight()+m_NowTracker.Width/2; pt.y = m_NowTracker.GetTop()+m_NowTracker.Height/2; break;
	}

	return pt;
}

bool CTracker::GetHandle( Rect & rect, int Handle)
{
	rect = Rect();

	// 1------2------3
	// 8-------------4
	// 7------6------5

	switch (Handle)
	{
	case 1: rect.X = m_NowTracker.X; rect.Y = m_NowTracker.Y; break;
	case 2: rect.X = m_NowTracker.X+m_NowTracker.Width/2; rect.Y = m_NowTracker.Y; break;
	case 3: rect.X = m_NowTracker.X+m_NowTracker.Width; rect.Y = m_NowTracker.Y; break;
	case 4: rect.X = m_NowTracker.X+m_NowTracker.Width; rect.Y = m_NowTracker.Y+m_NowTracker.Height/2; break;
	case 5: rect.X = m_NowTracker.X+m_NowTracker.Width; rect.Y = m_NowTracker.Y+m_NowTracker.Height; break;
	case 6: rect.X = m_NowTracker.X+m_NowTracker.Width/2; rect.Y = m_NowTracker.Y+m_NowTracker.Height; break;
	case 7: rect.X = m_NowTracker.X; rect.Y = m_NowTracker.Y+m_NowTracker.Height; break;
	case 8: rect.X = m_NowTracker.X; rect.Y = m_NowTracker.Y+m_NowTracker.Height/2; break;
	default: return false;
	}

	if (Handle%2 == 0)
	{
		rect.Inflate(3, 3);
	}
	else
	{
		rect.Inflate(4, 4);
	}

	return true;
}

int CTracker::HitTestHandle( const CPoint& point, int Hint /*= 0*/ )
{
	Rect rect;

	for (int handle = 1; GetHandle(rect, handle); handle++)
	{
		if (rect.Contains(ToPoint(point)))
		{
			return handle;
		}
	}
	return 0;
}

HCURSOR CTracker::GetCursor( const CPoint& point, int Hint /*= 0*/ )
{
	switch (HitTestHandle(point, Hint))
	{
	case 2:
	case 6:
		return AfxGetApp()->LoadStandardCursor(IDC_SIZENS);
	case 4:
	case 8:
		return AfxGetApp()->LoadStandardCursor(IDC_SIZEWE);
	case 1:
	case 5:
		return AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE);
	case 3:
	case 7:
		return AfxGetApp()->LoadStandardCursor(IDC_SIZENESW);
	default: return 0;
	}
}

bool CTracker::IsLoaded()
{
	return m_bLoaded;
}

bool CTracker::HandleTest( const CPoint& point, int Hint )
{
	m_Handle = HitTestHandle(point, Hint);

	return m_Handle > 0;
}