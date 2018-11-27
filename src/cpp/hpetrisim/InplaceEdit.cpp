/**************************************************************************
	InplaceEdit.cpp

	copyright (c) 2013/08/13 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#include "stdafx.h"
#include "HDrawObject.h"
#include "PetriSimView.h"
#include "InplaceEdit.h"

IMPLEMENT_DYNAMIC(CInplaceEdit, CEdit)

CInplaceEdit::CInplaceEdit(CPetriSimView* pView) : m_pView(pView)
, m_Margin(1)
, m_TextAutoSize(true)
{
	ASSERT_VALID(pView);
}

CInplaceEdit::~CInplaceEdit()
{
}

BEGIN_MESSAGE_MAP(CInplaceEdit, CEdit)
	ON_WM_KILLFOCUS()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_CONTROL_REFLECT(EN_CHANGE, &CInplaceEdit::OnEnChange)
END_MESSAGE_MAP()


void CInplaceEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	DestroyWindow();
}

BOOL CInplaceEdit::Create()
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pView);

	CHDrawObjectList& list = m_pView->GetSelectTool().GetSelectedObjects();

	if (list.GetCount() != 1)
	{
		return FALSE;
	}

	CHDrawObject* pObject = list.GetHead();
	ASSERT_VALID(pObject);

	CHPropertyValues vals;
	vals.Add(CHPropertyValue(ID_OBJECT_TEXT));
	vals.Add(CHPropertyValue(ID_OBJECT_FONT_NAME));
	vals.Add(CHPropertyValue(ID_OBJECT_FONT_SIZE));
	vals.Add(CHPropertyValue(ID_OBJECT_FONT_BOLD));
	vals.Add(CHPropertyValue(ID_OBJECT_FONT_ITALIC));
	vals.Add(CHPropertyValue(ID_OBJECT_FONT_UNDERLINE));
	vals.Add(CHPropertyValue(ID_OBJECT_FONT_STRIKEOUT));
	vals.Add(CHPropertyValue(ID_OBJECT_TEXT_AUTOSIZE));

	if (!pObject->GetProperties(vals))
	{
		return FALSE;
	}

	m_Margin = vals[2].GetLong(1);

	CRect bounds = FromRect(pObject->GetBounds());
	m_pView->LPtoDP(&bounds);
	bounds.InflateRect(5, 5, m_Margin, m_Margin);

	if (!__super::Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_MULTILINE|ES_WANTRETURN, bounds, m_pView, 1))
	{
		return FALSE;
	}

	long style = 0;
	style |= vals[3].GetBool() ? 1 : 0;
	style |= vals[4].GetBool() ? 2 : 0;
	style |= vals[5].GetBool() ? 4 : 0;
	style |= vals[6].GetBool() ? 8 : 0;

	Gdiplus::Font font(vals[1], vals[2], static_cast<FontStyle>(style));

	LOGFONTW logfont;
	Graphics g(GetSafeHwnd());
	font.GetLogFontW(&g, &logfont);

	m_cfont.CreateFontIndirect(&logfont);

	m_TextAutoSize = vals[7];

	SetFont(&m_cfont);
	SetWindowText(vals[0]);
	SetFocus();

	return TRUE;
}

void CInplaceEdit::OnDestroy()
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pView);

	CString Text;
	GetWindowText(Text);

	CHPropertyValues vals;
	vals.Add(CHPropertyValue(Text, ID_OBJECT_TEXT));

	m_pView->GetSelectTool().SetSelectedProperty(vals[0]);

	m_cfont.DeleteObject();

	CEdit::OnDestroy();
}

void CInplaceEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);

	switch(nChar)
	{
	case VK_ESCAPE: DestroyWindow(); break;
	}
}

void CInplaceEdit::OnEnChange()
{
	if (!m_TextAutoSize)
	{
		return;
	}

	CClientDC dc(this);

	CString Text;
	GetWindowText(Text);

	dc.SelectObject(GetFont());

	CRect bounds;
	GetWindowRect(&bounds);
	dc.DrawText(Text, &bounds, DT_CALCRECT|DT_WORDBREAK);
	bounds.InflateRect(0, 0, m_Margin, m_Margin);
	
	SetWindowPos(0, 0, 0, bounds.Width(), bounds.Height(), SWP_NOMOVE|SWP_NOOWNERZORDER);
}
