/**************************************************************************
	Tracker.h

	copyright (c) 2013/07/21 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once

class CPetriSimView;
class CTracker : public QTracker
{
public:
	CTracker(CPetriSimView* pView);
	virtual ~CTracker(void);

	enum Mode
	{
		TrackSelect,
		TrackResize,
		TrackMove,
		TransformMove,
		TransformScale
	};

	int Track(UINT nFlags, CPoint point, bool OnGrid = false);
	HCURSOR GetCursor(const CPoint& point, int Hint = 0);
	void GetTracker(CRect & rect);
	void Draw(Graphics& g);
	void Load(const Rect & rect);
	void Clear();
	bool IsLoaded();
	bool HandleTest( const CPoint& point, int Hint = 0 );

protected:
	virtual int OnMouseMessage(UINT msg, UINT nFlags, CPoint point);
	virtual void OnUpdate(CDC * pDC, UINT nMode);
	bool GetHandle( Rect & rect, int Handle);
	int HitTestHandle( const CPoint& point, int Hint = 0 );
	CPoint GetHandlePoint(int Handle, bool bOpposite);

	bool m_bLoaded;
	Mode m_Mode;
	Rect m_StartTracker;
	Rect m_NowTracker;
	int m_Handle;
	Matrix m_Transform;
	CPoint m_FixedSrc;
	CPoint m_lastpoint;
	CRect m_UpdateRect;
	CPetriSimView* m_pView;
};
