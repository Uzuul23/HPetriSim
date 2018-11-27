/**************************************************************************
	HLine.cpp
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#include "stdafx.h"
#include "Resource.h"
#include "IniSection.h"
#include "ArchiveHelper.h"
#include "GDI+Helper.h"
#include "HLine.h"

IMPLEMENT_SERIAL(CHLine, CHPoly, VERSIONABLE_SCHEMA|2)

CHLine::CHLine()
{
}

CHLine::CHLine(CPoint& start) : CHPoly(start)
{
	m_FillStyle = NoFill;
	m_CloseFigure = false;
}

CHLine::~CHLine()
{

}

void CHLine::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		CHPoly::Serialize(ar);
	}
	else
	{
		UINT nSchema = ar.GetObjectSchema();

		if(nSchema == 2)
		{
			ar.SetObjectSchema(nSchema);
			CHPoly::Serialize(ar);
		}
		else
		{
			WORD w;
			DWORD dw;
			CRect rect;
			ar >> dw; rect.left = dw;
			ar >> dw; rect.top = dw;
			ar >> dw; rect.right = dw;
			ar >> dw; rect.bottom = dw;
			ar >> w; m_DashStyle = ConvertPenStyle(w);
			ar >> w; m_LineWeight = static_cast<REAL>(w);
			ar >> dw; m_LineColor.SetFromCOLORREF(dw);

			m_Points.Add(ToPoint(rect.TopLeft()));
			m_Points.Add(ToPoint(rect.BottomRight()));

			m_FillStyle = NoFill;
			m_CloseFigure = false;
		}
	}		
}

#ifdef _DEBUG
void CHLine::AssertValid() const
{
	CObject::AssertValid();
}

void CHLine::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}
#endif //_DEBUG

