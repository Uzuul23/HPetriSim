// ==========================================================================
//                     Template Implementation : COXZoomView
// ==========================================================================

// Source file : OXZoomView.inl 

// Version: 9.3

// This software along with its related components, documentation and files ("The Libraries")
// is © 1994-2007 The Code Project (1612916 Ontario Limited) and use of The Libraries is
// governed by a software license agreement ("Agreement").  Copies of the Agreement are
// available at The Code Project (www.codeproject.com), as part of the package you downloaded
// to obtain this file, or directly from our office.  For a copy of the license governing
// this software, you may contact us at legalaffairs@codeproject.com, or by calling 416-849-8900.
			  

inline BOOL COXZoomView::GetAlignToBottom() const
{
	return( m_bAlignToBottom );
}

inline void COXZoomView::SetAlignToBottom( BOOL bAlignToBottom )
{
	m_bAlignToBottom = bAlignToBottom;
}

inline BOOL COXZoomView::IsCentered() const
{
	return( m_bCenter );
}

inline void COXZoomView::Center( BOOL bCenter )
{
	m_bCenter = bCenter;
}

inline CSize COXZoomView::GetTotalSize() const
{
	return m_totalLog;
}

inline COXZoomView::ZoomAlignment COXZoomView::GetZoomAlign()
{
	return( m_align );
}

inline int COXZoomView::GetZoomLevel() const
{
	return( m_nZoomLevel );
}


