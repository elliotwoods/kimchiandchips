#pragma once
/*
 *  PayloadBase.h
 *  PCEncode
 *
 *  Created by Elliot Woods on 15/10/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include <cmath>
#include "PCConfig.h"

class PayloadBase : public PCConfig
{
public:

	////////////////////////////////////////
	// VALUES
	////////////////////////////////////////
	int			interleaves;
	int			nPixels;
	int			nPixelsPerInterleave;
	int			nPixelsPerInterleaveX, nPixelsPerInterleaveY;
	
	int			calibrateFrames;
	int			totalFrames;
	int			totalFramesPerInterleave;
	int			dataFramesPerInterleave;
	
	long long int			*data;
	long long int			*dataInverse;
	long long int			*errorCheck;

	int			maxIndex;
	////////////////////////////////////////

	////////////////////////////////////////
	// FUCNTIONS
	////////////////////////////////////////
	PayloadBase()		{isInitialised=false;};
	void				calcCommon();
	virtual void		setup();
	virtual bool		decode(int reading, int &iX, int &iY) = 0;
	
	int					iScanInterleaveFrame(int iFrame);
	int					iInterleave(int iFrame);
	////////////////////////////////////////

protected:
	bool		isInitialised;
	void		clear();
	double		Log2(double n) { return log( n ) / log((double)2); };
};