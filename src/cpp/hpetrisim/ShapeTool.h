/**************************************************************************
	ShapeTool.h

	copyright (c) 2013/07/13 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once

class CPetriSimView;
class CShapeTool : public CTracker
{
public:
	CShapeTool(CPetriSimView * pWnd);
	~CShapeTool(void);

	void OnLButtonDown(UINT nFlags, const CPoint & point, CDC & dc);
	HCURSOR GetCursor(const CPoint& point);
	
private:
	virtual void OnUpdate(CDC * pDC, UINT nMode);
	CHDrawObject* m_pNewObject;

	static INT_PTR m_Instances;
	static HCURSOR m_hRectCursor;
	static HCURSOR m_hLineCursor;
	static HCURSOR m_hEllipseCursor;
	static HCURSOR m_hRoundedCursor;
	static HCURSOR m_hPolyCursor;
	static HCURSOR m_hTextCursor;

};
