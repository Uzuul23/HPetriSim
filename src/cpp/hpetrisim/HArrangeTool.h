/**************************************************************************
	HArrangeTool.h

	copyright (c) 2013/09/08 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once

class CPetriSimView;
class CHArrangeTool
{
public:
	CHArrangeTool(CPetriSimView* pView);
	virtual ~CHArrangeTool(void);

	void OnUpdateArrangeCommand( CCmdUI* pCmdUI );
	void OnArrangeCommand( UINT nID );

private:
	CPetriSimView* m_pView;
};
