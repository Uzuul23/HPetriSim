/**************************************************************************
	HConnector.h

	copyright (c) 2013/08/06 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once

#include "HPoly.h"
#include "HLabel.h"

class CHNetMember;
class CHPosition;
class CHConnector : public CHDrawObject, public CHLabelMember
{
protected:
	DECLARE_SERIAL(CHConnector);
	CHConnector();

public:
	CHConnector(const CPoint& start, ULONG32 NameHint);
	virtual ~CHConnector();

	enum ConnectorType
	{
		TypeNormal = 0,
		TypeInhibitor,
		TypeTest
	};

	virtual void Serialize(CArchive& ar);

	virtual void DrawObject(Graphics & g, const CHDrawInfo& Info);
	virtual void DrawHandles(Graphics & g, const CHDrawHandle& Info);

	virtual bool SetProperties(CArray<CHPropertyValue>& list, bool bPreview);
	virtual bool GetProperties(CArray<CHPropertyValue>& list);
	virtual CHLabelMember* LabelMember() { return this; }

	virtual void Resize(const CPoint& to, int handle);

	virtual Rect GetBounds(bool bRedraw = false);
	virtual void SetBounds(const Rect& rect) {};
	virtual void SetMinSize();

	virtual bool HitTest(const CRect& rect, int Hint = 0);
	virtual bool HitTest(const CPoint& point, int Hint = 0);
	virtual int HitTestHandle(const CPoint& point, int Hint = 0);
	virtual HCURSOR GetCursor(const CPoint& point, int Hint = 0);
	virtual bool GetHandle(Rect & rect, int Handle);

	virtual void StorePoints(CArray<Point>& fixpoints);
	virtual void TransformPoints(const CArray<Point>& fixpoints, const Gdiplus::Matrix& matrix);

	virtual bool AddPoint(int& handle, const CPoint& point);
	virtual bool RemovePoint(int handle);

	virtual bool GetLabelText(CString& text, long labenumber);
	virtual void RecalcLabelPosition();

	int FromHandle() { return 1; }
	int ToHandle() { return m_Points.GetSize()*2-1 ; }

	void StepTokenAnim(long Step);
	void StartTokenAnim();
	const Point& TokenLocation() { return m_TokenLocation; }

	//new
	virtual ULONG32 Id() { return m_Id; }
	virtual void Id(ULONG32 id);

	const CString& Name() const { return m_Name; }
	void Name(const CString& val) { m_Name = val; }

	ULONG32 FromId() const { return m_FromId; }
	void FromId(ULONG32 val) { m_FromId = val; }

	ULONG32 ToId() const { return m_ToId; }
	void ToId(ULONG32 val) { m_ToId = val; }

	void To(CHNetMember* pNetMember, bool cutting = false);
	CHNetMember* To() { return m_pTo; }
	
	void From(CHNetMember* pNetMember, bool cutting = false);
	CHNetMember* From() { return m_pFrom; }

	bool Connected() { return m_pTo != 0 && m_pFrom != 0; }

	CHPosition* FromPosition();
	CHPosition* ToPosition();

	CHTransition* FromTransition();
	CHTransition* ToTransition();

	ConnectorType Type() { return m_Type; }
	void Type(ConnectorType type){ m_Type = type; }

	long Weight(){ return m_Weight; }
	void Weigth(long weight){ m_Weight = weight; }

	void RecalcPoints();
	Rect GetArcBounds(CHNetMember* pNetMember);

protected:
	CString m_Name;
	ConnectorType m_Type;
	ULONG32 m_Id;
	ULONG32 m_FromId;
	ULONG32 m_ToId;
	ULONG32 m_Weight;
	CHNetMember* m_pFrom;
	CHNetMember* m_pTo;
	double m_doAnimLenghtLine;
	double m_AnimPieces;
	double m_AnimLenght;
	long m_AnimSteps;
	long m_AnimStepsMax;
	INT_PTR m_AnimIndex;
	Point m_TokenLocation;
	CArray<Point> m_Points;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};

typedef CArray<CHConnector*> ConnectorsArray;
typedef CList<CHConnector*, CHConnector*> ConnectorsList;