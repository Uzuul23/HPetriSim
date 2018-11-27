/**************************************************************************
	HSimToolbar.h
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#ifndef AFX_HSIMTOOLBAR_H__9BD83A52_F7CD_11D2_BA85_0000B45E2D1E__INCLUDED_
#define AFX_HSIMTOOLBAR_H__9BD83A52_F7CD_11D2_BA85_0000B45E2D1E__INCLUDED_

class CHSimToolbar : public CMFCToolBar
{

public:
	DECLARE_DYNAMIC(CHSimToolbar)
	CHSimToolbar();

public:
	CSliderCtrl m_cSliderSpeed;

public:
	CSliderCtrl* GetSliderSpeed()
	{
		ASSERT_VALID(this);
		return &m_cSliderSpeed;
	}

public:
	virtual ~CHSimToolbar();

protected:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	DECLARE_MESSAGE_MAP()
};

#endif // AFX_HSIMTOOLBAR_H__9BD83A52_F7CD_11D2_BA85_0000B45E2D1E__INCLUDED_
