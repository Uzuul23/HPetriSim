/**************************************************************************
	HErrorFile.h
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#if !defined(AFX_HERRORFILE_H__401DCBE1_0E9C_11D3_BAC0_0000B45E2D1E__INCLUDED_)
#define AFX_HERRORFILE_H__401DCBE1_0E9C_11D3_BAC0_0000B45E2D1E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CHErrorFile  
{
private:
	CHErrorFile();
	virtual ~CHErrorFile();
public:
	static bool WriteError(const TCHAR * lpszErrorFunktion,
		const TCHAR * lpszErrorText,
		CObject * rObject);
private:
	static CCriticalSection m_cCSection;

};

#endif // !defined(AFX_HERRORFILE_H__401DCBE1_0E9C_11D3_BAC0_0000B45E2D1E__INCLUDED_)
