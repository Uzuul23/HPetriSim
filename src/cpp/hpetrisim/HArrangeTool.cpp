/**************************************************************************
	HArrangeTool.cpp

	copyright (c) 2013/09/08 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#include "StdAfx.h"
#include "PetriSimView.h"
#include "HDrawObject.h"
#include "HUpdateObject.h"
#include "HArrangeTool.h"

CHArrangeTool::CHArrangeTool(CPetriSimView* pView) : m_pView(pView)
{
	ASSERT_VALID(pView);
}

CHArrangeTool::~CHArrangeTool(void)
{
}

void CHArrangeTool::OnUpdateArrangeCommand( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(m_pView->GetSelectTool().IsSelected() && m_pView->GetDoc().DrawObjectList().GetCount() > 1);
}

void CHArrangeTool::OnArrangeCommand( UINT nID )
{
	if (!m_pView->GetSelectTool().IsSelected() || m_pView->GetDoc().DrawObjectList().GetCount() < 2)
	{
		return;
	}

	CHDrawObjectList& sellist = m_pView->GetSelectTool().GetSelectedObjects();
	CHDrawObjectList& objlist = m_pView->GetDoc().DrawObjectList();

	CMap<CHDrawObject*, CHDrawObject*, POSITION, POSITION&> map(objlist.GetCount());

	POSITION oldpos, pos = objlist.GetHeadPosition();
	while (pos)
	{
		oldpos = pos;
		CHDrawObject* pObject = objlist.GetNext(pos);

		map.SetAt(pObject, oldpos);
	}

	switch (nID)
	{
	case ID_ARRANGE_TOFRONT:
		{
			Rect rect;
			POSITION pos = sellist.GetHeadPosition();
			while (pos)
			{
				CHDrawObject* pObject = sellist.GetNext(pos);
				POSITION pos = 0;
				if (map.Lookup(pObject, pos))
				{
					objlist.RemoveAt(pos);
					objlist.AddTail(pObject);

					UnionRect(rect, pObject->GetBounds(true));
				}
			}

			m_pView->GetDoc().UpdateAllViews(0, CPetriSimDoc::UpdateInvalidate, &CHUpdateObject(rect));
		}
		break;
	case ID_ARRANGE_UP:
		{
			Rect rect;
			POSITION pos = sellist.GetHeadPosition();
			while (pos)
			{
				CHDrawObject* pObject = sellist.GetNext(pos);
				POSITION pos = 0;
				if (map.Lookup(pObject, pos))
				{
					POSITION prevpos = pos;
					objlist.GetNext(prevpos);
					objlist.RemoveAt(pos);
					objlist.InsertAfter(prevpos, pObject);

					UnionRect(rect, pObject->GetBounds(true));
				}
			}

			m_pView->GetDoc().UpdateAllViews(0, CPetriSimDoc::UpdateInvalidate, &CHUpdateObject(rect));
		}
		break;
	case ID_ARRANGE_DOWN:
		{
			Rect rect;
			POSITION pos = sellist.GetTailPosition();
			while (pos)
			{
				CHDrawObject* pObject = sellist.GetPrev(pos);
				POSITION pos = 0;
				if (map.Lookup(pObject, pos))
				{
					POSITION prevpos = pos;
					objlist.GetPrev(prevpos);
					objlist.RemoveAt(pos);
					objlist.InsertBefore(prevpos, pObject);

					UnionRect(rect, pObject->GetBounds(true));
				}
			}

			m_pView->GetDoc().UpdateAllViews(0, CPetriSimDoc::UpdateInvalidate, &CHUpdateObject(rect));
		}
		break;
	case ID_ARRANGE_TOBACK: 
		{
			Rect rect;
			POSITION pos = sellist.GetTailPosition();
			while (pos)
			{
				CHDrawObject* pObject = sellist.GetPrev(pos);
				POSITION pos = 0;
				if (map.Lookup(pObject, pos))
				{
					objlist.RemoveAt(pos);
					objlist.AddHead(pObject);

					UnionRect(rect, pObject->GetBounds(true));
				}
			}

			m_pView->GetDoc().UpdateAllViews(0, CPetriSimDoc::UpdateInvalidate, &CHUpdateObject(rect));
		}
		break;
	}
}