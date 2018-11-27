#pragma once
#include "QTracker.h"

// =======================
// QTransformTracker
//
// This class allows a rectangle or a GDI+ GraphicsPath to be moved and transformed
// in different fashions.
// It is more or less like MFC's CRectTracker, but has far more options.
// QTransformTracker is derived from QTracker (see: QTracker.h).
//
// If an object (rectangle or path) is loaded in QTransformTracker, it is surrounded
// by eight handles. Another handle is at the center point.
//
// QTransformTracker has the following modes of operation.
// - move		drag the object at the body. If Shift is pressed, moving is restricted
//				to horizontal, vertical or diagonal;
// - scale		drag one of the eight handles. If Shift is pressed, the proportion of
//				the object is preserved;
// - rotate		press Ctrl, and drag one of the corner handles. If Shift is pressed,
//				the rotation angle is restricted to a multiple of 15 degrees;
// - shear		press Ctrl, and drag one of the edge handles. If Shift is not pressed,
//				you can scale in the other direction;
//
// In general, all transformations are with respect to the opposite handle. If Alt is
// pressed, the transformations are with respect to the Center Point.
//
// If the space bar is pressed, all transformations revert to move.
//
// The Center Point can be moved by dragging it. If Shift is pressed, moving is restricted
// to horizontal, vertical or diagonal.
//
// The transformation is accumulated in a GDI+ Matrix. After tracking, it can be retrieved
// with the GetTransform() member function.
//
// During tracking, member function GetIndicatorString() gives textual information about
// the current operation. This may be presented on the status bar or used for other forms
// of user feedback.
//
// Usage:
// - load QTransformTracker with one of the Load() member functions;
// - at mouse down, call the base member function Track() (see: QTracker.h);
// - if Track() returns with a positive value, retrieve the transformation matrix
//		with GetTransform().
//
// Note: QTransformTracker is incompatible with W95 or W98.
//===============================
// Version 1.0, August 20, 2003
// (c) Sjaak Priester, Amsterdam
// www.sjaakpriester.nl
//
// Freeware. Use at your own risk. Comments welcome.

class QTransformTracker :
	public QTracker
{
public:
	enum Option
	{
		OptionRotate = 0x0001,
		OptionShear = 0x0002,
		OptionAllowMirror = 0x0004,
		OptionCenter = 0x0008,
		OptionCenterMove = 0x0010,
		OptionRotateReverseAlt = 0x0020,
		OptionMarkDotted = 0x0040,
		OptionTrackDotted = 0x0080,
		OptionPathDotted = 0x0100,

		OptionDefault
			= OptionRotate
			| OptionShear
			| OptionAllowMirror
			| OptionCenter
			| OptionCenterMove
			| OptionRotateReverseAlt
			| OptionTrackDotted
	};

// Construction
	QTransformTracker(CWnd * pWnd);		// pWnd points to the associated window
	virtual ~QTransformTracker();


// Loading
	void Load(GraphicsPath& path, bool bSetCenter = true, CDC * pDC = NULL);
	void Load(CRect& rc, bool bSetCenter = true, CDC * pDC = NULL);
	void Load(Rect& rect, bool bSetCenter = true, CDC * pDC = NULL);
	// Loads QTransformTracker with a GraphicsPath or a rectangle.
	// After loading, transforming can be performed.
	//
	// If bSetCenter == true, the Center Point is set to the center point of the rectangle,
	// or to the center point of the bounding rectangle of path.
	// If bSetCenter == false, the Center Point is unchanged.
	//
	// If pDC is not NULL, pDC is updated.

	void Clear(CDC * pDC = NULL);
	// Clears any GraphicsPath or rectangle.
	// After clearing, no transformation can be performed.
	//
	// If pDC is not NULL, pDC is updated.

	bool IsLoaded(void) const	{ return m_bLoaded; }

	void Draw(CDC * pDC);
	// Draws the Marker Rectangle, the static appearance of QTransformTracker.
	// Drawing is done in NOT-XOR mode, meaning that a second call to Draw will
	// erase the results of the first call.
	// Typically, you should only call Draw() from the application's OnDraw-handler.

	Matrix * GetTransform(void) const	{ return m_Transform.Clone(); }
	// Get the result of the tracking operation as a GDI+ transformation matrix.
	// The return value is a clone; user should delete it.

	BOOL OnSetCursor(CDC * pDC);
	// Should be called by the OnSetCursor-handler of the associated window.
	// If QTransformTracker is loaded, it sets the cursor to indicate the mode of transform.
	// You may also call this at an OnKeyUp/Down-handler for the Ctrl-key and space bar.
	//
	// Parameter:
	// pDC		pointer to (prepared) DC of associated window.
	//
	// Return:
	// TRUE if OnSetCursor did set the cursor, FALSE otherwise.

	LPCTSTR GetIndicatorString() const	{ return m_IndicatorString; }

// Options and appearance
	UINT m_Options;
	// The following options can be set by setting this variable to
	// a combination of the following flags:
	//	- OptionRotate				allows rotation (default);
	//	- OptionShear				allows shearing (default);
	//	- OptionAllowMirror			allows mirroring of object by scaling (default);
	//	- OptionCenter				displays center point, allows Alt-functionality (default);
	//	- OptionCenterMove			make the center point moveable by dragging (default);
	//	- OptionRotateReverseAlt	reverses the function of the Alt key at rotation (default);
	//	- OptionMarkDotted			draw the Mark Rectangle with a dotted line;
	//	- OptionTrackDotted			draw the Track Rectangle with a dotted line (default);
	//	- OptionPathDotted			draw the GraphicsPath with a dotted line.
	// Note that the combination of OptionCenter == false and OptionCenterMove == true
	// leads to undefined behaviour.

	void SetMetrics(UINT handleSize, UINT innerMargin, UINT outerMargin, CDC * pDC = NULL);
	void GetMetrics(UINT& handleSize, UINT& innerMargin, UINT& outerMargin) const;
	// Set/Get some metrics in logical coordinates.
	// handleSize		size of handles on corners, edges and center. Default 4.
	// innerMargin		margin between loaded rectangle and Marker Rectangle. Default 4.
	// outerMargin		margin between Marker Rectangle and tracking rectangle. Default 2.
	// If pDC is not NULL, pDC is updated.

	COLORREF m_colorMark;		// color of Marking Rectangle. Default: light gray.
	COLORREF m_colorHandles;	// color of handles on corners and edges. Default: black.
	COLORREF m_colorCenter;		// color of center. Default: black.
	COLORREF m_colorTrack;		// color of Tracking Rectangle. Default: black.
	COLORREF m_colorPath;		// color of tracking path. Default: black.
	// Some colors. Change these if you like.

	enum CursorType
	{
		CursorNWSE,
		CursorNESW,
		CursorNS,
		CursorWE,
		CursorCenter,
		CursorMove,
		CursorCopy,
		CursorRotate,
		CursorShearHor,
		CursorShearVert
	};

	static bool LoadCursor(int type, UINT nResourceID, HINSTANCE hInst = NULL);
	// Load one of the cursors QTransformTracker will display.
	//
	// Parameters:
	// type			one of the values of CursorType;
	// nResourceID	resource identifier of the cursor;
	// hInst		the module in which the cursor resides. If NULL, the cursor is
	//				taken from the apllication's resource.
	//
	// Return value: true if succeeded.
	// QTransformTracker defaults to some MFC cursors.


protected:
	enum Mode
	{
		TransformNone,
		TransformMove,
		TransformScale,
		TransformRotate,
		TransformShear,
		TransformCenter,
	};

// Overrides from QTracker (see QTracker.h)
	virtual int OnBeginTrack(UINT nFlags, CPoint point);
	virtual int OnEndTrack(int trackResult);
	virtual int OnMouseMessage(UINT msg, UINT nFlags, CPoint point);
	virtual int OnKeyMessage(UINT msg, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnUpdate(CDC * pDC, UINT nMode);

// Overridable in derived classes
	virtual void SetIndicatorString(Mode mode, REAL x, REAL y = 0.0f);
	// Override this function to adapt the indicator string.

// Implementation
	void DrawMarkRect(CDC * pDC);
	void DrawCenter(CDC * pDC, POINT center, bool bTrack);
	CPoint GetHandlePoint(LPPOINT pPoints, int iHandle);
	CRect GetHandleRect(int iHandle);
	int HandleTest(CPoint point);
	BOOL SetCursor(int iHandle);
	void SetPoints(bool bSetCenter);
	void SetFixedPoints(bool bAlt);
	CPoint RestrictPoint(CPoint point, CPoint pntBase);

	bool m_bLoaded;
	bool m_bSpace;
	bool m_bMapFlip;
	bool m_bAlt;

	Mode m_Mode;
	int m_Handle;
	GraphicsPath *m_pGraphicsPath;
	CRect m_StartRect;

	POINT m_PointSrc[5];
	POINT m_PointDest[5];

	CPoint m_FixedSrc;
	CPoint m_FixedDest;

	Matrix m_Transform;
	REAL m_StartPhi;

	UINT m_HandleSize;
	UINT m_InnerMargin;
	UINT m_OuterMargin;

	CString m_IndicatorString;

	LPPOINT m_pPathPoints;
	LPBYTE m_pPathTypes;

	static HCURSOR g_hCursor[10];
};
