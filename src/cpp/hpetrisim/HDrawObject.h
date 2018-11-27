/**************************************************************************
	HDrawObject.h
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#pragma once

#include "GDI+Helper.h"

class CIniSection;
class CHDrawHandle;
class CHPropertyValue;
class CHDrawInfo;
class CHLabelMember;
class CHDrawObject : public CObject
{

protected:
	DECLARE_DYNAMIC(CHDrawObject)
	CHDrawObject();

public:
	virtual ~CHDrawObject();

	virtual ULONG32 Id() { return 0; };
	virtual void Id(ULONG32 id) {};

	virtual void DrawObject(Graphics & g, const CHDrawInfo& Info) {}
	virtual void DrawHandles(Graphics & g, const CHDrawHandle& Info) {}

	virtual bool GetProperties(CArray<CHPropertyValue>& list){ return false; }
	virtual bool SetProperties(CArray<CHPropertyValue>& list, bool bPreview){ return false; }
	virtual CHLabelMember* LabelMember() { return 0; }

	virtual Rect GetBounds(bool bRedraw = false) = 0;
	virtual void SetBounds(const Rect& rect) = 0;
	virtual void SetMinSize() {};

	virtual void Resize(const CPoint& to, int handle) {}
	virtual void Move(const CPoint& to, int Hint = 0)   {}

	virtual bool HitTest(const CRect& rect, int Hint = 0) { return false; }
	virtual bool HitTest(const CPoint& point, int Hint = 0) { return false; }
	virtual int HitTestHandle(const CPoint& point, int Hint = 0) { return 0; }
	virtual HCURSOR GetCursor(const CPoint& point, int Hint = 0) { return 0; }
	virtual bool GetHandle(Rect & rect, int Handle) { return false; }

	virtual void StorePoints(CArray<Point>& fixpoints) {}
	virtual void TransformPoints(const CArray<Point>& fixpoints, const Gdiplus::Matrix& matrix) {}

	virtual bool AddPoint(int& handle, const CPoint& point) { return false; }
	virtual bool RemovePoint(int handle) { return false; }

	virtual bool Reset() { return false; }

	bool Invalid() const { return m_Invalid; }
	void Invalid(bool val) { m_Invalid = val; }

	enum FillStyles 
	{ 
		NoFill = -2,
		FillStyleSolid = -1,
		HatchStyleFirst = Gdiplus::HatchStyleMin,
		HatchStyleLast = Gdiplus::HatchStyleMax,
		LinearGradientFirst = 200,
		LinearGradient_1 = LinearGradientFirst,
		LinearGradient_2,
		LinearGradient_3,
		LinearGradient_4,
		LinearGradient_5,
		LinearGradient_6,
		LinearGradientLast = LinearGradient_6,
		FillStylesTotal = (HatchStyleLast-HatchStyleFirst+1)+(LinearGradientLast-LinearGradientFirst+1)+2
	};

protected:
	bool m_Invalid;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
};

typedef CTypedPtrList<CObList,CHDrawObject*> CHDrawObjectList;
typedef CArray<CHDrawObject*, CHDrawObject*> CHDrawObjectArray;
typedef CMap<CHDrawObject*, CHDrawObject*, CHDrawObject*, CHDrawObject*&> CHDrawObjectMap;

inline void CopyList(CHDrawObjectList& to, const CHDrawObjectList& from)
{
	to.RemoveAll();
	POSITION pos = from.GetHeadPosition();
	while (pos)
	{
		to.AddTail(from.GetNext(pos));
	}
}

inline CArchive& operator>>(CArchive& ar, CHDrawObject::FillStyles& d)
{
	DWORD h = 0;
	ar.Read(&h, sizeof(DWORD));
	d = static_cast<CHDrawObject::FillStyles>(h);
	return ar;
}

inline CArchive& operator<<(CArchive& ar, CHDrawObject::FillStyles& d)
{
	DWORD h = static_cast<DWORD>(d);
	ar.Write(&h, sizeof(DWORD));
	return ar;
}

extern Brush* NewBrush(const Color& color1, const Color& color2, const Rect& bounds, CHDrawObject::FillStyles fillstyle);


inline CHDrawObject::FillStyles IndexToFillStyle(long index)
{
	return (index < 55) ? static_cast<CHDrawObject::FillStyles>(index-2) : static_cast<CHDrawObject::FillStyles>(index+145);
}

inline long FillStyleToIndex(CHDrawObject::FillStyles fillstyle)
{
	return (fillstyle <= CHDrawObject::HatchStyleLast) ? static_cast<long>(fillstyle+2) : static_cast<long>(fillstyle-145); 
}

