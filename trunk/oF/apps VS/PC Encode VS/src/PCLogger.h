/*
 *  PCLogger.h
 *  pixel correlation
 *
 *  Created by Elliot Woods on 26/01/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "PCconstants.h"
#include "PCConfig.h"
#include "PCEncode.h"
#include "PCDecode.h"
#include "ofMain.h"

#include <ctime>

class PCLogger : public PCConfig
{
	
public:
	PCLogger(PCEncode &encoder, PCDecode &decoder);
	
	void		save();
	
private:

	string		getDateString();
	string		getConfigString();
	
	PCDecode *	_decoder;
	PCEncode *	_encoder;	
	
	ofImage		_imgCameraSpacePreview, _imgProjectorSpacePreview;

};