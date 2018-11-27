/**************************************************************************
	PetriSim.cpp
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#include "stdafx.h"
#include "PetriSim.h"
#include "ChildFrm.h"
#include "PetriSimDoc.h"
#include "PetriSimView.h"
#include "MainFrm.h"
#include "Splash.h"
#include <direct.h>
#include "afxlinkctrl.h"
#include "HMultiDocTemplate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

QGdiPlus g_GdiPlus; //initialize GDI+

/*************************************************************************/
// CPetriSimApp
/*************************************************************************/

BEGIN_MESSAGE_MAP(CPetriSimApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CPetriSimApp::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()

CPetriSimApp::CPetriSimApp()
{
	m_bHiColorIcons = TRUE;
}

CPetriSimApp theApp;

BOOL CPetriSimApp::InitInstance()
{
	
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	__super::InitInstance();

	AfxEnableControlContainer();

	SetRegistryKey(IDS_REGISTRY);
	LoadStdProfileSettings();

	InitContextMenuManager();
	InitKeyboardManager();
	InitTooltipManager();

	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CHMultiDocTemplate(IDR_HPSTYPE, RUNTIME_CLASS(CPetriSimDoc), RUNTIME_CLASS(CChildFrame), RUNTIME_CLASS(CPetriSimView));
	//pDocTemplate->SetContainerInfo(IDR_DRAWCLTYPE_CNTR_IP);
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;

	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		return FALSE;
	}

	m_pMainWnd = pMainFrame;

	/*CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_HPSTYPE,
		RUNTIME_CLASS(CPetriSimDoc),
		RUNTIME_CLASS(CMainFrame), 
		RUNTIME_CLASS(CPetriSimView));

	AddDocTemplate(pDocTemplate);*/

	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}

BOOL CPetriSimApp::PreTranslateMessage(MSG* pMsg)
{
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;  

	return CWinAppEx::PreTranslateMessage(pMsg);
}

/*************************************************************************/
// CAboutDlg
/*************************************************************************/

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	CMFCLinkCtrl m_url;
	CMFCLinkCtrl m_lic_url;
	CMFCLinkCtrl m_mailto;
	CString m_strAuthor;
	CString m_strSoftware;
	CString m_strCopyright;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_AUTHOR, m_strAuthor);
	DDX_Text(pDX, IDC_STATIC_SOFTWARE, m_strSoftware);
	DDX_Text(pDX, IDC_STATIC_COPYRIGHT, m_strCopyright);
	DDX_Control(pDX, IDC_BUTTON_MAILTO, m_mailto);
	DDX_Control(pDX, IDC_BUTTON_URL, m_url);
	DDX_Control(pDX, IDC_BUTTON_LIC_URL, m_lic_url);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

void CPetriSimApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_strAuthor.LoadString(IDS_AUTHOR);
	m_strSoftware.LoadString(IDS_SOFTWARE);
	m_strCopyright.LoadString(IDS_COPYRIGHT);

	CString str;
	str.LoadString(IDS_MAILTO);
	m_mailto.SetURLPrefix(L"mailto:");
	m_mailto.SetWindowText(str);
	m_mailto.SetURL(str);
	m_mailto.SizeToContent();

	str.LoadString(IDS_URL);
	m_url.SetURLPrefix(L"http://");
	m_url.SetWindowText(str);
	m_url.SetURL(str);
	m_url.SizeToContent();

	str.LoadString(IDS_LIC_URL);
	m_lic_url.SetURLPrefix(L"http://");
	m_lic_url.SetWindowText(str);
	m_lic_url.SetURL(str);
	m_lic_url.SizeToContent();

	UpdateData(false);

	return TRUE;
}

