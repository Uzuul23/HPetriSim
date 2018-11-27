#pragma once

// Add GDI+ support to MFC or WTL application.
//
// Include this file in StdAfx.h
//
// MFC: Add a QGdiPlus variable to your application class to start and stop GDI+.
// ATL: Create a QGdiPlus local variable in _tWinMain.
// Constructor starts, destructor stops.



// Ensure that GdiPlus header files work properly with MFC DEBUG_NEW and STL header files.
// Q317799: PRB: Microsoft Foundation Classes DEBUG_NEW Does Not Work with GDI+

#define iterator _iterator

#ifdef _DEBUG

static int nGdiPlusObjects = 0;

namespace Gdiplus
{
	namespace DllExports
	{
		#include <GdiplusMem.h>
	};

	#ifndef _GDIPLUSBASE_H
	#define _GDIPLUSBASE_H
	class GdiplusBase
	{
		public:
			void (operator delete)(void* in_pVoid)
			{
				nGdiPlusObjects--;
				DllExports::GdipFree(in_pVoid);
			}

			void* (operator new)(size_t in_size)
			{
				nGdiPlusObjects++;
				return DllExports::GdipAlloc(in_size);
			}

			void (operator delete[])(void* in_pVoid)
			{
				nGdiPlusObjects--;
				DllExports::GdipFree(in_pVoid);
			}

			void* (operator new[])(size_t in_size)
			{
				nGdiPlusObjects++;
				return DllExports::GdipAlloc(in_size);
			}

			void * (operator new)(size_t nSize, LPCSTR /*lpszFileName*/, int /*nLine*/)
			{
				nGdiPlusObjects++;
				return DllExports::GdipAlloc(nSize);
			}

			void operator delete(void* p, LPCSTR /*lpszFileName*/, int /*nLine*/)
			{
				nGdiPlusObjects--;
				DllExports::GdipFree(p);
			}

		};
	#endif // #ifndef _GDIPLUSBASE_H
}
#endif // #ifdef _DEBUG

#include <gdiplus.h>
#ifdef _MFC_VER
#include <afx.h>
#endif
#undef iterator

using namespace Gdiplus;
#pragma comment (lib, "Gdiplus.lib")

class QGdiPlus
{
public:
	QGdiPlus(): m_Token(0)	{ Gdiplus::GdiplusStartupInput input; Gdiplus::GdiplusStartup(& m_Token, & input, NULL); }
	~QGdiPlus()
	{
		Gdiplus::GdiplusShutdown(m_Token);
#ifdef _DEBUG
#ifdef _MFC_VER
		if (nGdiPlusObjects > 0)
			afxDump << _T(">>> GdiPlus Memory Leaks: ") << nGdiPlusObjects << _T(" objects! <<<\n");
		else if (nGdiPlusObjects < 0)
			afxDump << _T(">>> GdiPlus Multiple Deletes: ") << -nGdiPlusObjects << _T(" objects! <<<\n");
#endif
#ifdef _ATL_VER
		if (nGdiPlusObjects > 0)
			ATLTRACE2(_T(">>> GdiPlus Memory Leaks: %d objects! <<<\n"), nGdiPlusObjects);
		else if (nGdiPlusObjects < 0)
			ATLTRACE2(_T(">>> GdiPlus Multiple Deletes: %d objects! <<<\n"), -nGdiPlusObjects);
#endif
#endif
	}
private:
	// The token we get from GDI+
	ULONG_PTR m_Token;
};
