/**************************************************************************
	MainFrm.cpp
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#include "stdafx.h"
#include "PetriSim.h"
#include "PetriSimDoc.h"
#include "HSimToolbar.h"
#include "Splash.h"
#include "Helper.h"
#include "HRibbonColorButton.h"
#include "PetriSimDoc.h"
#include "PetriSimView.h"
#include "HPropertyValue.h"
#include "HNetMember.h"
#include "RibbonEdit.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

/*************************************************************************/
// CMainFrame
/*************************************************************************/

IMPLEMENT_DYNCREATE(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_COMMAND(ID_WINDOWS_MENU, &CMainFrame::OnDummy)
	ON_COMMAND(ID_MDI_MOVE_TO_NEXT_GROUP, &CMainFrame::OnMdiMoveToNextGroup)
	ON_COMMAND(ID_MDI_MOVE_TO_PREV_GROUP, &CMainFrame::OnMdiMoveToPrevGroup)
	ON_COMMAND(ID_MDI_NEW_HORZ_TAB_GROUP, &CMainFrame::OnMdiNewHorzTabGroup)
	ON_COMMAND(ID_MDI_NEW_VERT_GROUP, &CMainFrame::OnMdiNewVertGroup)
	ON_MESSAGE(WM_SIMTHREAD, &CMainFrame::OnSimThreadMessage)
	ON_COMMAND(ID_HELP_FINDER, &__super::OnHelpFinder)
	ON_COMMAND(ID_HELP, &__super::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, &__super::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, &__super::OnHelpFinder)
	ON_REGISTERED_MESSAGE(AFX_WM_ON_HIGHLIGHT_RIBBON_LIST_ITEM, OnHighlightRibbonListItem)
	ON_COMMAND(ID_VIEW_OUTPUTWND, &CMainFrame::OnOutputWindow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUTWND, OnUpdateOutputWindow)
	ON_COMMAND(ID_VIEW_FULLSCREEN, &CMainFrame::OnViewFullscreen)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,
	ID_INDICATOR_SIMSTATUS,
	ID_INDICATOR_STEPCOUNT,
	ID_INDICATOR_SIMTIME,
	ID_INDICATOR_STEPTIME,
	ID_INDICATOR_MOUSEPOS,
	ID_INDICATOR_ZOOMSCALE,
	ID_INDICATOR_COUNT,
};

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
	long key = 0;
	RibbonArray* pVal;
	POSITION pos = m_mapRibbons.GetStartPosition();
	while (pos)
	{
		m_mapRibbons.GetNextAssoc(pos, key, pVal);
		delete pVal;
	}
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CTabbedPane::m_StyleTabWnd = CMFCTabCtrl::STYLE_3D_ONENOTE;
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
	//CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);

	CMDITabInfo mdiTabParams;

	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE;
	mdiTabParams.m_bAutoColor = FALSE;
	mdiTabParams.m_bDocumentMenu = FALSE;
	mdiTabParams.m_bActiveTabCloseButton = TRUE;
	mdiTabParams.m_bEnableTabSwap = TRUE;
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	//TODO:
	//EnableFullScreenMode(ID_VIEW_FULLSCREEN);
	//EnableFullScreenMainMenu(TRUE);

	EnableWindowsDialog(ID_WINDOW_MANAGER, _T("&Windows..."), TRUE);

	InitializeStatusBar();

	CreateDockingWindows();

	m_wndRibbonBar.Create(this);
	CHRibbonColorButton::CreateDocumentColors();

	InitializeMainRibbon();
	InitializeHomeRibbon();
	InitializeViewRibbon();
	InitializePropertiesRibbon();
	InitializeShapeFormatContextRibbon();
	InitializeObjectFormatContextRibbon();

	return 0;
}

void CMainFrame::UpdateStatusBar(int nId, const CString& text)
{
	CMFCRibbonBaseElement* pElem = m_wndStatusBar.FindByID(nId);

	if (pElem)
	{
		CString str = pElem->GetText();
		pElem->SetText(text);
		pElem->Redraw();

		if (str.GetLength() != text.GetLength())
		{
			m_wndStatusBar.RecalcLayout();
		}
	}
}

void CMainFrame::InitializeStatusBar()
{
	if (!m_wndStatusBar.Create(this))
	{
		return;
	}

	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_SIMSTATUS, L"", TRUE), L"Simulation Status");
	//TODO: m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_SIMSTATUS, L"", TRUE), L"Mouse Status");
}

void CMainFrame::CreateDockingWindows()
{
	CDockingManager::SetDockingMode(DT_SMART);
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	
	if (!m_wndOutput.Create(L"Output", this, CRect(0, 0, 500, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_BOTTOM|CBRS_FLOAT_MULTI))
	{
		ASSERT(FALSE);
		return;
	}

	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
}

void CMainFrame::InitializeMainRibbon()
{
	BOOL bNameValid;

	CString strTemp;
	bNameValid = strTemp.LoadString(IDS_RIBBON_FILE);
	ASSERT(bNameValid);

	// Load panel images:
	m_PanelImages.SetImageSize(CSize(16, 16));
	m_PanelImages.Load(IDB_BUTTONS);

	// Init main button:
	m_MainButton.SetImage(IDB_MAIN);
	m_MainButton.SetText(_T("\nf"));
	m_MainButton.SetToolTipText(strTemp);

	m_wndRibbonBar.SetApplicationButton(&m_MainButton, CSize (45, 45));
	CMFCRibbonMainPanel* pMainPanel = m_wndRibbonBar.AddMainCategory(strTemp, IDB_FILESMALL, IDB_FILELARGE);

	bNameValid = strTemp.LoadString(IDS_RIBBON_NEW);
	ASSERT(bNameValid);
	pMainPanel->Add(new CMFCRibbonButton(ID_FILE_NEW, strTemp, 0, 0));
	bNameValid = strTemp.LoadString(IDS_RIBBON_OPEN);
	ASSERT(bNameValid);
	pMainPanel->Add(new CMFCRibbonButton(ID_FILE_OPEN, strTemp, 1, 1));
	bNameValid = strTemp.LoadString(IDS_RIBBON_SAVE);
	ASSERT(bNameValid);
	pMainPanel->Add(new CMFCRibbonButton(ID_FILE_SAVE, strTemp, 2, 2));
	bNameValid = strTemp.LoadString(IDS_RIBBON_SAVEAS);
	ASSERT(bNameValid);
	pMainPanel->Add(new CMFCRibbonButton(ID_FILE_SAVE_AS, strTemp, 3, 3));

	bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT);
	ASSERT(bNameValid);
	CMFCRibbonButton* pBtnPrint = new CMFCRibbonButton(ID_FILE_PRINT, strTemp, 6, 6);
	pBtnPrint->SetKeys(_T("p"), _T("w"));
	bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT_LABEL);
	ASSERT(bNameValid);
	pBtnPrint->AddSubItem(new CMFCRibbonLabel(strTemp));
	bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT_QUICK);
	ASSERT(bNameValid);
	pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_DIRECT, strTemp, 7, 7, TRUE));
	bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT_PREVIEW);
	ASSERT(bNameValid);
	pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_PREVIEW, strTemp, 8, 8, TRUE));
	bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT_SETUP);
	ASSERT(bNameValid);
	pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_SETUP, strTemp, 11, 11, TRUE));
	pMainPanel->Add(pBtnPrint);
	pMainPanel->Add(new CMFCRibbonSeparator(TRUE));

	bNameValid = strTemp.LoadString(IDS_RIBBON_CLOSE);
	ASSERT(bNameValid);
	pMainPanel->Add(new CMFCRibbonButton(ID_FILE_CLOSE, strTemp, 9, 9));

	bNameValid = strTemp.LoadString(IDS_RIBBON_RECENT_DOCS);
	ASSERT(bNameValid);
	pMainPanel->AddRecentFilesList(strTemp);

	bNameValid = strTemp.LoadString(IDS_RIBBON_EXIT);
	ASSERT(bNameValid);
	pMainPanel->AddToBottom(new CMFCRibbonMainPanelButton(ID_APP_EXIT, strTemp, 15));

	// Add quick access toolbar commands:
	CList<UINT, UINT> lstQATCmds;

	lstQATCmds.AddTail(ID_FILE_NEW);
	lstQATCmds.AddTail(ID_FILE_OPEN);
	lstQATCmds.AddTail(ID_FILE_SAVE);
	lstQATCmds.AddTail(ID_FILE_PRINT_DIRECT);

	m_wndRibbonBar.SetQuickAccessCommands(lstQATCmds);

	m_wndRibbonBar.AddToTabs(new CMFCRibbonButton(ID_APP_ABOUT, _T("\na"), m_PanelImages.ExtractIcon (1)));
}

void CMainFrame::InitializePropertiesRibbon()
{	
	CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory(L"Properties", IDB_PROPERTIESSMALL, IDB_PROPERTIESLARGE);

	//  Create and add a "Document" panel
	CMFCRibbonPanel* pPanel = pCategory->AddPanel(L"Document\nd");

	pPanel->Add(new CMFCRibbonEdit(ID_DOCUMENT_WIDTH, 60, 0, 0));
	pPanel->Add(new CMFCRibbonEdit(ID_DOCUMENT_HEIGHT, 60, 0, 1));

	CMFCRibbonColorButton* pColorButton =  new CHRibbonColorButton(ID_DOCUMENT_COLOR, L"Paper Color\np", FALSE, 2, -1);
	pPanel->Add(pColorButton);

	// Simulation Properties
	pPanel = pCategory->AddPanel(L"Simulation Limits\nd");
	CMFCRibbonEdit* pEditButton = new CMFCRibbonEdit(ID_SIM_STEP_LIMIT, 60, L"\nt", 3);
	pEditButton->SetEditText(_T("1000"));
	pPanel->Add(pEditButton);

	pEditButton = new CMFCRibbonEdit(ID_SIM_TIME_LIMIT, 60, L"\nt", 4);
	pEditButton->SetEditText(_T("1000"));
	pPanel->Add(pEditButton);
}

void CMainFrame::InitializeHomeRibbon()
{	
	// Add "Home" category:
	CString strTemp;
	BOOL bNameValid = strTemp.LoadString(IDS_RIBBON_HOME);
	ASSERT(bNameValid);
	CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory(strTemp, IDB_HOMESMALL, IDB_WRITELARGE);

	// Create "Clipboard" panel:
	bNameValid = strTemp.LoadString(IDS_RIBBON_CLIPBOARD);
	ASSERT(bNameValid);
	CMFCRibbonPanel* pPanel = pCategory->AddPanel(strTemp, m_PanelImages.ExtractIcon(27));

	bNameValid = strTemp.LoadString(IDS_RIBBON_PASTE);
	ASSERT(bNameValid);
	CMFCRibbonButton* pBtnPaste = new CMFCRibbonButton(ID_EDIT_PASTE, strTemp, 0, 0);
	pPanel->Add(pBtnPaste);

	bNameValid = strTemp.LoadString(IDS_RIBBON_CUT);
	ASSERT(bNameValid);
	pPanel->Add(new CMFCRibbonButton(ID_EDIT_CUT, strTemp, 1));
	bNameValid = strTemp.LoadString(IDS_RIBBON_COPY);
	ASSERT(bNameValid);
	pPanel->Add(new CMFCRibbonButton(ID_EDIT_COPY, strTemp, 2));

	// Edit Panel
	pPanel = pCategory->AddPanel(LoadStr(IDS_RIBBON_EDIT));
	pPanel->Add(new CMFCRibbonButton(ID_SELECT_TOOL, LoadStr(IDS_RIBBON_SELECT), 4));
	pPanel->Add(new CMFCRibbonButton(ID_EDIT_CLEAR, LoadStr(IDS_RIBBON_DELETE), 3));
	pPanel->Add(new CMFCRibbonButton(ID_EDIT_SELECT_ALL, LoadStr(IDS_RIBBON_SELECTALL), -1));

	// Shape Panel
	pPanel = pCategory->AddPanel(LoadStr(IDS_RIBBON_SHAPES));
	
	pPanel->Add(new CMFCRibbonButton(ID_LINE_MODE, LoadStr(IDS_RIBBON_LINE), 5));
	pPanel->Add(new CMFCRibbonButton(ID_RECT_MODE, LoadStr(IDS_RIBBON_RECT), 6));
	pPanel->Add(new CMFCRibbonButton(ID_ROUNDRECT_MODE, LoadStr(IDS_RIBBON_ROUNDRECT), 7));
	pPanel->Add(new CMFCRibbonButton(ID_ELLIPSE_MODE, LoadStr(IDS_RIBBON_ELLIPSE), 8));
	pPanel->Add(new CMFCRibbonButton(ID_POLYGON_MODE, LoadStr(IDS_RIBBON_POLYGON), 9));
	pPanel->Add(new CMFCRibbonButton(ID_TEXT_MODE, LoadStr(IDS_RIBBON_TEXT), 10));

	// Objects Panel
	pPanel = pCategory->AddPanel(LoadStr(IDS_RIBBON_OBJECTS));
	pPanel->Add(new CMFCRibbonButton(ID_POSITION_MODE, LoadStr(IDS_RIBBON_POSITION), 11));
	pPanel->Add(new CMFCRibbonButton(ID_TRANSITION_MODE, LoadStr(IDS_RIBBON_TRANSITION), 12));
	pPanel->Add(new CMFCRibbonButton(ID_CONNECTOR_MODE, LoadStr(IDS_RIBBON_CONNECTOR), 13));

	// Simulation Panel
	pPanel = pCategory->AddPanel(L"Simulation");

	CMFCRibbonSlider* pSlider = new CMFCRibbonSlider(ID_SIM_SPEED, 145);
	pSlider->SetRange(10, 3000);
	pPanel->Add(pSlider);

	CMFCRibbonButtonsGroup* pGroup = new CMFCRibbonButtonsGroup();
	pGroup->AddButton(new CMFCRibbonButton(ID_SIM_RESET, 0, 14, -1));
	pGroup->AddButton(new CMFCRibbonButton(ID_SIM_ON, 0, 15, -1));
	pGroup->AddButton(new CMFCRibbonButton(ID_SIM_PAUSE, 0, 16, -1));
	pGroup->AddButton(new CMFCRibbonButton(ID_SIM_STEP, 0, 17, -1));
	pGroup->AddButton(new CMFCRibbonButton(ID_SIM_RUN, 0, 18, -1));
	pGroup->AddButton(new CMFCRibbonButton(ID_SIM_RUNFAST, 0, 19, -1));
	pPanel->Add(pGroup);

}

void CMainFrame::InitializeViewRibbon()
{
	CString strTemp;
	BOOL bNameValid = strTemp.LoadString(IDS_RIBBON_VIEW);
	ASSERT(bNameValid);
	CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory(strTemp, IDB_VIEWSMALL, IDB_VIEWLARGE);

	// Create and add a "Show or hide" panel:
	bNameValid = strTemp.LoadString(IDS_RIBBON_SHOWORHIDE);
	ASSERT(bNameValid);
	CMFCRibbonPanel* pPanel = pCategory->AddPanel(strTemp, m_PanelImages.ExtractIcon (7));

	bNameValid = strTemp.LoadString(IDS_RIBBON_STATUSBAR);
	ASSERT(bNameValid);
	CMFCRibbonButton* pBtnStatusBar = new CMFCRibbonCheckBox(ID_VIEW_STATUS_BAR, strTemp);
	pPanel->Add(pBtnStatusBar);

	CMFCRibbonButton* pBtnOutputWnd = new CMFCRibbonCheckBox(ID_VIEW_OUTPUTWND, LoadStr(IDS_RIBBON_OUTPUTWND));
	pPanel->Add(pBtnOutputWnd);

	bNameValid = strTemp.LoadString(IDS_RIBBON_SHOWLABEL);
	ASSERT(bNameValid);
	pPanel->Add(new CMFCRibbonCheckBox(ID_EDITOR_SHOWLABEL, strTemp));

	// Window Panel
	pPanel = pCategory->AddPanel(L"Window\nzw");
	pPanel->Add(new CMFCRibbonButton(ID_WINDOW_NEW, _T("New\nn"), 7, 4));

	CMFCRibbonButton* pBtnWindows = new CMFCRibbonButton(ID_WINDOWS_MENU, _T("Switch\ns"), 8, 5);
	pBtnWindows->SetMenu(IDR_WINDOWS_MENU, TRUE);
	pBtnWindows->SetDefaultCommand(FALSE);
	pPanel->Add(pBtnWindows);

	//TODO:
	//pPanel->Add(new CMFCRibbonButton(ID_VIEW_FULLSCREEN, _T("Fullscreen"), 9, 6));

	// Create and add a "Grid" panel
	pPanel = pCategory->AddPanel(LoadStr(IDS_RIBBON_GRID));

	pPanel->Add(new CMFCRibbonCheckBox(ID_SHOW_GRID, LoadStr(IDS_RIBBON_SHOW_GRID)));
	pPanel->Add(new CMFCRibbonCheckBox(ID_ALING_GRID, LoadStr(IDS_RIBBON_ALIGN_GRID)));

	CMFCRibbonEdit* pEditButton = new CMFCRibbonEdit(ID_GRID_SIZE, 40);
	pEditButton->EnableSpinButtons(10, 50);
	pEditButton->SetEditText(_T("10"));
	pPanel->Add(pEditButton);

	// Create and add a "Zoom" panel
	bNameValid = strTemp.LoadString(IDS_RIBBON_ZOOM);
	ASSERT(bNameValid);
	pPanel = pCategory->AddPanel(strTemp, m_PanelImages.ExtractIcon (7));

	bNameValid = strTemp.LoadString(IDS_RIBBON_ZOOM_IN);
	ASSERT(bNameValid);
	pPanel->Add(new CMFCRibbonButton(ID_ZOOM_IN, strTemp, 0, 0));

	bNameValid = strTemp.LoadString(IDS_RIBBON_ZOOM_OUT);
	ASSERT(bNameValid);
	pPanel->Add(new CMFCRibbonButton(ID_ZOOM_OUT, strTemp, 1, 1));

	bNameValid = strTemp.LoadString(IDS_RIBBON_ZOOM_NORM);
	ASSERT(bNameValid);
	pPanel->Add(new CMFCRibbonButton(ID_ZOOM_NORM, strTemp, 2, 2));

	bNameValid = strTemp.LoadString(IDS_RIBBON_ZOOM_FULL);
	ASSERT(bNameValid);
	pPanel->Add(new CMFCRibbonButton(ID_ZOOM_FULL, strTemp, 3, 3));
}

void CMainFrame::InitializeShapeFormatContextRibbon()
{
	CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddContextCategory(_T("Format"), _T("Shape tools"), ID_SHAPE_FORMAT_CONTEXT_TAB, AFX_CategoryColor_None, IDB_SHAPEFORMAT_SMALL, IDB_SHAPEFORMAT_LARGE);
	pCategory->SetKeys(_T("jd"));

	// Size Panel
	CMFCRibbonPanel* pPanel = pCategory->AddPanel(L"Location\ns");

	CMFCRibbonEdit* pEditButton = new CRibbonEdit(ID_OBJECT_X, 70, L"X");
	pEditButton->EnableSpinButtons(0, 32000);
	pEditButton->SetEditText(_T("0"));
	pPanel->Add(pEditButton);

	pEditButton = new CRibbonEdit(ID_OBJECT_Y, 70, L"Y");
	pEditButton->EnableSpinButtons(0, 32000);
	pEditButton->SetEditText(_T("0"));
	pPanel->Add(pEditButton);

	// Size Panel
	pPanel = pCategory->AddPanel(L"Size\ns");

	pEditButton = new CRibbonEdit(ID_OBJECT_CX, 70, L"CX");
	pEditButton->EnableSpinButtons(1, 32000);
	pEditButton->SetEditText(_T("0"));
	pPanel->Add(pEditButton);

	pEditButton = new CRibbonEdit(ID_OBJECT_CY, 70, L"CY");
	pEditButton->EnableSpinButtons(1, 32000);
	pEditButton->SetEditText(_T("0"));
	pPanel->Add(pEditButton);

	pPanel->Add(new CMFCRibbonButton(ID_OBJECT_TEXT_AUTOSIZE, 0, 18, -1));

	// Points Panel
	pPanel = pCategory->AddPanel(L"Points\np");

	pPanel->Add(new CMFCRibbonButton(ID_OBJECT_CLOSEFIGURE, 0, 19, -1));


	// Line Styles Panel
	pPanel = pCategory->AddPanel(L"Line\nl");
	CMFCRibbonColorButton* pColorButton =  new CHRibbonColorButton(ID_OBJECT_LINECOLOR, LoadStr(IDS_RIBBON_LINECOLOR), FALSE, 1, -1);
	pPanel->Add(pColorButton);

	CStringArray sa;
	sa.Add(_T("1 pt"));
	sa.Add(_T("2 pt"));
	sa.Add(_T("3 pt"));
	sa.Add(_T("4 pt"));
	sa.Add(_T("5 pt"));
	sa.Add(_T("6 pt"));
	sa.Add(_T("7 pt"));

	CRibbonListButton* pListButton = new CRibbonListButton(ID_OBJECT_LINEWEIGHT, LoadStr(IDS_RIBBON_LINEWEIGHT), 2, -1, IDB_LINEWEIGHT, 96, sa);
	pPanel->Add(pListButton);

	sa.RemoveAll();
	sa.Add(L"solid");
	sa.Add(L"dash");
	sa.Add(L"dot");
	sa.Add(L"dash dot");
	sa.Add(L"dash dot dot");

	pListButton = new CRibbonListButton(ID_OBJECT_LINESTYLE, L"Style", 3, -1, IDB_LINESTYLE, 50, sa);
	pListButton->AddSubItem(new CMFCRibbonButton(ID_OBJECT_NOBORDER, _T("&No Line\nn"), -1));
	pPanel->Add(pListButton);

	// Fill Styles Panel
	pPanel = pCategory->AddPanel(L"Fill\nf");
	pColorButton =  new CHRibbonColorButton(ID_OBJECT_FILLCOLOR, LoadStr(IDS_RIBBON_FILLCOLOR), FALSE, 0, -1);
	pPanel->Add(pColorButton);

	pColorButton =  new CHRibbonColorButton(ID_OBJECT_FILLCOLOR2, L"Back", FALSE, 0, -1);
	pPanel->Add(pColorButton);

	CMFCRibbonGallery* pGallery = new CMFCRibbonGallery(ID_OBJECT_FILLSTYLE, _T("Style\ns"), 4, -1, IDB_FILLSTYLES, 40);
	pGallery->SetButtonMode();
	pGallery->SetIconsInRow(10);
	pGallery->EnableMenuResize(TRUE, TRUE);
	pPanel->Add(pGallery);

	//Fonts Panel
	pPanel = pCategory->AddPanel(L"Fonts\nf");

	CMFCRibbonButtonsGroup* pGroup = new CMFCRibbonButtonsGroup();
	CMFCRibbonFontComboBox* pFontsCombo = new CMFCRibbonFontComboBox(ID_OBJECT_FONT_NAME);
	pFontsCombo->SelectItem(_T("Arial"));
	pGroup->AddButton(pFontsCombo);

	CMFCRibbonComboBox* pComboButton = new CMFCRibbonComboBox(ID_OBJECT_FONT_SIZE, TRUE, 40);
	pComboButton->AddItem(L"8");
	pComboButton->AddItem(L"9");
	pComboButton->AddItem(L"10");
	pComboButton->AddItem(L"11");
	pComboButton->AddItem(L"14");
	pComboButton->AddItem(L"16");
	pComboButton->AddItem(L"18");
	pComboButton->AddItem(L"20");
	pComboButton->AddItem(L"22");
	pComboButton->AddItem(L"24");
	pComboButton->AddItem(L"26");
	pComboButton->AddItem(L"28");
	pComboButton->AddItem(L"36");
	pComboButton->AddItem(L"48");
	pComboButton->AddItem(L"72");
	pComboButton->SelectItem(L"8");
	pGroup->AddButton(pComboButton);
	pPanel->Add(pGroup);

	pColorButton =  new CHRibbonColorButton(ID_OBJECT_FONT_COLOR, 0, FALSE, 11, -1);
	pPanel->Add(pColorButton);

	pGroup = new CMFCRibbonButtonsGroup();
	pGroup->AddButton(new CMFCRibbonButton(ID_OBJECT_FONT_SIZE_PLUS, 0, 5, -1));
	pGroup->AddButton(new CMFCRibbonButton(ID_OBJECT_FONT_SIZE_MINUS, 0, 6, -1));
	pPanel->Add(pGroup);

	pGroup = new CMFCRibbonButtonsGroup();
	pGroup->AddButton(new CMFCRibbonButton(ID_OBJECT_FONT_BOLD, 0, 7, -1));
	pGroup->AddButton(new CMFCRibbonButton(ID_OBJECT_FONT_ITALIC, 0, 8, -1));
	pGroup->AddButton(new CMFCRibbonButton(ID_OBJECT_FONT_UNDERLINE, 0, 9, -1));
	pGroup->AddButton(new CMFCRibbonButton(ID_OBJECT_FONT_STRIKEOUT, 0, 10, -1));
	pPanel->Add(pGroup);

	//Text align
	pPanel = pCategory->AddPanel(L"Text-align\na");

	pGroup = new CMFCRibbonButtonsGroup();
	pGroup->AddButton(new CMFCRibbonButton(ID_OBJECT_TEXT_LEFT, 0, 12, -1));
	pGroup->AddButton(new CMFCRibbonButton(ID_OBJECT_TEXT_CENTER, 0, 13, -1));
	pGroup->AddButton(new CMFCRibbonButton(ID_OBJECT_TEXT_RIGHT, 0, 14, -1));
	pPanel->Add(pGroup);

	pGroup = new CMFCRibbonButtonsGroup();
	pGroup->AddButton(new CMFCRibbonButton(ID_OBJECT_TEXT_TOP, 0, 15, -1));
	pGroup->AddButton(new CMFCRibbonButton(ID_OBJECT_TEXT_LINE_CENTER, 0, 16, -1));
	pGroup->AddButton(new CMFCRibbonButton(ID_OBJECT_TEXT_BOTTOM, 0, 17, -1));
	pPanel->Add(pGroup);

	//Arrange
	pPanel = pCategory->AddPanel(L"Arrange");

	CMFCRibbonButton* pButton = new CMFCRibbonButton(ID_ARRANGE_TOFRONT, L"Order", -1, 0);
	pButton->AddSubItem(new CMFCRibbonButton(ID_ARRANGE_TOFRONT, 0, -1, 0));
	pButton->AddSubItem(new CMFCRibbonButton(ID_ARRANGE_UP, 0, -1, 1));
	pButton->AddSubItem(new CMFCRibbonButton(ID_ARRANGE_DOWN, 0, -1, 2));
	pButton->AddSubItem(new CMFCRibbonButton(ID_ARRANGE_TOBACK, 0, -1, 3));

	pPanel->Add(pButton);
}

void CMainFrame::InitializeObjectFormatContextRibbon()
{
	CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddContextCategory(_T("Format"), _T("Object tools"), ID_OBJECT_FORMAT_CONTEXT_TAB, AFX_CategoryColor_None, IDB_OBJECT_FORMAT, 0);
	pCategory->SetKeys(_T("kd"));

	CMFCRibbonPanel* pPanel = pCategory->AddPanel(L"Size", 0);
	CMFCRibbonComboBox* pComboButton = new CMFCRibbonComboBox(ID_OBJECT_SIZE, FALSE, 70, L"\ns");

	pComboButton->AddItem(L"extra small", CHNetMember::PT_VERYSMALL);
	pComboButton->AddItem(L"small", CHNetMember::PT_SMALL);
	pComboButton->AddItem(L"normal", CHNetMember::PT_NORMAL);
	pComboButton->AddItem(L"large", CHNetMember::PT_LARGE);
	pComboButton->AddItem(L"extra large", CHNetMember::PT_VERYLARGE);
	pComboButton->SelectItem(2);
	pPanel->Add(pComboButton);

	//Name Panel
	pPanel = pCategory->AddPanel(L"Name", 0);
	pPanel->Add(new CMFCRibbonEdit(ID_OBJECT_NAME, 80, L"\nt", -1));

	//Label Panel
	pPanel = pCategory->AddPanel(L"Label", 0);
	pPanel->Add(new CMFCRibbonCheckBox(ID_SHOW_LABEL_01, L"Name"));
	pPanel->Add(new CMFCRibbonCheckBox(ID_SHOW_LABEL_02, L"Property"));

	//create and add "Position" panel
	pPanel = pCategory->AddPanel(LoadStr(IDS_RIBBON_POSITION), 0);

	CMFCRibbonEdit* pEditButton = new CMFCRibbonEdit(ID_POSITION_TOKEN_INITIAL, 70, L"\nt", 0);
	pEditButton->EnableSpinButtons(0, 50);
	pEditButton->SetEditText(_T("0"));
	pPanel->Add(pEditButton);

	pEditButton = new CMFCRibbonEdit(ID_POSITION_TOKEN_CAPACITY, 70, L"\nc", 1);
	pEditButton->EnableSpinButtons(1, 50);
	pEditButton->SetEditText(_T("0"));
	pPanel->Add(pEditButton);

	//create and add "Transition" panel
	pPanel = pCategory->AddPanel(L"Transition\nt", 0);

	pComboButton = new CMFCRibbonComboBox(ID_TRANSITION_TIME_MODE, FALSE, 74, L"\nm");

	pComboButton->AddItem(L"immediate", CHTransition::TTM_IMMIDIATE);
	pComboButton->AddItem(L"delay", CHTransition::TTM_DELAY);
	pComboButton->AddItem(L"exponential", CHTransition::TTM_EXPONENTIAL);
	pComboButton->AddItem(L"equal distr.", CHTransition::TTM_EQUAL_DISTR);
	pComboButton->SelectItem(0);
	pPanel->Add(pComboButton);

	pEditButton = new CMFCRibbonEdit(ID_TRANSITION_TIME_START, 70, L"\nt", 2);
	pEditButton->SetEditText(_T("1"));
	pEditButton->EnableSpinButtons(1, 1000);
	pPanel->Add(pEditButton);

	pEditButton = new CMFCRibbonEdit(ID_TRANSITION_TIME_RANGE, 70, L"\nt", 3);
	pEditButton->SetEditText(_T("1"));
	pEditButton->EnableSpinButtons(1, 1000);
	pPanel->Add(pEditButton);

	// Connector Panel
	pPanel = pCategory->AddPanel(L"Connector\nc", 0);

	pComboButton = new CMFCRibbonComboBox(ID_CONNECTOR_TYPE, FALSE, 74, L"\ncm");

	pComboButton->AddItem(L"normal", CHConnector::TypeNormal);
	pComboButton->AddItem(L"inhibitor", CHConnector::TypeInhibitor);
	pComboButton->AddItem(L"test", CHConnector::TypeTest);
	pComboButton->SelectItem(0);
	pPanel->Add(pComboButton);

	pEditButton = new CMFCRibbonEdit(ID_CONNECTOR_WEIGHT, 70, L"\ncw", 4);
	pEditButton->SetEditText(_T("1"));
	pEditButton->EnableSpinButtons(1, 1000);
	pPanel->Add(pEditButton);
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	__super::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}

#endif //_DEBUG

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust 
		= new CMFCToolBarsCustomizeDialog(this, TRUE);

	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	/*if (pMsg->message == WM_RBUTTONDOWN)
	{
		CWnd* pWnd = CWnd::FromHandlePermanent(pMsg->hwnd);
		CControlBar* pBar = DYNAMIC_DOWNCAST(CControlBar, pWnd);

		if (pBar != NULL)
		 {
			CMenu Menu;
			CPoint pt;

			pt.x = LOWORD(pMsg->lParam);
			pt.y = HIWORD(pMsg->lParam);
			pBar->ClientToScreen(&pt);

			if (Menu.LoadMenu(IDR_HPSTYPE))
			{
				CMenu* pSubMenu = Menu.GetSubMenu(2);

				if (pSubMenu!=NULL)
				{
					pSubMenu->TrackPopupMenu(TPM_LEFTALIGN
						|TPM_RIGHTBUTTON,pt.x,pt.y,this);
				}
			}
		}
	}*/
	return __super::PreTranslateMessage(pMsg);
}

LRESULT CMainFrame::OnSimThreadMessage(WPARAM wParam, LPARAM lParam)
{
	CPetriSimDoc* pDoc = reinterpret_cast<CPetriSimDoc*>(lParam);

	if (pDoc)
	{
		ASSERT_VALID(pDoc);
		pDoc->SimThreadMessage(wParam, lParam);
	}
	
	return 0L;
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent )
{

}

bool CMainFrame::SetValuesToRibbon(const CHPropertyValues& vals)
{
	bool bFind = false;

	for (INT_PTR loop=0; loop<vals.GetCount(); loop++)
	{
		const CHPropertyValue& val = vals[loop];

		if (val.IsEmty())
		{
			continue;
		}

		RibbonArray* pElements = 0;
		if (!m_mapRibbons.Lookup(val.Id, pElements))
		{
			pElements = new CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>;
			m_mapRibbons.SetAt(val.Id, pElements);

			m_wndRibbonBar.GetElementsByID(val.Id, *pElements);
		}

		for (INT_PTR loop=0; loop<pElements->GetSize(); loop++)
		{
			CMFCRibbonBaseElement* pelement = pElements->GetAt(loop);

			CMFCRibbonSlider* pSlider = DYNAMIC_DOWNCAST(CMFCRibbonSlider, pelement);
			if (pSlider)
			{
				pSlider->SetPos(val.GetLong());
				return true;
			}
	
			CMFCRibbonFontComboBox* pFontCombo = DYNAMIC_DOWNCAST(CMFCRibbonFontComboBox, pelement);
			if (pFontCombo)
			{
				if(pFontCombo->SelectItem(val.GetString()) == FALSE)
				{
					pFontCombo->SetEditText(val.GetString());
				}
				continue;
			}
	
			CMFCRibbonComboBox* pComboBox = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, pelement);
			if (pComboBox)
			{
				if (pComboBox->SelectItem(val.GetString()))
				{
					continue;
				}
	
				if (pComboBox->SelectItem(static_cast<DWORD_PTR>(val.GetLong())))
				{
					continue;
				}
	
				pComboBox->SelectItem(-1);
				
				continue;
			}

			CRibbonEdit* pEditBtn = DYNAMIC_DOWNCAST(CRibbonEdit, pelement);
			if (pEditBtn)
			{
				pEditBtn->SetEditText(val, false);
				bFind = true;
				continue;
			}
	
			CMFCRibbonEdit* pEditBtn2 = DYNAMIC_DOWNCAST(CMFCRibbonEdit, pelement);
			if (pEditBtn2)
			{
				pEditBtn2->SetEditText(val);
				bFind = true;
				continue;
			}
	
			CMFCRibbonColorButton* pColorButton = DYNAMIC_DOWNCAST(CMFCRibbonColorButton, pelement);
			if (pColorButton)
			{
				pColorButton->SetColor(val);
				bFind = true;
				continue;
			}
	
			CRibbonListButton* pListButton = DYNAMIC_DOWNCAST(CRibbonListButton, pelement);
			if (pListButton)
			{
				switch (val.Id)
				{
				case ID_OBJECT_LINEWEIGHT: pListButton->SelectItem(val.GetLong()-1); continue;			
				default: pListButton->SelectItem(val.GetLong()); continue;
				}
			}
	
			CMFCRibbonGallery* pGallery = DYNAMIC_DOWNCAST(CMFCRibbonGallery, pelement);
			if (pGallery)
			{
				switch (val.Id)
				{
				case ID_OBJECT_FILLSTYLE: pGallery->SelectItem(FillStyleToIndex(val)); continue;				
				default: pGallery->SelectItem(val.GetLong()); continue;
				}
			}
		}
	}
	return bFind;
}

bool  CMainFrame::GetValueFromRibbon(CHPropertyValue& val)
{
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> buttons;

	m_wndRibbonBar.GetElementsByID(val.Id, buttons);

	if (buttons.GetSize() == 0)
	{
		return false;
	}

	CMFCRibbonSlider* pSlider = DYNAMIC_DOWNCAST(CMFCRibbonSlider, buttons[0]);
	if (pSlider)
	{
		val = static_cast<long>(pSlider->GetPos());
		return true;
	}

	CMFCRibbonFontComboBox* pFontCombo = DYNAMIC_DOWNCAST(CMFCRibbonFontComboBox, buttons[0]);
	if(pFontCombo)
	{
		val = pFontCombo->GetEditText();
		return true;
	}

	CMFCRibbonComboBox* pComboBox = DYNAMIC_DOWNCAST(CMFCRibbonComboBox, buttons[0]);
	if (pComboBox)
	{
		int sel = pComboBox->GetCurSel();

		if (sel != -1)
		{
			DWORD_PTR data = pComboBox->GetItemData(sel);
			if (data)
			{
				val = static_cast<long>(data);
				return true;
			}
			val = pComboBox->GetItem(sel);
			return true;
		}
		return false;
	}

	CMFCRibbonEdit* pEditButton = DYNAMIC_DOWNCAST(CMFCRibbonEdit, buttons[0]);
	if (pEditButton)
	{
		val = pEditButton->GetEditText();
		return true;
	}

	CMFCRibbonColorButton* pColorButton = DYNAMIC_DOWNCAST(CMFCRibbonColorButton, buttons[0]);
	if (pColorButton)
	{
		COLORREF clr = pColorButton->GetColor();
		if (clr == (COLORREF)-1)
		{
			clr = pColorButton->GetAutomaticColor();
		}

		val = clr;
		return true;
	}

	CMFCRibbonGallery* pGalleryButton = DYNAMIC_DOWNCAST(CMFCRibbonGallery, buttons[0]);
	if (pGalleryButton)
	{
		long l = pGalleryButton->GetSelectedItem();

		if (l != -1)
		{
			switch (val.Id)
			{
			case ID_OBJECT_FILLSTYLE: val = IndexToFillStyle(l); return true;
			case ID_OBJECT_LINEWEIGHT: val = l+1; return true;
			case ID_OBJECT_LINESTYLE: val = l; return true;
			default: val = l; return true;
			}
		}
	}

	return false;
}

LRESULT CMainFrame::OnHighlightRibbonListItem( WPARAM wp, LPARAM lp )
{
	long index = (long) wp;

	CMFCRibbonBaseElement* pElem = (CMFCRibbonBaseElement*) lp;
	ASSERT_VALID(pElem);

	const int Id = pElem->GetID();

	CFrameWnd* pFrame = GetActiveFrame();
	if (pFrame == NULL)
	{
		return 0;
	}

	CPetriSimDoc* pDoc = (CPetriSimDoc*)pFrame->GetActiveDocument();
	if (pDoc == NULL)
	{
		return 0;
	}

	ASSERT_VALID(pDoc);

	CPetriSimView* pView = DYNAMIC_DOWNCAST(CPetriSimView, pFrame->GetActiveView());
	if (pView == NULL)
	{
		return 0;
	}

	if (index != -1)
	{
		pView->GetPreviewTool().StorePreviewState(Id);
	}
	else
	{
		pView->GetPreviewTool().RestorePreviewState();
		return 0;
	}

	switch (Id)
	{
	case ID_OBJECT_FILLCOLOR:
	case ID_OBJECT_LINECOLOR:
	case ID_OBJECT_FILLCOLOR2:
	case ID_OBJECT_FONT_COLOR:
		{
			COLORREF color = ((CMFCRibbonColorButton*) pElem)->GetHighlightedColor();
			pView->GetPreviewTool().SetPreview(CHPropertyValue(color, Id));
			CMFCPopupMenu::UpdateAllShadows();
		}
		break;
	case ID_OBJECT_LINEWEIGHT:
		{
			if (index != -1)
			{
				CHPropertyValue val(index+1, Id);
				pView->GetPreviewTool().SetPreview(val);
				CMFCPopupMenu::UpdateAllShadows();
			}
		}
		break;
	case ID_OBJECT_LINESTYLE:
		{
			if (index != -1)
			{
				CHPropertyValue val(index, Id);
				pView->GetPreviewTool().SetPreview(val);
				CMFCPopupMenu::UpdateAllShadows();
			}
		}
		break;
	case ID_OBJECT_FILLSTYLE:
		{
			if (index != -1)
			{
				CHPropertyValue val(IndexToFillStyle(index), Id);
				pView->GetPreviewTool().SetPreview(val);
				CMFCPopupMenu::UpdateAllShadows();
			}
		}
		break;
	case ID_OBJECT_FONT_NAME:
	case ID_OBJECT_FONT_SIZE:
		{
			if (index != -1)
			{
				CMFCRibbonComboBox* pCombo = static_cast<CMFCRibbonComboBox*>(pElem);

				CHPropertyValue val(pCombo->GetItem(index), Id);
				pView->GetPreviewTool().SetPreview(val);
				CMFCPopupMenu::UpdateAllShadows();
			}
		}
		break;
	}

	return 0;
}

void CMainFrame::HideAllContextCategories()
{
	if (m_wndRibbonBar.HideAllContextCategories())
	{
		m_wndRibbonBar.RecalcLayout();
		m_wndRibbonBar.RedrawWindow();

		SendMessage(WM_NCPAINT, 0, 0);
	}
}

void CMainFrame::ShowRibbonContextCategory(UINT Id)
{
	if (Id)
	{
		m_wndRibbonBar.ShowContextCategories(Id);
		m_wndRibbonBar.RecalcLayout();
		m_wndRibbonBar.RedrawWindow();

		SendMessage(WM_NCPAINT, 0, 0);
	}
	
	/*if (Id && m_wndRibbonBar.GetActiveCategory() && m_wndRibbonBar.GetActiveCategory()->GetContextID() == Id)
	{
		return;
	}

	BOOL bRecalc = m_wndRibbonBar.HideAllContextCategories();

	if (Id)
	{
		m_wndRibbonBar.ShowContextCategories(Id);
		bRecalc = TRUE;
	}

	if (bRecalc)
	{
		m_wndRibbonBar.RecalcLayout();
		m_wndRibbonBar.RedrawWindow();

		SendMessage(WM_NCPAINT, 0, 0);
	}*/
}

void CMainFrame::ActivateRibbonContextCategory(UINT Id)
{
	if (m_wndRibbonBar.GetHideFlags() == 0)
	{
		m_wndRibbonBar.ActivateContextCategory(Id);
	}
}

BOOL CMainFrame::OnShowMDITabContextMenu( CPoint point, DWORD dwAllowedItems, BOOL bDrop )
{
	CMenu menu;
	VERIFY(menu.LoadMenu(bDrop ? IDR_POPUP_DROP_MDITABS : IDR_POPUP_MDITABS));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	if (!pPopup)
	{
		return FALSE;
	}

	if ((dwAllowedItems & AFX_MDI_CREATE_HORZ_GROUP) == 0)
	{
		pPopup->DeleteMenu(ID_MDI_NEW_HORZ_TAB_GROUP, MF_BYCOMMAND);
	}

	if ((dwAllowedItems & AFX_MDI_CREATE_VERT_GROUP) == 0)
	{
		pPopup->DeleteMenu(ID_MDI_NEW_VERT_GROUP, MF_BYCOMMAND);
	}

	if ((dwAllowedItems & AFX_MDI_CAN_MOVE_NEXT) == 0)
	{
		pPopup->DeleteMenu(ID_MDI_MOVE_TO_NEXT_GROUP, MF_BYCOMMAND);
	}

	if ((dwAllowedItems & AFX_MDI_CAN_MOVE_PREV) == 0)
	{
		pPopup->DeleteMenu(ID_MDI_MOVE_TO_PREV_GROUP, MF_BYCOMMAND);
	}

	CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
	pPopupMenu->SetAutoDestroy(FALSE);
	pPopupMenu->Create(this, point.x, point.y, pPopup->GetSafeHmenu());

	return TRUE;
}

BOOL CMainFrame::OnShowPopupMenu( CMFCPopupMenu* pMenuPopup )
{
	BOOL bRes = CMDIFrameWndEx::OnShowPopupMenu(pMenuPopup);

	if (pMenuPopup != NULL && !pMenuPopup->IsCustomizePane())
	{
		//AdjustObjectSubmenu(pMenuPopup);
	}

	return bRes;
}

void CMainFrame::OnMdiMoveToNextGroup()
{
	MDITabMoveToNextGroup();
}

void CMainFrame::OnMdiMoveToPrevGroup()
{
	MDITabMoveToNextGroup(FALSE);
}

void CMainFrame::OnMdiNewHorzTabGroup()
{
	MDITabNewGroup(FALSE);
}

void CMainFrame::OnMdiNewVertGroup()
{
	MDITabNewGroup();
}

void CMainFrame::OnMdiCancel()
{

}

void CMainFrame::OnDummy()
{

}

void CMainFrame::OnOutputWindow()
{
	m_wndOutput.ShowPane(TRUE, FALSE, TRUE);
}

void CMainFrame::OnUpdateOutputWindow( CCmdUI* pCmdUI )
{
	//pCmdUI->SetCheck(m_wndOutput.IsDocked());
}

void CMainFrame::OnViewFullscreen()
{
	ShowFullScreen();
}