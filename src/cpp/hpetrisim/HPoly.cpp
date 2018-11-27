/**************************************************************************
	HPolyLine.cpp
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#include "StdAfx.h"
#include "Resource.h"
#include "IniSection.h"
#include "ArchiveHelper.h"
#include "DrawHandle.h"
#include "HPropertyValue.h"
#include "GDI+Helper.h"
#include "HPoly.h"

IMPLEMENT_SERIAL(CHPoly, CHRect, VERSIONABLE_SCHEMA|2)

CHPoly::CHPoly()
{

}

CHPoly::CHPoly(const CPoint& start, bool Polygon/* = true*/) : CHRect(start)
, m_CloseFigure(Polygon)
{
	m_Points.Add(ToPoint(start));
	m_Points.Add(ToPoint(start));

	if (!Polygon)
	{
		m_FillStyle = NoFill;
	}
}

CHPoly::~CHPoly()
{

}

bool CHPoly::GetProperties( CArray<CHPropertyValue>& list )
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
		case ID_OBJECT_FILLCOLOR: prop = m_FillColor; bFind = (m_Points.GetCount() > 2) ? true : false; break;
		case ID_OBJECT_FILLCOLOR2: prop = m_FillColor2; bFind = (m_Points.GetCount() > 2) ? true : false; break;
		case ID_OBJECT_LINEWEIGHT: prop = m_LineWeight; bFind = true; break;
		case ID_OBJECT_FILLSTYLE: prop = m_FillStyle; bFind = (m_Points.GetCount() > 2) ? true : false; break;
		case ID_OBJECT_CLOSEFIGURE: prop = m_CloseFigure; bFind = (m_Points.GetCount() > 2) ? true: false; break;
		}
	}

	return bFind;
}

bool CHPoly::SetProperties( CArray<CHPropertyValue>& list, bool bPreview )
{
	ASSERT_VALID(this);

	bool bFind = false;

	for (INT_PTR loop = 0; loop < list.GetCount(); loop++)
	{
		CHPropertyValue& prop = list[loop];

		switch (prop.Id)
		{
		//case ID_OBJECT_X: m_Bounds.X = static_cast<INT>(prop.GetLong(0, 32000)); bFind = true; break;
		//case ID_OBJECT_Y: m_Bounds.Y = static_cast<INT>(prop.GetLong(0, 32000)); bFind = true; break;
		//case ID_OBJECT_CX: m_Bounds.Width = static_cast<INT>(prop.GetLong(1, 32000)); bFind = true; break;
		//case ID_OBJECT_CY: m_Bounds.Height = static_cast<INT>(prop.GetLong(1, 32000)); bFind = true; break;
		case ID_OBJECT_LINECOLOR: m_LineColor = prop; bFind = true; m_NoBorder = false; break;
		case ID_OBJECT_LINESTYLE: m_DashStyle = prop; bFind = true; m_NoBorder = false; break;
		case ID_OBJECT_NOBORDER: m_NoBorder = prop; bFind = true; break;
		case ID_OBJECT_FILLCOLOR: m_FillColor = prop; bFind = true; m_CloseFigure = true; 
			m_FillStyle = (m_FillStyle == NoFill) ? FillStyleSolid : m_FillStyle; break;
		case ID_OBJECT_FILLCOLOR2: m_FillColor2 = prop; bFind = true; m_CloseFigure = true;
			m_FillStyle = (m_FillStyle == NoFill) ? LinearGradient_1 : m_FillStyle; break;
		case ID_OBJECT_LINEWEIGHT: m_LineWeight = prop; bFind = true; m_NoBorder = false; break;
		case ID_OBJECT_FILLSTYLE: m_FillStyle = prop; bFind = true; m_CloseFigure = true; break;
		case ID_OBJECT_CLOSEFIGURE: m_CloseFigure = prop; bFind = true; break;
		}
	}

	return bFind;
}

void CHPoly::DrawObject( Graphics & g, const CHDrawInfo& Info )
{
	ASSERT_VALID(this);

	if (m_CloseFigure && m_FillStyle != NoFill && m_Points.GetCount() > 2)
	{
		Brush* pbrush = NewBrush(m_FillColor, m_FillColor2, GetBounds(), m_FillStyle);

		g.FillPolygon(pbrush, m_Points.GetData(), m_Points.GetCount());

		delete pbrush;
	}

	Pen pen(m_LineColor, m_LineWeight);
	pen.SetDashStyle(m_DashStyle);

	if (m_CloseFigure)
	{
		g.DrawPolygon(&pen, m_Points.GetData(), m_Points.GetCount());
	}
	else
	{
		g.DrawLines(&pen, m_Points.GetData(), m_Points.GetCount());
	}

}

void CHPoly::DrawHandles( Graphics & g, const CHDrawHandle& Info )
{
	if (Info.m_nHint & CHDrawHandle::Hint_Preview)
	{
		Info.DrawPreview(g, m_Points, false);
		return;
	}

	Rect rect;
	CArray<Rect> handles;

	for(int handle = 1; GetHandle(rect, handle); handle++)
	{
		handles.Add(rect);
	}

	Info.DrawHandles(g, handles.GetData(), handles.GetCount());
}

void CHPoly::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		CHRect::Serialize(ar);

		ar << m_Points;
		ar << m_CloseFigure;
	}
	else
	{
		UINT nSchema = ar.GetObjectSchema();

		if(nSchema == 2)
		{
			ar.SetObjectSchema(nSchema);
			CHRect::Serialize(ar);

			ar >> m_Points;
			ar >> m_CloseFigure;
		}
		else
		{
			BYTE b;
			WORD w;
			DWORD dw;
			ar >> dw;// m_crRect.left = dw;
			ar >> dw;// m_crRect.top = dw;
			ar >> dw;// m_crRect.right = dw;
			ar >> dw;// m_crRect.bottom = dw;
			ar >> dw; m_LineColor.SetFromCOLORREF(dw);
			ar >> dw; m_FillColor.SetFromCOLORREF(dw);
			ar >> w; m_CloseFigure = (w == 1/*PM_POLYGON*/) ? true : false;
			ar >> w; m_DashStyle = ConvertPenStyle(w);
			ar >> w; m_LineWeight = static_cast<REAL>(w);
			ar >> b; m_FillStyle = (b != 0)? NoFill : FillStyleSolid;

			CArray<CPoint,CPoint> OldPoints;
			OldPoints.Serialize(ar);

			m_Points.RemoveAll();

			for (INT_PTR loop=0; loop<OldPoints.GetSize(); loop++)
			{
				m_Points.Add(ToPoint(OldPoints[loop]));
			}
		}
	}
}

#ifdef _DEBUG
void CHPoly::AssertValid() const
{
	ASSERT(m_Points.GetCount() > 1);
	CObject::AssertValid();
}

void CHPoly::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}

#endif //_DEBUG

void CHPoly::Resize( const CPoint& to, int handle )
{
	ASSERT_VALID(this);

	if (handle < 0 || handle/2 > m_Points.GetSize()-1)
	{
		return;
	}

	if (handle == 0)
	{
		m_Points[1] = ToPoint(to);
	}
	else if (handle % 2)
	{
		m_Points[handle/2] = ToPoint(to);
	}

	RecalcBounds();
}

Rect CHPoly::GetBounds( bool bRedraw /*= false*/ )
{
	Rect rect(m_Bounds);

	if (bRedraw)
	{
		rect.Inflate(max(static_cast<INT>(m_LineWeight), SL_GRIPPER), max(static_cast<INT>(m_LineWeight), SL_GRIPPER));
	}

	return rect;
}


void CHPoly::SetBounds( const Rect& rect )
{
	if (rect.IsEmptyArea())
	{
		return;
	}

	Matrix matrix;

	REAL tx = static_cast<REAL>(rect.X - m_Bounds.X);
	REAL ty = static_cast<REAL>(rect.Y - m_Bounds.Y);
	REAL sx = static_cast<REAL>(rect.X / m_Bounds.X);
	REAL sy = static_cast<REAL>(rect.Y / m_Bounds.Y);

	matrix.Translate(tx, ty);
	matrix.Scale(sx, sy, MatrixOrderAppend);

	matrix.TransformPoints(m_Points.GetData(), m_Points.GetCount());
	RecalcBounds();
}


bool CHPoly::HitTest( const CRect& rect, int Hint /*= 0*/ )
{
	CRect bounds(FromRect(m_Bounds));
	bounds.InflateRect(1,1);
	return (bounds & rect).IsRectEmpty() ? false : true;
}

bool CHPoly::HitTest( const CPoint& point, int Hint /*= 0*/ )
{
	Rect bounds(m_Bounds);
	bounds.Inflate(static_cast<INT>(m_LineWeight), static_cast<INT>(m_LineWeight));

	if (!bounds.Contains(ToPoint(point)))
	{
		return false;
	}

	GraphicsPath path;

	if (m_CloseFigure && m_Points.GetCount() > 2)
	{
		path.AddPolygon(m_Points.GetData(), m_Points.GetSize());
	} 
	else
	{
		path.AddLines(m_Points.GetData(), m_Points.GetSize());
	}

	if (m_CloseFigure && m_FillStyle != NoFill && m_Points.GetCount() > 2)
	{
		Region region(&path);
		return region.IsVisible(ToPoint(point)) ? true: false;

	}

	path.Widen(&Pen(Color::Black, max(2.0f, m_LineWeight)));

	return path.IsVisible(ToPoint(point)) ? true: false;

}

int CHPoly::HitTestHandle( const CPoint& point, int Hint /*= 0*/ )
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

HCURSOR CHPoly::GetCursor( const CPoint& point, int Hint /*= 0*/ )
{
	ASSERT_VALID(this);

	int handle = HitTestHandle(point, Hint);

	if (handle > 0)
	{
		if (handle%2 == 0)
		{
			return AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		}

		return AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);
	}

	return 0;
}

bool CHPoly::GetHandle( Rect & rect, int Handle )
{
	ASSERT_VALID(this);

	if (Handle < 1 || Handle > (m_Points.GetSize() * 2 - ((m_CloseFigure) ? 0 : 1)))
	{
		rect = Rect();
		return false;
	}

	int n = Handle % 2;

	if (n)
	{
		rect = Rect(m_Points[Handle/2], Size());
		rect.Inflate(4, 4);
	}
	else
	{
		if (Handle/2 == m_Points.GetSize())
		{
			rect.Width = rect.Height = 0;
			INT a = max(m_Points[0].X, m_Points[Handle/2-1].X);
			INT b = min(m_Points[0].X, m_Points[Handle/2-1].X);
			rect.X =  (a - b) / 2 + b;

			a = max(m_Points[0].Y, m_Points[Handle/2-1].Y);
			b = min(m_Points[0].Y, m_Points[Handle/2-1].Y);
			rect.Y =  (a - b) / 2 + b;
		} 
		else
		{
			rect.Width = rect.Height = 0;
			INT a = max(m_Points[Handle/2-1].X, m_Points[Handle/2].X);
			INT b = min(m_Points[Handle/2-1].X, m_Points[Handle/2].X);
			rect.X =  (a - b) / 2 + b;

			a = max(m_Points[Handle/2-1].Y, m_Points[Handle/2].Y);
			b = min(m_Points[Handle/2-1].Y, m_Points[Handle/2].Y);
			rect.Y =  (a - b) / 2 + b;
		}
		
		rect.Inflate(3, 3);
	}
	return true;
}

void CHPoly::StorePoints( CArray<Point>& fixpoints )
{
	ASSERT_VALID(this);

	fixpoints.Copy(m_Points);
}

void CHPoly::TransformPoints( const CArray<Point>& fixpoints, const Gdiplus::Matrix& matrix )
{
	ASSERT_VALID(this);

	m_Points.Copy(fixpoints);
	matrix.TransformPoints(m_Points.GetData(), m_Points.GetCount());
	RecalcBounds();
}

void CHPoly::SetMinSize()
{
	m_Points.SetSize(2);
	m_Points[1] = m_Points[0];
	m_Points[1].X += 100;
	RecalcBounds();
}

bool CHPoly::AddPoint( int& handle, const CPoint& point )
{
	if (handle > 0 && (handle % 2) == 0)
	{
		m_Points.InsertAt(handle/2, ToPoint(point));
		RecalcBounds();
		handle++;
		return true;
	}

	return false;
}

bool CHPoly::RemovePoint( int handle )
{
	if (handle > 0 && handle % 2 && handle/2 < m_Points.GetCount() && m_Points.GetCount() > 2)
	{
		m_Points.RemoveAt(handle/2);
		RecalcBounds();
		return true;
	}

	return false;
}

void CHPoly::RecalcBounds()
{
	if (m_Points.GetCount() == 0)
	{
		return;
	}

	INT top, left, button, right;
	
	top = left = m_Points[0].X;
	button = right = m_Points[0].Y;

	for (INT_PTR loop=0;loop<m_Points.GetCount();++loop)
	{
		left = min(left, m_Points[loop].X);
		right = max(right, m_Points[loop].X);
		top = min(top, m_Points[loop].Y);
		button = max(button, m_Points[loop].Y);
	}

	m_Bounds = Rect(left, top, right-left, button+top);
}