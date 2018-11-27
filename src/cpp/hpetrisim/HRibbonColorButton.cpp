// HRibbonColorButton.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "HRibbonColorButton.h"


// CHRibbonColorButton

IMPLEMENT_DYNCREATE(CHRibbonColorButton, CMFCRibbonColorButton)

CList<COLORREF,COLORREF> CHRibbonColorButton::m_lstMainColors;
CList<COLORREF,COLORREF> CHRibbonColorButton::m_lstAdditionalColors;
CList<COLORREF,COLORREF> CHRibbonColorButton::m_lstStandardColors;

CHRibbonColorButton::CHRibbonColorButton() : CMFCRibbonColorButton()
{

}

CHRibbonColorButton::CHRibbonColorButton( UINT nID, LPCTSTR lpszText, BOOL bSimpleButtonLook, int nSmallImageIndex, int nLargeImageIndex, COLORREF color /*= RGB(0, 0, 0)*/ )
 :  CMFCRibbonColorButton( nID, lpszText, bSimpleButtonLook, nSmallImageIndex, nLargeImageIndex, color)
{
	SetDefaultCommand(FALSE);
	//EnableAutomaticButton(_T("&Automatic"), RGB(128, 128, 128));
	EnableOtherButton(_T("&More Fill Colors..."), _T("More Fill Colors"));
	SetColumns(10);
	SetColorBoxSize(CSize(17, 17));
	AddColorsGroup(_T("Theme Colors"), m_lstMainColors);
	AddColorsGroup(_T(""), m_lstAdditionalColors, TRUE);
	AddColorsGroup(_T("Standard Colors"), m_lstStandardColors);
	SetColor((COLORREF)-1);
}

CHRibbonColorButton::~CHRibbonColorButton()
{
}

void CHRibbonColorButton::CreateDocumentColors()
{
	typedef struct
	{
		COLORREF color;
		TCHAR* szName;
	}
	ColorTableEntry;

	int i = 0;
	int nNumColours = 0;

	static ColorTableEntry colorsMain [] =
	{
		{ RGB(255, 255, 255), _T("White, Background 1") },
		{ RGB(0, 0, 0), _T("Black, Text 1") },
		{ RGB(238, 236, 225), _T("Tan, Background 2") },
		{ RGB(31, 73, 125), _T("Dark Blue, Text 2") },
		{ RGB(79, 129, 189), _T("Blue, Accent 1") },
		{ RGB(192, 80, 77), _T("Red, Accent 2") },
		{ RGB(155, 187, 89), _T("Olive Green, Accent 3") },
		{ RGB(128, 100, 162), _T("Purple, Accent 4") },
		{ RGB(75, 172, 198), _T("Aqua, Accent 5") },
		{ RGB(245, 150, 70), _T("Orange, Accent 6") }
	};

	static ColorTableEntry colorsAdditional [] =
	{
		{ RGB(242, 242, 242), _T("White, Background 1, Darker 5%") },
		{ RGB(127, 127, 127), _T("Black, Text 1, Lighter 50%") },
		{ RGB(214, 212, 202), _T("Tan, Background 2, Darker 10%") },
		{ RGB(210, 218, 229), _T("Dark Blue, Text 2, Lighter 80%") },
		{ RGB(217, 228, 240), _T("Blue, Accent 1, Lighter 80%") },
		{ RGB(244, 219, 218), _T("Red, Accent 2, Lighter 80%") },
		{ RGB(234, 241, 221), _T("Olive Green, Accent 3, Lighter 80%")},
		{ RGB(229, 223, 235), _T("Purple, Accent 4, Lighter 80%") },
		{ RGB(216, 237, 242), _T("Aqua, Accent 5, Lighter 80%") },
		{ RGB(255, 234, 218), _T("Orange, Accent 6, Lighter 80%") },
		{ RGB(215, 215, 215), _T("White, Background 1, Darker 15%") },
		{ RGB(89, 89, 89), _T("Black, Text 1, Lighter 35%") },
		{ RGB(177, 176, 167), _T("Tan, Background 2, Darker 25%") },
		{ RGB(161, 180, 201), _T("Dark Blue, Text 2, Lighter 60%") },
		{ RGB(179, 202, 226), _T("Blue, Accent 1, Lighter 60%") },
		{ RGB(233, 184, 182), _T("Red, Accent 2, Lighter 60%") },
		{ RGB(213, 226, 188), _T("Olive Green, Accent 3, Lighter 60%")},
		{ RGB(203, 191, 215), _T("Purple, Accent 4, Lighter 60%") },
		{ RGB(176, 220, 231), _T("Aqua, Accent 5, Lighter 60%") },
		{ RGB(255, 212, 181), _T("Orange, Accent 6, Lighter 60%") },
		{ RGB(190, 190, 190), _T("White, Background 1, Darker 25%") },
		{ RGB(65, 65, 65), _T("Black, Text 1, Lighter 25%") },
		{ RGB(118, 117, 112), _T("Tan, Background 2, Darker 35%") },
		{ RGB(115, 143, 175), _T("Dark Blue, Text 2, Lighter 40%") },
		{ RGB(143, 177, 213), _T("Blue, Accent 1, Lighter 40%") },
		{ RGB(222, 149, 147), _T("Red, Accent 2, Lighter 40%") },
		{ RGB(192, 213, 155), _T("Olive Green, Accent 3, Lighter 40%")},
		{ RGB(177, 160, 197), _T("Purple, Accent 4, Lighter 40%") },
		{ RGB(137, 203, 218), _T("Aqua, Accent 5, Lighter 40%") },
		{ RGB(255, 191, 145), _T("Orange, Accent 6, Lighter 40%") },
		{ RGB(163, 163, 163), _T("White, Background 1, Darker 35%") },
		{ RGB(42, 42, 42), _T("Black, Text 1, Lighter 15%") },
		{ RGB(61, 61, 59), _T("Tan, Background 2, Darker 50%") },
		{ RGB(20, 57, 92), _T("Dark Blue, Text 2, Darker 25%") },
		{ RGB(54, 96, 139), _T("Blue, Accent 1, Darker 25%") },
		{ RGB(149, 63, 60), _T("Red, Accent 2, Darker 25%") },
		{ RGB(114, 139, 71), _T("Olive Green, Accent 3, Darker 25%") },
		{ RGB(97, 76, 119), _T("Purple, Accent 4, Darker 25%") },
		{ RGB(41, 128, 146), _T("Aqua, Accent 5, Darker 25%") },
		{ RGB(190, 112, 59), _T("Orange, Accent 6, Darker 25%") },
		{ RGB(126, 126, 126), _T("White, Background 1, Darker 50%") },
		{ RGB(20, 20, 20), _T("Black, Text 1, Lighter 5%") },
		{ RGB(29, 29, 28), _T("Tan, Background 2, Darker 90%") },
		{ RGB(17, 40, 64), _T("Dark Blue, Text 2, Darker 50%") },
		{ RGB(38, 66, 94), _T("Blue, Accent 1, Darker 50%") },
		{ RGB(100, 44, 43), _T("Red, Accent 2, Darker 50%") },
		{ RGB(77, 93, 49), _T("Olive Green, Accent 3, Darker 50%") },
		{ RGB(67, 53, 81), _T("Purple, Accent 4, Darker 50%") },
		{ RGB(31, 86, 99), _T("Aqua, Accent 5, Darker 50%") },
		{ RGB(126, 77, 42), _T("Orange, Accent 6, Darker 50%") },
	};

	static ColorTableEntry colorsStandard [] =
	{
		{ RGB(200, 15, 18), _T("Dark Red") },
		{ RGB(255, 20, 24), _T("Red") },
		{ RGB(255, 191, 40), _T("Orange") },
		{ RGB(255, 255, 49), _T("Yellow") },
		{ RGB(138, 207, 87), _T("Light Green") },
		{ RGB(0, 175, 84), _T("Green") },
		{ RGB(0, 174, 238), _T("Light Blue") },
		{ RGB(0, 111, 189), _T("Blue") },
		{ RGB(0, 36, 95), _T("Black") },
		{ RGB(114, 50, 157), _T("Purple") },
	};

	m_lstMainColors.RemoveAll();
	nNumColours = sizeof(colorsMain) / sizeof(ColorTableEntry);

	for (i = 0; i < nNumColours; i++)
	{
		m_lstMainColors.AddTail(colorsMain [i].color);
		CMFCRibbonColorButton::SetColorName(colorsMain [i].color, colorsMain [i].szName);
	};

	m_lstAdditionalColors.RemoveAll();
	nNumColours = sizeof(colorsAdditional) / sizeof(ColorTableEntry);

	for (i = 0; i < nNumColours; i++)
	{
		m_lstAdditionalColors.AddTail(colorsAdditional [i].color);
		CMFCRibbonColorButton::SetColorName(colorsAdditional [i].color, colorsAdditional [i].szName);
	};

	m_lstStandardColors.RemoveAll();
	nNumColours = sizeof(colorsStandard) / sizeof(ColorTableEntry);

	for (i = 0; i < nNumColours; i++)
	{
		m_lstStandardColors.AddTail(colorsStandard [i].color);
		CMFCRibbonColorButton::SetColorName(colorsStandard [i].color, colorsStandard [i].szName);
	};
}





