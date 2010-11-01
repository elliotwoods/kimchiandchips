/*
 *  PCPixel.cpp
 *  PCEncode
 *
 *  Created by Elliot Woods on 14/01/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "PCincludes.h"

PCPixel::PCPixel()
{
	clear();
}

void PCPixel::clear()
{
	_nFinds=0;
	
	_meanXdash.x=0;
	_meanXdash.y=0;
		
	_iLastFoundPixel=-1;
	
	//we add the ifs in for speed in windows
	if (_findsIdash.size() > 0)
		_findsIdash.clear();

	if (_findsXdash.size() > 0)
		_findsXdash.clear();

}

void PCPixel::addFind(int iPixelDash, float xXdash, float xYdash)
{
	_nFinds++;
	_meanXdash.x = (_meanXdash.x*(_nFinds-1) + xXdash) / _nFinds;
	_meanXdash.y = (_meanXdash.y*(_nFinds-1) + xYdash) / _nFinds;
	
	_iLastFoundPixel = iPixelDash;
	
	if (sdev)
	{
		_findsIdash.push_back(iPixelDash);
		_findsXdash.push_back(ofPoint(xXdash,xYdash));
	}
}

void PCPixel::calcDeviation()
{
	float f_nFinds = _nFinds;
	
	_sigmaXdash.x=0;
	_sigmaXdash.y=0;
	
	for (int iFind = 0; iFind < _nFinds; iFind++)
	{
		_sigmaXdash.x += (_findsXdash.at(iFind).x - _meanXdash.x) * (_findsXdash.at(iFind).x - _meanXdash.x);
		_sigmaXdash.y += (_findsXdash.at(iFind).x - _meanXdash.y) * (_findsXdash.at(iFind).y - _meanXdash.y);
	}
	
	_sigmaXdash.x = sqrt(_sigmaXdash.x/f_nFinds);
	_sigmaXdash.y = sqrt(_sigmaXdash.y/f_nFinds);
	
	_sigmaRdash = sqrt((_sigmaXdash.x*_sigmaXdash.x) + (_sigmaXdash.y*_sigmaXdash.y));
	
}

void PCPixel::getData(float &MeanXdash, float &MeanYdash)
{
	MeanXdash = _meanXdash.x;
	MeanYdash = _meanXdash.y;	
}

void PCPixel::getData(float &MeanXdash, float &MeanYdash, float &SigmaXXdash, float &SigmaXYdash, int &iLastFoundIdash)
{
	MeanXdash = _meanXdash.x;
	MeanYdash = _meanXdash.y;
	SigmaXXdash = _sigmaXdash.x;
	SigmaXYdash = _sigmaXdash.y;
	iLastFoundIdash = _iLastFoundPixel;
	
}

void PCPixel::getData(std::vector<ofPoint> **ptrFindsXdash)
{
	*ptrFindsXdash = &_findsXdash;
}

void PCPixel::getFirstData(float &firstXdash, float &firstYdash)
{
	if (_nFinds>0)
	{
		firstXdash = _findsXdash.at(0).x;
		firstYdash = _findsXdash.at(0).y;
	} else {
		firstXdash = 0;
		firstYdash = 0;
	}

}

int	PCPixel::getNFinds()
{
	return _nFinds;
}