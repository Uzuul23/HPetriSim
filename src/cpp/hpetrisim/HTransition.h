/**************************************************************************
	HTransition.h

	copyright (c) 2013/08/07 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once
#include "HNetMember.h"

class CHTransition : public CHNetMember
{
protected:
	CHTransition();
	DECLARE_SERIAL(CHTransition)

public:
	CHTransition(const CPoint& point, ULONG32 NameHint);
	virtual ~CHTransition();

	enum TransitionTimeMode
	{
		TTM_IMMIDIATE,
		TTM_DELAY,
		TTM_EXPONENTIAL, 
		TTM_EQUAL_DISTR
	};

	virtual void Serialize(CArchive& ar);

	virtual void DrawObject(Graphics & g, const CHDrawInfo& Info);
	virtual void DrawHandles(Graphics & g, const CHDrawHandle& Info);

	virtual bool SetProperties(CArray<CHPropertyValue>& list, bool bPreview);
	virtual bool GetProperties(CArray<CHPropertyValue>& list);

	virtual Rect GetBounds(bool bRedraw = false);
	virtual void SetBounds(const Rect& rect) {};

	virtual bool HitTest(const CRect& rect, int Hint = 0);
	virtual bool HitTest(const CPoint& point, int Hint = 0);

	virtual bool GetLabelText(CString& text, long labenumber);
	virtual void RecalcLabelPosition();

	virtual void RecalcArcPoint(const Point& edge, Point& point);

	virtual bool Reset();

	TransitionTimeMode TimeMode() { return m_TimeMode; }
	void TimeMode(TransitionTimeMode val){ m_TimeMode = val; }

	void Enabled(bool val) { m_Enabled = val; }
	bool Enabled() { return m_Enabled; }

	void Activ(bool val) { m_Activ = val; }
	bool Activ() { return m_Activ; }
	
	long GetTokensCount(){return m_TokensCount;};
	void SetTokensCount(long count){m_TokensCount = count;};

	long GetRangeDelay(){return m_TimeRange;};
	long GetStartDelay(){return m_TimeStart;};

	long GetDelay(){return m_Time;};
	void SetDelay(UINT time){m_Time = time;};

	void SetRangeDelay(long time){m_TimeRange = time;};
	void SetStartDelay(long time){m_TimeStart = time;};

protected:
	TransitionTimeMode m_TimeMode;
	ULONG32 m_Time;
	ULONG32 m_TimeStart;
	ULONG32 m_TimeRange;
	ULONG32 m_TokensCount;
	bool m_Activ;
	bool m_Enabled;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};
