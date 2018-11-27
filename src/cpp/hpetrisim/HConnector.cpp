/**************************************************************************
	HConnector.cpp
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#include "StdAfx.h"
#include "Resource.h"
#include "IniSection.h"
#include "HDrawObject.h"
#include "HNetMember.h"
#include "HLabel.h"
#include "ArchiveHelper.h"
#include "HPropertyValue.h"
#include "DrawHandle.h"
#include "HDrawInfo.h"
#include "ArchiveHelper.h"
#include "HPosition.h"
#include "HConnector.h"

IMPLEMENT_SERIAL(CHConnector, CHDrawObject, VERSIONABLE_SCHEMA|2)

CHConnector::CHConnector()
{
}

CHConnector::CHConnector(const CPoint& start, ULONG32 NameHint)
: m_Id(0)
, m_FromId(0)
, m_ToId(0)
, m_pFrom(0)
, m_pTo(0)
, m_Type(TypeNormal)
, m_Weight(1)
{
	m_Name.Format(L"A%d", NameHint);
	m_Points.Add(ToPoint(start));
	m_Points.Add(ToPoint(start));
}

CHConnector::~CHConnector()
{

}

#ifdef _DEBUG
void CHConnector::AssertValid() const
{
	CObject::AssertValid();
}

void CHConnector::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}
#endif //_DEBUG

void CHConnector::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);
	if (ar.IsStoring())
	{
		ar << m_Id;
		ar << m_Points;	
		ar << m_ToId;
		ar << m_FromId;
		ar << static_cast<LONG32>(m_Type);
		ar << m_Weight;
	}
	else
	{
		if(ar.GetObjectSchema() == 2)
		{
			LONG32 l = 0;

			ar >> m_Id;
			ar >> m_Points;
			ar >> m_ToId;
			ar >> m_FromId;
			ar >> l; m_Type = static_cast<ConnectorType>(l);
			ar >> m_Weight;
		}
		else
		{
			BYTE b;
			WORD w;
			DWORD dw;
			double db;
			ar >> dw;//m_crRect.left = dw;
			ar >> dw;//m_crRect.top = dw;
			ar >> dw;//m_crRect.right = dw;
			ar >> dw;//m_crRect.bottom = dw;
			ar >> dw;//m_cpArrow[1].x = dw;
			ar >> dw;//m_cpArrow[1].y = dw;
			ar >> dw;//m_cpArrow[2].x = dw;
			ar >> dw;//m_cpArrow[2].y = dw;
			ar >> dw;m_Id = static_cast<ULONG32>(dw);
			ar >> dw;m_FromId = static_cast<ULONG32>(dw);
			ar >> dw;m_ToId = static_cast<ULONG32>(dw);
			ar >> dw;m_Weight = static_cast<ULONG32>(dw);
			ar >> dw;
			ar >> w;
			ar >> w;m_Type = static_cast<ConnectorType>(w);
			ar >> w;
			ar >> w;
			ar >> w;
			ar >> b;
			ar >> b;
			ar >> b;
			ar >> b;
			ar >> b;
			ar >> db;

			CArray<CPoint, CPoint> points;
			points.Serialize(ar);

			m_Points.SetSize(points.GetSize());

			for (INT_PTR loop=0; loop<m_Points.GetCount(); loop++)
			{
				m_Points[loop] = ToPoint(points[loop]);
			}
		}

		m_pFrom = 0;
		m_pTo = 0;
	}
}

void CHConnector::DrawObject( Graphics & g, const CHDrawInfo& Info )
{
	INT_PTR count = m_Points.GetCount();

	if (count < 2)
	{
		return;
	}

	Size sz(abs(m_Points[count-2].X - m_Points[count-1].X) , abs(m_Points[count-2].Y - m_Points[count-1].Y));

	if (sz.Height < 10 && sz.Width < 10)
	{
		switch (m_Type)
		{
		case TypeInhibitor:  g.DrawLines(&Info.LinePen, m_Points.GetData(), m_Points.GetCount()); break;
		case TypeTest:  g.DrawLines(&Info.DashLinePen, m_Points.GetData(), m_Points.GetCount()); break;
		default: g.DrawLines(&Info.LinePen, m_Points.GetData(), m_Points.GetCount()); break;
		}
	}
	else
	{
		switch (m_Type)
		{
		case TypeInhibitor:  g.DrawLines(&Info.LineCapEllipsePen, m_Points.GetData(), m_Points.GetCount()); break;
		case TypeTest:  g.DrawLines(&Info.DashLineCapArrowPen, m_Points.GetData(), m_Points.GetCount()); break;
		default: g.DrawLines(&Info.LineCapArrowPen, m_Points.GetData(), m_Points.GetCount()); break;
		}
	}

	if (!m_pTo || !m_pFrom)
	{
		Rect rect;

		INT a = max(m_Points[count-2].X, m_Points[count-1].X);
		INT b = min(m_Points[count-2].X, m_Points[count-1].X);
		rect.X =  (a - b) / 2 + b;

		a = max(m_Points[count-2].Y, m_Points[count-1].Y);
		b = min(m_Points[count-2].Y, m_Points[count-1].Y);
		rect.Y =  (a - b) / 2 + b;

		rect.Inflate(8, 8);

		g.DrawImage(Info.DisconnectedBitmap, rect);
	}

}

void CHConnector::DrawHandles( Graphics & g, const CHDrawHandle& Info )
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

bool CHConnector::GetProperties( CArray<CHPropertyValue>& list )
{
	ASSERT_VALID(this);

	bool bFind = false;

	for (INT_PTR loop = 0; loop < list.GetCount(); loop++)
	{
		CHPropertyValue& prop = list[loop];

		switch (prop.Id)
		{
		case ID_CONNECTOR_WEIGHT: prop = m_Weight; bFind = true; break;
		case ID_CONNECTOR_TYPE: prop = m_Type; bFind = true; break;
		case ID_OBJECT_NAME: prop = m_Name; bFind = true; break;
		case ID_SHOW_LABEL_01: if(GetLabelCount() >= 1) { prop = GetLabel(0)->Visible(); bFind = true; break; } else break;
		case ID_SHOW_LABEL_02: if(GetLabelCount() >= 2) { prop = GetLabel(1)->Visible(); bFind = true; break; } else break;
		}
	}

	return bFind;
}

bool CHConnector::SetProperties( CArray<CHPropertyValue>& list, bool bPreview )
{
	ASSERT_VALID(this);

	bool bFind = false;

	for (INT_PTR loop = 0; loop < list.GetCount(); loop++)
	{
		CHPropertyValue& prop = list[loop];

		switch (prop.Id)
		{
		case ID_CONNECTOR_WEIGHT: m_Weight = prop; bFind = true; break;
		case ID_CONNECTOR_TYPE: m_Type = prop; bFind = true; break;
		case ID_OBJECT_NAME: m_Name = prop.GetString(); bFind = true; UpdateLabelBounds(); break;
		case ID_SHOW_LABEL_01: if(GetLabelCount() >= 1) { GetLabel(0)->Visible(prop); bFind = true; break; } else break;
		case ID_SHOW_LABEL_02: if(GetLabelCount() >= 2) { GetLabel(1)->Visible(prop); bFind = true; break; } else break;
		}
	}

	return bFind;
}

void CHConnector::Resize( const CPoint& to, int handle )
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

	if ((ToHandle() != handle && FromHandle() != handle) 
		|| (ToHandle() == handle && m_pTo == 0) 
		|| (FromHandle() == handle && m_pFrom == 0))
	{
		RecalcPoints();
	}

	RecalcLabelPosition();
}


bool CHConnector::AddPoint( int& handle, const CPoint& point )
{
	if (handle > 0 && (handle % 2) == 0)
	{
		m_Points.InsertAt(handle/2, ToPoint(point));
		RecalcPoints();
		RecalcLabelPosition();
		handle++;
		return true;
	}

	return false;
}

bool CHConnector::RemovePoint( int handle)
{
	if (handle > 0 && handle % 2 && handle/2 < m_Points.GetCount() && m_Points.GetCount() > 2)
	{
		m_Points.RemoveAt(handle/2);
		RecalcPoints();
		RecalcLabelPosition();
		return true;
	}

	return false;
}

Rect CHConnector::GetBounds( bool bRedraw /*= false*/ )
{
	GraphicsPath path;
	path.AddLines(m_Points.GetData(), m_Points.GetSize());

	Rect rect;
	path.GetBounds(&rect);

	if (bRedraw)
	{
		rect.Inflate(SL_GRIPPER, SL_GRIPPER);
		UnionRect(rect, GetLabelBounds(true));
	}

	return rect;
}

Rect CHConnector::GetArcBounds( CHNetMember* pNetMember )
{
	//TODO:
	return GetBounds(true);
}

bool CHConnector::HitTest( const CRect& rect, int Hint /*= 0*/ )
{
	GraphicsPath path;
	path.AddLines(m_Points.GetData(), m_Points.GetSize());

	Rect bounds;
	path.GetBounds(&bounds);

	return (FromRect(bounds) & rect).IsRectEmpty() ? false : true;
}

bool CHConnector::HitTest( const CPoint& point, int Hint /*= 0*/ )
{
	GraphicsPath path;
	path.AddLines(m_Points.GetData(), m_Points.GetSize());
	path.Widen(&Pen(Color::Black, 4.0f));

	return path.IsVisible(ToPoint(point)) ? true: false;
}

int CHConnector::HitTestHandle( const CPoint& point, int Hint /*= 0*/ )
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

HCURSOR CHConnector::GetCursor( const CPoint& point, int Hint /*= 0*/ )
{
	ASSERT_VALID(this);

	switch (HitTestHandle(point, Hint))
	{
	case 0: return 0;
	default: return AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);;
	}	
}

bool CHConnector::GetHandle( Rect & rect, int Handle )
{
	ASSERT_VALID(this);

	if (Handle < 1 || Handle/2 > m_Points.GetSize()-1)
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
		rect.Width = rect.Height = 0;
		INT a = max(m_Points[Handle/2-1].X, m_Points[Handle/2].X);
		INT b = min(m_Points[Handle/2-1].X, m_Points[Handle/2].X);
		rect.X =  (a - b) / 2 + b;

		a = max(m_Points[Handle/2-1].Y, m_Points[Handle/2].Y);
		b = min(m_Points[Handle/2-1].Y, m_Points[Handle/2].Y);
		rect.Y =  (a - b) / 2 + b;
		rect.Inflate(3, 3);
	}
	return true;
}

void CHConnector::StorePoints( CArray<Point>& fixpoints )
{
	ASSERT_VALID(this);

	INT_PTR count = m_Points.GetCount() - ((m_pTo && m_pFrom) ? 2 : (m_pTo || m_pFrom) ?  1 : 0);

	if (count > 0)
	{
		fixpoints.SetSize(count);

		for (INT_PTR loop=0; loop<fixpoints.GetSize(); loop++)
		{
			fixpoints[loop] = m_Points[loop + ((m_pFrom) ? 1 : 0)];
		}
	}
	else
	{
		fixpoints.RemoveAll();
	}
}

void CHConnector::TransformPoints( const CArray<Point>& fixpoints, const Gdiplus::Matrix& matrix )
{
	ASSERT_VALID(this);

	if (fixpoints.GetSize() == 0)
	{
		return;
	}

	CArray<Point> points;
	points.Copy(fixpoints);

	matrix.TransformPoints(points.GetData(), points.GetCount());

	for (INT_PTR loop=0; loop<points.GetSize(); loop++)
	{
		m_Points[loop + ((m_pFrom) ? 1 : 0)] = points[loop];
	}

	RecalcLabelPosition();
}

bool CHConnector::GetLabelText( CString& text, long labenumber )
{
	switch (labenumber)
	{
	case 0: text = m_Name; return true;
	case 1: text.Format(L"%d", m_Weight); return true;
	default: return false;
	}
}


void CHConnector::RecalcLabelPosition()
{
	CHLabel* pLabel = GetLabel(0);

	if(pLabel)
	{
		Point point;
		INT_PTR count = m_Points.GetCount();

		INT a = max(m_Points[count-2].X, m_Points[count-1].X);
		INT b = min(m_Points[count-2].X, m_Points[count-1].X);
		point.X =  (a - b) / 2 + b;

		a = max(m_Points[count-2].Y, m_Points[count-1].Y);
		b = min(m_Points[count-2].Y, m_Points[count-1].Y);
		point.Y =  (a - b) / 2 + b;
		point.Y -= 15;

		pLabel->Position(point);
	}

	pLabel = GetLabel(1);

	if(pLabel)
	{
		Point point;
		INT_PTR count = m_Points.GetCount();

		INT a = max(m_Points[count-2].X, m_Points[count-1].X);
		INT b = min(m_Points[count-2].X, m_Points[count-1].X);
		point.X =  (a - b) / 2 + b;

		a = max(m_Points[count-2].Y, m_Points[count-1].Y);
		b = min(m_Points[count-2].Y, m_Points[count-1].Y);
		point.Y =  (a - b) / 2 + b;
		point.Y += 5;

		pLabel->Position(point);
	}
}

void CHConnector::To( CHNetMember* pNetMember, bool cutting /*= false*/ )
{
	if (pNetMember && pNetMember == m_pTo)
	{
		return;
	}

	if (m_pTo && pNetMember == 0)
	{
		m_ToId = cutting ? 0: m_pTo->Id();
		m_pTo->RemoveIn(this);
	}

	if (pNetMember)
	{
		ASSERT_VALID(pNetMember);

		m_ToId = pNetMember->Id();
		pNetMember->AddIn(this);
	}

	m_pTo = pNetMember;
}


void CHConnector::From( CHNetMember* pNetMember, bool cutting /*= false*/ )
{
	if (pNetMember && pNetMember == m_pFrom)
	{
		return;
	}

	if (m_pFrom && pNetMember == 0)
	{
		m_FromId = cutting ? 0 : m_pFrom->Id();
		m_pFrom->RemoveOut(this);
	}

	if (pNetMember)
	{
		ASSERT_VALID(pNetMember);

		m_FromId = pNetMember->Id();
		pNetMember->AddOut(this);
	}

	m_pFrom = pNetMember;
}


void CHConnector::RecalcPoints()
{
	if (m_Points.GetSize() < 2)
	{
		return;
	}

	if (m_pTo)
	{
		m_pTo->RecalcArcPoint(m_Points[m_Points.GetSize()-2], m_Points[m_Points.GetSize()-1]);
	}

	if (m_pFrom)
	{
		m_pFrom->RecalcArcPoint(m_Points[1], m_Points[0]);
	}

	RecalcLabelPosition();
}

void CHConnector::SetMinSize()
{
	m_Points.SetSize(2);

	m_Points[1] = m_Points[0];
	m_Points[1].X += 100;
}

void CHConnector::Id( ULONG32 id )
{
	m_Id = id;
	LabelOwnerId(id);
}

void CHConnector::StartTokenAnim()
{
	ASSERT_VALID(this);

	m_TokenLocation = m_Points[0];
	m_AnimIndex = 0;
	m_AnimPieces = 0;
	m_AnimLenght = 0;

	for(int i = 1; i < m_Points.GetSize(); i++)
	{
		INT a = m_Points[i].X -  m_Points[i - 1].X;
		INT b = m_Points[i].Y -  m_Points[i - 1].Y;
		double c = (a*a + b*b);
		c = sqrt(c);
		m_AnimLenght += c;
	}

	int a = m_Points[1].X -  m_Points[0].X;
	int b = m_Points[1].Y -  m_Points[0].Y;
	double c = (a*a + b*b);
	m_doAnimLenghtLine = sqrt(c);
}

void CHConnector::StepTokenAnim(long Step)
{
	ASSERT_VALID(this);

	m_AnimPieces += m_AnimLenght / Step;
	double l = (m_AnimPieces > m_doAnimLenghtLine)? m_doAnimLenghtLine : m_AnimPieces;

	INT ax = m_Points[m_AnimIndex].X;
	INT ay = m_Points[m_AnimIndex].Y;
	INT bx = m_Points[m_AnimIndex + 1].X;
	INT by = m_Points[m_AnimIndex + 1].Y;

	double d = sqrt( l * l / (((bx - ax) * (bx - ax)) + ((by-ay)*(by - ay))));
	m_TokenLocation.X = (INT)(ax + 1 * d * (bx - ax));
	m_TokenLocation.Y = (INT)(ay + 1 * d * (by - ay));
	
	if(m_AnimPieces >= m_doAnimLenghtLine && m_AnimIndex < (m_Points.GetSize()-2))
	{
		m_AnimIndex++;
		m_AnimPieces = m_AnimPieces - m_doAnimLenghtLine;
		
		INT a = m_Points[m_AnimIndex+1].X -  m_Points[m_AnimIndex].X;
		INT b = m_Points[m_AnimIndex+1].Y -  m_Points[m_AnimIndex].Y;
		double c = (a*a + b*b);
		m_doAnimLenghtLine = sqrt(c);	
	}
}

CHPosition* CHConnector::FromPosition()
{
	ASSERT_VALID(this);

	CHPosition* pPos = DYNAMIC_DOWNCAST(CHPosition, m_pFrom);
	ASSERT(pPos);

	return pPos;
}

CHPosition* CHConnector::ToPosition()
{
	ASSERT_VALID(this);

	CHPosition* pPos = DYNAMIC_DOWNCAST(CHPosition, m_pTo);
	ASSERT(pPos);

	return pPos;
}

CHTransition* CHConnector::FromTransition()
{
	ASSERT_VALID(this);

	CHTransition* pTran = DYNAMIC_DOWNCAST(CHTransition, m_pFrom);
	ASSERT(pTran);

	return pTran;
}

CHTransition* CHConnector::ToTransition()
{
	ASSERT_VALID(this);

	CHTransition* pTran = DYNAMIC_DOWNCAST(CHTransition, m_pTo);
	ASSERT(pTran);

	return pTran;
}