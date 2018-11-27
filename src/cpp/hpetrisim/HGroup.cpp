/**************************************************************************
	HGroup.cpp
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#include "stdafx.h"
#include "Resource.h"
#include "HGroup.h"

IMPLEMENT_SERIAL(CHGroup, CObject, DRAW_VERSION)
CHGroup::CHGroup()
{
//	m_crRect.SetRectEmpty();
}

CHGroup::~CHGroup()
{
	while (!m_colMembers.IsEmpty())
	{
		delete m_colMembers.RemoveHead();
	}
}

void CHGroup::Serialize(CArchive& ar)
{
	/*if (ar.IsStoring())
	{
		ar << (DWORD)m_crRect.left;
		ar << (DWORD)m_crRect.top;
		ar << (DWORD)m_crRect.right;
		ar << (DWORD)m_crRect.bottom;
	}
	else
	{
		DWORD dw;
		ar >> dw; m_crRect.left = dw;
		ar >> dw; m_crRect.top = dw;
		ar >> dw; m_crRect.right = dw;
		ar >> dw; m_crRect.bottom = dw;
	}
	m_colMembers.Serialize(ar);*/
}

#ifdef _DEBUG
void CHGroup::AssertValid() const
{
	CObject::AssertValid();
}

void CHGroup::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}
#endif //_DEBUG

void CHGroup::Add(CHDrawObject * object)
{
//	m_colMembers.AddTail(object);
//	m_crRect.UnionRect(m_crRect,object->GetBoundingRect());
	//m_crRect.NormalizeRect();
}

CHDrawObject* CHGroup::Remove()
{
	if(!m_colMembers.IsEmpty())
	{
		return m_colMembers.RemoveHead();
	}
	return NULL;
}

void CHGroup::DrawObject(CDC * pDC)
{
//	POSITION pos = m_colMembers.GetHeadPosition();
//	while (pos != NULL)
//	{
////		m_colMembers.GetNext(pos)->DrawObject(pDC);
//	}
//	if(m_blSelected)
//	{
//		for(int i = 1;i<9;i++)
//		{
//			pDC->Rectangle(GetHandle(i));
//		}
//	}
}

bool CHGroup::PtInObject(POINT & point)
{
	//if(GetBoundingRect(NULL).PtInRect(point))return true;
	return false;
}

void CHGroup::MoveTo(POINT & to)
{
	POSITION pos = m_colMembers.GetHeadPosition();
	while (pos != NULL)
	{
//		m_colMembers.GetNext(pos)->MoveTo(to);
	}
	//m_crRect.OffsetRect(to);
}

CRect CHGroup::GetBoundingRect(CDC* pDC,bool mode)
{
	CRect crect;
	/*crect.NormalizeRect();
	crect.InflateRect(SL_GRIPPER,SL_GRIPPER);*/
	return crect;
}

int CHGroup::PtInGripper(POINT & point)
{
//	ASSERT(m_blSelected);//Falscher Aufruf der Funktion!
	return NULL;
}

void CHGroup::Resize(POINT & to,int handle)
{
//	ASSERT(m_blSelected);//Falscher Aufruf der Funktion!
}

CRect CHGroup::GetHandle(int nHandle)
{
//	ASSERT_VALID(this);
//	int x, y, xCenter, yCenter;
//	xCenter = m_crRect.left + m_crRect.Width() / 2;
//	yCenter = m_crRect.top + m_crRect.Height() / 2;
//	switch (nHandle)
//	{
//	default:
//		ASSERT(FALSE);
//	case 1:
//		x = m_crRect.left;
//		y = m_crRect.top;
//		break;
//	case 2:
//		x = xCenter;
//		y = m_crRect.top;
//		break;
//	case 3:
//		x = m_crRect.right;
//		y = m_crRect.top;
//		break;
//	case 4:
//		x = m_crRect.right;
//		y = yCenter;
//		break;
//	case 5:
//		x = m_crRect.right;
//		y = m_crRect.bottom;
//		break;
//	case 6:
//		x = xCenter;
//		y = m_crRect.bottom;
//		break;
//	case 7:
//		x = m_crRect.left;
//		y = m_crRect.bottom;
//		break;
//	case 8:
//		x = m_crRect.left;
//		y = yCenter;
//		break;
//	case 9:
////		x = m_crRect.TopLeft().x + m_cpRound.x;
////		y = m_crRect.TopLeft().y + m_cpRound.y;
//		break;
//	}
	return CRect();
}

void CHGroup::InitMenu(CMenu * menu,CPoint & point)
{
}

void CHGroup::DoID(UINT nID)
{

}

LPCTSTR CHGroup::GetCursor(int handle)
{
	return NULL;
}
