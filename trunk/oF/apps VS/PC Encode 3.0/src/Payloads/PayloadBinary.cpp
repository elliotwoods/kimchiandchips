/*
 *  PayloadBinary.cpp
 *  PCEncode
 *
 *  Created by Elliot Woods on 15/10/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "PayloadBinary.h"

void PayloadBinary::setup()
{
	PayloadBase::setup();

	////////////////////////////////////////
	// BUILD DATA ARRAYS
	////////////////////////////////////////
	for (int i=0; i<nPixelsPerInterleave; i++)
	{
		//for binary, we just output the binary representation
		//of the pixel's index
		data[i]=i;
		dataInverse[i]=i;

		//make a 64bit random int for the error check
		errorCheck[i] = rand() + (rand()*(long long int)(1)<<32);
	}
	////////////////////////////////////////

	////////////////////////////////////////
	// CALCULATE VALUES
	////////////////////////////////////////
	dataFramesPerInterleave = ceil(Log2(nPixelsPerInterleave));
	totalFramesPerInterleave = dataFramesPerInterleave + errorBits;
	totalFrames = totalFramesPerInterleave * calibrateFrames;
	////////////////////////////////////////
}

bool PayloadBinary::decode(int reading, int &iX, int &iY)
{
	bool valid = (reading > 0) && (reading < maxIndex);
				
	iX = reading % nPixelsPerInterleaveX;
	iY = reading / nPixelsPerInterleaveY;
	
	return valid;
}