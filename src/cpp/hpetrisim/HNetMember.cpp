/**************************************************************************
	HNetMember.cpp

	copyright (c) 2013/08/07 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#include "StdAfx.h"
#include "resource.h"
#include "ArchiveHelper.h"
#include "HPropertyValue.h"
#include "HConnector.h"
#include "HNetMember.h"

IMPLEMENT_SERIAL(CHNetMember, CHDrawObject, VERSIONABLE_SCHEMA|2)

CHNetMember::CHNetMember(void)
{
}

CHNetMember::CHNetMember(const CPoint& point) 
: m_Position(ToPoint(point))
, m_id(0)
, m_Size(PT_NORMAL)
{

}
CHNetMember::~CHNetMember(void)
{
}

bool CHNetMember::GetProperties( CArray<CHPropertyValue>& list )
{
	ASSERT_VALID(this);

	bool bFind = false;

	for (INT_PTR loop = 0; loop < list.GetCount(); loop++)
	{
		CHPropertyValue& prop = list[loop];

		switch (prop.Id)
		{
		case ID_OBJECT_SIZE: prop = m_Size; bFind = true; break;
		case ID_OBJECT_NAME: prop = m_Name; bFind = true; break;
		case ID_SHOW_LABEL_01: if(GetLabelCount() >= 1) { prop = GetLabel(0)->Visible(); bFind = true; break; } else break;
		case ID_SHOW_LABEL_02: if(GetLabelCount() >= 2) { prop = GetLabel(1)->Visible(); bFind = true; break; } else break;
		}
	}

	return bFind;
}

bool CHNetMember::SetProperties( CArray<CHPropertyValue>& list, bool bPreview )
{
	ASSERT_VALID(this);

	bool bFind = false;

	for (INT_PTR loop = 0; loop < list.GetCount(); loop++)
	{
		CHPropertyValue& prop = list[loop];

		switch (prop.Id)
		{
		case ID_OBJECT_SIZE: m_Size = prop; bFind = true; RecalcLabelPosition(); RecalcArcs(); break;
		case ID_OBJECT_NAME: m_Name = prop.GetString(); bFind = true; UpdateLabelBounds(); break;
		case ID_SHOW_LABEL_01: if(GetLabelCount() >= 1) { GetLabel(0)->Visible(prop); bFind = true; break; } else break;
		case ID_SHOW_LABEL_02: if(GetLabelCount() >= 2) { GetLabel(1)->Visible(prop); bFind = true; break; } else break;
		}
	}

	return bFind;
}

void CHNetMember::Serialize( CArchive& ar )
{
	ASSERT_VALID(this);

	if (ar.IsStoring())
	{
		ar << m_id;
		ar << m_Position;
		ar << m_Size;
		ar << m_Name;
	}
	else
	{
		ar >> m_id;
		ar >> m_Position;
		ar >> m_Size;
		ar >> m_Name;
	}
}

void CHNetMember::StorePoints( CArray<Point>& fixpoints )
{
	ASSERT_VALID(this);

	fixpoints.SetSize(1);
	fixpoints[0] = m_Position;
}

void CHNetMember::TransformPoints( const CArray<Point>& fixpoints, const Gdiplus::Matrix& matrix )
{	
	ASSERT_VALID(this);

	m_Position = fixpoints[0];
	matrix.TransformPoints(&m_Position);

	RecalcArcs();
	RecalcLabelPosition();
}

void CHNetMember::AddIn( CHConnector* pConnector )
{
	ASSERT_VALID(this);
	ASSERT_VALID(pConnector);
	
	m_ConnectorsIn.Add(pConnector);
}

void CHNetMember::RemoveIn( CHConnector* pConnector )
{
	ASSERT_VALID(this);
	ASSERT_VALID(pConnector);

	for (INT_PTR loop=0; loop<m_ConnectorsIn.GetCount(); loop++)
	{
		if (m_ConnectorsIn[loop] == pConnector)
		{
			m_ConnectorsIn.RemoveAt(loop);
			return;
		}
	}

	ASSERT(FALSE);
}

void CHNetMember::AddOut( CHConnector* pConnector )
{
	ASSERT_VALID(this);
	ASSERT_VALID(pConnector);

	m_ConnectorsOut.Add(pConnector);
}

void CHNetMember::RemoveOut( CHConnector* pConnector )
{
	ASSERT_VALID(this);
	ASSERT_VALID(pConnector);

	for (INT_PTR loop=0; loop<m_ConnectorsOut.GetCount(); loop++)
	{
		if (m_ConnectorsOut[loop] == pConnector)
		{
			m_ConnectorsOut.RemoveAt(loop);
			return;
		}
	}

	ASSERT(FALSE);
}

void CHNetMember::RecalcArcs()
{
	for (INT_PTR loop=0; loop<m_ConnectorsIn.GetCount(); loop++)
	{
		m_ConnectorsIn[loop]->RecalcPoints();
	}

	
	for (INT_PTR loop=0; loop<m_ConnectorsOut.GetCount(); loop++)
	{
		m_ConnectorsOut[loop]->RecalcPoints();
	}
}

void CHNetMember::RemoveArcs()
{
	for (INT_PTR loop=0; loop<m_ConnectorsIn.GetCount(); loop++)
	{
		m_ConnectorsIn[loop]->To(0);
	}


	for (INT_PTR loop=0; loop<m_ConnectorsOut.GetCount(); loop++)
	{
		m_ConnectorsOut[loop]->From(0);
	}
}

void CHNetMember::RecalcArcPoint( const Point& edge, Point& point )
{
	point = m_Position;
}

Rect CHNetMember::GetArcBounds()
{
	Rect rect;

	for (INT_PTR loop=0; loop<m_ConnectorsIn.GetCount(); loop++)
	{
		UnionRect(rect, m_ConnectorsIn[loop]->GetArcBounds(this));
	}


	for (INT_PTR loop=0; loop<m_ConnectorsOut.GetCount(); loop++)
	{
		UnionRect(rect, m_ConnectorsOut[loop]->GetArcBounds(this));
	}

	return rect;

}

void CHNetMember::Id( ULONG32 id )
{
	m_id = id;

	for (INT_PTR loop=0; loop<m_ConnectorsIn.GetCount(); loop++)
	{
		m_ConnectorsIn[loop]->ToId(id);
	}


	for (INT_PTR loop=0; loop<m_ConnectorsOut.GetCount(); loop++)
	{
		m_ConnectorsOut[loop]->FromId(id);
	}

	LabelOwnerId(id);
}

