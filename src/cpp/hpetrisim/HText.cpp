/**************************************************************************
	HText.cpp
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#include "stdafx.h"
#include "Resource.h"
#include "HDrawObject.h"
#include "ArchiveHelper.h"
#include "GDI+Helper.h"
#include "IniSection.h"
#include "DrawHandle.h"
#include "HPropertyValue.h"
#include "Htext.h"

IMPLEMENT_SERIAL(CHText, CHRect, VERSIONABLE_SCHEMA|2)

CHText::CHText()
{
}

CHText::CHText(const CPoint& topleft) : CHRect(topleft)
, m_FontColor(Color::DarkGray)
, m_FontName(L"Arial")
, m_FontStyle(FontStyleRegular)
, m_FontSize(11.0f)
, m_Alignment(StringAlignmentCenter)
, m_LineAlignment(StringAlignmentCenter)
, m_Text(L"Text")
, m_AutoSize(true)
{
	m_FillStyle = CHDrawObject::NoFill;
	m_NoBorder = true;

	DrawString(0, true);
}

CHText::~CHText()
{

}

#ifdef _DEBUG
void CHText::AssertValid() const
{
	CObject::AssertValid();
}

void CHText::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}

#endif //_DEBUG

void CHText::DrawObject( Graphics & g, const CHDrawInfo& Info )
{
	ASSERT_VALID(this);

	CHRect::DrawObject(g, Info);
	DrawString(&g);
}

void CHText::DrawString( Graphics* pg, bool bMessure /*= false*/ )
{
	ASSERT_VALID(this);

	SolidBrush brush(m_FontColor);
	Gdiplus::Font font(m_FontName, m_FontSize, m_FontStyle);

	StringFormat stringformat;
	stringformat.SetAlignment(m_Alignment);
	stringformat.SetLineAlignment(m_LineAlignment);

	RectF rectf(ToRectF(m_Bounds));

	if (bMessure)
	{
		SizeF sizef(64000.0f, 64000.0f);
		SizeF textsize;

		if (pg == 0)
		{
			CWnd* pWnd = AfxGetMainWnd();
			
			if (pWnd)
			{
				Graphics* pg2 = new Graphics(pWnd->GetSafeHwnd());
				pg2->MeasureString(m_Text, m_Text.GetLength(), &font, sizef, &stringformat, &textsize);
				m_Bounds = Rect(m_Bounds.X, m_Bounds.Y, static_cast<INT>(textsize.Width+1.0), static_cast<INT>(textsize.Height+1.0));

				delete pg2;
			}
		}
		else
		{
			pg->MeasureString(m_Text, m_Text.GetLength(), &font, sizef, &stringformat, &textsize);
			m_Bounds = Rect(m_Bounds.X, m_Bounds.Y, static_cast<INT>(textsize.Width), static_cast<INT>(textsize.Height));
		}
	}

	if (pg != 0 && bMessure == false)
	{
		pg->DrawString(m_Text, m_Text.GetLength(), &font, rectf, &stringformat, &brush);
	}	
}

void CHText::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

	if (ar.IsStoring())
	{
		CHRect::Serialize(ar);

		ar << m_FontColor;
		ar << m_Text;
		ar << m_FontName;
		ar << m_FontStyle;
		ar << m_FontSize;
		ar << m_Alignment;
		ar << m_LineAlignment;
		ar << m_AutoSize;
	}
	else
	{
		UINT nSchema = ar.GetObjectSchema();

		if(nSchema == 2)
		{
			ar.SetObjectSchema(nSchema);
			CHRect::Serialize(ar);

			ar >> m_FontColor;
			ar >> m_Text;
			ar >> m_FontName;
			ar >> m_FontStyle;
			ar >> m_FontSize;
			ar >> m_Alignment;
			ar >> m_LineAlignment;
			ar >> m_AutoSize;
		}
		else
		{
			CRect rect;
			DWORD dw;
			BYTE b;
			CHAR c;
			CStringA astring;
			LOGFONT logfont;
			ar >> dw; rect.top = dw;
			ar >> dw; rect.left = dw;
			ar >> dw; rect.bottom = dw;
			ar >> dw; rect.right = dw;
			ar >> dw; m_FontColor.SetFromCOLORREF(dw);
			ar >> dw; logfont.lfHeight = dw;
			ar >> dw; logfont.lfWidth = dw;
			ar >> dw; logfont.lfEscapement = dw;
			ar >> dw; logfont.lfOrientation = dw;
			ar >> dw; logfont.lfWeight = dw;
			ar >> b; logfont.lfItalic = b;
			ar >> b; logfont.lfUnderline = b;
			ar >> b; logfont.lfStrikeOut = b;
			ar >> b; logfont.lfCharSet = b;
			ar >> b; logfont.lfOutPrecision = b;
			ar >> b; logfont.lfClipPrecision = b;
			ar >> b; logfont.lfQuality = b;
			ar >> b; logfont.lfPitchAndFamily = b;
			for(int i = 0;i < 32;i++)
			{
				ar >> c;
				logfont.lfFaceName[i] = c;
			}

			ar >> astring;
			m_Text = astring;

			rect.NormalizeRect();
			m_Bounds = Rect(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top);

			m_FontName = logfont.lfFaceName;
			m_FontSize = (static_cast<REAL>(abs(logfont.lfHeight)) /96.0f)*72.0f;

			if (logfont.lfItalic && logfont.lfWeight > FW_REGULAR)
			{
				m_FontStyle = FontStyleBoldItalic;
			}
			else if (logfont.lfItalic)
			{
				m_FontStyle = FontStyleItalic;
			}
			else if (logfont.lfWeight > FW_REGULAR)
			{
				m_FontStyle = FontStyleBold;
			}
			else
			{
				m_FontStyle = FontStyleRegular;
			}

			if (logfont.lfUnderline)
			{
				m_FontStyle = (FontStyle)(m_FontStyle|FontStyleUnderline);
			}

			if (logfont.lfStrikeOut)
			{
				m_FontStyle = (FontStyle)(m_FontStyle|FontStyleStrikeout);
			}

			m_Alignment = StringAlignmentCenter;
			m_LineAlignment = StringAlignmentCenter;

			m_FillColor = Color::Aquamarine;
			m_LineColor = Color::Gray;
			m_RectStyle = Rectangle;
			m_LineWeight = 2.0;
			m_DashStyle = DashStyleSolid;
			m_FillStyle = CHDrawObject::NoFill;
			m_NoBorder = true;
			m_AutoSize = true;

			DrawString(0, true);
		}
	}
}

void CHText::SetMinSize()
{
	ASSERT_VALID(this);
	DrawString(0, true);
}

void CHText::DrawHandles( Graphics & g, const CHDrawHandle& Info )
{
	ASSERT_VALID(this);

	if (!m_AutoSize)
	{
		CHRect::DrawHandles(g, Info);
	}
	
	Info.DrawPreview(g, m_Bounds);
}

bool CHText::SetProperties( CArray<CHPropertyValue>& list, bool bPreview )
{
	ASSERT_VALID(this);

	bool bFind = CHRect::SetProperties(list, bPreview);

	for (INT_PTR loop = 0; loop < list.GetCount(); loop++)
	{
		CHPropertyValue& prop = list[loop];

		switch (prop.Id)
		{
		case ID_OBJECT_TEXT: m_Text = prop.GetString(); bFind = true; break;
		case ID_OBJECT_FONT_NAME: m_FontName = prop.GetString(); bFind = true; break;
		case ID_OBJECT_FONT_SIZE: m_FontSize = prop.GetFloat(6.0); bFind = true; break;
		case ID_OBJECT_FONT_COLOR: m_FontColor = prop; bFind = true; break;
		case ID_OBJECT_FONT_BOLD: { long l = static_cast<long>(m_FontStyle) ; prop.GetBool() ? l |= 1 : l ^= 1; m_FontStyle = static_cast<FontStyle>(l); bFind = true; break; }
		case ID_OBJECT_FONT_ITALIC: { long l = static_cast<long>(m_FontStyle) ; prop.GetBool() ? l |= 2 : l ^= 2; m_FontStyle = static_cast<FontStyle>(l); bFind = true; break; }
		case ID_OBJECT_FONT_UNDERLINE: { long l = static_cast<long>(m_FontStyle) ; prop.GetBool() ? l |= 4 : l ^= 4; m_FontStyle = static_cast<FontStyle>(l); bFind = true; break; }
		case ID_OBJECT_FONT_STRIKEOUT: { long l = static_cast<long>(m_FontStyle) ; prop.GetBool() ? l |= 8 : l ^= 8; m_FontStyle = static_cast<FontStyle>(l); bFind = true; break; }
		case ID_OBJECT_TEXT_AUTOSIZE: m_AutoSize = prop; bFind = true; break;
		case ID_OBJECT_TEXT_LEFT: m_Alignment = StringAlignmentNear; bFind = true; break;
		case ID_OBJECT_TEXT_CENTER: m_Alignment = StringAlignmentCenter; bFind = true; break;
		case ID_OBJECT_TEXT_RIGHT: m_Alignment = StringAlignmentFar; bFind = true; break;
		case ID_OBJECT_TEXT_TOP: m_LineAlignment = StringAlignmentNear; bFind = true; break;
		case ID_OBJECT_TEXT_LINE_CENTER: m_LineAlignment = StringAlignmentCenter; bFind = true; break;
		case ID_OBJECT_TEXT_BOTTOM: m_LineAlignment = StringAlignmentFar; bFind = true; break;
		case ID_OBJECT_FONT_SIZE_PLUS: m_FontSize = min(72, m_FontSize++); bFind = true; break;
		case ID_OBJECT_FONT_SIZE_MINUS: m_FontSize = max(4, m_FontSize--); bFind = true; break;
		}
	}

	if (m_AutoSize)
	{
		DrawString(0, true);
	}

	return bFind;
}

bool CHText::GetProperties( CArray<CHPropertyValue>& list )
{
	ASSERT_VALID(this);

	bool bFind = CHRect::GetProperties(list);

	for (INT_PTR loop = 0; loop < list.GetCount(); loop++)
	{
		CHPropertyValue& prop = list[loop];

		switch (prop.Id)
		{
		case ID_OBJECT_TEXT: prop = m_Text; bFind = true; break;
		case ID_OBJECT_FONT_NAME: prop = m_FontName; bFind = true; break;
		case ID_OBJECT_FONT_SIZE: prop = static_cast<long>(m_FontSize); bFind = true; break;
		case ID_OBJECT_FONT_COLOR: prop = m_FontColor; bFind = true; break;
		case ID_OBJECT_FONT_BOLD: { bool b = (m_FontStyle & 1) !=  0 ? true : false; prop = b; bFind = true; break; }
		case ID_OBJECT_FONT_ITALIC: { bool b = (m_FontStyle & 2) !=  0 ? true : false; prop = b; bFind = true; break; }
		case ID_OBJECT_FONT_UNDERLINE: { bool b = (m_FontStyle & 4) !=  0 ? true : false; prop = b; bFind = true; break; }
		case ID_OBJECT_FONT_STRIKEOUT: { bool b = (m_FontStyle & 8) !=  0 ? true : false; prop = b; bFind = true; break; }
		case ID_OBJECT_TEXT_AUTOSIZE: prop = m_AutoSize; bFind = true; break;
		case ID_OBJECT_TEXT_LEFT: prop = m_Alignment == StringAlignmentNear ? true : false; bFind = true; break;
		case ID_OBJECT_TEXT_CENTER: prop = m_Alignment == StringAlignmentCenter ? true : false; bFind = true; break;
		case ID_OBJECT_TEXT_RIGHT: prop = m_Alignment == StringAlignmentFar ? true : false; bFind = true; break;
		case ID_OBJECT_TEXT_TOP: prop = m_LineAlignment == StringAlignmentNear ? true : false; bFind = true; break;
		case ID_OBJECT_TEXT_LINE_CENTER: prop = m_LineAlignment == StringAlignmentCenter ? true : false; bFind = true; break;
		case ID_OBJECT_TEXT_BOTTOM: prop = m_LineAlignment == StringAlignmentFar ? true : false; bFind = true; break;
		case ID_OBJECT_FONT_SIZE_PLUS: bFind = true; break;
		case ID_OBJECT_FONT_SIZE_MINUS: bFind = true; break;
		}
	}

	return bFind;
}

void CHText::Resize( const CPoint& to, int handle )
{
	ASSERT_VALID(this);

	if (!m_AutoSize)
	{
		CHRect::Resize(to, handle);
	}
}

void CHText::SetBounds( const Rect& rect )
{
	ASSERT_VALID(this);

	if (!m_AutoSize)
	{
		CHRect::SetBounds(rect);
		return;
	}

	if (rect.IsEmptyArea())
	{
		return;
	}

	m_Bounds.X = rect.X;
	m_Bounds.Y = rect.Y;
}

bool CHText::GetHandle( Rect & rect, int Handle )
{
	ASSERT_VALID(this);

	if (!m_AutoSize)
	{
		return CHRect::GetHandle(rect, Handle);
	}

	return 0;
}

void CHText::TransformPoints( const CArray<Point>& fixpoints, const Gdiplus::Matrix& matrix )
{
	ASSERT_VALID(this);

	if (!m_AutoSize)
	{
		CHRect::TransformPoints(fixpoints, matrix);
		return;
	}

	Point point(fixpoints[0]);
	matrix.TransformPoints(&point);

	m_Bounds = Rect(point.X, point.Y, m_Bounds.Width, m_Bounds.Height);
}

bool CHText::HitTest( const CRect& rect, int Hint /*= 0*/ )
{
	return (FromRect(m_Bounds) & rect).IsRectEmpty() ? false : true;
}

bool CHText::HitTest( const CPoint& point, int Hint /*= 0*/ )
{
	Rect bounds(m_Bounds);
	bounds.Inflate(static_cast<INT>(m_LineWeight), static_cast<INT>(m_LineWeight));

	return bounds.Contains(ToPoint(point)) ? true : false;
}