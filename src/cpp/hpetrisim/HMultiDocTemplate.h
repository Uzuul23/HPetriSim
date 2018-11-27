/**************************************************************************
	HMultiDocTemplate.h

	copyright (c) 2013/08/31 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once



class CHMultiDocTemplate : public CMultiDocTemplate
{
public:
	CHMultiDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);
	virtual ~CHMultiDocTemplate();

	virtual void AddDocument(CDocument* pDoc);
	virtual void RemoveDocument(CDocument* pDoc);
};


