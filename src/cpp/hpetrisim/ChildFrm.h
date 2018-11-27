/**************************************************************************
	ChildFrm.h
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#pragma once

class CChildFrame : public CMDIChildWndEx
{
protected:
	DECLARE_DYNCREATE(CChildFrame)
	CChildFrame();

public:
	virtual ~CChildFrame();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CSplitterWnd m_wndSplitter;
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);


	DECLARE_MESSAGE_MAP()
};
