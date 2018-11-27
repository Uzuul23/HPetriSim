/**************************************************************************
	IniSection.h
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#if !defined(AFX_INISECTION_H__1E42E872_0832_11D3_BAB0_0000B45E2D1E__INCLUDED_)
#define AFX_INISECTION_H__1E42E872_0832_11D3_BAB0_0000B45E2D1E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

const long SECTION_NAME_LENGHT = 80;

class CIniSection  
{
public:
	CIniSection();
	virtual ~CIniSection();

	bool FindSection(const TCHAR * section);
	CString& GetSection();
	bool SetSection(const TCHAR * section);
	bool WriteSection();
	void CloseFile();
	void Reset();
	bool ReadNextSection();
	CStdioFile& GetFile(){return m_ioFile;};
	bool OpenFile(const TCHAR * filename, bool write = false);	
	bool SetValue(const TCHAR * name, const TCHAR * value);
	bool SetValue(const TCHAR * name, const CPoint & point);
	bool SetValue(const TCHAR * name, const CRect & rect);
	bool SetValue(const TCHAR * name, const CSize & size);
	bool SetValue(const TCHAR * name, const COLORREF & color);
	bool SetValue(const TCHAR * name, const LOGFONT & font);
	bool SetValue(const TCHAR * name, const long & value);
	bool SetValue(const TCHAR * name, const int & value);
	bool SetValue(const TCHAR * name, const bool & value);
	bool SetValue(const TCHAR * name, const double & value);

#ifdef  UNICODE
	bool SetValue(const TCHAR * name, const CHAR * value);
#endif

	//
	bool GetValue(const TCHAR * key, CString& value);
	bool GetValue(const TCHAR * key, CPoint & point);
	bool GetValue(const TCHAR * key, CRect & rect);
	bool GetValue(const TCHAR * key, CSize & size);
	bool GetValue(const TCHAR * key, COLORREF & color);
	bool GetValue(const TCHAR * key, LOGFONT & font);
	bool GetValue(const TCHAR * key, long & value);
	bool GetValue(const TCHAR * key, int & value);
	bool GetValue(const TCHAR * key, bool & value);
	bool GetValue(const TCHAR * key, double & value);

private:
	TCHAR m_cbuffer[1024];
	bool WriteLine(const TCHAR * line);
	CString m_cSectionName;
	bool m_bFileOpenWrite;
	bool m_bFileOpenRead;
	CStdioFile m_ioFile;
	CMapStringToString m_MapSection;

};

#endif // !defined(AFX_INISECTION_H__1E42E872_0832_11D3_BAB0_0000B45E2D1E__INCLUDED_)
