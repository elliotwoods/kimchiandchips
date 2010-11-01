/*
 *  PayloadBase.cpp
 *  PCEncode
 *
 *  Created by Elliot Woods on 15/10/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "PayloadBase.h"

void PayloadBase::calcCommon()
{
	////////////////////////////////////////
	// CALCULATE COMMON VALUES
	////////////////////////////////////////
	interleaves = interleaveWidth * interleaveHeight;
	
	nPixels = projWidth * projHeight;
	nPixelsPerInterleaveX = projWidth / interleaveWidth;
	nPixelsPerInterleaveY = projHeight / interleaveHeight;
	nPixelsPerInterleave = nPixelsPerInterleaveX * nPixelsPerInterleaveY;

	calibrateFrames = 1 + interleaves;
	////////////////////////////////////////
}

void PayloadBase::setup()
{
	////////////////////////////////////////
	// CLEAR / SETUP ARRAYS
	////////////////////////////////////////
	if (isInitialised)
	{
		delete[] data;
		delete[] dataInverse;
		delete[] errorCheck;
	}
	data = new long long int[maxIndex];
	dataInverse = new long long int[maxIndex];
	errorCheck = new long long int[maxIndex];
	isInitialised=true;
	////////////////////////////////////////
}


//helper functions
int	PayloadBase::iScanInterleaveFrame(int iFrame)
{
	return iFrame % totalFramesPerInterleave;
}

int PayloadBase::iInterleave(int iFrame)
{
	return iFrame / totalFramesPerInterleave;
}

