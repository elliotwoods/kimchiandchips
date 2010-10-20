/*
 *  PCLogger.cpp
 *  pixel correlation
 *
 *  Created by Elliot Woods on 26/01/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "PCincludes.h"

PCLogger::PCLogger(PCEncode &encoder, PCDecode &decoder)
{
	_encoder = &encoder;
	_decoder = &decoder;
	
	_imgCameraSpacePreview.setUseTexture(false);
	_imgProjectorSpacePreview.setUseTexture(false);
	
}

void PCLogger::save()
{
	// if loging is turned off, then skip
	if (!isLogging)
		return;
	
	string strFilename = getDateString() + getConfigString();
	string strExtension = ".png";
	
	_imgCameraSpacePreview.setFromPixels(_decoder->_charCameraSpacePreview,
										 projWidth, projHeight,
										 OF_IMAGE_COLOR, true);
	
	_imgProjectorSpacePreview.setFromPixels(_decoder->_charProjectorSpacePreview,
											camWidth, camHeight,
											OF_IMAGE_COLOR, true);
	
	_imgCameraSpacePreview.saveImage(strFilename + "-camera" + strExtension);
	_imgProjectorSpacePreview.saveImage(strFilename + "-projector" + strExtension);
}

string PCLogger::getDateString()
{
	char datestring[10];

	time_t now = time(0);
	tm hereandnow=*localtime(&now);

	const char format[]="%Y%m%d";
	strftime(datestring, 50, format, &hereandnow);
	
	return string(datestring);
}

string PCLogger::getConfigString()
{
	char configstring[50];
	
	sprintf(configstring, " p=%d,%d c=%d,%d i=%d,%d, e=%d, t=%1.1f",
			projWidth, projHeight,
			camWidth, camHeight,
			interleaveWidth, interleaveHeight,
			errorBits, thresholdPercentile);
	
	return string(configstring);
}