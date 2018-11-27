#pragma once



// CZoomView

class CZoomView : public COXZoomView
{
	DECLARE_DYNCREATE(CZoomView)

protected:
	CZoomView(); // create from serialization only
	virtual ~CZoomView();
	
	int GetZoomMode()
	{
		return m_ZoomMode;
	};
	typedef enum {MODE_ZOOMOFF, MODE_ZOOMIN, MODE_ZOOMOUT} ZOOM_MODE;

public:

	void DPtoLP(CPoint* pPoint, int count = 1);
	void DPtoLP(CRect* pRect);
	void LPtoDP(CPoint* pPoint, int count = 1);
	void LPtoDP(CRect* pRect);

	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);

	#ifdef _DEBUG
		virtual void AssertValid() const;
	#ifndef _WIN32_WCE
		virtual void Dump(CDumpContext& dc) const;
	#endif
	#endif

private:
	
	ZOOM_MODE m_ZoomMode;
	HCURSOR m_hZoomInCursor;
	HCURSOR m_hZoomOutCursor;
	
protected:
	virtual void OnDraw(CDC* pDC);
	virtual void OnInitialUpdate();
	void PersistRatio(const CSize& orig, CSize& dest, CPoint& remainder);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnZoomNorm();
	afx_msg void OnUpdateZoomNorm(CCmdUI *pCmdUI);
	afx_msg void OnZoomIn();
	afx_msg void OnUpdateZoomIn(CCmdUI *pCmdUI);
	afx_msg void OnZoomOut();
	afx_msg void OnUpdateZoomOut(CCmdUI *pCmdUI);
	afx_msg void OnZoomFull();
	afx_msg void OnUpdateZoomFull(CCmdUI *pCmdUI);
	DECLARE_MESSAGE_MAP()
	
};


