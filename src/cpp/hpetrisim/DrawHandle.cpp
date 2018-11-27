/**************************************************************************
	DrawHandle.cpp

	copyright (c) 2013/07/20 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#include "stdafx.h"
#include "GDI+Helper.h"
#include "DrawHandle.h"

CHDrawHandle::CHDrawHandle() : m_nHint(0), m_Pen1(Color::Blue)
, m_Brush1(Color(50,0,0,0))
//, m_PreviewPen(&HatchBrush(HatchStyle50Percent, Color::LightSkyBlue, Color::DarkBlue), 2.0)
, m_PreviewPen(Color::Violet, 2.0)
, m_PreviewPen2(Color::Violet, 2.0)
{
	m_PreviewPen.SetDashStyle(DashStyleDot);
}

void CHDrawHandle::DrawPreview( Graphics & g, const Rect& rc ) const
{
	if (rc.Width < 40 || rc.Height < 40)
	{
		Rect rect2(rc);
		rect2.Inflate(-1,-1);
		g.DrawRectangle(&m_PreviewPen2, rc);
	}

	Rect rect2(rc);
	rect2.Inflate(-1,-1);
	g.DrawRectangle(&m_PreviewPen, rc);
}

void CHDrawHandle::DrawPreview( Graphics & g, const CArray<Point>& points, bool bClose /*= false*/ ) const
{
	g.DrawLines(&m_PreviewPen, points.GetData(), points.GetCount());
}
void CHDrawHandle::DrawHandles( Graphics & g, const Rect* rcs, INT count ) const
{
	g.FillRectangles(&m_Brush1, rcs, count);
	g.DrawRectangles(&m_Pen1, rcs, count);
}

void CHDrawHandle::DrawHandles( Graphics & g, int Hints /*= 0*/ )
{
	Rect rcs[8];

	for (int loop = 0; loop<8; loop++)
	{
		GetHandle(rcs[loop], loop+1);
	}

	g.DrawRectangle(&m_Pen1, m_Bounds);

	g.FillRectangles(&m_Brush1, rcs, 8);
	g.DrawRectangles(&m_Pen1, rcs, 8);
}

bool CHDrawHandle::GetHandle( Rect & rect, int Handle)
{
	rect = Rect();

	// 1------2------3
	// 8-------------4
	// 7------6------5

	switch (Handle)
	{
	case 1: rect.X = m_Bounds.X; rect.Y = m_Bounds.Y; break;
	case 2: rect.X = m_Bounds.X+m_Bounds.Width/2; rect.Y = m_Bounds.Y; break;
	case 3: rect.X = m_Bounds.X+m_Bounds.Width; rect.Y = m_Bounds.Y; break;
	case 4: rect.X = m_Bounds.X+m_Bounds.Width; rect.Y = m_Bounds.Y+m_Bounds.Height/2; break;
	case 5: rect.X = m_Bounds.X+m_Bounds.Width; rect.Y = m_Bounds.Y+m_Bounds.Height; break;
	case 6: rect.X = m_Bounds.X+m_Bounds.Width/2; rect.Y = m_Bounds.Y+m_Bounds.Height; break;
	case 7: rect.X = m_Bounds.X; rect.Y = m_Bounds.Y+m_Bounds.Height; break;
	case 8: rect.X = m_Bounds.X; rect.Y = m_Bounds.Y+m_Bounds.Height/2; break;
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

int CHDrawHandle::HitTestHandle( const CPoint& point, int Hint /*= 0*/ )
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

HCURSOR CHDrawHandle::GetCursor( const CPoint& point, int Hint /*= 0*/ )
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