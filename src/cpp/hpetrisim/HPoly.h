/**************************************************************************
	HPoly.h

	copyright (c) 2013/08/06 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once
#include "HRect.h"

class CHRect;
class CHPoly : public CHRect  
{
protected:
	CHPoly();
	DECLARE_SERIAL(CHPoly)

public:

	CHPoly(const CPoint& start, bool Polygon = true);
	virtual ~CHPoly();

	virtual void Serialize(CArchive& ar);
	virtual void DrawObject(Graphics & g, const CHDrawInfo& Info);
	virtual void DrawHandles(Graphics & g, const CHDrawHandle& Info);

	virtual bool SetProperties(CArray<CHPropertyValue>& list, bool bPreview);
	virtual bool GetProperties(CArray<CHPropertyValue>& list);

	virtual void Resize(const CPoint& to, int handle);
	virtual Rect GetBounds(bool bRedraw = false);
	virtual void SetBounds(const Rect& rect);
	virtual void SetMinSize();;
	virtual bool HitTest(const CRect& rect, int Hint = 0);
	virtual bool HitTest(const CPoint& point, int Hint = 0);
	virtual int HitTestHandle(const CPoint& point, int Hint = 0);
	virtual HCURSOR GetCursor(const CPoint& point, int Hint = 0);
	virtual bool GetHandle(Rect & rect, int Handle);

	virtual void StorePoints(CArray<Point>& fixpoints);
	virtual void TransformPoints(const CArray<Point>& fixpoints, const Gdiplus::Matrix& matrix);

	virtual bool AddPoint(int& handle, const CPoint& point);
	virtual bool RemovePoint(int handle);

protected:
	void RecalcBounds();

	bool m_CloseFigure;
	CArray<Point> m_Points;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};
