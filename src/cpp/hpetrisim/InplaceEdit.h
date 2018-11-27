/**************************************************************************
	InplaceEdit.h

	copyright (c) 2013/08/13 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once


class CHDrawObject;
class CPetriSimView;
class CInplaceEdit : public CEdit
{
	DECLARE_DYNAMIC(CInplaceEdit)

public:
	CInplaceEdit(CPetriSimView* pView);
	virtual ~CInplaceEdit();
	BOOL Create();

private:
	CPetriSimView* m_pView;
	CFont m_cfont;
	int m_Margin;
	bool m_TextAutoSize;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEnChange();
};


