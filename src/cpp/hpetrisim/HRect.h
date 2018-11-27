/**************************************************************************
	HRect.h

	copyright (c) 2013/07/06 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once
#include "HDrawObject.h"

class CHRect : public CHDrawObject  
{

protected:
	CHRect();
	DECLARE_SERIAL(CHRect)

public:
	CHRect(const CPoint & start, int type = Rectangle);
	virtual ~CHRect();

	virtual void ToGrid(int grid,int handle = 0);

	virtual void Serialize(CArchive& ar);

	virtual ULONG32 Id();
	virtual void Id(ULONG32 id);

	virtual void DrawObject(Graphics & g, const CHDrawInfo& Info);
	virtual void DrawHandles(Graphics & g, const CHDrawHandle& Info);

	virtual bool SetProperties(CArray<CHPropertyValue>& list, bool bPreview);
	virtual bool GetProperties(CArray<CHPropertyValue>& list);

	virtual void Resize(const CPoint& to, int handle);
	virtual void SetMinSize();

	virtual Rect GetBounds(bool bRedraw = false);
	virtual void SetBounds(const Rect& rect);

	virtual bool HitTest(const CRect& rect, int Hint = 0);
	virtual bool HitTest(const CPoint& point, int Hint = 0);
	virtual int HitTestHandle(const CPoint& point, int Hint = 0);
	virtual HCURSOR GetCursor(const CPoint& point, int Hint = 0);
	virtual bool GetHandle(Rect & rect, int Handle);

	virtual void StorePoints(CArray<Point>& fixpoints);
	virtual void TransformPoints(const CArray<Point>& fixpoints, const Gdiplus::Matrix& matrix);

	enum Types { Rectangle, Rounded, Ellipse };

protected:
	ULONG32 m_id;
	bool m_NoBorder;
	Rect m_Bounds;
	REAL m_LineWeight;
	ULONG32 m_RectStyle;
	DashStyle m_DashStyle;
	FillStyles m_FillStyle;
	Color m_LineColor;
	Color m_FillColor;
	Color m_FillColor2;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};

inline ULONG32 CHRect::Id()
{
	return m_id;
}

inline void CHRect::Id(ULONG32 id)
{
	m_id = id;
}