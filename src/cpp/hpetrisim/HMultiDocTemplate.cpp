/**************************************************************************
	HMultiDocTemplate.cpp

	copyright (c) 2013/08/31 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#include "stdafx.h"
#include "HMultiDocTemplate.h"
#include "MainFrm.h"
#include "PetriSimDoc.h"

CHMultiDocTemplate::CHMultiDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
: CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
}

CHMultiDocTemplate::~CHMultiDocTemplate()
{
}

void CHMultiDocTemplate::AddDocument( CDocument* pDoc )
{

	CMainFrame* pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CPetriSimDoc* pDoc2 = DYNAMIC_DOWNCAST(CPetriSimDoc, pDoc);

	
	static_cast<CPetriSimDoc*>(pDoc)->OutputWnd(static_cast<CMainFrame*>(AfxGetMainWnd())->GetOutputWnd().Add(pDoc2->GetTitle()));

	__super::AddDocument(pDoc);
}

void CHMultiDocTemplate::RemoveDocument( CDocument* pDoc )
{
	static_cast<CMainFrame*>(AfxGetMainWnd())->GetOutputWnd().Remove(static_cast<CPetriSimDoc*>(pDoc)->OutputWnd());

	__super::RemoveDocument(pDoc);
}