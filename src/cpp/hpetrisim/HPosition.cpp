/**************************************************************************
	HPosition.cpp
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "HDrawObject.h"
#include "IniSection.h"
#include "PetriSimDoc.h"
#include "DrawHandle.h"
#include "HPropertyValue.h"
#include "HDrawInfo.h"
#include "HPosition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CHPosition, CHNetMember, VERSIONABLE_SCHEMA|2)

CHPosition::CHPosition()
{
}

CHPosition::CHPosition(const CPoint& point, ULONG32 NameHint) : CHNetMember(point)
, m_Tokens(0)
, m_TokensMax(1)
, m_TokensStart(0)
, m_TokensCount(0)
{
	m_Name.Format(L"P%d", NameHint);
}

CHPosition::~CHPosition()
{
}

void CHPosition::DrawObject( Graphics & g, const CHDrawInfo& Info )
{
	Pen pen(Color::DarkGray, 2.0f);
	SolidBrush brush(Color::White);

	Rect bounds = GetBounds();
	
	g.FillEllipse(&brush, bounds);
	g.DrawEllipse(&pen, bounds);

	switch (m_Tokens)
	{
	case 0: break;
	case 1:
		{
			Rect token(m_Position, Size());
			token.Inflate(m_Size/4, m_Size/4);

			g.FillEllipse(&Info.TokenBrush, token);
		}
		break;
	case 2:
		{
			Rect token(m_Position, Size());
			token.Inflate(m_Size/6, m_Size/6);

			token.X -= m_Size/5;
			g.FillEllipse(&Info.TokenBrush, token);

			token.X += m_Size/5;
			token.X += m_Size/5;
			g.FillEllipse(&Info.TokenBrush, token);

		}
		break;
	case 3:
		{
			Rect token(m_Position, Size());
			token.Inflate(m_Size/6, m_Size/6);

			token.Y += m_Size/6;
			token.X -= m_Size/5;
			g.FillEllipse(&Info.TokenBrush, token);

			token.X += m_Size/5;
			token.X += m_Size/5;
			g.FillEllipse(&Info.TokenBrush, token);

			token.Y -= m_Size/3;
			token.X -= m_Size/5;
			g.FillEllipse(&Info.TokenBrush, token);
		}
		break;
	case 4:
		{
			Rect token(m_Position, Size());
			token.Inflate(m_Size/7, m_Size/7);

			token.Y += m_Size/6;
			token.X -= m_Size/6;
			g.FillEllipse(&Info.TokenBrush, token);

			token.X += m_Size/3;
			g.FillEllipse(&Info.TokenBrush, token);

			token.Y -= m_Size/3;
			g.FillEllipse(&Info.TokenBrush, token);

			token.X -= m_Size/3;
			g.FillEllipse(&Info.TokenBrush, token);
		}
		break;
	default:
		{
			CString str;
			str.Format(L"%d", m_Tokens);

			g.DrawString(str, str.GetLength(), &Info.TokenFont, ToRectF(bounds), &Info.StringFormatCenter, &Info.TokenBrush);
		}
	}
}

void CHPosition::DrawHandles( Graphics & g, const CHDrawHandle& Info )
{
	g.DrawEllipse(&Info.m_PreviewPen2, GetBounds());
}

bool CHPosition::GetProperties( CArray<CHPropertyValue>& list )
{
	ASSERT_VALID(this);

	bool bFind = CHNetMember::GetProperties(list);

	for (INT_PTR loop = 0; loop < list.GetCount(); loop++)
	{
		CHPropertyValue& prop = list[loop];

		switch (prop.Id)
		{
		case ID_POSITION_TOKEN_NOW: prop = m_Tokens; bFind = true; break;
		case ID_POSITION_TOKEN_INITIAL: prop = m_TokensStart; bFind = true; break;
		case ID_POSITION_TOKEN_CAPACITY: prop = m_TokensMax; bFind = true; break;
		}
	}

	return bFind;
}

bool CHPosition::SetProperties( CArray<CHPropertyValue>& list, bool bPreview )
{
	ASSERT_VALID(this);

	bool bFind = CHNetMember::SetProperties(list, bPreview);

	for (INT_PTR loop = 0; loop < list.GetCount(); loop++)
	{
		CHPropertyValue& prop = list[loop];

		switch (prop.Id)
		{
		case ID_POSITION_TOKEN_NOW: m_Tokens = prop.GetLong(0); bFind = true; break;
		case ID_POSITION_TOKEN_INITIAL: m_TokensStart = prop.GetLong(0); m_Tokens = m_TokensStart; bFind = true; break;
		case ID_POSITION_TOKEN_CAPACITY: m_TokensMax = prop.GetLong(1); bFind = true; break;
		}
	}

	return bFind;
}

Rect CHPosition::GetBounds( bool bRedraw /*= false*/ )
{
	Rect rect(m_Position, Size());
	rect.Inflate(m_Size/2, m_Size/2);

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

bool CHPosition::HitTest( const CRect& rect, int Hint /*= 0*/ )
{
	return (FromRect(GetBounds()) & rect).IsRectEmpty() ? false : true;
}

bool CHPosition::HitTest( const CPoint& point, int Hint /*= 0*/ )
{
	return FromRect(GetBounds()).PtInRect(point) ? true : false;
}

#ifdef _DEBUG
void CHPosition::AssertValid() const
{
	CObject::AssertValid();
}
void CHPosition::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}
#endif //_DEBUG

void CHPosition::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		CHNetMember::Serialize(ar);

		ar << m_Tokens;
		ar << m_TokensMax;
		ar << m_TokensStart;
		ar << m_TokensCount;
	}
	else
	{
		UINT nSchema = ar.GetObjectSchema();

		if(nSchema == 2)
		{
			CHNetMember::Serialize(ar);

			ar >> m_Tokens;
			ar >> m_TokensMax;
			ar >> m_TokensStart;
			ar >> m_TokensCount;
		}
		else
		{
			DWORD dw;
			BYTE b;
			double db;
			CRect rect;
			ar >> dw; m_Tokens = dw;
			ar >> dw; m_TokensMax = dw;
			ar >> dw; m_TokensStart = dw;
			ar >> dw; m_TokensCount = dw;
			ar >> dw; rect.left = dw;
			ar >> dw; rect.top = dw;
			ar >> dw; rect.right = dw;
			ar >> dw; rect.bottom = dw;		
			ar >> dw; m_Size = (PT_SIZE)dw;
			ar >> dw; Id(dw);
			ar >> dw;
			ar >> b;
			ar >> b;
			ar >> b;
			ar >> b;
			ar >> b;
			ar >> b;
			ar >> b;
			ar >> db;

			m_Position = ToPoint(rect.CenterPoint());
		}
	}
}

bool CHPosition::Reset()
{
	m_Tokens = m_TokensStart;
	m_TokensCount = 0;

	return true;
}

bool CHPosition::GetLabelText( CString& text, long labenumber )
{
	switch (labenumber)
	{
	case 0: text = m_Name; return true;
	case 1: text.Format(L"%d", m_TokensMax); return true;
	default: return false;
	}
}

void CHPosition::RecalcArcPoint( const Point& edge, Point& point )
{
	ASSERT_VALID(this);

	float ax = (float)m_Position.X;
	float ay = (float)m_Position.Y;
	float bx = (float)edge.X;
	float by = (float)edge.Y;
	float gx = bx - ax;
	float gy = by - ay;

	if (gx == 0.0f || gy == 0.0f)
	{
		INT a = m_Size/2;
		INT b = m_Size/2;

		if((gx == 0) && (gy > 0))
		{
			//center bottom
			point.X = m_Position.X;
			point.Y = m_Position.Y + a;
		}
		if((gx == 0) && (gy < 0))
		{
			//center top
			point.X = m_Position.X;
			point.Y = m_Position.Y - a;
		}
		if((gx > 0) && (gy == 0))
		{
			//center right
			point.X = m_Position.X + b;
			point.Y = m_Position.Y;
		}
		if((gx < 0) && (gy == 0))
		{
			//center left
			point.X = m_Position.X - b;
			point.Y = m_Position.Y;
		}
	}
	else
	{
		float a = (float)m_Size/2;
		float b = (float)m_Size/2;
		float m = gy / gx;
		
		float ex = a * b / (b * b + a * a * m * m) * sqrt(a * a * m * m + b * b);
		float ey = b / a * sqrt( a * a - ex * ex);
		if(gy < 0) ey = -ey;
		if(gx < 0) ex = -ex;

		point.X = static_cast<INT>(ex + ax);
		point.Y = static_cast<INT>(ey + ay);
	}
}

void CHPosition::RecalcLabelPosition()
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