#pragma once

// QTracker.h

// ===============
// QTracker
//
// A versatile MFC-class implementing mouse tracking.
// Inherit from this class, adding drawing methods.
//
//===============================
// Version 1.0, August 20, 2003
// (c) Sjaak Priester, Amsterdam
// www.sjaakpriester.nl
//
// Freeware. Use at your own risk. Comments welcome.

class QTracker
{
public:
	enum TrackResult
	{
		TrackFailed = -2,
		TrackCancelled = -1,
		TrackContinue = 0,
		TrackSucceeded = 1,
		TrackCopy = 2,
		TrackNoMove = 3
	};
// Construction
	QTracker(CWnd * pWnd);		// pWnd points to the associated window.
	virtual ~QTracker();

// Attributes
	CPoint m_Point;				// the most recent point visited in logical coordinates
	CPoint m_PreviousPoint;		// the point visited before m_Point (logical coordinates)
	CPoint m_StartPoint;		// the starting point of the track operation (logical coordinates)

// Methods
	int Track(CDC * pDC, UINT nFlags, CPoint point, bool bClipCursor = false);
	// Track mouse, starting at position point.
	//
	// Parameters:
	// pDC			pointer to the (prepared) device context QTracker draws to. May be
	//				NULL for applications that don't draw;
	// nFlags		user flags, not used internally. Set this to the nFlags parameter
	//				of the mouse message that started the operation to have it available
	//				in OnBeginTrack() and OnUpdate();
	// point		the mouse position at which the track operation starts,
	//				in logical coordinates;
	// bClipCursor	if true, cursor movement is restricted to the client area of the 
	//				associated window.
	//
	// Return value: the track result
	// < 0			tracking operation was cancelled;
	// > 0			tracking completed, return value was set by OnMouseMessage().
	// The enum TrackResult lists some return values, but derived classes may return
	// other values as well.

	bool IsTracking() const		{ return m_bTracking; }
	// Returns true if Tracking.

protected:
	enum UpdateMode
	{
		UpdateMouseFlags = 0x007f, // MK_LBUTTON... etc.
		UpdateRemove = 0,
		UpdateDraw = 0x0400,
		UpdateFirst = 0x0800,
		UpdateEnter = 0x1000,
		UpdateLeave = 0x2000,
		UpdateLast = 0x4000
	};

// Overrideables
	virtual int OnBeginTrack(UINT nFlags, CPoint point);
	// Called at the beginning of the tracking operation.
	//
	// Parameters: the parameters of the Track() operation.
	// nFlags		user flags, as given to Track();
	// point		the position at which tracking starts, in logical coordinates.
	//
	// The return value determines further processing:
	// TrackContinue (0)	continue tracking;
	// < 0					cancel tracking, Track() returns with this value;
	// > 0					stop tracking, indicate success, Track() returns with this value.
	//
	// The default version just returns TrackContinue.

	virtual int OnEndTrack(int trackResult);
	// Called at the end of the tracking operation.
	//
	// Parameter:
	// trackResult	the proposed result of the tracking operation:
	//				< 0	tracking cancelled;
	//				> 0	tracking succeeded.
	//
	// The return value is the return value of the Track() operation.
	// Default does nothing, just returns trackResult.

	virtual int OnMouseMessage(UINT msg, UINT nFlags, CPoint point);
	// Update the state of QTracker.
	//
	// This function is called repeatedly during track. Shoud be overridden to
	// present feedback on the screen.
	//
	// Parameters:
	// msg			the Windows message that triggered the call, i.e. WM_MOUSEMOVE. May be
	//				any mouse message between WM_MOUSEFIRST and WM_MOUSELAST;
	// nFlags		the flags associated with the mouse message,
	//				as documented for WM_ONMOUSEMOVE or CWnd::OnMouseMove();
	// point		the position of the mouse pointer, in logical coordinates. If parameter
	//				pDC of Track() is NULL, point is in device coordinates.
	//
	// The return value determines further processing:
	// < 0					cancel tracking, Track() returns with this value;
	// TrackContinue (0)	continue tracking;
	// > 0					stop tracking, indicate success, Track() returns with this value.
	//
	// The default version does the following:
	// - returns TrackSucceeded at WM_LBUTTONUP;
	// - returns TrackCanceled at WM_RBUTTONDOWN;
	// - returns TrackContinue otherwise.

	// Called after pressing or releasing a key during track. May be overridden.
	virtual int OnKeyMessage(UINT msg, UINT nChar, UINT nRepCnt, UINT nFlags);
	//
	// Parameters:
	// msg			the Windows message that triggered the call, i.e. WM_KEYDOWN. May be
	//				any key message between WM_KEYFIRST and WM_KEYLAST;
	// nChar		the virtual key code;
	// nRepCnt		the repeat count;
	// nFlags		the flags associated with the key message.
	// See the documentation for WM_KEYDOWN or CWnd::OnKeyDown().
	//
	// The return value determines further processing:
	// < 0					cancel tracking, Track() returns with this value;
	// TrackContinue (0)	continue tracking;
	// > 0					stop tracking, indicate success, Track() returns with this value.
	//
	// The default version returns TrackCanceled if the Escape key is pressed,
	// TrackContinue otherwise.

	// Called for any other windows message during track. May be overridden.
	virtual int OnMessage(MSG& msg);
	//
	// Parameters:
	// msg			the Windows message that triggered the call.
	//
	// The return value determines further processing:
	// < 0					cancel tracking, Track() returns with this value;
	// TrackContinue (0)	continue tracking;
	// > 0					stop tracking, indicate success, Track() returns with this value.
	//
	// The default version dispatches the message and returns TrackContinue.
	// Overrides in derived classes may call this base function.

	// Called during tracking whenever screen should be updated. Should be overridden.
	virtual void OnUpdate(CDC * pDC, UINT nMode);
	//
	// Parameters:
	// pDC			pointer to the device context;
	// nMode		combination of UpdateMode flags and parameter nFlags of Track().
	//
	// Default draws a line between m_StartPoint and m_Point in debug build, does
	// nothing in release build.
	
// Implementation
	CWnd * m_pWnd;		// The associated window
	bool m_bTracking;	// true if tracking
	bool m_bDirty;
};
