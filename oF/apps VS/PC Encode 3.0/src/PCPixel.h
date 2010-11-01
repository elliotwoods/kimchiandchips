#pragma once
/*
 *  PCPixel.h
 *  PCEncode
 *
 *  Created by Elliot Woods on 14/01/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "PCconstants.h"

#include "ofMain.h"
#include <math.h>

class PCPixelSlim
{
public:
	ofPoint _meanXdash, _sigmaXdash;
	float				_sigmaRdash; // standard deviation on X
	
	int _iLastFoundPixel;
	int _nFinds;	
};

class PCPixel : public PCPixelSlim, public PCConfig
{
	public:
		PCPixel();
		void clear();
	
		void addFind(int iCamPixel, float xCamX, float xCamY);
		
		void calcDeviation();

		void getData(float &MeanXdash, float &MeanYdash);
		void getData(float &MeanXdash, float &MeanYdash, float &SigmaXXdash, float &SigmaXYdash, int &iLastFoundIdash);
		void getData(std::vector<ofPoint> **ptrFindsXdash);
	
		void getFirstData(float &firstXdash, float &firstYdash);
	
		int	getNFinds();
	
	protected:
		std::vector<int>		_findsIdash;
		std::vector<ofPoint>	_findsXdash;
	

};