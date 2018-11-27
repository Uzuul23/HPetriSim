/**************************************************************************
	PetriSimView.cpp
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#include "stdafx.h"
#include "HDrawObject.h"
#include "PetriSim.h"
#include "PetriSimDoc.h"
#include "HSimToolBar.h"
#include "MainFrm.h"
#include "HGroup.h"
#include "GDI+Helper.h"
#include "HUpdateObject.h"
#include "HPropertyValue.h"
#include "HDrawInfo.h"
#include "PetriSimView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CPetriSimView, CZoomView)

BEGIN_MESSAGE_MAP(CPetriSimView, CZoomView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_SETCURSOR()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDBLCLK()
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CPetriSimView::OnUpdateEditCopyCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CPetriSimView::OnUpdateEditCopyCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CPetriSimView::OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_COPY, &CPetriSimView::OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, &CPetriSimView::OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, &CPetriSimView::OnEditPaste)
	ON_COMMAND(ID_CONTEXT_POPUP, &CPetriSimView::OnContextPopup)
	ON_COMMAND(ID_CONTEXT_PUSHBACK, &CPetriSimView::OnContextPushback)
	ON_COMMAND(ID_GROUP, &CPetriSimView::OnGroup)
	ON_COMMAND(ID_UNGROUP, &CPetriSimView::OnUngroup)
	ON_UPDATE_COMMAND_UI(ID_CONTEXT_POPUP, &CPetriSimView::OnUpdateContextPopup)
	ON_UPDATE_COMMAND_UI(ID_CONTEXT_PUSHBACK, &CPetriSimView::OnUpdateContextPushback)
	ON_UPDATE_COMMAND_UI(ID_GROUP, &CPetriSimView::OnUpdateGroup)
	ON_UPDATE_COMMAND_UI(ID_UNGROUP, &CPetriSimView::OnUpdateUngroup)
	ON_COMMAND(ID_STORE_SCREEN, &CPetriSimView::OnStoreScreen)
	ON_COMMAND(ID_STORE_CLIENT, &CPetriSimView::OnStoreClient)
	ON_COMMAND(ID_FILE_PRINT, &CPetriSimView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CPetriSimView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CPetriSimView::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_MOUSEPOS, &CPetriSimView::OnUpdateIndicatorMousePos)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ZOOMSCALE,&CPetriSimView::OnUpdateIndicatorZoomScale)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_EDIT_SELECT_ALL, &CPetriSimView::OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, &CPetriSimView::OnUpdateEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, &CPetriSimView::OnUpdateEditClear)
	ON_COMMAND_RANGE(ID_FIRST_MODE, ID_LAST_MODE, &CPetriSimView::OnMode)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FIRST_MODE, ID_LAST_MODE, &CPetriSimView::OnUpdateMode)
	ON_COMMAND_RANGE(ID_FIRST_CUSTOM_PROPERTY, ID_LAST_CUSTOM_PROPERTY, &CPetriSimView::OnEditCustomProperty)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FIRST_CUSTOM_PROPERTY, ID_LAST_CUSTOM_PROPERTY, &CPetriSimView::OnUpdateEditCustomProperty)
	ON_COMMAND_RANGE(ID_FIRST_BOOL_PROPERTY, ID_LAST_BOOL_PROPERTY, &CPetriSimView::OnEditBoolProperty)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FIRST_BOOL_PROPERTY, ID_LAST_BOOL_PROPERTY, &CPetriSimView::OnUpdateEditBoolProperty)
	ON_WM_RENDERFORMAT()
	ON_WM_RENDERALLFORMATS()
	ON_COMMAND(ID_CANCEL_EDIT, &CPetriSimView::OnCancelEdit)
	ON_COMMAND(ID_EDIT_CLEAR, &CPetriSimView::OnEditClear)
	ON_COMMAND_RANGE(ID_FIRST_ARRANGE, ID_LAST_ARRANGE, &CPetriSimView::OnArrangeCommand)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FIRST_ARRANGE, ID_LAST_ARRANGE, &CPetriSimView::OnUpdateArrangeCommand)
END_MESSAGE_MAP()

CPetriSimView::CPetriSimView() 
	: m_ShapeTool(this)
	, m_SelectTool(this)
	, m_PreviewTool(this)
	, m_NetObjectTool(this)
	, m_ClipboardTool(this)
	, m_ArrangeTool(this)
	, m_MousePosition(0,0)
{
	
}

CPetriSimView::~CPetriSimView()
{
}

BOOL CPetriSimView::PreCreateWindow(CREATESTRUCT& cs)
{
	return __super::PreCreateWindow(cs);
}

void CPetriSimView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CPetriSimView::OnPreparePrinting(CPrintInfo* pInfo)
{
	GetDoc().Sim().OnSimCommand(ID_SIM_PAUSE);

	return DoPreparePrinting(pInfo);
}

void CPetriSimView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{

}

void CPetriSimView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{

}

void CPetriSimView::OnPrint( CDC* pDC, CPrintInfo* pInfo )
{
	OnDraw(pDC);
}

void CPetriSimView::OnDraw(CDC* pDC)
{
	QBufferDC dc(pDC);
	//Graphics g(*pDC);
	Graphics g(dc);
	CPetriSimDoc& doc = GetDoc();

	CRect clip;
	pDC->GetClipBox(&clip);
	clip.InflateRect(1, 1);
	g.SetClip(ToRect(clip));

	if (!pDC->IsPrinting())
	{
		Color backgroundcolor;
		backgroundcolor.SetFromCOLORREF(::GetSysColor(COLOR_APPWORKSPACE));
		SolidBrush brush(backgroundcolor);
		g.FillRectangle(&brush, ToRect(clip));
	}

	Rect DocRect(Point(0, 0), ToSize(doc.GetDocSize()));

	CRect docclip(clip);
	docclip.IntersectRect(docclip, FromRect(DocRect));

	if(!doc.SimOnline() && doc.ShowGrid() && !pDC->IsPrinting() && GetZoomLevel() >= 100)
	{
		int grid = doc.GetGrid();

		Rect bmprect(0, 0, grid, grid);
		Bitmap bmp(bmprect.Height, bmprect.Width, &g);
		Graphics* pg = Graphics::FromImage(&bmp);

		pg->FillRectangle(&SolidBrush(doc.GetPaperColor()), bmprect);

		Pen pen(Color::LightSteelBlue);
		pen.SetDashStyle(DashStyleDot);
		pg->DrawRectangle(&pen, bmprect);

		delete pg;

		TextureBrush brush(&bmp);
		g.FillRectangle(&brush, ToRect(docclip));
	}
	else
	{
		SolidBrush brush(doc.GetPaperColor());
		if (!pDC->IsPrinting())
		{
			g.FillRectangle(&brush, ToRect(docclip));
		} 
		else
		{
			g.FillRectangle(&brush, ToRect(clip));
		}
	}

	g.SetSmoothingMode(SmoothingModeHighQuality);
	g.SetPixelOffsetMode(PixelOffsetModeHighQuality);

	CHDrawInfo Info;
	Info.Online = doc.SimOnline();
	CHDrawObjectList& list = doc.DrawObjectList();

	TRACE("clipbox: %d, %d, %d, %d\r\n", clip.top, clip.left, clip.bottom, clip.right);

	POSITION pos = list.GetHeadPosition();
	while (pos)
	{
		CHDrawObject* pObject = list.GetNext(pos);

		if (g.IsVisible(pObject->GetBounds(true)) && IsVisible(pObject))
		{
			pObject->DrawObject(g, Info);
		}
	}

	if (!pDC->IsPrinting())
	{
		m_SelectTool.Draw(g);
		doc.Simulation().DrawTokenAnimation(g);
	}
}

void CPetriSimView::OnInitialUpdate()
{
	__super::OnInitialUpdate();

	SetScrollSizes(MM_TEXT, GetDoc().GetDocSize());
	m_Tool = ID_SELECT_TOOL;

	GetDoc().Subscribe(&m_SelectTool);
}

#ifdef _DEBUG

void CPetriSimView::AssertValid() const
{
	__super::AssertValid();
}

void CPetriSimView::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}

CPetriSimDoc* CPetriSimView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPetriSimDoc)));
	return (CPetriSimDoc*)m_pDocument;
}

#endif //_DEBUG

CPetriSimDoc& CPetriSimView::GetDoc()
{
	CPetriSimDoc* pDoc = GetDocument();
	if (pDoc == 0)
	{
		AfxThrowMemoryException();
	}
	ASSERT_VALID(pDoc);
	return *pDoc;
}

void CPetriSimView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (GetZoomMode() != MODE_ZOOMOFF)
	{
		__super::OnLButtonDown(nFlags, point);
		return;
	}
	
	CClientDC dc(this);
	OnPrepareDC(&dc);

	CPoint point2(point);
	dc.DPtoLP(&point2);

	m_cpFirstPoint = point2;
	m_cpLine = point2;

	switch (m_Tool)
	{
	case ID_SELECT_TOOL: 
		m_SelectTool.OnLButtonDown(nFlags, point, dc); 
		break;
	case ID_RECT_MODE:
	case ID_ROUNDRECT_MODE:
	case ID_LINE_MODE:
	case ID_ELLIPSE_MODE:
	case ID_POLYGON_MODE:
	case ID_TEXT_MODE: 
		m_ShapeTool.OnLButtonDown(nFlags, point, dc);
		break;
	case ID_POSITION_MODE:
	case ID_TRANSITION_MODE:
	case ID_CONNECTOR_MODE:
		m_NetObjectTool.OnLButtonDown(nFlags, point, dc);
		break;
	}
}

void CPetriSimView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CPetriSimDoc& doc = GetDoc();

	CClientDC dc(this);
	OnPrepareDC(&dc);

	CPoint point2(point);
	dc.DPtoLP(&point2);

	/*switch (m_Tool)
	{
	case ID_SELECT_TOOL: m_SelectTool.OnLButtonUp(nFlags, point, dc, this); 
		break;
	
	}*/

//	if(doc.GetDrawMode() == ID_MOVE_MODE || doc.GetDrawMode() == ID_RESIZE_MODE)
//		doc.SetDrawMode(ID_SELECT_TOOL);

	

	//__super::OnLButtonUp(nFlags, point);
}

void CPetriSimView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CClientDC dc(this);
	OnPrepareDC(&dc);

	CPoint point2(point);
	dc.DPtoLP(&point2);
	m_MousePosition = point2;

	//m_ToolTip.Pop();

	//CString strText;
	//strText.Format(_T("x=%d y=%d"), point.x, point.y);
	//m_ToolTip.UpdateTipText(strText, this, 0);
	//m_ToolTip.Update();

//	__super::OnMouseMove(nFlags, point);
}



void CPetriSimView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	m_SelectTool.OnKeyDown(nChar, nRepCnt, nFlags);

	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

int CPetriSimView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	//TODO:
	//CMFCToolTipInfo params;
	//params.m_bVislManagerTheme = TRUE;
	//m_ToolTip.SetParams (&params);
	//m_ToolTip.AddTool (this, _T("BASE LABLE"));
	//m_ToolTip.AddTool (this, _T("NEXT LABLE"), &CRect(0,0,100,100), 1);
	//m_ToolTip.SetDelayTime(3000);

	//m_ToolTip.Create(this);
	//m_ToolTip.AddTool(this, L"Test");
	//m_ToolTip.AddTool(this, L"Test"/*IDS_NOMATTER_TEXT*/, CRect(0,0,0,0), 1/*IDC_TOOLTIP*/);
	//m_ToolTip.Activate(TRUE);

	m_Tool = ID_SELECT_TOOL;
	return 0;
}

void CPetriSimView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	//CPetriSimDoc* pDoc = GetDocument();
	//ASSERT_VALID(pDoc);

	//CPoint cpoint(point);
	//ScreenToClient(&point);
	//CClientDC dc(this);
	//OnPrepareDC(&dc);
	//dc.DPtoLP(&point);
	//pDoc->Select(point);
	//
	//if(pDoc->m_listSelected.GetCount() == 1)
	//{
	//	//CMenu menu;
	//	//menu.LoadMenu(IDR_HPSTYPE);
	//	//// Das Object kann spezielle Items einfügen
	//	//// Reserviert sind ID_CONTEXT_COMMAND1 bis
	//	//// ID_CONTEXT_COMMAND20
	//	//pDoc->m_listSelected.GetHead()->InitMenu
	//	//	(menu.GetSubMenu(1),point);
	//	//menu.GetSubMenu(1)->TrackPopupMenu(TPM_LEFTALIGN | 
	//	//TPM_RIGHTBUTTON, cpoint.x, cpoint.y, this);
	//	//pDoc->DetachExplObject();
	//}
	//else if(pDoc->GetSimMode())
	//{
	//	//Menu der Ansicht aufrufen
	//	CMenu menu;
	//	menu.LoadMenu(IDR_HPSTYPE);
	//	menu.GetSubMenu(7)->TrackPopupMenu(TPM_LEFTALIGN | 
	//	TPM_RIGHTBUTTON, cpoint.x, cpoint.y, this);
	//}
	//else
	//{
	//	//Menu der Ansicht aufrufen
	//	CMenu menu;
	//	menu.LoadMenu(IDR_HPSTYPE);
	//	menu.GetSubMenu(1)->TrackPopupMenu(TPM_LEFTALIGN | 
	//	TPM_RIGHTBUTTON, cpoint.x, cpoint.y, this);
	//}	
}

void CPetriSimView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{

	switch (lHint)
	{
	case CPetriSimDoc::UpdateDeleteContent:
		m_SelectTool.DeleteContent();
		break;
	case CPetriSimDoc::UpdateInvalidate:
		if (pHint)
		{
			CHDrawObject* pObject = dynamic_cast<CHDrawObject*>(pHint);
			if (pObject)
			{
				ASSERT_VALID(pObject);
				InvalidateLg(pObject->GetBounds(true));
			}

			CHUpdateObject* pUpdateObject = dynamic_cast<CHUpdateObject*>(pHint);
			if (pUpdateObject)
			{
				ASSERT_VALID(pUpdateObject);
				InvalidateLg(pUpdateObject->m_Update);
			}
		}
		break;
	case CPetriSimDoc::UpdateDocSize:
		SetScrollSizes(MM_TEXT, GetDoc().GetDocSize());
		Invalidate();
		break;
	default:
		Invalidate();
	}
}

void CPetriSimView::OnUpdateEditCopyCut(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_ClipboardTool.CanCopyCut());
}

void CPetriSimView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_ClipboardTool.CanPaste());
}

void CPetriSimView::OnEditCopy()
{
	m_ClipboardTool.Copy();	
}

void CPetriSimView::OnEditCut()
{
	m_ClipboardTool.Copy();
	m_SelectTool.DeleteSelected();
}

void CPetriSimView::OnEditPaste()
{
	m_ClipboardTool.Paste();
}

void CPetriSimView::OnRenderFormat(UINT nFormat)
{
	m_ClipboardTool.RenderFormat(nFormat);

	__super::OnRenderFormat(nFormat);
}

void CPetriSimView::OnRenderAllFormats()
{
	__super::OnRenderAllFormats();

	m_ClipboardTool.RenderAllFormats();
}

BOOL CPetriSimView::IsSelected()
{
	return m_SelectTool.IsSelected();
}

void CPetriSimView::OnCancelEdit()
{
	m_SelectTool.OnCancelEdit();
}

void CPetriSimView::OnUpdateContextPopup(CCmdUI* pCmdUI) 
{
	/*if((GetDocument()->GetSimMode())|| 
	(GetDocument()->m_listSelected.GetCount() == 0))
		pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);*/
}

void CPetriSimView::OnUpdateIndicatorMousePos(CCmdUI* pCmdUI) 
{
	CString cstring;
	cstring.Format(_T("P. %d, %d") ,m_MousePosition.x, m_MousePosition.y);
	pCmdUI->SetText(cstring);
	
}

void CPetriSimView::OnUpdateIndicatorZoomScale(CCmdUI* pCmdUI) 
{
	CString str;
	str.Format(_T("Z. %d%%") , GetZoomLevel());
	pCmdUI->SetText(str);
	
}
 
void CPetriSimView::OnUpdateContextPushback(CCmdUI* pCmdUI) 
{
	/*if((GetDocument()->GetSimMode())|| 
	(GetDocument()->m_listSelected.GetCount() == 0))
		pCmdUI->Enable(FALSE);
	else 
		pCmdUI->Enable(TRUE);*/
}

void CPetriSimView::OnUpdateGroup(CCmdUI* pCmdUI) 
{
	/*if((GetDocument()->GetSimMode())|| 
		(GetDocument()->m_listSelected.GetCount() < 2))
		pCmdUI->Enable(FALSE);
	else 
		pCmdUI->Enable(TRUE);*/
}

void CPetriSimView::OnUpdateUngroup(CCmdUI* pCmdUI) 
{
	/*if((GetDocument()->GetSimMode())|| 
		(GetDocument()->m_listSelected.GetCount() == 0))
		pCmdUI->Enable(FALSE);
	else if(GetDocument()->m_listSelected.GetHead()->
	IsKindOf( RUNTIME_CLASS(CHGroup)))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);*/
}

void CPetriSimView::OnContextPopup() 
{
//	GetDocument()->PopUp();	
}

void CPetriSimView::OnContextPushback() 
{
//	GetDocument()->PushBack();
}

void CPetriSimView::OnGroup() 
{
//	GetDocument()->Group();
}

void CPetriSimView::OnUngroup() 
{
//	GetDocument()->UnGroup();
}

void CPetriSimView::OnContextCommand(UINT nID)
{
	//// Die ID in das selektierte Object zur Bearbeitung 
	//// Senden
	//UINT id = nID - ID_CONTEXT_COMMAND1 + 1;
	////TRACE(_T("ContextCommand->ID: %d\n",nID);
	//if(GetDocument()->m_listSelected.GetCount() == 1)
	//	GetDocument()->m_listSelected.GetHead()->DoID(id);
	////Object Neuzeichnen
	//GetDocument()->UpdateAllViews(0);
}

BOOL CPetriSimView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	HCURSOR hCursor = GetCursor();

	if (hCursor)
	{
		::SetCursor(hCursor);
	}
	else
	{
		return __super::OnSetCursor(pWnd, nHitTest, message);
	}
	return false;
}

void CPetriSimView::InvalidateDcRect(CRect & crect)
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.LPtoDP(&crect);

	InvalidateRect(&crect, false);
}

void CPetriSimView::OnSetFocus(CWnd* pOldWnd) 
{
	__super::OnSetFocus(pOldWnd);
}

void CPetriSimView::OnStoreClient() 
{
	StoreScreenToBitmap(false);
}

void CPetriSimView::OnStoreScreen() 
{
	StoreScreenToBitmap(true);
}

void CPetriSimView::StoreScreenToBitmap(bool all)
{
	//CPetriSimDoc* pDoc = GetDocument();
	//ASSERT_VALID(pDoc);

	//CFileDialog dlg(false, 0, 0, OFN_HIDEREADONLY,
	//	_T("Bitmap Files (*.bmp)|*.bmp|All Files|*.*||"));
	//if(dlg.DoModal()==IDOK)
	//{
	//	CString path(dlg.GetPathName());
	//	if(dlg.GetFileExt().IsEmpty())
	//		path += ".bmp";

	//	CFile cfile(path, CFile::modeCreate|CFile::modeWrite);

	//	//die Zoom Einstellungen könnten
	//	//anschliessend wieder Restauriert werden ! :-)
	//	//if(all)DoZoomNorm();

	//	CClientDC dc(this);
	//	OnPrepareDC(&dc);
	//	CDC cdc;
	//	if(!cdc.CreateCompatibleDC(&dc))return;		
	//	
	//	CRect client;
	//	if(all)
	//	{
	//		client.SetRect(0,0,pDoc->GetDocSize().cx,
	//			pDoc->GetDocSize().cy);
	//	}
	//	else
	//	{
	//		GetClientRect(&client);
	//		dc.DPtoLP(&client);
	//	}
	//	CSize csize(client.Size());
	//	if(!all)dc.LPtoDP(&csize);
	//	CDib cdib(csize, 24);//true color
	//	//erzeugt eine dibsection kompatibel zum Geräte Kontext
	//	HBITMAP hbitmap = cdib.CreateSection(&cdc);
	//	if(!hbitmap)return;

	//	OnPrepareDC(&cdc);
	//	cdc.SelectObject(hbitmap);

	//	CBrush brush;
	//	if (!brush.CreateSolidBrush(pDoc->GetPaperColor()))
	//		return;
	//	//in die Bitmap zeichnen
	//	cdc.FillRect(client, &brush);
	//	pDoc->Draw(cdc, false);
	//	//Bitmap speichern
	//	cdib.Write(&cfile);
	//}
}

void CPetriSimView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	m_SelectTool.OnLButtonDblClk(nFlags, point);

	__super::OnLButtonDblClk(nFlags, point);
}

BOOL CPetriSimView::OnEraseBkgnd(CDC* pDC)
{
	//suppress erasing
	//background will be draw in "OnDraw"
	return TRUE;
	//return __super::OnEraseBkgnd(pDC);
}

void CPetriSimView::OnUpdateMode(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(pCmdUI->m_nID == m_Tool);
	pCmdUI->Enable(!GetDoc().SimOnline());
}

void CPetriSimView::OnMode(UINT nID) 
{
	if (!GetDoc().SimOnline())
	{
		m_Tool = nID;
	}
}

void CPetriSimView::OnUpdateArrangeCommand( CCmdUI* pCmdUI )
{
	m_ArrangeTool.OnUpdateArrangeCommand(pCmdUI);
}

void CPetriSimView::OnArrangeCommand( UINT nID )
{
	m_ArrangeTool.OnArrangeCommand(nID);
}

void CPetriSimView::InvalidateLg( Rect& rect )
{
	if (rect.IsEmptyArea())
	{
		return;
	}

	CRect rect2(FromRect(rect));

	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.LPtoDP(&rect2);

	InvalidateRect(&rect2);
}

void CPetriSimView::OnEditSelectAll()
{
	if (!GetDoc().SimOnline())
	{
		m_SelectTool.SelectAll();
	}
}

void CPetriSimView::OnUpdateEditSelectAll( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(!GetDoc().SimOnline() && GetDoc().DrawObjectList().GetCount() > 0);
}

void CPetriSimView::OnEditClear()
{
	if (!GetDoc().SimOnline())
	{
		m_SelectTool.DeleteSelected();
	}
}

void CPetriSimView::OnUpdateEditClear( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(!GetDoc().SimOnline() && m_SelectTool.IsSelected());
}

void CPetriSimView::OnUpdateEditCustomProperty( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(m_SelectTool.IsPropertyPossible(pCmdUI->m_nID));
}

void CPetriSimView::OnEditCustomProperty( UINT nID )
{
	CMainFrame* pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pFrame == 0)
	{
		return;
	}

	CHPropertyValue value(static_cast<int>(nID));

	if (pFrame->GetValueFromRibbon(value))
	{
		m_SelectTool.SetSelectedProperty(value);
	}
}

void CPetriSimView::OnUpdateEditBoolProperty( CCmdUI* pCmdUI )
{
	int check = 2;
	if (m_SelectTool.GetSelectedBoolProperty(check, pCmdUI->m_nID))
	{
		pCmdUI->Enable();
		pCmdUI->SetCheck(check);
		return;
	}

	pCmdUI->Enable(FALSE);
}

void CPetriSimView::OnEditBoolProperty( UINT nID )
{
	m_SelectTool.OnBoolCommand(nID);
}


BOOL CPetriSimView::PreTranslateMessage(MSG* pMsg)
{
	//m_ToolTip.RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

void CPetriSimView::SetTool( int Tool /*= ID_SELECT_TOOL*/ )
{
	m_Tool = Tool;
	SetCursor(GetCursor());
}

HCURSOR CPetriSimView::GetCursor()
{
	HCURSOR hCursor = m_SelectTool.GetCursor(m_MousePosition);

	if (hCursor == 0)
	{
		hCursor = m_ShapeTool.GetCursor(m_MousePosition);

		if (hCursor == 0)
		{
			hCursor = m_NetObjectTool.GetCursor(m_MousePosition);
		}

		if (hCursor == 0)
		{
			hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		}
	}

	return hCursor;
}