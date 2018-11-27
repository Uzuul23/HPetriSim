/**************************************************************************
	HNetMember.h

	copyright (c) 2013/08/07 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once
#include "HDrawObject.h"
#include "HLabel.h"

typedef CList<CHConnector*, CHConnector*> ConnectorsList;
typedef CArray<CHConnector*> Connectors;

class CHNetMember : public CHDrawObject, public CHLabelMember
{
protected:
	DECLARE_SERIAL(CHNetMember)
	CHNetMember();

public:
	CHNetMember(const CPoint& point);
	virtual ~CHNetMember();

	virtual void Serialize(CArchive& ar);

	virtual CRect GetBoundingRect(CDC* pDC = NULL,bool mode = false){ return CRect(); }

	virtual ULONG32 Id() { return m_id; };
	virtual void Id(ULONG32 id);

	virtual bool SetProperties(CArray<CHPropertyValue>& list, bool bPreview);
	virtual bool GetProperties(CArray<CHPropertyValue>& list);
	virtual CHLabelMember* LabelMember() { return this; }

	virtual Rect GetBounds(bool bRedraw = false) { return Rect(); };
	virtual void SetBounds(const Rect& rect) {};

	virtual void StorePoints(CArray<Point>& fixpoints);
	virtual void TransformPoints(const CArray<Point>& fixpoints, const Gdiplus::Matrix& matrix);

	virtual bool GetLabelText(CString& text, long labenumber) { return false; };
	virtual void RecalcLabelPosition() {}

	virtual void RecalcArcPoint(const Point& edge, Point& point);

	Connectors& ConnectorsIn() { return m_ConnectorsIn; }
	Connectors& ConnectorsOut() { return m_ConnectorsOut; }

	const CString& Name() const { return m_Name; }
	void Name(const CString& val) { m_Name = val; }

	void AddIn(CHConnector* pConnector);
	void RemoveIn(CHConnector* pConnector);
	void AddOut(CHConnector* pConnector);
	void RemoveOut(CHConnector* pConnector);
	void RemoveArcs();
	void RecalcArcs();

	enum PT_SIZE 
	{
		PT_VERYSMALL = 15,
		PT_SMALL = 20,
		PT_NORMAL = 25,
		PT_LARGE = 35,
		PT_VERYLARGE = 50
	};

protected:
	
	Rect GetArcBounds();

	Connectors m_ConnectorsIn;
	Connectors m_ConnectorsOut;

	ULONG32 m_id;
	Point m_Position;
	ULONG32 m_Size;
	CString m_Name;
	
};


