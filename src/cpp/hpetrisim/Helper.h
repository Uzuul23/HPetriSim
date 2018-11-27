/**************************************************************************
	Helper.h

	copyright (c) 2013/07/07 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once

inline LPCWSTR LoadStr(UINT uID)
{
	static CString str;
	BOOL bRet = str.LoadString(uID);
	if (bRet)
	{
		return str;
	}
	ASSERT(bRet);
	return L"???";
}