/**************************************************************************
	GDI+Helper.h

	copyright (c) 2013/07/07 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once

namespace Gdiplus
{
	inline Gdiplus::RectF ToRectF(const Gdiplus::Rect & rect)
	{
		return Gdiplus::RectF(static_cast<REAL>(rect.X), static_cast<REAL>(rect.Y), static_cast<REAL>(rect.Width), static_cast<REAL>(rect.Height));
	}

	inline Gdiplus::Rect ToRect(const CRect & rc)
	{
		return Gdiplus::Rect(rc.left, rc.top, rc.Width(), rc.Height());
	}

	inline Gdiplus::Rect ToRect(const CPoint & pt1, const CPoint & pt2)
	{
		return Gdiplus::Rect(pt1.x, pt1.y, pt2.x - pt1.x, pt2.y - pt1.y);
	}

	inline void UnionRect(Rect& rc1, const Rect& rc2)
	{
		if (rc1.IsEmptyArea())
		{
			rc1 = rc2;
		}
		else
		{
			Rect::Union(rc1, rc1, rc2);
		}
	}

	inline Gdiplus::Point ToPoint(const CPoint& pt)
	{
		return Gdiplus::Point(pt.x, pt.y);
	}

	inline Gdiplus::Size ToSize(const CSize & cx)
	{
		return Gdiplus::Size(cx.cx, cx.cy);
	}

	inline CRect FromRect(const Gdiplus::Rect & rc)
	{
		return CRect(rc.X, rc.Y, rc.X + rc.Width, rc.Y + rc.Height);
	}

	inline void Transform(const Gdiplus::Matrix & matrix, Rect & rect)
	{
		Point points[2];
		points[0] = Point(rect.GetLeft(), rect.GetTop());
		points[1] = Point(rect.GetRight(), rect.GetBottom());

		matrix.TransformPoints(points, 2);

		rect = Rect(points[0], Size(points[1].X-points[0].X
			, points[1].Y-points[0].Y));
	}

	inline Gdiplus::DashStyle ConvertPenStyle(WORD PenStyle)
	{
		switch (PenStyle)
		{
		case PS_DASH: return DashStyleDash;
		case PS_DOT: return DashStyleDot;
		case PS_DASHDOT: return DashStyleDashDot;
		case PS_DASHDOTDOT: return DashStyleDashDotDot;
		default: return DashStyleSolid;
		}
	}
}