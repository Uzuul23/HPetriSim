/**************************************************************************
	HLabel.cpp
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#include "stdafx.h"
#include "Resource.h"
#include "HDrawObject.h"
#include "IniSection.h"
#include "ArchiveHelper.h"
#include "DrawHandle.h"
#include "PetriSimDoc.h"
#include "HDrawInfo.h"
#include "HLabel.h"

IMPLEMENT_SERIAL(CHLabel, CObject, VERSIONABLE_SCHEMA|2)

CHLabel::CHLabel()
{
}

CHLabel::CHLabel(CHLabelMember* pOwner, long labelnumber, bool visible /*= true*/ )
: m_Bounds(0, 0, 10, 10)
, m_LabelNumber(labelnumber)
, m_Visible(visible)
{
	Owner(pOwner);
}

CHLabel::~CHLabel()
{
}

#ifdef _DEBUG
void CHLabel::AssertValid() const
{
	CObject::AssertValid();
}

void CHLabel::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}

#endif //_DEBUG

void CHLabel::Owner( CHLabelMember* pOwner, bool cutting /*= false*/ )
{
	if (m_pOwner && pOwner == 0)
	{
		m_OwnerId = cutting ? 0 : m_pOwner->Id();
		m_pOwner->SetLabel(0, m_LabelNumber);
	}
	
	if (pOwner)
	{
		m_OwnerId = pOwner->Id();
		pOwner->SetLabel(this, m_LabelNumber);
	}

	m_pOwner = pOwner;
}

void CHLabel::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << m_Bounds;
		ar << m_OwnerId;
		ar << m_LabelNumber;
		ar << m_Visible;
	}
	else
	{
		if(ar.GetObjectSchema() == 2)
		{
			ar >> m_Bounds;
			ar >> m_OwnerId;
			ar >> m_LabelNumber;
			ar >> m_Visible;
		}
		else
		{
			DWORD dw;
			BYTE b;
			CRect rect;
			CString str;
			ar >> str;
			ar >> dw; rect.top = dw;
			ar >> dw; rect.left = dw;
			ar >> dw; rect.bottom = dw;
			ar >> dw; rect.right = dw;
			ar >> dw; m_OwnerId = static_cast<ULONG32>(dw);
			ar >> dw; m_LabelNumber = static_cast<long>(dw);
			ar >> b; m_Visible = (b == 0)? false : true;
			ar >> b;
			ar >> b;

			m_Bounds = Rect(rect.left, rect.top, rect.right-rect.left+4, rect.bottom-rect.top+1);

			if (m_LabelNumber == 0)
			{
				//for name transfer
				CPetriSimDoc* pDoc = static_cast<CPetriSimDoc*>(ar.m_pDocument);
				pDoc->AddToConvertOldLabelMap(m_OwnerId, str);
			}
		}

		m_pOwner = 0;
	}	
}

void CHLabel::DrawObject( Graphics & g, const CHDrawInfo& Info )
{
	CString text;
	if (m_Visible && m_pOwner && m_pOwner->GetLabelText(text, m_LabelNumber))
	{
		PointF point(static_cast<REAL>(m_Bounds.X), static_cast<REAL>(m_Bounds.Y));
		g.DrawString(text, text.GetLength(), &Info.LabelFont, point, &Info.LabelTextBrush);
	}
}

void CHLabel::DrawHandles( Graphics & g, const CHDrawHandle& Info )
{
	if (m_Visible)
	{
		Info.DrawPreview(g, m_Bounds);
	}
}

Rect CHLabel::GetBounds( bool bRedraw /*= false*/ )
{
	if (bRedraw)
	{
		Rect bounds(m_Bounds);
		bounds.Inflate(5,5);
		return bounds;
	}

	return m_Bounds;
}

void CHLabel::SetBounds( const Rect& rect )
{
	m_Bounds.X = rect.X;
	m_Bounds.Y = rect.Y;
}

bool CHLabel::HitTest( const CRect& rect, int Hint /*= 0*/ )
{
	return (!m_Visible || (FromRect(m_Bounds) & rect).IsRectEmpty()) ? false : true;
}

bool CHLabel::HitTest( const CPoint& point, int Hint /*= 0*/ )
{
	return (m_Visible && FromRect(m_Bounds).PtInRect(point)) ? true : false;
}

void CHLabel::StorePoints( CArray<Point>& fixpoints )
{
	ASSERT_VALID(this);

	fixpoints.SetSize(1);
	fixpoints[0] = Point(m_Bounds.X, m_Bounds.Y);

}

void CHLabel::TransformPoints( const CArray<Point>& fixpoints, const Gdiplus::Matrix& matrix )
{
	ASSERT_VALID(this);

	Point point(fixpoints[0]);
	matrix.TransformPoints(&point, 1);

	m_Bounds = Rect(point.X, point.Y, m_Bounds.Width, m_Bounds.Height);
}

void CHLabel::UpdateBounds()
{
	CString text;
	if (m_pOwner && m_pOwner->GetLabelText(text, m_LabelNumber) && text.GetLength() > 0)
	{
		CWnd* pWnd = AfxGetMainWnd();

		if (pWnd)
		{
			SizeF sizef(64000.0f, 64000.0f);
			SizeF textsize;
			CHDrawInfo Info;

			StringFormat stringformat;
			stringformat.SetAlignment(StringAlignmentNear);
			stringformat.SetLineAlignment(StringAlignmentNear);

			Graphics* pg2 = new Graphics(pWnd->GetSafeHwnd());

			pg2->MeasureString(text, text.GetLength(), &Info.LabelFont, sizef, &stringformat, &textsize);
			m_Bounds = Rect(m_Bounds.X, m_Bounds.Y, static_cast<INT>(textsize.Width+1.0), static_cast<INT>(textsize.Height+1.0));

			delete pg2;
		}

		return;
	}

	m_Bounds = Rect(m_Bounds.X, m_Bounds.Y, 0, 0);
}

void CHLabel::Position( const Point& point )
{
	m_Bounds = Rect(point.X, point.Y, m_Bounds.Width, m_Bounds.Height);
}

