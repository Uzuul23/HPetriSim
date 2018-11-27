/**************************************************************************
	HRandom.h
	
	Copyright (C) 1999 - 2010 Henryk Anschuetz
	All rights reserved.
	
	www.winpesim.de
**************************************************************************/

#include "stdafx.h"

class CHRandom
{  
public:
	CHRandom()
	{
		srand(static_cast<unsigned int>(time(NULL)));
	}
	
	double RandDouble(double min = 0, double max = RAND_MAX)
	{
		return ((double(rand())/RAND_MAX) * (max - min)) + min;
	}
	
	long RandLong(double min = 0, double max = RAND_MAX)
	{
		double val = ((double(rand())/RAND_MAX) * (max - min) + min);

		return (val == max)? int(val) - 1 : int(val);
	}

	long ExpRandLong(long middle)
	{
		return static_cast<long>(middle * (1- log((RAND_MAX - CHRandom::RandDouble()) / RAND_MAX + 0.5)));
	}

};