// IniSection.cpp: Implementierung der Klasse CIniSection.
// Written by Henryk Anschuetz (s0140382@rz.fhtw-berlin.de)          //
// Copyright (c) 1999                                                //
///////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IniSection.h"

#ifdef _DEBUG
#undef THIS_FILE
static CHAR THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CIniSection::CIniSection() : m_MapSection(20)
{
	m_bFileOpenWrite = false;
	m_bFileOpenRead = false;
}

CIniSection::~CIniSection()
{

}

bool CIniSection::OpenFile(const TCHAR * filename,
						   bool write/* = false*/)
{
	if(write)
	{
		if(m_ioFile.Open(filename, 
			CFile::modeCreate|CFile::modeWrite))
		{
			m_bFileOpenWrite = true;
			return true;
		}
	}
	else
	{
		if(m_ioFile.Open(filename, CFile::modeRead))
		{
			m_bFileOpenRead = true;
			return true;
		}
	}
	return false;
}

bool CIniSection::FindSection(const TCHAR * section)
{
	if(m_bFileOpenRead)
	{
		CString value;
		Reset();
		bool sectionfound = false;
		long oldpos = 0;
		long startpos = static_cast<long>(m_ioFile.GetPosition());
		m_ioFile.SeekToBegin();
		while(m_ioFile.ReadString(m_cbuffer, 1023))
		{
			CString value(m_cbuffer);
			value.TrimRight();
			value.TrimLeft();
			if (value.GetLength() > 2 && value.Left(1) == '[' &&
				value.Right(1) == ']')
			{
				if(!sectionfound)
				{
					value = value.Mid(1, value.GetLength() - 2);
					if(value == section)
					{
						m_cSectionName = value;
						sectionfound = true;
					}
					continue;
				}
				else
				{
					m_ioFile.Seek(oldpos, CFile::begin);
					return true;
				}
			}
			if(sectionfound)
			{
				int token = value.Find('=');
				if(token == -1)
					return false;
				m_MapSection.SetAt(value.Left(token),
					value.Mid(token + 1));
			}
			oldpos = static_cast<long>(m_ioFile.GetPosition()); 
		}
		m_ioFile.Seek(startpos, CFile::begin);
		if(sectionfound)
			return true;
		else return false;
	}
	return false;
}

bool CIniSection::ReadNextSection()
{
	if(m_bFileOpenRead)
	{
		CString value;
		Reset();
		bool sectionfound = false;
		long oldpos = 0;
		while(m_ioFile.ReadString(m_cbuffer, 1023))
		{
			CString value(m_cbuffer);
			value.TrimRight();
			value.TrimLeft();
			if (value.GetLength() > 2 && value.Left(1) == '[' &&
				value.Right(1) == ']')
			{
				if(!sectionfound)
				{
					sectionfound = true;
					m_cSectionName = value;
					continue;
				}
				else
				{
					m_ioFile.Seek(oldpos, CFile::begin);
					return true;
				}
			}
			if(sectionfound)
			{
				int token = value.Find('=');
				if(token == -1)
					return false;
				m_MapSection.SetAt(value.Left(token),
					value.Mid(token + 1));
			}
			oldpos = static_cast<long>(m_ioFile.GetPosition()); 
		}
		if(sectionfound)
			return true;
		else return false;
	}
	return false;
}

void CIniSection::Reset()
{
	m_MapSection.RemoveAll();
}

void CIniSection::CloseFile()
{
	if(m_bFileOpenWrite || m_bFileOpenRead)
	{
		m_ioFile.Close();
		m_bFileOpenWrite = false;
		m_bFileOpenRead = false;
	}
}

bool CIniSection::WriteSection()
{
	CString buffer;
	CString key;
	CString value;
	buffer.Format(_T("[%s]"), m_cSectionName);
	if(!WriteLine(buffer))
			return false;
	POSITION pos = m_MapSection.GetStartPosition();
	while(pos)
	{
		m_MapSection.GetNextAssoc(pos, key, value);
		buffer.Format(_T("%s=%s"), key, value);
		if(!WriteLine(buffer))
			return false;
	}
	return true;
}

bool CIniSection::WriteLine(const TCHAR * line)
{
	if(m_bFileOpenWrite)
	{
		TRY
		{
			m_ioFile.WriteString(line);
			m_ioFile.WriteString(_T("\n"));
		}
		CATCH(CFileException, e)
		{
			m_ioFile.Abort();
			m_bFileOpenWrite = false;
			m_bFileOpenRead = false;
			return false;
		}
		END_CATCH
		return true;
	}
	return false;
}

bool CIniSection::SetSection(const TCHAR * section)
{
	Reset();
	if(section)
	{
		m_cSectionName = section;
		return true;
	}
	else return false;
}

CString& CIniSection::GetSection()
{
	return m_cSectionName;
}

bool CIniSection::SetValue(const TCHAR * name, const TCHAR * value)
{
	m_MapSection.SetAt(name, value);
	return true;
}

#ifdef  UNICODE
bool  CIniSection::SetValue(const TCHAR * name, const CHAR * value)
{
	size_t cb = 0;
	HRESULT hr = StringCbLengthA(value, 1024, &cb);
	if (FAILED(hr))
	{
		return false;
	}

	WCHAR* psz = new WCHAR[cb]; 

	if (MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, value, cb, psz, cb))
	{
		SetValue(name, psz);
		delete psz;
		return true;
	}

	delete psz;
	return false;
}
#endif

bool CIniSection::SetValue(const TCHAR * name, const CPoint & point)
{
	StringCbPrintf(m_cbuffer, sizeof(m_cbuffer)
		, _T("POINT(%d,%d)"), point.x, point.y);

	m_MapSection.SetAt(name, m_cbuffer);
	return true;
}

bool CIniSection::SetValue(const TCHAR * name, const CRect & rect)
{
	StringCbPrintf(m_cbuffer, sizeof(m_cbuffer)
		, _T("RECT(%d,%d,%d,%d)"), rect.left, rect.top
		, rect.right, rect.bottom);

	m_MapSection.SetAt(name, m_cbuffer);
	return true;
}

bool CIniSection::SetValue(const TCHAR * name, const CSize & size)
{
	StringCbPrintf(m_cbuffer, sizeof(m_cbuffer)
		, _T("SIZE(%d,%d)"), size.cx, size.cy);

	m_MapSection.SetAt(name, m_cbuffer);
	return true;
}

bool CIniSection::SetValue(const TCHAR * name, const COLORREF & color)
{
	StringCbPrintf(m_cbuffer, sizeof(m_cbuffer), _T("COLORREF(%d)")
		, (long)color);

	m_MapSection.SetAt(name, m_cbuffer);
	return true;
}

bool CIniSection::SetValue(const TCHAR * name, const LOGFONT & font)
{
	StringCbPrintf(
		m_cbuffer, sizeof(m_cbuffer)
		, _T("LOGFONT(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s)")
		, font.lfHeight,font.lfWidth, font.lfEscapement, 
		font.lfOrientation, font.lfWeight, font.lfItalic, 
		font.lfUnderline, font.lfStrikeOut, font.lfCharSet, 
		font.lfOutPrecision, font.lfClipPrecision, font.lfQuality, 
		font.lfPitchAndFamily, font.lfFaceName);
	m_MapSection.SetAt(name, m_cbuffer);
	return true;
}

bool CIniSection::SetValue(const TCHAR * name, const int & value)
{
	StringCbPrintf(m_cbuffer, sizeof(m_cbuffer), _T("%d"), value);

	m_MapSection.SetAt(name, m_cbuffer);
	return true;
}

bool CIniSection::SetValue(const TCHAR * name, const bool & value)
{
	if(value)
		m_MapSection.SetAt(name, _T("TRUE"));
	else
		m_MapSection.SetAt(name, _T("FALSE"));
	return true;
}

bool CIniSection::SetValue(const TCHAR * name, const long & value)
{
	StringCbPrintf(m_cbuffer, sizeof(m_cbuffer), _T("%d"), value);

	m_MapSection.SetAt(name, m_cbuffer);
	return true;
}

bool CIniSection::SetValue(const TCHAR * name, const double & value)
{
	StringCbPrintf(m_cbuffer, sizeof(m_cbuffer), _T("%f"), value);

	m_MapSection.SetAt(name, m_cbuffer);
	return true;
}

bool CIniSection::GetValue(const TCHAR * key, CString & value)
{
	return (m_MapSection.Lookup(key, value) != 0)? true : false; 
}

bool CIniSection::GetValue(const TCHAR * key, CPoint & point)
{
	CString cvalue;
	if(m_MapSection.Lookup(key, cvalue) != 0)
	{
		long x,y;
		long convert =   _stscanf_s(cvalue, _T("POINT(%d,%d)"), &x, &y);
		if(convert != 2)return false;
		point.x = x;
		point.y = y;
		return true;
	}
	return false;
}

bool CIniSection::GetValue(const TCHAR * key, CRect & rect)
{
	CString cvalue;
	if(m_MapSection.Lookup(key, cvalue) != 0)
	{
		long left, top, right, bottom;
		long convert = _stscanf_s(cvalue, _T("RECT(%d,%d,%d,%d)"),
			&left, &top, &right, &bottom);
		if(convert != 4)return false;
		rect.left = left;
		rect.top = top;
		rect.right = right;
		rect.bottom = bottom;
		return true;
	}
	return false;
}

bool CIniSection::GetValue(const TCHAR * key, CSize & size)
{
	CString cvalue;
	if(m_MapSection.Lookup(key, cvalue) != 0)
	{
		long cx, cy;
		long convert = _stscanf_s(cvalue, _T("SIZE(%d,%d)"),
			&cx, &cy);
		if(convert != 2)return false;
		size.cx = cx;
		size.cy = cy;
		return true;
	}
	return false;
}

bool CIniSection::GetValue(const TCHAR * key, COLORREF & color)
{
	CString cvalue;
	if(m_MapSection.Lookup(key, cvalue) != 0)
	{
		long x;
		long convert = _stscanf_s(cvalue, _T("COLORREF(%d)"), &x);
		if(convert != 1)return false;
		color = (COLORREF)x;
		return true;
	}
	return false;
}

bool CIniSection::GetValue(const TCHAR * key, LOGFONT & font)
{
	CString cvalue;
	LOGFONT lfFont;
	if(m_MapSection.Lookup(key, cvalue) != 0)
	{
	long convert = _stscanf_s(
		cvalue,_T("LOGFONT(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s)"),
		&lfFont.lfHeight,&lfFont.lfWidth, &lfFont.lfEscapement, 
		&lfFont.lfOrientation, &lfFont.lfWeight, &lfFont.lfItalic, 
		&lfFont.lfUnderline, &lfFont.lfStrikeOut, &lfFont.lfCharSet, 
		&lfFont.lfOutPrecision, &lfFont.lfClipPrecision, &lfFont.lfQuality, 
		&lfFont.lfPitchAndFamily, &lfFont.lfFaceName);
		if(convert != 14)return false;
		memcpy(&font, &lfFont, sizeof(font));
		return true;
	}
	return false;
}

bool CIniSection::GetValue(const TCHAR * key, long & value)
{
	CString cvalue;
	if(m_MapSection.Lookup(key, cvalue) != 0)
	{
		value = _tstol(cvalue);
		return true;
	}
	return false;
}

bool CIniSection::GetValue(const TCHAR * key, int & value)
{
	CString cvalue;
	if(m_MapSection.Lookup(key, cvalue) != 0)
	{
		value = _tstoi(cvalue);
		return true;
	}
	return false;
}

bool CIniSection::GetValue(const TCHAR * key, bool & value)
{
	CString cvalue;
	if(m_MapSection.Lookup(key, cvalue) != 0)
	{
		if(cvalue == _T("TRUE"))
		{
			value = true;
			return true;
		}
		if(cvalue == _T("FALSE"))
		{
			value = false;
			return true;
		}
	}
	return false;
}

bool CIniSection::GetValue(const TCHAR * key, double & value)
{
	CString cvalue;
	if(m_MapSection.Lookup(key, cvalue) != 0)
	{
		value = _tstof(cvalue);
		return true;
	}
	return false;
}
