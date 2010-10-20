/*
 *  PayloadGraycode.cpp
 *  PCEncode
 *
 *  Created by Elliot Woods on 15/10/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "PayloadGraycode.h"

void PayloadGraycode::setup()
{
	PayloadBase::setup();

	////////////////////////////////////////
	// BUILD DATA ARRAYS
	////////////////////////////////////////
	int grayCode, grayCodeX, grayCodeY;
	int iX, iY;
	int nPixelsPerInterleaveX = (projWidth/interleaveWidth);

	int nBitsX = ceil(Log2(projWidth/interleaveWidth));
	int nBitsY = ceil(Log2(projHeight/interleaveHeight));

	for (int i=0; i<nPixelsPerInterleave; i++)
	{
		iX = i % nPixelsPerInterleaveX;
		iY = i / nPixelsPerInterleaveX;

		grayCodeX = iX ^ (iX >> 1);
		grayCodeY = iY ^ (iY >> 1);
		grayCode = grayCodeX + (grayCodeY << nBitsX);

		data[i] = grayCode;
		dataInverse[grayCode] =i;

		//make a 64bit random int for the error check
		errorCheck[i] = rand() + (rand()*(long long int)(1)<<32);
	}
	////////////////////////////////////////

	////////////////////////////////////////
	// CALCULATE VALUES
	////////////////////////////////////////
	dataFramesPerInterleave = nBitsX + nBitsY;
	totalFramesPerInterleave = dataFramesPerInterleave + errorBits;
	totalFrames = totalFramesPerInterleave * interleaves;
	////////////////////////////////////////
}

void PayloadGraycode::decode(int reading, int &iX, int &iY, bool &valid)
{
	valid = (reading > 0) && (reading < maxIndex);
	if (!valid)
		return;

	int pixIndex = dataInverse[reading];
				
	iX = pixIndex % nPixelsPerInterleaveX;
	iY = pixIndex / nPixelsPerInterleaveX;

	valid &= (iX>=0) && (iX<nPixelsPerInterleaveX);
	valid &= (iY>=0) && (iX<nPixelsPerInterleaveY);
}