/**************************************************************************
	HConst.h
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#include <LIMITS.H>

//some user massages
#define WM_SIMTHREAD	WM_USER + 6
#define WM_UPDATEOUTPUT WM_USER + 7
#define WM_ANIMSTEP WM_USER + 8


//file version
const UINT DRAW_VERSION = 10002;
// enumerate draw tools
//enum {DR_SELECT,DR_LINE,DR_RECT,DR_MOVE,DR_RESIZE,
//		DR_TRANSITION,DR_POSITION,DR_CONNECTOR,DR_POLYLINE,DR_TEXT,
//		DR_ZOOM_IN,DR_ZOOM_OUT,DR_PAN};
//grid
const int DRAW_GRID = 10;
const int SL_GRIPPER = 8;

const TCHAR TRANSITION_NAME[] = _T("T");
const TCHAR POSITION_NAME[] = _T("P");

// identical numbers
const DWORD FIRST_TRANSITION = 0;
const DWORD FIRST_POSITION = 1000000;
const DWORD FIRST_CONNECTOR = 2000000;
//enum PT_SIZE {PT_VERYSMALL = 15
//			, PT_SMALL = 20
//			, PT_NORMAL = 25
//			, PT_LARGE = 35
//			, PT_VERYLARGE = 50};

//target arrows
const double ARROW_LENGHT = 10;
const double ARROW_ARC = 0.523;
//const double ARROW_ARC = 0.35;// 0 bis pi/2


//limitations
const long MAX_TOKENS = LONG_MAX;
const long MAX_DELAY_TIME = LONG_MAX / 2;

//label Sub
//enum LABEL_SUB{LS_LABEL_A, LS_LABEL_B};
const int LABEL_OFFSET_X = 15;
const int LABEL_OFFSET_Y = 5;


//limited count of objects 
const int MAX_DRAW_OBJECTS = 64000;

//animation times (ms)
const int MAX_ANIM_TIME = 3000;
const int MIN_ANIM_TIME = 200;
const int MIN_TIME = 10;
const int INCR_DECR_TIME = 100;

class CHConnector;
typedef CTypedPtrList<CPtrList, CHConnector*> CONN_LIST;