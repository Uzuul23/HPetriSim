/**************************************************************************
	HRect.cpp
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#include "StdAfx.h"
#include "Resource.h"
#include "IniSection.h"
#include "ArchiveHelper.h"
#include "GDI+Helper.h"
#include "DrawHandle.h"
#include "HPropertyValue.h"
#include "HRect.h"

IMPLEMENT_SERIAL(CHRect, CObject, VERSIONABLE_SCHEMA|2)

CHRect::CHRect()
{
}

CHRect::CHRect(const CPoint& topleft, int type) 
: m_FillColor(Color::LightGray)
, m_FillColor2(Color::DarkGray)
, m_FillStyle(LinearGradient_1)
, m_LineColor(Color::Gray)
, m_RectStyle(type)
, m_LineWeight(2.0)
, m_DashStyle(DashStyleSolid)
, m_NoBorder(false)
{
	m_Bounds = Rect(ToPoint(topleft), Size());
}

CHRect::~CHRect()
{
}

bool CHRect::GetProperties( CArray<CHPropertyValue>& list )
{
	ASSERT_VALID(this);

	bool bFind = false;

	for (INT_PTR loop = 0; loop < list.GetCount(); loop++)
	{
		CHPropertyValue& prop = list[loop];

		switch (prop.Id)
		{
		case ID_OBJECT_X: prop = static_cast<long>(m_Bounds.X); bFind = true; break;
		case ID_OBJECT_Y: prop = static_cast<long>(m_Bounds.Y); bFind = true; break;
		case ID_OBJECT_CX: prop = static_cast<long>(m_Bounds.Width); bFind = true; break;
		case ID_OBJECT_CY: prop = static_cast<long>(m_Bounds.Height); bFind = true; break;
		case ID_OBJECT_LINECOLOR: prop = m_LineColor; bFind = true; break;
		case ID_OBJECT_LINESTYLE: prop = m_DashStyle; bFind = true; break;
		case ID_OBJECT_NOBORDER: prop = m_NoBorder; bFind = true; break;
		case ID_OBJECT_FILLCOLOR: prop = m_FillColor; bFind = true; break;
		case ID_OBJECT_FILLCOLOR2: prop = m_FillColor2; bFind = true; break;
		case ID_OBJECT_LINEWEIGHT: prop = m_LineWeight; bFind = true; break;
		case ID_OBJECT_FILLSTYLE: prop = m_FillStyle; bFind = true; break;
		}
	}

	return bFind;
}

bool CHRect::SetProperties( CArray<CHPropertyValue>& list, bool bPreview )
{
	ASSERT_VALID(this);

	bool bFind = false;

	for (INT_PTR loop = 0; loop < list.GetCount(); loop++)
	{
		CHPropertyValue& prop = list[loop];

		switch (prop.Id)
		{
		case ID_OBJECT_X: m_Bounds.X = static_cast<INT>(prop.GetLong(0, 32000)); bFind = true; break;
		case ID_OBJECT_Y: m_Bounds.Y = static_cast<INT>(prop.GetLong(0, 32000)); bFind = true; break;
		case ID_OBJECT_CX: m_Bounds.Width = static_cast<INT>(prop.GetLong(1, 32000)); bFind = true; break;
		case ID_OBJECT_CY: m_Bounds.Height = static_cast<INT>(prop.GetLong(1, 32000)); bFind = true; break;
		case ID_OBJECT_LINECOLOR: m_LineColor = prop; bFind = true; m_NoBorder = false; break;
		case ID_OBJECT_LINESTYLE: m_DashStyle = prop; bFind = true; m_NoBorder = false; break;
		case ID_OBJECT_NOBORDER: m_NoBorder = prop; bFind = true; break;
		case ID_OBJECT_FILLCOLOR: m_FillColor = prop; bFind = true; 
			m_FillStyle = (m_FillStyle == NoFill) ? FillStyleSolid : m_FillStyle; break;
		case ID_OBJECT_FILLCOLOR2: m_FillColor2 = prop; bFind = true; 
			m_FillStyle = (m_FillStyle == NoFill) ? LinearGradient_1 : m_FillStyle; break;
		case ID_OBJECT_LINEWEIGHT: m_LineWeight = prop; m_NoBorder = false; bFind = true; break;
		case ID_OBJECT_FILLSTYLE: m_FillStyle = prop; bFind = true; break;
		}
	}

	return bFind;
}

void CHRect::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << m_Bounds;
		ar << m_FillColor;
		ar << m_FillColor2;
		ar << m_LineColor;
		ar << m_LineWeight;
		ar << m_DashStyle;
		ar << m_FillStyle;
		ar << m_RectStyle;
		ar << m_NoBorder;
	}
	else
	{
		if(ar.GetObjectSchema() == 2)
		{
			ar >> m_Bounds;
			ar >> m_FillColor;
			ar >> m_FillColor2;
			ar >> m_LineColor;
			ar >> m_LineWeight;
			ar >> m_DashStyle;
			ar >> m_FillStyle;
			ar >> m_RectStyle;
			ar >> m_NoBorder;
		}
		else
		{
			CRect rect;
			DWORD dw;
			WORD w;
			BYTE b;
			ar >> dw; rect.left = dw;
			ar >> dw; rect.top = dw;
			ar >> dw; rect.right = dw;
			ar >> dw; rect.bottom = dw;
			ar >> dw; m_FillColor.SetFromCOLORREF(dw);
			ar >> dw; m_LineColor.SetFromCOLORREF(dw);
			ar >> w; m_DashStyle = ConvertPenStyle(w);
			ar >> w;
			REAL l =  static_cast<REAL>(w);
			if (l == 0.0)
			{
				m_NoBorder = true;
				m_LineWeight = 1.0;
			}
			else
			{
				m_NoBorder = false;
				m_LineWeight = l;
			}
			ar >> w; m_RectStyle = w;
			ar >> w; //m_cpRound.x = w;
			ar >> w; //m_cpRound.y = w;
			ar >> b; m_FillStyle = (b == 0)? FillStyleSolid : NoFill;

			rect.NormalizeRect();

			m_Bounds = Rect(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top);
		}
	}	
}

#ifdef _DEBUG
void CHRect::AssertValid() const
{
	CObject::AssertValid();
}

void CHRect::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}
#endif //_DEBUG

void CHRect::DrawObject(Graphics & g, const CHDrawInfo& Info)
{
	ASSERT_VALID(this);

	if (m_FillStyle != NoFill)
	{
		Brush* pbrush = NewBrush(m_FillColor, m_FillColor2, m_Bounds, m_FillStyle);

		switch(m_RectStyle)
		{
		case Rectangle: g.FillRectangle(pbrush, m_Bounds); break;
		case Rounded: g.FillRectangle(pbrush, m_Bounds); break;
		case Ellipse: g.FillEllipse(pbrush, m_Bounds); break;
		}

		delete pbrush;
	}

	if (!m_NoBorder)
	{
		Pen pen(m_LineColor, m_LineWeight);
		pen.SetDashStyle(m_DashStyle);

		switch(m_RectStyle)
		{
		case Rectangle: g.DrawRectangle(&pen, m_Bounds); break;
		case Rounded: g.DrawRectangle(&pen, m_Bounds); break;
		case Ellipse: g.DrawEllipse(&pen, m_Bounds); break;
		}
	}
}

void CHRect::DrawHandles( Graphics & g, const CHDrawHandle& Info )
{
	if (Info.m_nHint & CHDrawHandle::Hint_Preview)
	{
		Info.DrawPreview(g, m_Bounds);
		return;
	}

	Rect rect;
	Rect rcs[9];

	for(int handle = 1; GetHandle(rect, handle); handle++)
	{
		rcs[handle] = rect;
	}

	Info.DrawHandles(g, rcs, 9);

}

Rect CHRect::GetBounds( bool bRedraw /*= false*/ )
{
	if (bRedraw)
	{
		Rect rect(m_Bounds);
		rect.Inflate(max(static_cast<int>(m_LineWeight), SL_GRIPPER), max(static_cast<int>(m_LineWeight), SL_GRIPPER));
		return rect;
	}

	return m_Bounds;
}

void CHRect::SetBounds( const Rect& rect )
{
	m_Bounds = rect;
}

bool CHRect::HitTest( const CRect& rect, int Hint /*= 0*/)
{
	return (FromRect(m_Bounds) & rect).IsRectEmpty() ? false : true;
}

bool CHRect::HitTest( const CPoint& point, int Hint /*= 0*/ )
{
	Rect bounds(m_Bounds);
	bounds.Inflate(static_cast<INT>(m_LineWeight), static_cast<INT>(m_LineWeight));

	if (!bounds.Contains(ToPoint(point)))
	{
		return false;
	}

	GraphicsPath path;

	switch (m_RectStyle)
	{
	case Rounded:
	case Rectangle: path.AddRectangle(m_Bounds); break;
	case Ellipse: path.AddEllipse(m_Bounds); break;
	}

	if (m_FillStyle != NoFill)
	{
		Region region(&path);
		return region.IsVisible(ToPoint(point)) ? true: false;
	}

	path.Widen(&Pen(Color::Black, max(2.0f, m_LineWeight)));

	return path.IsVisible(ToPoint(point)) ? true: false;
}

void CHRect::Resize(const CPoint& to, int handle)
{
	ASSERT_VALID(this);

	CRect rect(FromRect(m_Bounds));
	
	switch(handle)
	{
	case 1: rect.left = to.x; rect.top = to.y; break;
	case 2: rect.top = to.y; break;
	case 3: rect.right = to.x; rect.top = to.y; break;
	case 4: rect.right = to.x; break;
	case 0:
	case 5: rect.right = to.x; rect.bottom = to.y; break;
	case 6: rect.bottom = to.y; break;
	case 7: rect.left = to.x; rect.bottom = to.y; break;
	case 8: rect.left = to.x; break;
	}

	m_Bounds = ToRect(rect);
}

bool CHRect::GetHandle( Rect & rect, int Handle )
{
	ASSERT_VALID(this);

	Rect rect2(m_Bounds);

	rect = Rect();

	// 1------2------3
	// 8-------------4
	// 7------6------5

	switch (Handle)
	{
	case 1: rect.X = rect2.X; rect.Y = rect2.Y; break;
	case 2: rect.X = rect2.X+rect2.Width/2; rect.Y = rect2.Y; break;
	case 3: rect.X = rect2.X+rect2.Width; rect.Y = rect2.Y; break;
	case 4: rect.X = rect2.X+rect2.Width; rect.Y = rect2.Y+rect2.Height/2; break;
	case 5: rect.X = rect2.X+rect2.Width; rect.Y = rect2.Y+rect2.Height; break;
	case 6: rect.X = rect2.X+rect2.Width/2; rect.Y = rect2.Y+rect2.Height; break;
	case 7: rect.X = rect2.X; rect.Y = rect2.Y+rect2.Height; break;
	case 8: rect.X = rect2.X; rect.Y = rect2.Y+rect2.Height/2; break;
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

HCURSOR CHRect::GetCursor( const CPoint& point, int Hint /*= 0*/ )
{
	ASSERT_VALID(this);

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

void CHRect::ToGrid(int grid,int handle)
{
	ASSERT_VALID(this);

	/*CPoint point;
	int r;
	switch(handle)
	{
	case NULL:
		point = m_crRect.TopLeft();
		point.x = ((r = point.x % grid) >= grid / 2) ? grid - r : -r;
		point.y = ((r = point.y % grid) >= grid / 2) ? grid - r : -r;
		m_crRect.left += point.x;
		m_crRect.top += point.y;
		point = m_crRect.BottomRight();
		point.x = ((r = point.x % grid) >= grid / 2) ? grid - r : -r;
		point.y = ((r = point.y % grid) >= grid / 2) ? grid - r : -r;
		m_crRect.right += point.x;
		m_crRect.bottom += point.y;
		break;
	case 1:
		point = m_crRect.TopLeft();
		point.x = ((r = point.x % grid) >= grid / 2) ? grid - r : -r;
		point.y = ((r = point.y % grid) >= grid / 2) ? grid - r : -r;
		m_crRect.left += point.x;
		m_crRect.top += point.y;
		break;
	case 2:
		point.y = m_crRect.top;
		point.y = ((r = point.y % grid) >= grid / 2) ? grid - r : -r;
		m_crRect.top += point.y;
		break;
	case 3:
		point.x = m_crRect.right;
		point.y = m_crRect.top;
		point.x = ((r = point.x % grid) >= grid / 2) ? grid - r : -r;
		point.y = ((r = point.y % grid) >= grid / 2) ? grid - r : -r;
		m_crRect.right += point.x;
		m_crRect.top += point.y;
		break;
	case 4:
		point.x = m_crRect.right;
		point.x = ((r = point.x % grid) >= grid / 2) ? grid - r : -r;
		m_crRect.right += point.x;
		break;
	case 5:
		point = m_crRect.BottomRight();
		point.x = ((r = point.x % grid) >= grid / 2) ? grid - r : -r;
		point.y = ((r = point.y % grid) >= grid / 2) ? grid - r : -r;
		m_crRect.right += point.x;
		m_crRect.bottom += point.y;
		break;
	case 6:
		point.y = m_crRect.bottom;
		point.y = ((r = point.y % grid) >= grid / 2) ? grid - r : -r;
		m_crRect.bottom += point.y;
		break;
	case 7:
		point.x = m_crRect.left;
		point.y = m_crRect.bottom;
		point.x = ((r = point.y % grid) >= grid / 2) ? grid - r : -r;
		point.y = ((r = point.y % grid) >= grid / 2) ? grid - r : -r;
		m_crRect.left += point.x;
		m_crRect.bottom += point.y;
		break;
	case 8:
		point.x = m_crRect.left;
		point.x = ((r = point.x % grid) >= grid / 2) ? grid - r : -r;
		m_crRect.left += point.x;
		break;
	case 9:
		point = m_cpRound;
		point.x = ((r = point.x % grid) >= grid / 2) ? grid - r : -r;
		point.y = ((r = point.y % grid) >= grid / 2) ? grid - r : -r;
		m_cpRound += point;
		break;
	default:
		ASSERT(false);
		break;
	}*/
}

int CHRect::HitTestHandle( const CPoint& point, int Hint /*= 0*/ )
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

void CHRect::StorePoints( CArray<Point>& fixpoints )
{
	ASSERT_VALID(this);

	fixpoints.SetSize(2);
	fixpoints[0] = Point(m_Bounds.X, m_Bounds.Y);
	fixpoints[1] = Point(m_Bounds.X+m_Bounds.Width, m_Bounds.Y+m_Bounds.Height);
}

void CHRect::TransformPoints( const CArray<Point>& fixpoints, const Gdiplus::Matrix& matrix )
{
	ASSERT_VALID(this);

	Point points[2] = {fixpoints[0], fixpoints[1]};
	matrix.TransformPoints(points, 2);

	m_Bounds = Rect(points[0].X, points[0].Y, points[1].X - points[0].X,  points[1].Y - points[0].Y);
}

void CHRect::SetMinSize()
{
	m_Bounds = Rect(m_Bounds.X, m_Bounds.Y, 100, 100);
}

