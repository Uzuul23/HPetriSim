/**************************************************************************
	PetriSimDoc.cpp
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#include "stdafx.h"
#include "PetriSimView.h"
#include "MainFrm.h"
#include "HSimToolBar.h"
#include "IniSection.h"
#include "HErrorFile.h"
#include "PetriSim.h"
#include "HRect.h"
#include "HGroup.h"
#include "HLine.h"
#include "GDI+Helper.h"
#include "HUpdateObject.h"
#include "HConnector.h"
#include "HPosition.h"
#include "HTransition.h"
#include "HLabel.h"
#include "ArchiveHelper.h"
#include "StoreFillStyleGallery.h"
#include "PetriSimDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

/*************************************************************************/
// CPetriSimDoc
/*************************************************************************/

IMPLEMENT_DYNCREATE(CPetriSimDoc, CDocument)

BEGIN_MESSAGE_MAP(CPetriSimDoc, CDocument)

	ON_COMMAND(ID_ALING_GRID, OnAlignGrid)
	ON_UPDATE_COMMAND_UI(ID_ALING_GRID, OnUpdateAlignGrid)
	ON_COMMAND(ID_SHOW_GRID, OnShowGrid)
	ON_UPDATE_COMMAND_UI(ID_SHOW_GRID, OnUpdateShowGrid)
	ON_COMMAND(ID_GRID_SIZE, OnGridSize)
	ON_UPDATE_COMMAND_UI(ID_GRID_SIZE, OnUpdateGridSize)
	ON_COMMAND(ID_DOCUMENT_HEIGHT, OnDocumentHeight)
	ON_COMMAND(ID_DOCUMENT_WIDTH, OnDocumentWeidth)
	ON_COMMAND(ID_DOCUMENT_COLOR, OnDocumentColor)
	ON_COMMAND(ID_EDITOR_SHOWLABEL, OnShowLabel)
	ON_UPDATE_COMMAND_UI(ID_EDITOR_SHOWLABEL, OnUpdateShowLabel)
	ON_COMMAND(ID_SIM_SPEED, OnSimSpeed)
	ON_COMMAND_RANGE(ID_FIRST_SIM_COMMAND, ID_LAST_SIM_COMMAND, &CPetriSimDoc::OnSimCommand)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FIRST_SIM_COMMAND, ID_LAST_SIM_COMMAND, &CPetriSimDoc::OnUpdateSimCommand)
	
	END_MESSAGE_MAP()


CPetriSimDoc::CPetriSimDoc() : m_mapMembers(100)
, m_Sim(this)
, m_pOutputWnd(0)
, m_ClipboardId(GUID_NULL)
, m_PasteCount(0)
, m_ConvertVersion(false)
{
}

void CPetriSimDoc::DeleteContents() 
{
	//TODO:
	//StoreFillStyleGallery();

	m_Sim.Shutdown();

	UpdateAllViews(0, UpdateDeleteContent);

	POSITION pos = m_listObjects.GetHeadPosition();
	while (pos)
	{
		delete m_listObjects.GetNext(pos);
	}

	CHDrawObject* pKey = 0;
	CHDrawObject* pObject = 0;
	pos = m_mapStoredObjects.GetStartPosition();
	while (pos)
	{
		m_mapStoredObjects.GetNextAssoc(pos, pKey, pObject);
		ASSERT_VALID(pObject);

		delete pObject;
	}

	m_mapStoredObjects.RemoveAll();
	m_listObjects.RemoveAll();
	m_mapMembers.RemoveAll();

	m_NextId = 0;
	m_NextPositionIdent = 0;
	m_NextTransitionIdent = 0;
	m_NextConnectorIdent = 0;
	m_PaperColor = Color(Color::White);
	m_bShowLabel = true;
	m_sizeDoc = CSize(1000, 1500);
	m_Grid = 10;
	m_bShowGrid = true;
	m_bAlignNo = true;
	m_bAlign = true;
	m_nIDEvent = 0;
	
	UpdateUI();

	UpdateAllViews(0);

	__super::DeleteContents();
}

CPetriSimDoc::~CPetriSimDoc()
{
}

BOOL CPetriSimDoc::OnNewDocument()
{
	if (!__super::OnNewDocument())
	{
		return FALSE;
	}
	return TRUE;
}

void CPetriSimDoc::OnCloseDocument()
{
	return __super::OnCloseDocument();
}

BOOL CPetriSimDoc::OnSaveDocument( LPCTSTR lpszPathName )
{
	if (m_ConvertVersion)
	{
		if (AfxMessageBox(IDS_CONVERT_OLDDOCVER, MB_OKCANCEL) != IDOK)
		{
			return FALSE;
		}
	}

	if (__super::OnSaveDocument(lpszPathName))
	{
		m_ConvertVersion = false;
		return TRUE;
	}

	return FALSE;
}

void CPetriSimDoc::SetTitle( LPCTSTR lpszTitle )
{
	__super::SetTitle(lpszTitle);

	if (m_pOutputWnd)
	{
		CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

		if (pFrame)
		{
			pFrame->GetOutputWnd().Label(m_pOutputWnd, CString(lpszTitle));
		}
	}
}

void CPetriSimDoc::Serialize(CArchive& ar)
{
	
	if (ar.IsStoring())
	{
		ar.WriteCount(-1); //detect newer version
		ar.WriteCount(2); //actual document version

		m_listObjects.Serialize(ar);

		m_Sim.Serialize(ar);

		ar << m_sizeDoc;
		ar << m_NextId;
		ar << m_Grid;
		ar << m_bShowGrid;
		ar << m_bAlign;
		ar << m_bShowLabel;
		ar << m_PaperColor;
		ar << m_NextPositionIdent;
		ar << m_NextTransitionIdent;
		ar << m_NextConnectorIdent;
	}
	else
	{
			
		DWORD count = ar.ReadCount();

		if (count == -1)
		{
			m_ConvertVersion = false;

			DWORD version = ar.ReadCount();

			if (version == 2)
			{
				m_listObjects.Serialize(ar);

				m_Sim.Serialize(ar);

				ar >> m_sizeDoc;
				ar >> m_NextId;
				ar >> m_Grid;
				ar >> m_bShowGrid;
				ar >> m_bAlign;
				ar >> m_bShowLabel;
				ar >> m_PaperColor;
				ar >> m_NextPositionIdent;
				ar >> m_NextTransitionIdent;
				ar >> m_NextConnectorIdent;
			}

			InitializeNet(m_listObjects);

		}
		else //old version
		{
			m_ConvertVersion = true;

			DWORD dw;
			BYTE b;
			CObject* newData;
			while (count--)
			{
				ar >> newData;
				m_listObjects.AddTail((CHDrawObject*)newData);
			}
			ar >> dw; m_sizeDoc.cx = dw;
			ar >> dw; m_sizeDoc.cy = dw;
			ar >> dw; //m_NextConnectorIdent = dw - 2000000;
			ar >> dw; m_NextPositionIdent = dw - 1000000;
			ar >> dw; m_NextTransitionIdent = dw;
			ar >> dw;m_Grid = dw;
			ar >> dw;//m_iStepCount = dw;
			ar >> dw;//m_iStopTime = dw;
			ar >> dw;//m_iStopStep = dw;
			ar >> dw; m_Sim.SimSpeed(static_cast<long>(dw)); //m_iSimSpeed = dw;
			ar >> dw; m_Sim.SimTime(static_cast<long>(dw)); //m_iSimTime = dw;
			ar >> dw; m_Sim.SampleTime(static_cast<long>(dw)); //m_iSimSampleTime = dw;
			ar >> dw;m_PaperColor.SetFromCOLORREF(dw);
			ar >> dw;
			ar >> dw;
			ar >> dw;
			ar >> dw;
			ar >> dw;
			ar >> dw;
			ar >> dw;
			ar >> b;m_bShowLabel = (b != 0)? true : false;
			ar >> b;m_bShowGrid = (b != 0)? true : false;
			ar >> b;m_bAlignNo = (b != 0)? true : false;
			ar >> b;m_bAlign = (b != 0)? true : false;
			ar >> b;//m_bPopupMessage = (b != 0)? true : false;
			ar >> b;//m_bPopupExplorer = (b != 0)? true : false;
			ar >> b;
			ar >> b;
			ar >> b;
			ar >> b;

			InitializeNet(m_listObjects, true);

		}

		m_NextId = 0;
		m_mapMembers.RemoveAll();
		
		POSITION pos = m_listObjects.GetHeadPosition();
		while (pos)
		{
			POSITION oldpos = pos;
			CHDrawObject* pObject = m_listObjects.GetNext(pos);

			pObject->Id(++m_NextId);
			m_mapMembers.SetAt(pObject, oldpos);
		}

		UpdateUI();
	}
}

#ifdef _DEBUG

void CPetriSimDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPetriSimDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}

#endif //_DEBUG

BOOL CPetriSimDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return false;

	CString cstring(lpszPathName);
	int point = cstring.ReverseFind('.');
	m_csOutputFilePath = cstring.Left(point);
	m_csOutputFilePath += "_OUT.csv";

	return TRUE;
}

void CPetriSimDoc::ToGrid(CPoint& point)
{
	if (!m_bAlign)
	{
		return;
	}

	int r;
	if(point.x > 0)
	{
		r = point.x % m_Grid;
		if(r < m_Grid / 2)
			point.x -= r;
		if(r >= m_Grid / 2)
			point.x += (m_Grid - r);
	}
	else
	{
		r = -(point.x % m_Grid);
		if(r < m_Grid / 2)
			point.x += r;
		if(r >=	m_Grid / 2)
			point.x -= (m_Grid - r);
	}
	if(point.y > 0)
	{
		r = point.y % m_Grid;
		if(r < m_Grid / 2)
			point.y -= r;
		if(r >= m_Grid / 2)
			point.y += (m_Grid - r);
	}
	else
	{
		r = -(point.y % m_Grid);

		if(r < m_Grid / 2)
			point.y += r;
		if(r >= m_Grid / 2)
			point.y -= (m_Grid - r);
	}
}

bool CPetriSimDoc::InitializeNet(CHDrawObjectList & list, bool OldFormat /*= false*/)
{

	CMap<ULONG32, ULONG32, CHNetMember*, CHNetMember*&> netmap(100);
	CMap<ULONG32, ULONG32, CHLabelMember*, CHLabelMember*&> labelmap(100);

	POSITION pos = list.GetHeadPosition();
	while (pos)
	{
		CHDrawObject* pObject = list.GetNext(pos);
		ASSERT_VALID(pObject);

		CHNetMember* pNetMember = DYNAMIC_DOWNCAST(CHNetMember, pObject);

		if (pNetMember)
		{
			ASSERT(netmap.Lookup(pNetMember->Id(), pNetMember) == FALSE);
			netmap.SetAt(pNetMember->Id(), pNetMember);
		}

		CHLabelMember* pLabelMember = pObject->LabelMember();

		if (pLabelMember)
		{
			ASSERT(labelmap.Lookup(pLabelMember->Id(), pLabelMember) == FALSE);
			labelmap.SetAt(pLabelMember->Id(), pLabelMember);
		}

		if (OldFormat)
		{
			//name arcs
			CHConnector* pCon = DYNAMIC_DOWNCAST(CHConnector, pObject);

			if (pCon)
			{
				CString str;
				str.Format(L"A%d", NextConnectorName());
				pCon->Name(str);
			}
		}
	}
	
	if (OldFormat)
	{
		//transfer name
		POSITION pos = m_mapConvertOldLabel.GetStartPosition();
		while (pos)
		{
			ULONG32 id = 0;
			CString name;
			m_mapConvertOldLabel.GetNextAssoc(pos, id, name);

			CHNetMember* pObject = 0;
			if (netmap.Lookup(id, pObject))
			{
				pObject->Name(name);
			}
		}

		m_mapConvertOldLabel.RemoveAll();
	}

	CHDrawObjectList addlist;

	pos = list.GetHeadPosition();
	while (pos)
	{
		CHDrawObject* pObject = list.GetNext(pos);
		ASSERT_VALID(pObject);

		CHLabel* pLabel = DYNAMIC_DOWNCAST(CHLabel, pObject);
		if (pLabel)
		{
			CHLabelMember* pLabelMember = 0;

			ASSERT(labelmap.Lookup(pLabel->OwnerId(), pLabelMember));

			if (labelmap.Lookup(pLabel->OwnerId(), pLabelMember))
			{
				pLabel->Owner(pLabelMember);
			}
		}

		CHConnector* pConnector = DYNAMIC_DOWNCAST(CHConnector, pObject);
		if (pConnector)
		{
			CHNetMember* pNetMember = 0;
			if (netmap.Lookup(pConnector->ToId(), pNetMember))
			{
				pConnector->To(pNetMember);
			}
			if (netmap.Lookup(pConnector->FromId(), pNetMember))
			{
				pConnector->From(pNetMember);
			}

			if (OldFormat)
			{
				addlist.AddTail(new CHLabel(pConnector, 1, false));
				pConnector->RecalcLabelPosition();
			}
		}
	}

	AddObjects(addlist);

	return true;
}

void CPetriSimDoc::StrippNet(CHDrawObjectList& list)
{
	CHDrawObjectList addlist;

	POSITION pos = list.GetHeadPosition();
	while (pos)
	{
		POSITION posa = pos;
		CHDrawObject* pObject = list.GetNext(pos);

		CHLabel* pLabel = DYNAMIC_DOWNCAST(CHLabel, pObject);
		if (pLabel)
		{
			list.RemoveAt(posa);
			continue;
		}

		//TODO:
		CHConnector* pConnector = DYNAMIC_DOWNCAST(CHConnector, pObject);
		if (pConnector)
		{
		}

		CHLabelMember* pLabelMember = pObject->LabelMember();
		if (pLabelMember)
		{
			for (INT_PTR loop=0; loop<CHLabelMember::labelmax; loop++)
			{
				CHLabel* pLabel = pLabelMember->GetLabel(loop);
				
				if (pLabel)
				{
					addlist.AddTail(pLabel);
				}
			}
		}	
	}
	
	pos = addlist.GetHeadPosition();
	while (pos)
	{
		list.AddTail(addlist.GetNext(pos));
	}
}

void CPetriSimDoc::PushBack()
{
	/*POSITION pos = m_listSelected.GetHeadPosition();
	while (pos != NULL)
	{
		object = m_listSelected.GetNext(pos);
		m_listObjects.RemoveAt(m_listObjects.Find(object));
		m_listObjects.AddHead(object);
		UpdateAllViews(NULL,0L,object);
	}*/
	SetModifiedFlag(true);
}

void CPetriSimDoc::ToGrid()
{
//	POSITION pos = m_listSelected.GetHeadPosition();
//	while (pos)
//	{
//		CHDrawObject* pObject = m_listSelected.GetNext(pos);
//		UpdateAllViews(NULL,0L,pObject);
////		pObject->ToGrid(false);
//		UpdateAllViews(NULL,0L,pObject);
//	}
	SetModifiedFlag(true);
}

//void CPetriSimDoc::UpdateOtherViews(CView * pView)
//{
////	UpdateAllViews(pView, 1L, &m_UpdateRect);
////	m_UpdateRect.m_cRect.SetRect(0,0,0,0);
//}

void CPetriSimDoc::OnUpdateIndicatorsStepTime(CCmdUI* pCmdUI) 
{
	/*pCmdUI->Enable(true);
	if(m_bSimMode)
	{
		CString strPage;
		strPage.Format(_T("S. %d/%dms"), m_iStepTime, m_iCalcTime);
		pCmdUI->SetText( strPage ); 
	}
	else pCmdUI->SetText(_T("Step/Calc Time"));*/
}

void CPetriSimDoc::OnUpdateIndicatorsStepCount(CCmdUI* pCmdUI) 
{
	/*pCmdUI->Enable(true);
	CString strPage;
	strPage.Format(_T("SC. %d"), m_iStepCount);
	pCmdUI->SetText( strPage );*/
}

void CPetriSimDoc::OnUpdateIndicatorsCount(CCmdUI* pCmdUI) 
{	
	/*pCmdUI->Enable(true);
	CString strPage;
	strPage.Format(_T("C.%d/%d"), m_listObjects.GetCount(), 
		m_listSelected.GetCount());
	pCmdUI->SetText(strPage);*/
}

void CPetriSimDoc::OnUpdateSliderSimspeed(CCmdUI* pCmdUI) 
{	
	/*if(m_bSimMode)
		pCmdUI->Enable(true);
	else 
		pCmdUI->Enable(false);*/
}

void CPetriSimDoc::OnUpdateIndicatorsSimTime(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(true);
	
	CString strPage;
//	strPage.Format( _T("T. %dms"), m_iSimTime);
	pCmdUI->SetText( strPage ); 
}

void CPetriSimDoc::OnUpdateIndicatorsSimStatus(CCmdUI* pCmdUI) 
{
	/*pCmdUI->Enable(true);
	if(m_pSimThread)
	{
		if(m_bSingleStep)
			pCmdUI->SetText(_T("Step..."));
		else if (m_bRunNormal)
			pCmdUI->SetText(_T("Running..."));
		else if (m_bRunFast)
			pCmdUI->SetText(_T("Fast..."));
		else
			pCmdUI->SetText(_T("Online"));
	}
	else
		pCmdUI->SetText(_T("Offline"));*/
}

void CPetriSimDoc::OnUpdateUpdatePropertys(CCmdUI* pCmdUI) 
{
//	if(!m_listSelected.IsEmpty())pCmdUI->Enable(true);
//	else pCmdUI->Enable(false);
}

void CPetriSimDoc::OnUpdatePropertys() 
{
	/*CHDrawObject* ptrObject = m_listSelected.GetHead();
	ptrObject->Propertys();
	UpdateAllViews(NULL,0L,ptrObject);*/
	
}

void CPetriSimDoc::OnEditorShowlabel() 
{
	m_bShowLabel = !m_bShowLabel;
	UpdateAllViews(0);
}

void CPetriSimDoc::OnUpdateEditorShowlabel(CCmdUI* pCmdUI) 
{
	if(m_bShowLabel)pCmdUI->SetCheck(true);
	else pCmdUI->SetCheck(false);		
}

void CPetriSimDoc::OnChangedViewList() 
{
	/*
	if(m_pSimThread)
	{
		POSITION pos = GetFirstViewPosition();
		while (pos)
		{
			CView* pView = GetNextView(pos);
			if(pView->IsKindOf(RUNTIME_CLASS(CHSymStateFormView)))
			{
				m_pSimThread->SetOutputWindow(pView->GetSafeHwnd());
			}
		} 
	}
	*/
	CDocument::OnChangedViewList();
}

void CPetriSimDoc::OnUpdateEnableWriteoutputfile(CCmdUI* pCmdUI) 
{
	/*if(!m_bRunFast && !m_bSingleStep && !m_bRunNormal && m_bSimMode)
		pCmdUI->Enable(true);
	else 
		pCmdUI->Enable(false);
	if(m_bEnableOutputFile)
		pCmdUI->SetCheck(true);
	else
		pCmdUI->SetCheck(false);*/
}

void CPetriSimDoc::OnUpdateEditorProperties(CCmdUI* pCmdUI) 
{
	/*if(!m_bSimMode)
		pCmdUI->Enable(true);
	else 
		pCmdUI->Enable(false);*/
}

void CPetriSimDoc::OnFileExportNet() 
{
//	CFileDialog dlg(false, 0, 0, OFN_HIDEREADONLY,
//		_T("Text Files (*.txt)|*.txt|All Files|*.*||"));
//	if(dlg.DoModal()==IDOK)
//	{
//		CString path(dlg.GetPathName());
//		if(dlg.GetFileExt().IsEmpty())
//			path += ".txt";
//
//		CStdioFile cfile(path, CFile::modeCreate|CFile::modeWrite);
//
//		CWaitCursor wait;
//
//		TCHAR buffer[80];
//		CTypedPtrList<CPtrList,CHTransition*> TranList;
//		CTypedPtrList<CPtrList,CHPosition*> PosiList;
//		//aus der gesamten Liste die Transitionen
//		//und Plätze aussortieren
//		POSITION pos = m_listObjects.GetHeadPosition();
//		while (pos)
//		{
//			CHDrawObject* pObject = m_listObjects.GetNext(pos);
//			if(pObject->IsKindOf( RUNTIME_CLASS(CHTransition)))
//			{
//				TranList.
//					AddTail(static_cast <CHTransition*> (pObject));
//			}
//			if(pObject->IsKindOf( RUNTIME_CLASS(CHPosition)))
//			{
//				PosiList.
//					AddTail(static_cast <CHPosition*> (pObject));
//			}
//		}
//
//	try{
//			//Transition List
//			cfile.WriteString(_T("// Transition Name Vector:\n"));
//			pos = TranList.GetHeadPosition();
//			while (pos)
//			{
//				CHTransition* pTran = TranList.GetNext(pos);
////				cfile.WriteString(pTran->GetLabelA()->GetText());
//				cfile.WriteString(_T(" ;"));
//			}
//			cfile.WriteString(_T(")\n"));
//			//Position List
//			cfile.WriteString(_T("// Position Name Vector:\n(_T("));
//			pos = PosiList.GetHeadPosition();
//			while (pos)
//			{
//				CHPosition* pPosi = PosiList.GetNext(pos);
////				cfile.WriteString(pPosi->GetLabelA()->GetText());
//				cfile.WriteString(_T(";"));
//			}
//			cfile.WriteString(_T(")\n"));
//			//Inzidenzmatrix
//			int maxwight = 0;
//			POSITION pos = m_listObjects.GetHeadPosition();
//			while (pos)
//			{
//				CHDrawObject* pObject = m_listObjects.GetNext(pos);
//				if(pObject->IsKindOf( RUNTIME_CLASS(CHConnector)))
//				{
//					CHConnector* pConn = static_cast <CHConnector*> (pObject);
//					maxwight = max(maxwight, (int)pConn->GetWeight());
//				}
//			}
//			//Erzeugt aus der Liste die entsprechende Inzidenz 
//			//Matrix.
//			//Diese Form könnte für weitere Berechnungen
//			//nützlich sein
//			cfile.WriteString(_T("// Inzidenz Matrix:\n{\n"));
//			pos = PosiList.GetHeadPosition();
//			while (pos)
//			{
//				cfile.WriteString(_T("(_T("));
//				CHPosition* pPosi = PosiList.GetNext(pos);
//				POSITION posA = TranList.GetHeadPosition();
//				while (posA)
//				{
//					long connection = 0;
//					CHTransition* pTran = TranList.GetNext(posA);
//					CONN_LIST& rInList = pTran->GetInList();		
//					POSITION posB = rInList.GetHeadPosition();
//					while (posB)
//					{
//						CHConnector* pConn = rInList.GetNext(posB);
//						CHPosition* pPosiA = static_cast <CHPosition*> 
//							(pConn->GetPtrFrom());
//						if((pConn->GetTyp() == CT_NORMAL) &&
//							(pPosiA == pPosi))
//						{
//							connection += pConn->GetWeight();
//							//connection = -connection;
//						}
//					}
//					CONN_LIST& rOutList = pTran->GetOutList();		
//					posB = rOutList.GetHeadPosition();
//					while (posB)
//					{
//						CHConnector* pConn = rOutList.GetNext(posB);
//						CHPosition* pPosiA = static_cast<CHPosition*> 
//							(pConn->GetPtrTo());
//						if((pConn->GetTyp() == CT_NORMAL) &&
//							(pPosiA == pPosi))
//						{
//							connection = pConn->GetWeight();
//							connection = -connection;
//						}
//					}
//					TCHAR format[10];
//					StringCbPrintf(format, sizeof(format)
//						, _T("%%%dd "), maxwight + 1);
//					StringCbPrintf(buffer, sizeof(buffer)
//						, format, connection);
//					cfile.WriteString(buffer);
//				}
//				cfile.WriteString(_T(")\n"));
//			}
//			cfile.WriteString(_T("}\n"));
//			//Marking Vector
//			cfile.WriteString(_T("// Marking Vector:\n(_T("));
//			pos = PosiList.GetHeadPosition();
//			while (pos)
//			{
//				CHPosition* pPosi = PosiList.GetNext(pos);
//				StringCbPrintf(buffer, sizeof(buffer), _T("%d ")
//					, pPosi->GetToken());
//
//				cfile.WriteString(buffer);
//			}
//			cfile.WriteString(_T(")\n"));
//			//Arc Type Matrix
//			cfile.WriteString(_T("// Arc Type Matrix:\n"));
//			cfile.WriteString(_T("// Code:0 = None; 1 = Normal; 2 = Inhibitor; 3 = Test \n{\n"));
//			pos = PosiList.GetHeadPosition();
//			while (pos)
//			{
//				cfile.WriteString(_T("(_T("));
//				CHPosition* pPosi = PosiList.GetNext(pos);
//				POSITION posA = TranList.GetHeadPosition();
//				while (posA)
//				{
//					CT_TYP connectortyp = CT_NONE;
//					CHTransition* pTran = TranList.GetNext(posA);
//					CONN_LIST& rInList = pTran->GetInList();		
//					POSITION posB = rInList.GetHeadPosition();
//					while (posB)
//					{
//						CHConnector* pConn = rInList.GetNext(posB);
//						CHPosition* pPosiA = static_cast <CHPosition*> 
//							(pConn->GetPtrFrom());
//						if(pPosiA == pPosi)
//						{
//							connectortyp = pConn->GetTyp();
//						}
//					}
//					CONN_LIST& rOutList = pTran->GetOutList();		
//					posB = rOutList.GetHeadPosition();
//					while (posB)
//					{
//						CHConnector* pConn = rOutList.GetNext(posB);
//						CHPosition* pPosiA = static_cast <CHPosition*> 
//							(pConn->GetPtrTo());
//						if(pPosiA == pPosi)
//						{
//							connectortyp = pConn->GetTyp();
//						}
//					}
//					switch(connectortyp)
//					{
//					case CT_NONE:
//						cfile.WriteString(_T("0 "));
//						break;
//					case CT_NORMAL:
//						cfile.WriteString(_T("1 "));
//						break;
//					case CT_INHIBITOR:
//						cfile.WriteString(_T("2 "));
//						break;
//					case CT_TEST:
//						cfile.WriteString(_T("3 "));
//						break;
//					}
//				}
//				cfile.WriteString(_T(")\n"));
//			}
//			cfile.WriteString(_T("}\n"));
//			//Transition Time Model Vektor
//			cfile.WriteString(_T("// Transition Time Model Vektor:\n"));
//			cfile.WriteString(_T("// Code:1 = Immidiate; 2= Delay;3 = Exponential; 4 = Equal Distibution;\n(_T("));
//			pos = TranList.GetHeadPosition();
//			while (pos)
//			{
//				CHTransition* pTran = TranList.GetNext(pos);
//				switch(pTran->GetTimeMode())
//				{
//				case CHTransition::TTM_IMMIDIATE:
//					cfile.WriteString(_T("1 "));
//					break;
//				case CHTransition::TTM_DELAY:
//					cfile.WriteString(_T("2 "));
//					break;
//				case CHTransition::TTM_EXPONENTIAL:
//					cfile.WriteString(_T("3 "));
//					break;
//				case CHTransition::TTM_EQUAL_DISTR:
//					cfile.WriteString(_T("4 "));
//					break;
//				}
//				cfile.WriteString(_T(";"));
//			}
//
//		}catch(CFileException e)
//		{
//			return;
//		}
//	}	
}

void CPetriSimDoc::OnFileExportInterchange() 
{

	/*CFileDialog dlg(false, 0, 0, OFN_HIDEREADONLY,
		_T("HPSim Export Files (*.hpx)|*.hpx|All Files|*.*||"));

	if(dlg.DoModal()==IDOK)
	{
		CString path(dlg.GetPathName());
		if(dlg.GetFileExt().IsEmpty())
			path += ".hpx";
		CIniSection cini;
		cini.OpenFile(path, true);
		CWaitCursor wait;
		cini.SetSection(_T("DOCUMENT"));
		cini.SetValue(_T("APPLICATION"), "HPSim");
		cini.SetValue(_T("VERSION"), "0.9");
		cini.SetValue(_T("LAST_CONN_ID"), m_NextConnectorIdent);
		cini.SetValue(_T("LAST_POSI_ID"), m_NextPositionIdent);
		cini.SetValue(_T("LAST_TRAN_ID"), m_NextTransitionIdent);
		cini.SetValue(_T("DOC_SIZE"), m_sizeDoc);
		cini.WriteSection();
		CString buffer;
		int isectioncount = 0;
		POSITION pos = m_listObjects.GetHeadPosition();
		CProgressBar bar(_T("Write File..."), 70, m_listObjects.GetCount());
		bar.SetStepRate(m_listObjects.GetCount() / 100);
		while (pos)
		{
			CHDrawObject* pObject = m_listObjects.GetNext(pos);
			isectioncount++;
			buffer.Format(_T("OBJECT %d"), isectioncount);
			cini.SetSection(buffer);
			pObject->WriteExportFormat(cini);
			cini.WriteSection();
			bar.StepIt();
		}
	}	*/
}

void CPetriSimDoc::OnFileImportInterchange() 
{
	/*CFileDialog dlg(true, 0, 0, OFN_HIDEREADONLY,
		_T("HPSim Export Files (*.hpx)|*.hpx|All Files|*.*||"));

	if((dlg.DoModal()==IDOK) &&
		(AfxMessageBox(_T("All data in this Document will be erased!")
		, MB_OKCANCEL) == IDOK))
	{
		CString path(dlg.GetPathName());
		if(dlg.GetFileExt().IsEmpty())
			path += _T(".hpx");
		CIniSection ini;
		if(!ini.OpenFile(path))
		{
			AfxMessageBox(IDS_ERROR_FILE_OPEN);
			return;
		}
		CWaitCursor wait;
		CString classname;
		ini.FindSection(_T("DOCUMENT"));
		ini.GetValue(_T("DOC_SIZE"), m_sizeDoc);
		ini.GetValue(_T("APPLICATION"), classname);
		if(classname != "HPSim")
		{
			CHErrorFile::WriteError(_T("OnFileImportInterchange")
				, _T("Unknown App Name"), this);

			AfxMessageBox(_T("Error: Unknown App Name"));
			return;
		}
		ini.GetValue(_T("VERSION"), classname);
		if(classname != "0.9")
		{
			CHErrorFile::WriteError(_T("OnFileImportInterchange"),
				_T("Can not import this Version"), this);
			AfxMessageBox(_T("Error: Can not import this Version"));
			return;
		}
		DeleteContents();
		ini.GetValue(_T("LAST_CONN_ID"), m_NextConnectorIdent);
		ini.GetValue(_T("LAST_POSI_ID"), m_NextPositionIdent);
		ini.GetValue(_T("LAST_TRAN_ID"), m_NextTransitionIdent);
		CHDrawObject* pObject = 0;

		CProgressBar* pbar = new CProgressBar(_T("Read File..."), 70,
			static_cast<int>(ini.GetFile().GetLength()));

		pbar->SetStepRate(static_cast<int>(ini.GetFile().GetLength()) / 100);
		while(ini.ReadNextSection())
		{
			if(ini.GetSection().Find(_T("OBJECT")) == -1)
				continue;
			ini.GetValue(_T("RUNTIME_CLASS"), classname);
			if(classname == "CHRect")
				pObject = new CHRect(CPoint(0,0));
			else if(classname == "CHText")
				pObject = new CHText(CPoint(0,0), &m_lfDefaultFont);
			else if(classname == "CHLabel")
				pObject = new CHLabel();
			else if(classname == "CHPoly")
				pObject = new CHPoly();
			else if(classname == "CHLine")
				pObject = new CHLine();
			else if(classname == "CHConnector")
				pObject = new CHConnector();
			else if(classname == "CHPosition")
				pObject = new CHPosition();
			else if(classname == "CHTransition")
				pObject = new CHTransition();
			if(pObject)
			{
				pObject->ReadExportFormat(ini);
				m_listObjects.AddTail(pObject);
			}
			else
			{
				CHErrorFile::WriteError(_T("OnFileImportInterchange")
					, _T("Memory"), this);
				AfxMessageBox(IDS_ERROR_MEMORY);
				DeleteContents();
				UpdateAllViews(NULL,3L,NULL);
				return;
			}
			pbar->SetPos(static_cast<int>(ini.GetFile().GetPosition()));
		}
		delete pbar;
		if(!InitializeNet(m_listObjects))
		{
			DeleteContents();
		}
		InitMap(m_listObjects);
		UpdateAllViews(NULL,3L,NULL);
		PeakAndPump();
	}*/
}
void CPetriSimDoc::OnUpdateShowLabel( CCmdUI* pCmdUI )
{
	pCmdUI->SetCheck(m_bShowLabel ? 1 : 0);
}

void CPetriSimDoc::OnShowLabel()
{
	m_bShowLabel = !m_bShowLabel;
	UpdateAllViews(0);
}

void CPetriSimDoc::OnAlignGrid() 
{
	m_bAlign = !m_bAlign;
}

void CPetriSimDoc::OnUpdateAlignGrid(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bAlign);
}

void CPetriSimDoc::OnShowGrid() 
{
	m_bShowGrid = !m_bShowGrid;
	UpdateAllViews(0);
}

void CPetriSimDoc::OnUpdateShowGrid(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowGrid);
}

void CPetriSimDoc::OnGridSize()
{
	CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	ASSERT_VALID(pFrame);

	CHPropertyValues vals;
	vals.Add(CHPropertyValue(static_cast<int>(ID_GRID_SIZE)));

	pFrame->GetValueFromRibbon(vals[0]);

	long grid = vals[0];
	grid = max(min(grid, 50), 5);
	vals[0] = grid;

	m_Grid = grid;
	pFrame->SetValuesToRibbon(vals);

	UpdateAllViews(0);
}

void CPetriSimDoc::OnDocumentHeight()
{
	CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	ASSERT_VALID(pFrame);

	CHPropertyValues vals;
	vals.Add(CHPropertyValue(static_cast<int>(ID_DOCUMENT_HEIGHT)));

	pFrame->GetValueFromRibbon(vals[0]);

	long l = vals[0];
	l = max(min(l, 5000), 500);
	vals[0] = l;
	m_sizeDoc.cy = l;

	pFrame->SetValuesToRibbon(vals);
	UpdateAllViews(0, UpdateDocSize);
}

void CPetriSimDoc::OnDocumentWeidth()
{
	CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	ASSERT_VALID(pFrame);

	CHPropertyValues vals;
	vals.Add(CHPropertyValue(static_cast<int>(ID_DOCUMENT_WIDTH)));

	pFrame->GetValueFromRibbon(vals[0]);

	long l = vals[0];
	l = max(min(l, 5000), 500);
	vals[0] = l;
	m_sizeDoc.cx = l;

	pFrame->SetValuesToRibbon(vals);
	UpdateAllViews(0, UpdateDocSize);
}

void CPetriSimDoc::OnDocumentColor()
{
	CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	ASSERT_VALID(pFrame);

	CHPropertyValues vals;
	vals.Add(CHPropertyValue(static_cast<int>(ID_DOCUMENT_COLOR)));

	pFrame->GetValueFromRibbon(vals[0]);
	m_PaperColor = vals[0];
	
	UpdateAllViews(0);
}

void CPetriSimDoc::OnUpdateGridSize( CCmdUI* pCmdUI )
{

}

void CPetriSimDoc::OnUpdateSimCommand( CCmdUI* pCmdUI )
{
	m_Sim.OnUpdateSimCommand(pCmdUI);
}

void CPetriSimDoc::OnSimCommand( UINT nID )
{
	m_Sim.OnSimCommand(nID);
}

void CPetriSimDoc::OnSimSpeed()
{
	CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	if (pFrame)
	{
		CHPropertyValue val(ID_SIM_SPEED);
		pFrame->GetValueFromRibbon(val);

		m_Sim.SimSpeed(val);
	}
}

void CPetriSimDoc::SimThreadMessage(WPARAM wParam, LPARAM lParam)
{
	m_Sim.OnSimThreadMessage(wParam, lParam);
}

void CPetriSimDoc::AddObjects(const CHDrawObjectList& list, CView* pView/* = 0*/)
{
	if (list.GetCount() == 0)
	{
		return;
	}

	POSITION pos = list.GetHeadPosition();
	while (pos)
	{
		CHDrawObject* pObject = list.GetNext(pos);

		pObject->Id(++m_NextId);
	
		POSITION newpos = m_listObjects.AddTail(pObject);
		m_mapMembers.SetAt(pObject, newpos);
	}

	SetModifiedFlag(true);
	UpdateAllViews(pView);
}

void CPetriSimDoc::UniqueName(const CHDrawObjectList& list)
{
	CString str;
	POSITION pos = list.GetHeadPosition();
	while (pos)
	{
		CHDrawObject* pObject = list.GetNext(pos);

		CHPosition* pPosition = dynamic_cast<CHPosition*>(pObject);

		if (pPosition)
		{
			str.Format(L"P%d", NextPositionName());
			pPosition->Name(str);
		}

		CHTransition* pTransition = dynamic_cast<CHTransition*>(pObject);

		if (pTransition)
		{
			str.Format(L"T%d", NextTransitionName());
			pTransition->Name(str);
		}

		CHConnector* pConnector = dynamic_cast<CHConnector*>(pObject);

		if (pConnector)
		{
			str.Format(L"A%d", NextConnectorName());
			pConnector->Name(str);
		}
	}
}

void CPetriSimDoc::RemoveObjects( const CHDrawObjectList& list )
{
	if (list.GetCount() == 0)
	{
		return;
	}

	Rect rect;

	POSITION pos = list.GetHeadPosition();
	while(pos)
	{
		CHDrawObject* pObject = list.GetNext(pos);

		UnionRect(rect, pObject->GetBounds(true));

		CHNetMember* pNetMember = DYNAMIC_DOWNCAST(CHNetMember, pObject);

		if (pNetMember)
		{
			pNetMember->RemoveArcs();
		}

		CHConnector* pConnector = DYNAMIC_DOWNCAST(CHConnector, pObject);

		if (pConnector)
		{
			pConnector->From(0);
			pConnector->To(0);
		}

		POSITION pos2 = 0;
		ASSERT(m_mapMembers.Lookup(pObject, pos2) == TRUE);

		if (m_mapMembers.Lookup(pObject, pos2))
		{
			m_mapMembers.RemoveKey(pObject);
			m_listObjects.RemoveAt(pos2);
		}

		CHDrawObject* key = pObject;
		ASSERT(m_mapStoredObjects.Lookup(key, pObject) == FALSE);

		m_mapStoredObjects.SetAt(key, pObject);
	}

	SetModifiedFlag(true);
	UpdateAllViews(0, UpdateInvalidate, &CHUpdateObject(rect));
}

ULONG32 CPetriSimDoc::GetNextId()
{
	return ++m_NextId;
}

void CPetriSimDoc::UpdateUI()
{
	CMainFrame* pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());

	if (pFrame == 0)
	{
		return;
	}

	CHPropertyValues vals;

	m_Sim.AddUpateUI(vals);

	vals.Add(CHPropertyValue(m_Grid, ID_GRID_SIZE));
	vals.Add(CHPropertyValue(m_sizeDoc.cx, ID_DOCUMENT_WIDTH));
	vals.Add(CHPropertyValue(m_sizeDoc.cy, ID_DOCUMENT_HEIGHT));
	vals.Add(CHPropertyValue(m_PaperColor, ID_DOCUMENT_COLOR));
	vals.Add(CHPropertyValue(m_Sim.SimSpeed(), ID_SIM_SPEED));

	pFrame->SetValuesToRibbon(vals);
}

void CPetriSimDoc::AddToConvertOldLabelMap( ULONG32 id, CString & name )
{
	m_mapConvertOldLabel.SetAt(id, name);
}

CHDrawObject* CPetriSimDoc::ObjectHit( const CPoint& point )
{
	POSITION pos = m_listObjects.GetTailPosition();
	while (pos)
	{
		CHDrawObject* pObject = m_listObjects.GetPrev(pos);

		if (pObject->HitTest(point))
		{
			return pObject;
		}
	}
	return 0;
}

CHNetMember* CPetriSimDoc::NetMemberHit( const CPoint& point )
{
	POSITION pos = m_listObjects.GetTailPosition();
	while (pos)
	{
		CHDrawObject* pObject = m_listObjects.GetPrev(pos);

		if (pObject->HitTest(point))
		{
			CHNetMember* pNetMember = dynamic_cast<CHNetMember*>(pObject);

			if (pNetMember)
			{
				return pNetMember;
			}
		}
	}
	return 0;
}

bool CPetriSimDoc::CanConnect( CHConnector* pConnector, CHNetMember* pNetMember , int Handle )
{
	if (pConnector == 0 || pNetMember == 0)
	{
		return false;
	}

	if (pConnector->ToHandle() == Handle)
	{
		if (pConnector->To() == pNetMember)
		{
			return true;
		}

		if (pConnector->From() == 0)
		{
			return true;
		}

		Connectors& cons = pNetMember->ConnectorsIn();

		for (INT_PTR loop=0; loop<cons.GetCount(); ++loop)
		{
			if (cons[loop]->From() == pConnector->From())
			{
				return false;
			}
		}

		if (dynamic_cast<CHPosition*>(pNetMember))
		{
			return (dynamic_cast<CHTransition*>(pConnector->From())) ? true : false;
		}

		if (dynamic_cast<CHTransition*>(pNetMember))
		{
			return (dynamic_cast<CHPosition*>(pConnector->From())) ? true : false;
		}
	}

	if (pConnector->FromHandle() == Handle)
	{
		if (pConnector->From() == pNetMember)
		{
			return true;
		}

		if (pConnector->To() == 0)
		{
			return true;
		}

		Connectors& cons = pNetMember->ConnectorsOut();

		for (INT_PTR loop=0; loop<cons.GetCount(); ++loop)
		{
			if (cons[loop]->To() == pConnector->To())
			{
				return false;
			}
		}

		if (dynamic_cast<CHPosition*>(pNetMember))
		{
			return (dynamic_cast<CHTransition*>(pConnector->To())) ? true : false;
		}

		if (dynamic_cast<CHTransition*>(pNetMember))
		{
			return (dynamic_cast<CHPosition*>(pConnector->To())) ? true : false;
		}
	}

	return true;
}

bool CPetriSimDoc::SimOnline()
{
	return m_Sim.IsOnline();
}

void CPetriSimDoc::Subscribe( CHEditorEvent* p )
{

#ifdef _DEBUG
	for (INT_PTR loop=0; loop<m_EventReceiver.GetCount(); loop++)
	{
		if (m_EventReceiver[loop] == p)
		{
			ASSERT(FALSE);
		}
	}
#endif

	m_EventReceiver.Add(p);
}

void CPetriSimDoc::UnSubscribe( CHEditorEvent* p )
{
	for (INT_PTR loop=0; loop<m_EventReceiver.GetCount(); loop++)
	{
		if (m_EventReceiver[loop] == p)
		{
			m_EventReceiver.RemoveAt(loop);
			return;
		}
	}

	ASSERT(FALSE);
}

void CPetriSimDoc::OnEditorEvent( CHEditorEvent::EditorEvent event )
{
	for (INT_PTR loop=0; loop<m_EventReceiver.GetCount(); loop++)
	{
		m_EventReceiver[loop]->OnEditorEvent(event);
	}
}