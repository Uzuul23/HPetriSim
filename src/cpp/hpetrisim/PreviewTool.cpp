/**************************************************************************
	PreviewTool.cpp

	copyright (c) 2013/07/16 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#include "StdAfx.h"
#include "PetriSimView.h"
#include "PreviewTool.h"

CPreviewTool::CPreviewTool(CPetriSimView * pWnd) : m_pView(pWnd)
{
	ASSERT_VALID(m_pView);
}

CPreviewTool::~CPreviewTool(void)
{
}

bool CPreviewTool::StorePreviewState( int Id )
{
	ASSERT_VALID(m_pView);

	CHDrawObjectList& list = m_pView->GetSelectTool().GetSelectedObjects();

	if (list.GetCount() == 0)
	{
		return false;
	}

	if (m_StorePreviewState.GetCount() > 0)
	{
		if(m_StorePreviewState[0][0].Id != Id)
		{
			Reset();
		}
		else
		{
			return false;
		}
	}

	m_StorePreviewState.SetSize(list.GetCount());

	for (INT_PTR loop=0; loop<m_StorePreviewState.GetCount(); loop++)
	{
		CHPropertyValues& vals = m_StorePreviewState[loop];

		vals.Add(CHPropertyValue(Id));

		switch (Id)
		{
		case ID_OBJECT_FILLCOLOR:
		case ID_OBJECT_FILLCOLOR2:
			vals.Add(CHPropertyValue(ID_OBJECT_FILLSTYLE));
			vals.Add(CHPropertyValue(ID_OBJECT_CLOSEFIGURE));
			break;
		case ID_OBJECT_FILLSTYLE:
			vals.Add(CHPropertyValue(ID_OBJECT_CLOSEFIGURE));
			break;
		case ID_OBJECT_LINESTYLE:
		case ID_OBJECT_LINEWEIGHT:
		case ID_OBJECT_LINECOLOR:
			vals.Add(CHPropertyValue(ID_OBJECT_NOBORDER));
			break;
		}
	}

	INT_PTR loop = 0;
	POSITION pos = list.GetHeadPosition();
	while (pos)
	{
		list.GetNext(pos)->GetProperties(m_StorePreviewState[loop]);
		loop++;
	}

	return true;
}

bool CPreviewTool::RestorePreviewState()
{
	ASSERT_VALID(m_pView);

	CHDrawObjectList& list = m_pView->GetSelectTool().GetSelectedObjects();

	if (list.GetCount() == 0)
	{
		return false;
	}

	if(list.GetCount() != m_StorePreviewState.GetCount())
	{
		return false;
	}

	Rect rect;

	INT_PTR loop = 0;
	POSITION pos = list.GetHeadPosition();
	while (pos)
	{
		CHDrawObject* pObject = list.GetNext(pos);
		ASSERT_VALID(pObject);

		UnionRect(rect, pObject->GetBounds(true));
		pObject->SetProperties(m_StorePreviewState[loop], false);
		UnionRect(rect, pObject->GetBounds(true));

		loop++;
	}

	m_pView->InvalidateLg(rect);

	return true;
}

bool CPreviewTool::SetPreview( CHPropertyValue& val )
{
	ASSERT_VALID(m_pView);

	CHDrawObjectList& list = m_pView->GetSelectTool().GetSelectedObjects();

	if (list.GetCount() == 0)
	{
		return false;
	}

	CHPropertyValues vals;
	vals.Add(val);
	
	Rect rect;
	POSITION pos = list.GetHeadPosition();
	while (pos)
	{
		CHDrawObject* pObject = list.GetNext(pos);
		ASSERT_VALID(pObject);

		UnionRect(rect, pObject->GetBounds(true));
		pObject->SetProperties(vals, true);
		UnionRect(rect, pObject->GetBounds(true));
	}

	m_pView->InvalidateLg(rect);

	return true;
}

void CPreviewTool::Reset()
{
	m_StorePreviewState.RemoveAll();
}