/**************************************************************************
	DrawHandle.h

	copyright (c) 2013/07/20 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once

class CHDrawHandle
{
public:
	CHDrawHandle();

	void DrawPreview(Graphics & g, const Rect& rc) const;
	void DrawPreview(Graphics & g, const CArray<Point>& points, bool bClose = false) const;
	void DrawHandles(Graphics & g, const Rect* rcs, INT count) const;
	void DrawHandles(Graphics & g, int Hints = 0);
	bool GetHandle( Rect & rect, int Handle);
	int HitTestHandle( const CPoint& point, int Hint = 0 );
	HCURSOR GetCursor( const CPoint& point, int Hint = 0 );

	UINT m_nHint;
	Pen m_Pen1;
	SolidBrush m_Brush1;
	Pen m_PreviewPen;
	Pen m_PreviewPen2;
	Rect m_Bounds;

	static const UINT Hint_Selected = 0x1;
	static const UINT Hint_Preview = 0x2;
};