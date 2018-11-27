/**************************************************************************
	HUpdateObject.cpp

	copyright (c) 2013/07/16 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#include "stdafx.h"
#include "HUpdateObject.h"

IMPLEMENT_DYNAMIC(CHUpdateObject, CObject)

CHUpdateObject::CHUpdateObject()
{
}

CHUpdateObject::CHUpdateObject(const Rect& rect) : m_Update(rect)
{

}

CHUpdateObject::~CHUpdateObject()
{
}

