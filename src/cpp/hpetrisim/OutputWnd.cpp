/**************************************************************************
	OutputWnd.cpp

	Copyright (C) 2010/9/25 Henryk Anschuetz 
	
	www.winpesim.de
**************************************************************************/

#include "stdafx.h"
#include "OutputWnd.h"
#include "resource.h"

/*************************************************************************/
// COutputWnd
/*************************************************************************/

IMPLEMENT_DYNAMIC(COutputWnd, CDockablePane)

COutputWnd::COutputWnd()
{

}

COutputWnd::~COutputWnd()
{
	for (int loop=0; loop<m_wndTabs.GetTabsNum(); loop++)
	{
		delete m_wndTabs.GetTabWnd(loop);
	}
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_Font.CreateStockObject(DEFAULT_GUI_FONT);

	CRect rect;
	rect.SetRectEmpty();

	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_3D_ONENOTE, rect, this, 1))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}

	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// Tab control should cover the whole client area:
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	/*CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&m_Font);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = max(cxExtentMax, dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);*/
}

COutputWndTab* COutputWnd::Add(const CString & name)
{
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT|WS_CHILD|WS_VISIBLE|WS_HSCROLL|WS_VSCROLL|ES_MULTILINE|ES_READONLY;

	CRect rect;
	rect.SetRectEmpty();
	COutputWndTab* pWnd = new COutputWndTab();

	if (!pWnd->Create(dwStyle, rect, &m_wndTabs, 1))
	{
		ASSERT(FALSE);
	}

	pWnd->SetFont(&m_Font);

	m_wndTabs.AddTab(pWnd, name, (UINT)0);

	return pWnd;
}

void COutputWnd::Remove( COutputWndTab* pWnd )
{
	
	for (int loop=0; loop<m_wndTabs.GetTabsNum(); loop++)
	{
		CWnd* pWnd2 = m_wndTabs.GetTabWnd(loop);

		if (pWnd2 == pWnd)
		{
			m_wndTabs.RemoveTab(loop);
			delete pWnd2;
			return;
		}
	}

	ASSERT(FALSE);
}

void COutputWnd::Label( COutputWndTab* pWnd, const CString & label )
{
	for (int loop=0; loop<m_wndTabs.GetTabsNum(); loop++)
	{
		if (m_wndTabs.GetTabWnd(loop) == pWnd)
		{
			m_wndTabs.SetTabLabel(loop, label);
			return;
		}
	}

	ASSERT(FALSE);
}

/*************************************************************************/
// COutputWndTab
/*************************************************************************/

COutputWndTab::COutputWndTab()
{
}

COutputWndTab::~COutputWndTab()
{
}

BEGIN_MESSAGE_MAP(COutputWndTab, CListBox)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

void COutputWndTab::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	/*CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y
			, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}*/

	SetFocus();
}

void COutputWndTab::OnEditCopy()
{
	// TODO:
	
}

void COutputWndTab::OnEditClear()
{
	SetWindowText(_T(""));
}

void COutputWndTab::OnViewOutput()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != NULL && pParentBar != NULL)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();

	}
}

HBRUSH COutputWndTab::CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/)
{
	//white background
	return ::CreateSolidBrush(RGB(255, 255, 255));
}

void COutputWndTab::Show()
{

	CMFCTabCtrl* pTab = DYNAMIC_DOWNCAST(CMFCTabCtrl, GetOwner());

	if (pTab)
	{
		ASSERT_VALID(pTab);

		for (int loop=0; loop<pTab->GetTabsNum(); loop++)
		{
			CWnd* pWnd = pTab->GetTabWnd(loop);

			if (pWnd == this)
			{
				pTab->SetActiveTab(loop);
				return;
			}
		}
	}
}

void COutputWndTab::SetText( const CString& text )
{
	SetWindowText(text);
	
	int count = GetLineCount();
	LineScroll(count, 0);
}