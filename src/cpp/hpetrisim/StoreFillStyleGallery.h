/**************************************************************************
	StoreFillStyleGallery.h

	copyright (c) 2013/08/11 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once
#include "HDrawObject.h"

inline int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}


inline void StoreFillStyleGallery()
{
	CWnd* pWnd = AfxGetMainWnd();
	if (pWnd == 0)
	{
		return;
	}
	
	Graphics g(pWnd->GetSafeHwnd());

	INT weidth = 40*CHDrawObject::FillStylesTotal;

	Size size(36, 36);
	Rect rect(0, 0, 40, weidth);
	Bitmap bmp(rect.Height, rect.Width, &g);
	Graphics* pg = Graphics::FromImage(&bmp);

	pg->FillRectangle(&SolidBrush(Color::Transparent), rect);

	Point point(2, 2);

	//Empty
	pg->DrawRectangle(&Pen(Color::LightGray), Rect(Point(2,2), Size(35,35)));
	point.X += 40;

	//Solid
	pg->FillRectangle(&SolidBrush(Color::LightGray), Rect(Point(42,2), size));
	point.X += 40;

	//HatchStyles
	for (int loop=CHDrawObject::HatchStyleFirst; loop<=CHDrawObject::HatchStyleLast; loop++)
	{
		Brush* pb = NewBrush(Color::LightGray, Color::Black, Rect(point, size), static_cast<CHDrawObject::FillStyles>(loop));
	
		pg->FillRectangle(pb, Rect(point, size));

		delete pb;

		point.X += 40;
	}

	//LinearGradient
	for (int loop=CHDrawObject::LinearGradientFirst; loop<=CHDrawObject::LinearGradientLast; loop++)
	{
		Brush* pb = NewBrush(Color::White, Color::Black, Rect(point, size), static_cast<CHDrawObject::FillStyles>(loop));

		pg->FillRectangle(pb, Rect(point, size));

		delete pb;

		point.X += 40;
	}


	CLSID clsid;
	GetEncoderClsid(L"image/bmp", &clsid);

	Status status = bmp.Save(L"C:\\Users\\Paule\\Documents\\Visual Studio 2008\\Projects\\HPetriSim\\HPetriSim\\res\\fillstyles.bmp", &clsid, 0);

	ASSERT(status == Ok);

	delete pg;
}