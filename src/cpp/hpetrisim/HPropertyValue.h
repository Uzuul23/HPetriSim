/**************************************************************************
	HPropertyValue.h

	copyright (c) 2013/07/21 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once
#include "HDrawObject.h"
#include "HTransition.h"
#include "HConnector.h"

class CHPropertyValue
{
public:
	CHPropertyValue(int Id = -1) : Id(Id), Type(TypeEmpty), indet(false)
	{	
	}
	~CHPropertyValue()
	{

	}
	CHPropertyValue(const CHPropertyValue& d) 
	{
		*this = d;
	}
	const CHPropertyValue& operator= (const CHPropertyValue & d)
	{
		Type = d.Type;
		Id = d.Id;
		switch (Type)
		{
		case TypeBool: bVal = d.bVal; break;
		case TypeFloat: fVal = d.fVal; break;
		case TypeLong: lVal = d.lVal; break;
		case TypeColor: clrVal = d.clrVal; break;
		case TypeString: strVal = d.strVal; break;
		}
		return *this;
	}
	//COLORREF
	CHPropertyValue(COLORREF d, int Id = -1) : Id(Id), indet(false)
	{
		Type = TypeColor;
		clrVal.SetFromCOLORREF(d);
	}
	const COLORREF& operator= (const COLORREF & d)
	{
		clrVal.SetFromCOLORREF(d);
		Type = TypeColor;
		return d;
	}
	operator COLORREF() const
	{
		return clrVal.ToCOLORREF();
	}
	//Gdiplus::Color
	CHPropertyValue(const Gdiplus::Color& d, int Id = -1) : Id(Id), indet(false)
	{
		Type = TypeColor;
		clrVal = d;
	}
	const Color& operator= (const Color & d)
	{
		clrVal = d;
		Type = TypeColor;
		return d;
	}
	operator Color() const
	{
		return clrVal;
	}
	//bool
	CHPropertyValue(const bool& d, int Id = -1) : Id(Id), indet(false)
	{
		Type = TypeBool;
		bVal = d;
	}
	const bool& operator= (const bool& d)
	{
		if (Type == TypeBool && bVal != d)
		{
			indet = true;
		}

		Type = TypeBool;
		bVal = d;
		return d;
	}
	operator bool() const
	{
		return GetBool();
	}
	bool GetBool() const
	{
		switch (Type)
		{
		case TypeEmpty:
		case TypeBool: return bVal;
		case TypeFloat: return fVal != 0;
		case TypeLong:  return lVal != 0;
		case TypeColor:	return false;
		case TypeString: return strVal == L"true";
		default: return false;
		}
	}
	//float
	CHPropertyValue(float d, int Id = -1) : Id(Id), indet(false)
	{
		Type = TypeFloat;
		fVal = d;
	}
	const float& operator= (const float& d)
	{
		if (Type == TypeFloat && fVal != d)
		{
			indet = true;
		}

		Type = TypeFloat;
		fVal = d;
		return d;
	}
	operator float() const
	{
		return GetFloat();
	}
	float GetFloat(float minimum = 1E-37, float maximum = 1E+37) const
	{
		float f = 0.0f;

		switch (Type)
		{
		case TypeEmpty: break;
		case TypeBool: f = static_cast<float>(bVal); break;
		case TypeFloat: f = fVal; break;
		case TypeLong:  f = static_cast<float>(lVal); break;
		case TypeColor:	break;
		case TypeString: f = static_cast<float>(_wtof(strVal)); break;
		}

		return max(min(f, maximum), minimum);
	}
	//long
	CHPropertyValue(const long& d, int Id = -1) : Id(Id), indet(false)
	{
		Type = TypeLong;
		lVal = d;
	}
	const long& operator= (const long& d)
	{
		if (Type == TypeLong && lVal != d)
		{
			indet = true;
		}

		Type = TypeLong;
		lVal = d;
		return d;
	}
	operator long() const
	{
		return GetLong();
	}
	long GetLong(long minimum = LONG_MIN, long maximum = LONG_MAX) const
	{
		long l = 0;
		switch (Type)
		{
		case TypeEmpty: break;
		case TypeBool: l = static_cast<long>(bVal); break;
		case TypeFloat: l =  static_cast<long>(fVal); break; 
		case TypeLong:  l = lVal; break;
		case TypeColor:	break;
		case TypeString: l =  _wtol(strVal); break;
		}

		return max(min(l, maximum), minimum);
	}
	//CString
	CHPropertyValue(const CString& d, int Id = -1) : Id(Id), indet(false)
	{
		Type = TypeString;
		strVal = d;
	}
	const CString& operator= (const CString & d)
	{
		strVal = d;
		Type = TypeString;
		return d;
	}
	operator CString() const
	{
		return GetString();
	}
	CString GetString() const
	{
		switch (Type)
		{
		case TypeEmpty: return L"";
		case TypeBool: return bVal == true ? L"true" : L"false";
		case TypeFloat:
			{
				CString str;
				str.Format(L"%f", fVal);
				return str;
			}
		case TypeLong:
			{
				CString str;
				str.Format(L"%d", lVal);
				return str;
			}
		case TypeColor:	return L"";
		case TypeString: return strVal;
		default: return L"";
		}
	}
	//LPCTSTR
	CHPropertyValue(LPCTSTR d, int Id = -1) : Id(Id), indet(false)
	{
		Type = TypeString;
		strVal = d;
	}
	const LPCTSTR operator= (LPCTSTR d)
	{
		strVal = d;
		Type = TypeString;
		return d;
	}
	operator LPCTSTR() const
	{
		return GetString();
	}
	//ULONG32
	const ULONG32& operator= (const ULONG32 & d)
	{
		lVal = d;
		Type = TypeLong;
		return d;
	}
	operator ULONG32() const
	{
		if (Type == TypeString)
		{
			return _wtoi(strVal);
		}

		return lVal;
	}
	//FillStyles
	CHPropertyValue(const CHDrawObject::FillStyles& d, int Id = -1) : Id(Id), indet(false)
	{
		Type = TypeLong;
		lVal = d;
	}
	const CHDrawObject::FillStyles& operator= (const CHDrawObject::FillStyles& d)
	{
		if (Type == TypeLong && lVal != d)
		{
			indet = true;
		}

		Type = TypeLong;
		lVal = d;
		return d;
	}
	operator CHDrawObject::FillStyles() const
	{
		return static_cast<CHDrawObject::FillStyles>(GetLong());
	}
	// Gdiplus::DashStyle
	CHPropertyValue(const  Gdiplus::DashStyle& d, int Id = -1) : Id(Id), indet(false)
	{
		Type = TypeLong;
		lVal = d;
	}
	const Gdiplus::DashStyle& operator= (const  Gdiplus::DashStyle& d)
	{
		if (Type == TypeLong && lVal != d)
		{
			indet = true;
		}

		Type = TypeLong;
		lVal = d;
		return d;
	}
	operator Gdiplus::DashStyle() const
	{
		return static_cast<Gdiplus::DashStyle>(GetLong());
	}
	//TransitionTimeMode
	CHPropertyValue(const CHTransition::TransitionTimeMode& d, int Id = -1) : Id(Id), indet(false)
	{
		Type = TypeLong;
		lVal = d;
	}
	const CHTransition::TransitionTimeMode& operator= (const CHTransition::TransitionTimeMode& d)
	{
		if (Type == TypeLong && lVal != d)
		{
			indet = true;
		}

		Type = TypeLong;
		lVal = d;
		return d;
	}
	operator CHTransition::TransitionTimeMode() const
	{
		return static_cast<CHTransition::TransitionTimeMode>(GetLong());
	}
	//CHConnector::ConnectorType
	CHPropertyValue(const CHConnector::ConnectorType& d, int Id = -1) : Id(Id), indet(false)
	{
		Type = TypeLong;
		lVal = d;
	}
	const CHConnector::ConnectorType& operator= (const CHConnector::ConnectorType& d)
	{
		if (Type == TypeLong && lVal != d)
		{
			indet = true;
		}

		Type = TypeLong;
		lVal = d;
		return d;
	}
	operator CHConnector::ConnectorType() const
	{
		return static_cast<CHConnector::ConnectorType>(GetLong());
	}
	//helper
	bool IsEmty() const
	{
		return Type == TypeEmpty;
	}
	bool IsIndet() const //indeterminate
	{
		return indet;
	}
	void Toggle()
	{
		bool d = GetBool();
		Type = TypeBool;
		bVal = !d;
	}

	enum ValueType
	{
		TypeEmpty,
		TypeBool,
		TypeFloat,
		TypeLong,
		TypeColor,
		TypeString
	};
	ValueType Type;
	int Id;

	union
	{
		bool bVal;
		float fVal;
		long lVal;
	};

	Gdiplus::Color clrVal;
	CString strVal;

	bool indet;

};


typedef CArray<CHPropertyValue> CHPropertyValues;