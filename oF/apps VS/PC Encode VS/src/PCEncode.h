#ifndef _PCEncode
#define _PCEncode
/*
 *  PCEncode.h
 *  PCEncode
 *
 *  Created by Elliot Woods on 25/10/2009.
 *  Copyright 2009 Kimchi and Chips. All rights reserved.
 *
 */


#include "ofMain.h"
#include "PCconstants.h"
#include "GUI.h"
#include "scrTexture.h"
#include "PCConfig.h"

#include "PayloadBase.h"

class PCEncode : public PCConfig 
{
public:
	PCEncode(GUI *screens, PayloadBase *payload);
	~PCEncode();
	
	int						getnProjectorPixels();
	int						getnScanFrames();
	int						getnScanFramesPerInterleave();
	int						getnScanDataFramesPerInterleave();
	int						getnCalibrationFrames();
	int						getnInterleaves();

	void					updateScanFrame(int iScanInterleaveFrame, int iInterleave);
	void					updateCalibrationFrame(int iCalibrationFrame);
	

	ofTexture				_texOutput;
	unsigned char *			_texCharOutput;
	
private:
	
	GUI						*_screens;
	PayloadBase				*_payload;
	
};


#endif


