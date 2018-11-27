/**************************************************************************
	HGroup.h

	copyright (c) 2013/07/06 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once
#include "HRect.h"

class CHGroup : public CHRect 
{

public:
	CHGroup();
	virtual ~CHGroup();

	//virtual bool GetProperty(GV_ITEM* pItem){return false;};
	//virtual bool SetProperty(GV_ITEM* pItem){return false;};
	virtual CString GetPropertyText(UINT par){return "NULL";};
	virtual LPCTSTR GetCursor(int handle);
	virtual void Serialize(CArchive& ar);
	virtual void DrawObject(CDC* pDC);
	virtual void DoID(UINT nID);
	virtual void InitMenu(CMenu* menu,CPoint & point);
	virtual void MoveTo(POINT & to);
	virtual void Resize(POINT & to,int handle);
	virtual bool PtInObject(POINT & point);
	virtual int PtInGripper(POINT& point);
	virtual CRect GetBoundingRect(CDC* pDC = NULL,bool mode = false);
	void Add(CHDrawObject * object);
	CHDrawObject* Remove();


protected:
	DECLARE_SERIAL(CHGroup)

protected:
	CRect GetHandle(int nHandle);
	CTypedPtrList<CObList, CHDrawObject*> m_colMembers;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};