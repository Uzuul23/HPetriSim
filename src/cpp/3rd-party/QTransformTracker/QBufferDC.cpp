#include "StdAfx.h"
#include "QBufferDC.h"

#ifdef _DEBUG
// #define DEBUG_QBUFFERDC	// Uncomment this if you want to see debugging info.
#endif

QBufferDC::QBufferDC(CDC * pDC, DWORD dwRopCode /* = SRCCOPY */)
: m_pDC(pDC)
, m_pOldBitmap(NULL)
, m_RopCode(dwRopCode)
{
	if (! pDC) return;
	ASSERT_VALID(pDC);

	if (pDC->IsPrinting())
	{
		// Do not use a bitmap when printing. There is nothing to gain.

		if (pDC->m_hDC != pDC->m_hAttribDC)		// Print Preview
		{
			// This is a hack. In debug mode, Print Preview yields an assert,
			// stating: "Cannot Release Output hDC on Attached CDC."
			// Although I have not found any nasty consequences, up till now,
			// simply setting the handles in stead of attaching seems to work.
			m_hDC = pDC->m_hDC;
			m_hAttribDC = pDC->m_hAttribDC;
		}
		else VERIFY(Attach(pDC->Detach())); 	// Attach this to the mother DC's handle,
												// so this in effect becomes a 'normal' DC.
		return;
	}

	// Get the clipping boundary of the mother DC, in logical coordinates
	CRect rcClip;
	VERIFY(pDC->GetClipBox(rcClip) != ERROR);

	// Transform to device coordinates (pixels), and normalize
	pDC->LPtoDP(rcClip);
	rcClip.NormalizeRect();

	if (m_BufferBitmap.ReserveBitmap(pDC, rcClip.Size()))
	{
		// Create a compatible DC
		VERIFY(CreateCompatibleDC(pDC));

		// Select the bitmap in it
		ASSERT(m_pOldBitmap == 0);
		m_pOldBitmap = (CBitmap *) SelectObject(& m_BufferBitmap.m_Bitmap);

		// Copy the mapping settings
		int mapmode = pDC->GetMapMode();
		SetMapMode(mapmode);
		SetWindowOrg(pDC->GetWindowOrg());
		SetViewportOrg(pDC->GetViewportOrg());

		if (mapmode > MM_MAX_FIXEDSCALE)
		{
			// These are only relevant to MM_ISOTROPIC and MM_ANISOTROPIC
			SetWindowExt(pDC->GetWindowExt());
			SetViewportExt(pDC->GetViewportExt());
		}

		// Fill the clipping boundary with pDC's background color
		COLORREF col = pDC->GetBkColor();

#ifdef QBUFFER_DEMO
		// In demo mode, change the color slightly so we can see which parts are updated
		if (m_bDemoMode) col ^= RGB(rand() % 32, rand() % 32, rand() % 32);
#endif

		// Get the mother DC's clipping boundary in logical coordinates
		// and fill it.
		VERIFY(pDC->GetClipBox(rcClip) != ERROR);

		rcClip.NormalizeRect();
		if (mapmode != MM_TEXT)
		{
			// Other mapping modes may lead to roundof errors, causing artefacts
			// on the screen. To compensate, we inflate the bounding rectangle
			// with two pixels.
			CSize szPixels(2, 2);
			DPtoLP(& szPixels);
			rcClip.InflateRect(szPixels.cx, szPixels.cy);
		}

		FillSolidRect(rcClip, col);

		// Initialize accumulation of boundary information
		SetBoundsRect(NULL, DCB_ENABLE | DCB_ACCUMULATE | DCB_RESET);
	}
	else	// We don't have a bitmap
	{
#ifdef DEBUG_QBUFFERDC
		afxDump << _T("We can't make a bitmap\n");
#endif
		// Attach this to the mother DC's handle, so this in effect becomes a 'normal' DC.
		VERIFY(Attach(pDC->Detach()));
	}
}

QBufferDC::~QBufferDC(void)
{
	if (!m_pDC) return;

	if (m_pDC->IsPrinting() && m_pDC->m_hDC != m_pDC->m_hAttribDC) return;
			// Nothing to do if Print Previewing. See remarks in constructor code.

	if (m_BufferBitmap.IsValid() && !m_pDC->IsPrinting())
									// We have a bitmap, and we are not printing.
	{
		// We only have to bitblt what is inside the accumulated bounding rectangle.
		CRect rcBounds;
		GetBoundsRect(rcBounds, DCB_RESET);
		rcBounds.NormalizeRect();

#ifdef DEBUG_QBUFFERDC
		afxDump << _T("Bounding rectangle: ") << rcBounds << _T("\n");
#endif

		// No point in bitblt'ing anything outside the clipping box
		CRect rcClip;
		m_pDC->GetClipBox(rcClip);
		rcClip.NormalizeRect();

		// So intersect it with the bounding rectangle.
		rcBounds &= rcClip;


#ifdef DEBUG_QBUFFERDC
		afxDump << _T("  after clipping: ") << rcBounds
			<< _T("\n  clipping rectangle: ") << rcClip << _T("\n");
#endif

		if (! rcBounds.IsRectEmpty())
		{
			if (GetMapMode() != MM_TEXT)
			{
				// Other mapping modes may lead to roundof errors, causing artefacts
				// on the screen. To compensate, we inflate the bounding rectangle
				// with two pixels.
				CSize szPixels(2, 2);
				DPtoLP(& szPixels);
				rcBounds.InflateRect(szPixels.cx, szPixels.cy);
			}

			// BitBlt the important part of the bitmap to the screen
			VERIFY(m_pDC->BitBlt(
				rcBounds.left, rcBounds.top,
				rcBounds.Width(), rcBounds.Height(),
				this,
				rcBounds.left, rcBounds.top,
				m_RopCode));
		}

		// Clean up, deselect the bitmap.
		if (m_pOldBitmap) SelectObject(m_pOldBitmap);
	}
	else	// We don't have a bitmap, or we are printing.
	{
		// Detach from the mother's DC handle, and reattach to the mother
		VERIFY(m_pDC->Attach(Detach()));
	}
}

BOOL QBufferDC::BufferBitmap::ReserveBitmap(CDC * pDC, CSize sz)
{
	if (IsValid())		// We have a bitmap
	{
		BITMAP bm;
		m_Bitmap.GetBitmap(& bm);

#ifdef DEBUG_QBUFFERDC
		afxDump << _T("We already have a bitmap, size: ") << CSize(bm.bmWidth, bm.bmHeight) << _T("\n");
#endif

		// Compare the bitmap size (in pixels) with the requested size (also in pixels)
		if (sz.cx > bm.bmWidth || sz.cy > bm.bmHeight)
		{
			// If the bitmap is too small, delete it; handle will be set to zero
			m_Bitmap.DeleteObject();
#ifdef DEBUG_QBUFFERDC
			afxDump << _T("Too small, deleted\n");
#endif
		}
		else return TRUE;	// Bitmap is big enough
	}

	// Try to create a bitmap of sufficient size
	BOOL r = m_Bitmap.CreateCompatibleBitmap(pDC, sz.cx, sz.cy);

#ifdef DEBUG_QBUFFERDC
	afxDump << _T("Tried to create a bitmap, size: ") << sz
		<< (r ? _T(" - succeeded\n") : _T(" - failed\n"));
#endif
	return r;
}

QBufferDC::BufferBitmap QBufferDC::m_BufferBitmap;

#ifdef QBUFFER_DEMO
BOOL QBufferDC::m_bDemoMode = TRUE;
#endif
