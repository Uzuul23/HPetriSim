/**************************************************************************
	PreviewTool.h

	copyright (c) 2013/07/16 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once

#include "HDrawObject.h"
#include "HPropertyValue.h"

class CPetriSimView;
class CPreviewTool
{
public:
	CPreviewTool(CPetriSimView * pView);
	~CPreviewTool(void);

	bool StorePreviewState( int Id );
	bool RestorePreviewState();
	bool SetPreview( CHPropertyValue& val );
	void Reset();

private:
	CPetriSimView* m_pView;
	CArray<CHPropertyValues> m_StorePreviewState;
};
