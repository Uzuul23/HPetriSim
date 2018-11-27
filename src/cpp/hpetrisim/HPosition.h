/**************************************************************************
	HPosition.h

	copyright (c) 2013/08/07 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once
#include "HNetMember.h"

class CHPosition : public CHNetMember
{
protected:
	DECLARE_SERIAL(CHPosition)
	CHPosition();

public:
	
	virtual ~CHPosition();
	CHPosition(const CPoint& point, ULONG32 NameHint);

	virtual void Serialize(CArchive& ar);

	virtual void DrawObject(Graphics & g, const CHDrawInfo& Info);
	virtual void DrawHandles(Graphics & g, const CHDrawHandle& Info);

	virtual bool SetProperties(CArray<CHPropertyValue>& list, bool bPreview);
	virtual bool GetProperties(CArray<CHPropertyValue>& list);

	virtual Rect GetBounds(bool bRedraw = false);
	virtual void SetBounds(const Rect& rect) {};

	virtual bool HitTest(const CRect& rect, int Hint = 0);
	virtual bool HitTest(const CPoint& point, int Hint = 0);

	virtual bool Reset();

	virtual bool GetLabelText(CString& text, long labenumber);
	virtual void RecalcLabelPosition();

	virtual void RecalcArcPoint(const Point& edge, Point& point);

	virtual bool IsPMember(){return true;};
	long GetToken(){return m_Tokens;};
	void SetToken(long tokens){m_Tokens = tokens;};
	long GetTokenCount(){return m_TokensCount;};
	void SetTokenCount(long tokens){m_TokensCount = tokens;};
	long GetTokenMax(){return m_TokensMax;};
	void SetTokenMax(long tokens){m_TokensMax = tokens;};

protected:
	
	ULONG32 m_Tokens;
	ULONG32 m_TokensMax;
	ULONG32 m_TokensStart;
	ULONG32 m_TokensCount;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};