/**************************************************************************
	ArchiveHelper.h

	copyright (c) 2013/07/07 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once

using namespace Gdiplus;

inline CArchive& operator>>(CArchive& ar, Color& color)
{
	ARGB argb;
	ar.Read(&argb, sizeof(ARGB));
	color.SetValue(argb);
	return ar;
}

inline CArchive& operator<<(CArchive& ar, const Color& color)
{
	ARGB argb = color.GetValue();
	ar.Write(&argb, sizeof(ARGB));
	return ar;
}

inline CArchive& operator>>(CArchive& ar,  DashStyle& style)
{
	ar.Read(&style, sizeof(DWORD));
	return ar;
}

inline CArchive& operator<<(CArchive& ar, const DashStyle& style)
{
	ar.Write(&style, sizeof(DWORD));
	return ar;
}

inline CArchive& operator>>(CArchive& ar, CArray<Gdiplus::Point> & points)
{
	INT_PTR count = ar.ReadCount();
	points.SetSize(count);
	ar.Read(points.GetData(), count * sizeof(Point));
	return ar;
}

inline CArchive& operator<<(CArchive& ar, const CArray<Gdiplus::Point>& points)
{
	INT_PTR count = points.GetCount();
	ar.WriteCount(count);
	ar.Write(points.GetData(), count * sizeof(Point));
	return ar;
}

inline CArchive& operator>>(CArchive& ar, Gdiplus::Point& point)
{
	ar.Read(&point, sizeof(Gdiplus::Point));
	return ar;
}

inline CArchive& operator<<(CArchive& ar, const Gdiplus::Point& point)
{
	ar.Write(&point, sizeof(Gdiplus::Point));
	return ar;
}

inline CArchive& operator>>(CArchive& ar, Gdiplus::Size& size)
{
	ar.Read(&size, sizeof(Gdiplus::Size));
	return ar;
}

inline CArchive& operator<<(CArchive& ar, const Gdiplus::Size& size)
{
	ar.Write(&size, sizeof(Gdiplus::Size));
	return ar;
}

inline CArchive& operator>>(CArchive& ar, Gdiplus::Rect& rc)
{
	ar.Read(&rc, sizeof(Gdiplus::Rect));
	return ar;
}

inline CArchive& operator<<(CArchive& ar, const Gdiplus::Rect& rc)
{
	ar.Write(&rc, sizeof(Gdiplus::Rect));
	return ar;
}

inline CArchive& operator>>(CArchive& ar, Gdiplus::FontStyle& style)
{
	BYTE by = 0;
	ar.Read(&by, sizeof(BYTE));
	style = static_cast<Gdiplus::FontStyle>(by);
	return ar;
}

inline CArchive& operator<<(CArchive& ar, const Gdiplus::FontStyle& style)
{
	BYTE by = static_cast<BYTE>(style);
	ar.Write(&by, sizeof(BYTE));
	return ar;
}

inline CArchive& operator>>(CArchive& ar, Gdiplus::StringAlignment& align)
{
	BYTE by = 0;
	ar.Read(&by, sizeof(BYTE));
	align = static_cast<Gdiplus::StringAlignment>(by);
	return ar;
}

inline CArchive& operator<<(CArchive& ar, const Gdiplus::StringAlignment& align)
{
	BYTE by = static_cast<BYTE>(align);
	ar.Write(&by, sizeof(BYTE));
	return ar;
}

inline CArchive& operator>>(CArchive& ar, GUID& guid)
{
	CString str;
	ar >> str;
	VERIFY(SUCCEEDED(CLSIDFromString(str.GetBuffer(), &guid)));

	return ar;
}

inline CArchive& operator<<(CArchive& ar, const GUID& guid)
{
	CString str;
	VERIFY(SUCCEEDED(StringFromGUID2(guid, str.GetBuffer(40), 40)));
	str.ReleaseBuffer();
	ar << str;

	return ar;
}
