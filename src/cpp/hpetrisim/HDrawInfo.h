/**************************************************************************
	CHDrawInfo.h

	copyright (c) 2013/08/07 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once

class CHDrawInfo
{
public:
	CHDrawInfo(void);
	~CHDrawInfo(void);

	Gdiplus::Font LabelFont;
	Gdiplus::Font TokenFont;
	Gdiplus::SolidBrush LabelTextBrush;
	Gdiplus::StringFormat StringFormatCenter; 
	Gdiplus::Pen LineCapArrowPen;
	Gdiplus::Pen LinePen;
	Gdiplus::Pen DashLineCapArrowPen;
	Gdiplus::Pen DashLinePen;
	Gdiplus::Pen LineCapEllipsePen;
	Gdiplus::SolidBrush TokenBrush;
	Gdiplus::Bitmap* DisconnectedBitmap;
	Gdiplus::Bitmap* TimerBitmap;

	bool Online;
};
