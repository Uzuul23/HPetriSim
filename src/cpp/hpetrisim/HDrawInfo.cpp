#include "StdAfx.h"
#include "resource.h"
#include "HDrawInfo.h"

CHDrawInfo::CHDrawInfo(void) : LabelFont(L"Arial", 8.0f, FontStyleRegular)
, TokenFont(L"Arial", 8.0f, FontStyleBold)
, LabelTextBrush(Color::Black)
, LineCapArrowPen(Color::DarkGray,  2.0f)
, LinePen(Color::DarkGray,  2.0f)
, DashLineCapArrowPen(Color::DarkGray,  2.0f)
, DashLinePen(Color::DarkGray,  2.0f)
, LineCapEllipsePen(Color::DarkGray,  2.0f)
, TokenBrush(Color::DarkGray)
, DisconnectedBitmap(new Bitmap(AfxGetApp()->LoadIcon(IDI_DISCONNECTED)))
, TimerBitmap(new Bitmap(AfxGetApp()->LoadIcon(IDI_TIMER)))
, Online(false)
{
	StringFormatCenter.SetAlignment(StringAlignmentCenter);
	StringFormatCenter.SetLineAlignment(StringAlignmentCenter);

	Point points[] = { Point(-1, -6) , Point(1, -6), Point(0, -2) };

	GraphicsPath path;
	path.AddPolygon(points, 3);

	CustomLineCap custCap(0, &path);
	LineCapArrowPen.SetCustomEndCap(&custCap);
	DashLineCapArrowPen.SetCustomEndCap(&custCap);

	DashLineCapArrowPen.SetDashStyle(DashStyleDash);
	DashLinePen.SetDashStyle(DashStyleDash);

	Rect rect(-1, -2, 2, 2);

	path.Reset();
	path.AddEllipse(rect);

	CustomLineCap custCap2(0, &path);
	LineCapEllipsePen.SetCustomEndCap(&custCap2);

}

CHDrawInfo::~CHDrawInfo(void)
{
	delete DisconnectedBitmap;
	delete TimerBitmap;
}
