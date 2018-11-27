/**************************************************************************
	HDrawObject.cpp
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#include "stdafx.h"
#include "PetriSim.h"
#include "HDrawObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CHDrawObject, CObject)

CHDrawObject::CHDrawObject() : m_Invalid(false)
{
}

CHDrawObject::~CHDrawObject()
{
}

#ifdef _DEBUG
void CHDrawObject::AssertValid() const
{
	CObject::AssertValid();
}

void CHDrawObject::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}
#endif //_DEBUG