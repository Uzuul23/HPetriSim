/**************************************************************************
	HUpdateObject.h

	copyright (c) 2013/07/16 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once

class CHUpdateObject : public CObject
{
protected:
	CHUpdateObject();
	DECLARE_DYNAMIC(CHUpdateObject)

public:
	CHUpdateObject(const Rect& rect);
	virtual ~CHUpdateObject();

	Rect m_Update;
};


