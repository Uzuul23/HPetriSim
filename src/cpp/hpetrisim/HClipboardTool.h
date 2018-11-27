/**************************************************************************
	HClipboardTool.h

	copyright (c) 2013/08/17 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once

class CPetriSimView;
class CHClipboardTool
{
public:
	CHClipboardTool(CPetriSimView* pView);
	virtual ~CHClipboardTool(void);

	BOOL CanCopyCut();
	BOOL CanPaste();
	void Copy();
	void Paste();
	void RenderFormat(UINT nFormat);
	void RenderAllFormats();
	
private:
	void Serialize(CArchive& ar);

	CPetriSimView* m_pView;
	static INT_PTR m_Instances;
	static UINT m_nClipboardFormat;
	static CPetriSimDoc* m_pSourceDoc;
};
