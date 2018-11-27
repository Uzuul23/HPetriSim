/**************************************************************************
	PetriSim.h
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#if !defined(AFX_H_PETRI_SYM_H__1DD387A5_E907_11D1_94E3_9187B43D300C__INCLUDED_)
#define AFX_H_PETRI_SYM_H__1DD387A5_E907_11D1_94E3_9187B43D300C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h" 

class CPetriSimApp : public CWinAppEx
{
public:
	CPetriSimApp();
	CString m_strOwner, m_strSerial;
	BOOL m_bHiColorIcons;
	
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL InitInstance();

	afx_msg void OnAppAbout();

	DECLARE_MESSAGE_MAP()

};

extern CPetriSimApp theApp;

#endif // !defined(AFX_H_PETRI_SYM_H__1DD387A5_E907_11D1_94E3_9187B43D300C__INCLUDED_)
