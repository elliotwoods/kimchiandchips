/*
 *  PCManager.cpp
 *  PC Encode VS
 *
 *  Created by Elliot Woods on 26/10/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */


#include "PCManager.h"

#define FOREACH_CAMERA for (int iCam=0; iCam<nCameras; iCam++)

PCManager::PCManager()
{
	isInitialised=false;
	hasData=false;	
	state=0;
	_firstFrame=false;
}

PCManager::~PCManager()
{
	if (isInitialised)
		close();
	
}

void PCManager::setup()
{
	
	if (!isInitialised)
	{		
		//initialise payload
		_payload = new PayloadGraycode();
		_payload->setup();
		
		//instantiate the encoder
		_encoder = new PCEncode(_payload);
		
		//instantiate cameras, decoders
		FOREACH_CAMERA
		{
			_camera.push_back(new Camera());
			_decoder.push_back(new PCDecode(_payload, _camera[iCam], _encoder->_texOutput));
			
			_camera[iCam]->ID = camIDs[iCam];
			_camera[iCam]->init();
		}
		
		//instantiate logger
		_logger = new PCLogger(_encoder, &_decoder);
		
		isInitialised=true;
	} else
		ofLog(OF_LOG_ERROR, "PCManager: Can't run setup() more than once");

}

void PCManager::update()
{
	
	//////////////////////////
	// CAPTURE
	//////////////////////////
	//if we're scanning,
	//then loop until a fresh frame
	//**TODO** code this with parallel timing for each cam!
	bool hasNewImageSum=false;
	bool *hasNewImage = new bool[nCameras];
	FOREACH_CAMERA
		hasNewImage[iCam] = false;

	while (hasNewImageSum==false)
	{
		hasNewImageSum = true;
		FOREACH_CAMERA
			if (!hasNewImage[iCam])
			{
				hasNewImage[iCam] = _decoder[iCam]->capture();
				hasNewImageSum &= hasNewImage[iCam];
			}

		//if we're not scanning, it doesn't matter
		//if the frame is new, let's break out
		if (state==0)
			break;

		ofSleepMillis(10);
	}
	delete[] hasNewImage;		
	//////////////////////////
	
	//////////////////////////
	// SCANNING STEPS
	//////////////////////////
	if (state>0 && !_firstFrame)
	{
		readFrame();
		
		advanceFrame();
	}
	
	if (state>0) //if we are (still) scanning
		writeFrame();
	//////////////////////////
	
	_firstFrame=false;
	
}

void PCManager::close()
{
	for (int i=0; i<nCameras; i++)
	{
		delete _decoder[i];
	}
	
	delete _logger;
	delete _encoder;
	delete _payload;
}

////////////////////////////////////////////////////////////

void PCManager::videoSettings()
{
	FOREACH_CAMERA
		_camera[iCam]->videoSettings();
}

////////////////////////////////////////////////////////////
void PCManager::calibrate()
{
	FOREACH_CAMERA
		_decoder[iCam]->resetCalibration();
	
	state = STATE_CALIBRATING;
	iFrame=0;
	_firstFrame=true;	
}

void PCManager::start()
{
	clear();
	
	state = STATE_SCANNING;
	iFrame=0;
	_firstFrame=true;
}

void PCManager::stop()
{
	state = STATE_STANDBY;
	iFrame = 0;
}

void PCManager::clear()
{
	FOREACH_CAMERA
		_decoder[iCam]->clear();
}
////////////////////////////////////////////////////////////

void PCManager::readFrame()
{
//	ofLog(OF_LOG_VERBOSE, "PCManager: Read frame " + ofToString(iFrame, 0));
	if (state==STATE_CALIBRATING)
		FOREACH_CAMERA
		_decoder[iCam]->addCalibrationFrame();
	
	else if(state==STATE_SCANNING)
	{
		FOREACH_CAMERA
		_decoder[iCam]->addScanFrame(_payload->iScanInterleaveFrame(iFrame),
									 _payload->iInterleave(iFrame));
		
		//check whether we're changing interleaves
		if (_payload->iInterleave(iFrame) !=
			_payload->iInterleave(iFrame+1))
		{
			//if so, calculate for this interleave
			FOREACH_CAMERA
			_decoder[iCam]->calcInterleave(_payload->iInterleave(iFrame));
			
			ofLog(OF_LOG_VERBOSE, "PCManager: Calc interleave " + 
				  ofToString(_payload->iInterleave(iFrame), 0));
		}
	}
}

void PCManager::advanceFrame()
{
//	ofLog(OF_LOG_VERBOSE, "PCManager: Advance frame from " + ofToString(iFrame, 0));
	if (state>0)
		iFrame++;
	
	//check whether we've hit the end
	switch (state) {
		case STATE_CALIBRATING:
			if (iFrame >= _payload->interleaves+1)
			{
				FOREACH_CAMERA
					_decoder[iCam]->calcThreshold();
				stop();
				ofLog(OF_LOG_VERBOSE, "PCManager: end calibrate");
			}
			break;
			
		case STATE_SCANNING:
			if (iFrame >= _payload->totalFrames)
			{
				_logger->save();
				stop();
				ofLog(OF_LOG_VERBOSE, "PCManager: end scan");
			}
			break;
	}
//	ofLog(OF_LOG_VERBOSE, "PCManager: Advance frame to " + ofToString(iFrame, 0));
}

void PCManager::writeFrame()
{
	ofLog(OF_LOG_VERBOSE, "PCManager: Write frame " + ofToString(iFrame, 0));
	
	if (state==STATE_CALIBRATING)
		_encoder->updateCalibrationFrame(iFrame);
	
	else if(state==STATE_SCANNING)
	{
//		if (_payload->iScanInterleaveFrame(iFrame)==0)
//			FOREACH_CAMERA
//				_decoder[iCam]->clearInterleave();
		
		_encoder->updateScanFrame(_payload->iScanInterleaveFrame(iFrame),
								  _payload->iInterleave(iFrame));
	}	
	
	if (state>0)
		_camera.clear();
}