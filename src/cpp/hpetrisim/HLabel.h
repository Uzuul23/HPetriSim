/**************************************************************************
	HLabel.h

	copyright (c) 2013/08/08 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once
#include "HDrawObject.h"

class CHLabelMember;
class CHLabel : public CHDrawObject  
{
protected:
	DECLARE_SERIAL(CHLabel);
	CHLabel();

public:
	CHLabel(CHLabelMember* pOwner, long labelnumber, bool visible = true);

	virtual ~CHLabel();

	virtual void Serialize(CArchive& ar);

	virtual void DrawObject(Graphics & g, const CHDrawInfo& Info);
	virtual void DrawHandles(Graphics & g, const CHDrawHandle& Info);

	virtual Rect GetBounds(bool bRedraw = false);
	virtual void SetBounds(const Rect& rect);

	virtual bool HitTest(const CRect& rect, int Hint = 0);
	virtual bool HitTest(const CPoint& point, int Hint = 0);

	virtual void StorePoints(CArray<Point>& fixpoints);
	virtual void TransformPoints(const CArray<Point>& fixpoints, const Gdiplus::Matrix& matrix);

	bool Visible() const { return m_Visible; }
	void Visible(bool val) { m_Visible = val; }

	virtual ULONG32 Id();
	virtual void Id(ULONG32 id);

	void Owner( CHLabelMember* pOwner, bool cutting = false);
	void Position(const Point& point);

	ULONG32 OwnerId() const { return m_OwnerId; }
	void OwnerId(ULONG32 val) { m_OwnerId = val; }

	void UpdateBounds();

protected:

	ULONG32 m_id;
	bool m_Visible;
	Rect m_Bounds;
	CHLabelMember* m_pOwner;
	long m_LabelNumber;
	ULONG32 m_OwnerId;
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};

inline ULONG32 CHLabel::Id()
{
	return m_id;
}

inline void CHLabel::Id(ULONG32 id)
{
	m_id = id;
}

class CHLabelMember
{
public:
	CHLabelMember() 
	{ 
		ZeroMemory(&m_pLabels, sizeof(m_pLabels));
	}
	virtual ~CHLabelMember(){}

	static const INT_PTR labelmax = 2;

	virtual ULONG32 Id() = 0;
	virtual bool GetLabelText(CString& text, long labenumber) = 0;
	virtual void RecalcLabelPosition() = 0;

	void LabelOwnerId(ULONG32 Id)
	{
		for (INT_PTR loop=0; loop<labelmax; loop++)
		{
			if (m_pLabels[loop])
			{
				m_pLabels[loop]->OwnerId(Id);
			}
		}
	}

	void SetLabel(CHLabel* pLabel, long labenumber)
	{
		if (labenumber >= 0 && labenumber < labelmax)
		{
			m_pLabels[labenumber] =  pLabel; 
		}
	}

	CHLabel* GetLabel(long labenumber)
	{
		if (labenumber >= 0 && labenumber < labelmax)
		{
			return m_pLabels[labenumber]; 
		}
		return 0;
	}

	INT_PTR GetLabelCount() 
	{
		INT_PTR count = 0;

		for (INT_PTR loop=0; loop<labelmax; loop++)
		{
			if (m_pLabels[loop])
			{
				count++;
			}
			else
			{
				break;
			}
		}

		return count;
	}

	void UpdateLabelBounds()
	{
		for (INT_PTR loop=0; loop<labelmax; loop++)
		{
			if (m_pLabels[loop])
			{
				m_pLabels[loop]->UpdateBounds();
			}
		}
	}

	Rect GetLabelBounds(bool Redraw = true)
	{
		Rect rect;

		for (INT_PTR loop=0; loop<labelmax; loop++)
		{
			if (m_pLabels[loop])
			{
				UnionRect(rect, m_pLabels[loop]->GetBounds(Redraw));
			}
		}

		return rect;
	}

protected:
	CHLabel* m_pLabels[labelmax];
};