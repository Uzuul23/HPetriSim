/**************************************************************************
	HTransition.cpp
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "HDrawObject.h"
#include "HRandom.h"
#include "IniSection.h"
#include "HLabel.h"
#include "ArchiveHelper.h"
#include "DrawHandle.h"
#include "HPropertyValue.h"
#include "HDrawInfo.h"
#include "HTransition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CHTransition, CHNetMember, VERSIONABLE_SCHEMA|2)

CHTransition::CHTransition()
{
}

CHTransition::CHTransition(const CPoint& point, ULONG32 NameHint) : CHNetMember(point)
, m_TimeMode(TTM_IMMIDIATE)
, m_Time(0)
, m_TimeStart(0)
, m_TokensCount(0)
, m_TimeRange(0)
, m_Activ(false)
, m_Enabled(false)
{
	m_Name.Format(L"T%d", NameHint);
}

CHTransition::~CHTransition()
{
}

#ifdef _DEBUG
void CHTransition::AssertValid() const
{
	CObject::AssertValid();
}

void CHTransition::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}
#endif //_DEBUG

void CHTransition::DrawObject( Graphics & g, const CHDrawInfo& Info )
{
	Pen pen(Color::DarkGray, 2.0f);

	Rect rect(GetBounds());

	if (!Info.Online)
	{
		SolidBrush brush(Color::White);

		g.FillRectangle(&brush, GetBounds());
		g.DrawRectangle(&pen, GetBounds());

		if (m_TimeMode != TTM_IMMIDIATE)
		{
			Rect rect2(rect.X+rect.Width/2, rect.Y+rect.Height/2, 0, 0);
			rect2.Inflate(8, 8);
			g.DrawImage(Info.TimerBitmap, rect2);
		}
	}
	else if (m_Activ)
	{
		SolidBrush brush(Color::DarkGray);

		g.FillRectangle(&brush, rect);
		g.DrawRectangle(&pen, rect);
	}
	else if (m_Enabled)
	{
		SolidBrush brush(Color::White);

		g.FillRectangle(&brush, rect);
		g.DrawRectangle(&pen, rect);

		Rect rect2(rect.X+rect.Width/2, rect.Y+rect.Height/2, 0, 0);
		rect2.Inflate(8, 8);
		g.DrawImage(Info.TimerBitmap, rect2);
	}
	else
	{
		SolidBrush brush(Color::White);

		g.FillRectangle(&brush, GetBounds());
		g.DrawRectangle(&pen, GetBounds());
	}
}

void CHTransition::DrawHandles( Graphics & g, const CHDrawHandle& Info )
{
	Info.DrawPreview(g, GetBounds());
}

bool CHTransition::GetProperties( CArray<CHPropertyValue>& list )
{
	bool bFind = CHNetMember::GetProperties(list);

	for (INT_PTR loop = 0; loop < list.GetCount(); loop++)
	{
		CHPropertyValue& prop = list[loop];

		switch (prop.Id)
		{
		case ID_TRANSITION_TIME_MODE: prop = m_TimeMode; bFind = true; break;
		case ID_TRANSITION_TIME_START: prop = m_TimeStart; bFind = true; break;
		case ID_TRANSITION_TIME_RANGE: prop = m_TimeRange; bFind = true; break;
		case ID_TRANSITION_TOKEN_COUNTER: prop = m_TokensCount; bFind = true; break;
		}
	}

	return bFind;
}

bool CHTransition::SetProperties( CArray<CHPropertyValue>& list, bool bPreview )
{
	bool bFind = CHNetMember::SetProperties(list, bPreview);

	for (INT_PTR loop = 0; loop < list.GetCount(); loop++)
	{
		CHPropertyValue& prop = list[loop];

		switch (prop.Id)
		{
		case ID_TRANSITION_TIME_MODE: m_TimeMode = prop; bFind = true; break;
		case ID_TRANSITION_TIME_START: m_TimeStart = prop; bFind = true; break;
		case ID_TRANSITION_TIME_RANGE: m_TimeRange = prop; bFind = true; break;
		case ID_TRANSITION_TOKEN_COUNTER: m_TokensCount = prop; bFind = true; break;
		}
	}

	return bFind;
}

Rect CHTransition::GetBounds( bool bRedraw /*= false*/ )
{
	Rect rect(m_Position, Size());
	rect.Inflate(m_Size/2, m_Size/6);

	if (bRedraw)
	{
		rect.Inflate(5, 5);

		Rect rect2 = GetArcBounds();

		if (!rect.IsEmptyArea())
		{
			UnionRect(rect, rect2);
		}

		rect2 = GetLabelBounds();

		if (!rect.IsEmptyArea())
		{
			UnionRect(rect, rect2);
		}
	}

	return rect;
}

bool CHTransition::HitTest( const CRect& rect, int Hint /*= 0*/ )
{
	return (FromRect(GetBounds()) & rect).IsRectEmpty() ? false : true;
}

bool CHTransition::HitTest( const CPoint& point, int Hint /*= 0*/ )
{
	return FromRect(GetBounds()).PtInRect(point) ? true : false;
}

void CHTransition::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		CHNetMember::Serialize(ar);

		ar << m_Time;
		ar << m_TimeStart;
		ar << m_TokensCount;
		ar << m_TimeRange;
		ar << static_cast<LONG32>(m_TimeMode);
	}
	else
	{
		UINT nSchema = ar.GetObjectSchema();

		if(nSchema == 2)
		{
			CHNetMember::Serialize(ar);

			LONG32 l = 0;

			ar >> m_Time;
			ar >> m_TimeStart;
			ar >> m_TokensCount;
			ar >> m_TimeRange;
			ar >> l; m_TimeMode = static_cast<TransitionTimeMode>(l);
		}
		else
		{
			BYTE b;
			DWORD dw;
			double db;
			CRect rect;
			ar >> dw; m_Size = (PT_SIZE)dw;
			ar >> dw; rect.left = dw;
			ar >> dw; rect.top = dw;
			ar >> dw; rect.right = dw;
			ar >> dw; rect.bottom = dw;
			ar >> dw; Id(dw);
			ar >> dw; m_Time = dw;
			ar >> dw; m_TimeStart = dw;
			ar >> dw; m_TokensCount = dw;
			ar >> dw; m_TimeRange = dw;
			ar >> dw;
			ar >> dw;
			ar >> b;
			ar >> b;
			ar >> b; m_TimeMode = (TransitionTimeMode)b;
			ar >> b;
			ar >> b;
			ar >> b;
			ar >> b;
			ar >> b;
			ar >> b;
			ar >> db;

			m_Position = ToPoint(rect.CenterPoint());
		}
		
		m_Activ = false;
		m_Enabled = false;
	}
}

//void CHTransition::DrawObject(CDC * pDC)
//{
//	/*ASSERT_VALID(this);
//	CPetriSimApp* pApp = (CPetriSimApp*)AfxGetApp();
//	CHResHolder& rRes = pApp->m_cResHolder;
//	CBrush* oldbrush;
//	if(m_blSelected)
//	{
//		if(m_Activ)
//		{
//			oldbrush = pDC->SelectObject(&rRes.m_cBrushGreen);
//			pDC->Rectangle(m_crRect);
//			oldbrush = pDC->SelectObject(&rRes.m_cBrushYellow);
//			for(int i = 1;i<5;i++)
//				pDC->Rectangle(GetGripper(i));
//		}
//		else if(m_Enabled)
//		{
//			oldbrush = pDC->SelectObject(&rRes.m_cBrushYellow);
//			pDC->Rectangle(m_crRect);
//			for(int i = 1;i<5;i++)
//				pDC->Rectangle(GetGripper(i));
//		}
//		else
//		{
//			oldbrush = pDC->SelectObject(&rRes.m_cBrushRed);
//			pDC->Rectangle(m_crRect);
//			oldbrush = pDC->SelectObject(&rRes.m_cBrushYellow);
//			for(int i = 1;i<5;i++)
//				pDC->Rectangle(GetGripper(i));
//		}
//	}
//	else
//	{
//		if(m_Activ)
//		{
//			oldbrush = pDC->SelectObject(&rRes.m_cBrushGreen);
//			pDC->Rectangle(m_crRect);
//		}
//		else if(m_Enabled)
//		{
//			oldbrush = pDC->SelectObject(&rRes.m_cBrushYellow);
//			pDC->Rectangle(m_crRect);
//		}
//		else
//		{
//			if(m_TimeMode == CHTransition::TTM_IMMIDIATE)
//			oldbrush = pDC->SelectObject(&rRes.m_cBrushBlack);
//			else
//				oldbrush = pDC->SelectObject(&rRes.m_cBrushWhite);
//			pDC->Rectangle(m_crRect);
//		}	
//	}
//	pDC->SelectObject(oldbrush);*/
//}
//

void CHTransition::RecalcLabelPosition()
{
	CHLabel* pLabel = GetLabel(0);
	if (pLabel)
	{
		Point point(m_Position);
		point.X -= m_Size;
		point.Y -= m_Size;
		
		pLabel->Position(point);
	}

	pLabel = GetLabel(1);
	if (pLabel)
	{
		Point point(m_Position);
		point.X -= m_Size;
		point.Y += m_Size - 14;

		pLabel->Position(point);
	}
}

bool CHTransition::Reset()
{
	if(m_TimeMode == CHTransition::TTM_IMMIDIATE)
		m_Time = m_TimeStart;
	m_TokensCount = 0;
	m_Activ = false;
	m_Enabled = false;

	return true;
}

void CHTransition::RecalcArcPoint( const Point& edge, Point& point )
{
	ASSERT_VALID(this);

	Rect rect(GetBounds());

	float ax = (float)m_Position.X;
	float ay = (float)m_Position.Y;
	float bx = (float)edge.X;
	float by = (float)edge.Y;
	float a = (float)m_Size/2;
	float b = (float)m_Size/6;
	float gx = bx - ax;
	float gy = by - ay;

	
	float m = sqrt(gx * gx + gy * gy);
	float alpha = (float)(3.14 / 2.0 - atan(b / a));
	float beta = acos(((gy / m ) < 0) ? -(gy / m) : (gy / m));

	if(beta < alpha)
	{
		float ex = b * tan(beta);

		if(gy < 0)
			point.Y = rect.Y;
		else
			point.Y = rect.Y + rect.Height;

		if(gx > 0)
			point.X = INT(a + rect.X + ex);
		else
			point.X = INT(a + rect.X - ex);
	}
	else
	{
		float ex = a / tan(beta);

		if(gx < 0)
			point.X = rect.X;
		else
			point.X = rect.X + rect.Width;

		if(gy > 0)
			point.Y = INT(b + rect.Y + ex);
		else
			point.Y = INT(b + rect.Y - ex);
	}
}

bool CHTransition::GetLabelText( CString& text, long labenumber )
{
	switch (labenumber)
	{
	case 0: text = m_Name; return true;
	case 1: text.Format(L"%d", m_TimeStart); return true;
	default: return false;
	}
}
