/**************************************************************************
	HLine.h

	copyright (c) 2013/07/10 Henryk Anschuetz
	
	www.winpesim.de
***************************************************************************/

#pragma once
#include "HPoly.h"

class CHLine : public CHPoly  
{

protected:
	DECLARE_SERIAL(CHLine)
	CHLine();

public:
	
	virtual ~CHLine();
	CHLine(CPoint& start);

	virtual void Serialize(CArchive& ar);

protected:

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};