/**************************************************************************
	HErrorFile.cpp
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#include "stdafx.h"
#include "HErrorFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static CHAR THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CCriticalSection CHErrorFile::m_cCSection;

CHErrorFile::CHErrorFile()
{

}

CHErrorFile::~CHErrorFile()
{

}

bool CHErrorFile::WriteError(const TCHAR * lpszErrorFunktion,
							 const TCHAR * lpszErrorText,
							CObject * rObject)
{
	CSingleLock sLock(&m_cCSection, true);

	TCHAR lpPathBuffer[MAX_PATH + 15];
	if(rObject && GetCurrentDirectory(MAX_PATH, lpPathBuffer))
	{
		StringCchCat(lpPathBuffer, sizeof(lpPathBuffer), _T("\\ERROR.log"));
		CStdioFile file;

		if(file.Open(lpPathBuffer, CFile::modeWrite))
		{
			file.SeekToEnd();
		}
		else if(file.Open(lpPathBuffer, 
			CFile::modeWrite|CFile::modeCreate));
		else
			return false;

		CTime ctime = CTime::GetCurrentTime();
		file.WriteString(ctime.Format(_T("%b-%d-%Y %H:%M:%S ")));
		// TODO: file.WriteString(rObject->GetRuntimeClass()->m_lpszClassName);
		file.WriteString(_T("::"));
		file.WriteString(lpszErrorFunktion);
		file.WriteString(_T("->cause:"));
		file.WriteString(lpszErrorText);
		file.WriteString(_T("\n"));
	}
	return false;
}