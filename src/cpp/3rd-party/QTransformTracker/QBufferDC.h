#pragma once

///////////////////////////////////////////////////
// QBufferDC
//
// A versatile MFC-class implementing flickerfree, buffered drawing.
// Derived from class CDC and supports all it's operations.
//
// However, all drawing is done to a memory bitmap in stead of to the screen.
// At destruction time, the resulting bitmap is blt'ed to the screen.
//
// Feature: it makes use of 'bounding accumulation' to determine which
// parts of the screen need updating.
//
// Compatible with all mapping modes.
// Compatible with GDI+.
//
// If memory is insufficient to create the buffer bitmap, QBufferDC
// behaves like a normal CDC, without buffered drawing.
//
// QBufferDC's constructor has two parameters:
// pDC			points to the mother device context. At destruction time,
//				QBufferDC copies the graphics output to this DC;
// dwRopCode	the 'ternary raster operation' used for bitblt'ing the
//				graphics to the screen. The default just copies; for advanced
//				uses, dwRopCode may have another value like SRCINVERT.
//
// QBufferDC clears the buffer bitmap to the background color of the mother DC.
//
// If QBUFFER_DEMO is defined, a special version of this class is compiled.
// It has one extra static member variable, m_bDemoMode. If m_bDemoMode is TRUE
// (default), the background color is slightly and randomly modified, showing
// which parts of the screen are updated.
// QBUFFER_DEMO might be entered as a 'Preprocessor definition' in the project settings.
// In normal use, QBUFFER_DEMO should not be defined.
////////////////////////////////////////////////////
//
// (c) Sjaak Priester, Amsterdam, 2003.
// www.sjaakpriester.nl
////////////////////////////////////////////////////


// Microsoft defined for 17 of the 255 possible 'ternary raster operations' a mnemonic
// name like SRCCOPY or PATINVERT. They really should have defined this one too,
// because it goes well with the R2_NOTXORPEN secondary raster operation, which
// comes in handy for rubber banding applications. It might be used as dwRopCode
// in constructing a QBufferDC.
#ifndef NOTSRCINVERT
#define NOTSRCINVERT	0x00990066		// DSxn => Destination, Source, xor, not
#endif



class QBufferDC : public CDC
{
public:
// Construction
	QBufferDC(CDC * pDC, DWORD dwRopCode = SRCCOPY);

	virtual ~QBufferDC();

protected:
// Implementation
	
	CDC *m_pDC;			// The mother DC
	DWORD m_RopCode;	// The rop code used for bitblt'ing the final result

private:
	CBitmap *m_pOldBitmap;

	// Helper class
	static class BufferBitmap
	{
	public:
		BOOL ReserveBitmap(CDC * pDC, CSize sz);
		BOOL IsValid() const		{ return m_Bitmap.GetSafeHandle() != NULL; }
		CBitmap m_Bitmap;
	} m_BufferBitmap;

#ifdef QBUFFER_DEMO
public:
	static BOOL m_bDemoMode;
#endif
};
