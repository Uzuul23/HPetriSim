/**************************************************************************
	HClipboardTool.cpp

	copyright (c) 2013/08/17 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#include "StdAfx.h"
#include "PetriSimView.h"
#include "SelectTool.h"
#include "HDrawObject.h"
#include "ArchiveHelper.h"
#include "HClipboardTool.h"

INT_PTR CHClipboardTool::m_Instances = 0;
UINT CHClipboardTool::m_nClipboardFormat = 0;
CPetriSimDoc* CHClipboardTool::m_pSourceDoc = 0;

CHClipboardTool::CHClipboardTool(CPetriSimView* pView) : m_pView(pView)
{
	if (m_Instances == 0)
	{
		m_nClipboardFormat = ::RegisterClipboardFormat(L"HPetriSim 1.0 Format");
	}
	m_Instances++;
}

CHClipboardTool::~CHClipboardTool(void)
{
}

BOOL CHClipboardTool::CanPaste()
{
	return !m_pView->GetDoc().SimOnline() && ::IsClipboardFormatAvailable(m_nClipboardFormat);
}

BOOL CHClipboardTool::CanCopyCut()
{
	return !m_pView->GetDoc().SimOnline() && m_pView->GetSelectTool().IsSelected();
}

void CHClipboardTool::Copy()
{
	if (m_pView->GetDoc().SimOnline())
	{
		return;
	}

	if (::OpenClipboard(0) == FALSE)
	{
		return;
	}

	if (!m_pView->IsSelected())
	{
		return;
	}

//	::EmptyClipboard();
//	::SetClipboardData(CF_TEXT, 0);

	CSharedFile	file;
	CArchive ar(&file, CArchive::store|CArchive::bNoFlushOnDelete);

	Serialize(ar);
	ar.Flush();

	::SetClipboardData(m_nClipboardFormat, file.Detach());
	::CloseClipboard();

	m_pSourceDoc = &m_pView->GetDoc();
}

void CHClipboardTool::Paste()
{
	if (m_pView->GetDoc().SimOnline())
	{
		return;
	}

	if (::OpenClipboard(0) == FALSE)
	{
		return;
	}

	HANDLE hData = ::GetClipboardData(m_nClipboardFormat);

	if (hData)
	{
		CSharedFile file;
		file.SetHandle(hData, FALSE);
		CArchive ar(&file, CArchive::load);

		Serialize(ar);

		ar.Close();
		file.Detach();
	}	
}

void CHClipboardTool::RenderFormat( UINT nFormat )
{

}

void CHClipboardTool::RenderAllFormats()
{

}

void CHClipboardTool::Serialize(CArchive& ar)
{
	CPetriSimDoc& doc = m_pView->GetDoc();
	CSelectTool& tool = m_pView->GetSelectTool();
	ar.m_pDocument = &doc;

	if(ar.IsStoring())
	{
		CHDrawObjectList& list = tool.GetSelectedObjects();

		CHDrawObjectList list2;
		CopyList(list2, list);

		doc.StrippNet(list2);

		GUID guid = GUID_NULL;
		VERIFY(SUCCEEDED(CoCreateGuid(&guid)));

		ar << guid;
		list2.Serialize(ar);
	}
	else
	{
		CHDrawObjectList list;

		GUID guid = GUID_NULL;
		ar >> guid;
		list.Serialize(ar);

		if (doc.ClipboardId() != guid)
		{
			doc.ClipboardId(guid);
			doc.PasteCount(m_pSourceDoc == &doc ? 1 : 0);
		}
		else
		{
			doc.PasteCount(doc.PasteCount()+1);
		}

		POSITION pos = list.GetHeadPosition();
		while (pos)
		{
			CHDrawObject* pObject = list.GetNext(pos);
			Rect rect = pObject->GetBounds();
			
			rect.Offset(20*doc.PasteCount(), 20*doc.PasteCount());
			if (rect.X + rect.Width > doc.GetDocSize().cx || rect.Y + rect.Height > doc.GetDocSize().cy)
			{
				doc.PasteCount(0);
				rect.X = rect.Y = 20;
			}
			pObject->SetBounds(rect);
		}

		doc.InitializeNet(list);
		doc.UniqueName(list);
		doc.AddObjects(list);
		tool.Select(list);
	}
}

