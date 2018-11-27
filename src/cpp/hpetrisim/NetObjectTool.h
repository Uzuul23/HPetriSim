/**************************************************************************
	NetObjectTool.h

	copyright (c) 2013/08/08 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once
#include "Tracker.h"

class CPetriSimView;
class CHDrawObject;
class CNetObjectTool : public CTracker
{
public:
	CNetObjectTool(CPetriSimView * pWnd);
	virtual ~CNetObjectTool(void);

	void OnLButtonDown(UINT nFlags, const CPoint & point, CDC & dc);
	HCURSOR GetCursor(const CPoint& point);

private:
	virtual void OnUpdate(CDC * pDC, UINT nMode);
	CHDrawObject* m_pNewObject;
	CPoint m_lastpoint;

	static INT_PTR m_Instances;
	static HCURSOR m_hPositionCursor;
	static HCURSOR m_hTransitionCursor;
	static HCURSOR m_hConnectorCursor;
};
