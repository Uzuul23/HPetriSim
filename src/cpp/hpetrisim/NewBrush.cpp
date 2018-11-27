/**************************************************************************
	NewBrush.cpp

	copyright (c) 2013/08/14 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#include "stdafx.h"
#include "HDrawObject.h"

Brush* NewBrush(const Color& color1, const Color& color2, const Rect& bounds, CHDrawObject::FillStyles fillstyle)
{
	if (fillstyle >= CHDrawObject::HatchStyleFirst && fillstyle <= CHDrawObject::HatchStyleLast)
	{
		return new HatchBrush(static_cast<HatchStyle>(fillstyle), color2, color1);
	}
	else if (fillstyle >= CHDrawObject::LinearGradientFirst && fillstyle <= CHDrawObject::LinearGradientLast)
	{
		switch (fillstyle)
		{
		case CHDrawObject::LinearGradient_1:
			{
				LinearGradientBrush* pBrush  = new LinearGradientBrush(bounds, color1, color2, 0.0);
				return pBrush;
			}
		case CHDrawObject::LinearGradient_2:
			{
				LinearGradientBrush* pBrush  = new LinearGradientBrush(bounds, color1, color2, 45.0);
				return pBrush;
			}
		case CHDrawObject::LinearGradient_3:
			{
				LinearGradientBrush* pBrush  = new LinearGradientBrush(bounds, color1, color2, 90.0);
				return pBrush;
			}
		case CHDrawObject::LinearGradient_4:
			{
				LinearGradientBrush* pBrush  = new LinearGradientBrush(bounds, color1, color2, 135.0);
				return pBrush;
			}
		case CHDrawObject::LinearGradient_5:
			{
				LinearGradientBrush* pBrush  = new LinearGradientBrush(bounds, color2, color1, 0.0);
				pBrush->SetBlendBellShape(0.5f);
				return pBrush;
			}
		case CHDrawObject::LinearGradient_6:
			{
				LinearGradientBrush* pBrush  = new LinearGradientBrush(bounds, color2, color1, 90.0);
				pBrush->SetBlendBellShape(0.5f);
				return pBrush;
			}
		default : return new SolidBrush(Color::Magenta);
		}
	}

	return new SolidBrush(color1);
}