/**************************************************************************
	HText.h

	copyright (c) 2013/08/06 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once
#include "HRect.h"

class CHText : public CHRect  
{
protected:
	CHText();
	DECLARE_SERIAL(CHText)

public:
	CHText(const CPoint& topleft);
	virtual ~CHText();

	virtual void Serialize(CArchive& ar);

	virtual void DrawObject(Graphics & g, const CHDrawInfo& Info);
	virtual void DrawHandles(Graphics & g, const CHDrawHandle& Info);

	virtual bool SetProperties(CArray<CHPropertyValue>& list, bool bPreview);
	virtual bool GetProperties(CArray<CHPropertyValue>& list);

	virtual void Resize(const CPoint& to, int handle);
	virtual void SetBounds(const Rect& rect);
	virtual void SetMinSize();

	virtual bool HitTest(const CRect& rect, int Hint = 0);
	virtual bool HitTest(const CPoint& point, int Hint = 0);
	virtual bool GetHandle(Rect & rect, int Handle);

	virtual void TransformPoints(const CArray<Point>& fixpoints, const Gdiplus::Matrix& matrix);

protected:
	void DrawString(Graphics* pg, bool bMessure = false);

	CString m_FontName;
	FontStyle m_FontStyle;
	REAL m_FontSize;
	StringAlignment m_Alignment;
	StringAlignment m_LineAlignment;
	CString m_Text;
	Color m_FontColor;
	bool m_AutoSize; 

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};