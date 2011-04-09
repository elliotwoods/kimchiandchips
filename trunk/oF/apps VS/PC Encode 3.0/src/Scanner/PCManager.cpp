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

PCManager::PCManager() :
_scrProjectorMask(cursor_none, false, &_texProjectorMask, "Projector mask"),
_wdgStartScan("Start scan"),
_wdgClear("Clear all (including mask)"),
_scrControls("Controls"),
_wdgDistance("Screen distance", screenDistance, 0, 1, 0.01,"meters"),
_wdgIFrame("iFrame",iFrame,0),
_wdgScanForever("Scan forever", scanForever)

{
	isInitialised=false;
	hasData=false;	
	state=0;
	iFrame=0;
	_firstFrame=false;
	
	//projector mask
	_charProjectorMask = new unsigned char[projWidth*projHeight];
	_boolProjectorMask = new bool[projWidth*projHeight];
	_texProjectorMask.allocate(projWidth, projHeight, GL_LUMINANCE);
	_texProjectorMask.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	
	//interface
	_scrControls.push(&_wdgStartScan);
	_scrControls.push(&_wdgClear);
	_scrControls.push(&_wdgDistance);
	_scrControls.push(&_wdgIFrame);
	_scrControls.push(&_wdgScanForever);
	
	screenDistance = 0.5;

	scanForever = false;
	
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
		_encoder = new PCEncode(_payload, _boolProjectorMask);
		
		//instantiate cameras, decoders
		FOREACH_CAMERA
		{
			#ifdef PCENCODE_CAM_DEFAULT
			_camera.push_back(new CameraDefault());
			#endif
			
			#ifdef PCENCODE_CAM_VIDEO_INPUT
			_camera.push_back(new CameraTheosVideoInput());
			#endif

			_decoder.push_back(new PCDecode(_payload, _camera[iCam], _boolProjectorMask));
			
			if (!_camera[iCam]->setup(camIDs[iCam]))
				std::exit(0);
		}
		
		//instantiate logger
		_logger = new PCLogger(_encoder, &_decoder);
		
		//clear all data
		clearProjectorMask();
		
		isInitialised=true;
	} else
		ofLog(OF_LOG_ERROR, "PCManager: Can't run setup() more than once");
}

void PCManager::update()
{
	//////////////////////////
	// CHECK INPUTS
	//////////////////////////
	if (_wdgClear.getBang())
		clear(true);
	if (_wdgStartScan.getBang())
		start();
	//////////////////////////	
	
	//////////////////////////
	// CAPTURE
	//////////////////////////
	//if we're scanning,
	//then loop until a fresh frame
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
				//set forceFreshFrame=true if we're state>0
				hasNewImage[iCam] = _decoder[iCam]->capture(state>0);
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
	//if it's the first frame
	//then we need to write before reading
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
	
	_wdgIFrame.setMax(_payload->interleaves + 1);
}

void PCManager::start()
{
	clear(false);
	
	state = STATE_SCANNING;
	iFrame=0;
	_firstFrame=true;
	
	_wdgIFrame.setMax(_payload->totalFrames);
}

void PCManager::stop()
{
	state = STATE_STANDBY;
}

void PCManager::clear(bool clearMask)
{
	FOREACH_CAMERA
		_decoder[iCam]->clear(clearMask);
	
	if (clearMask)
		clearProjectorMask();
}

void PCManager::save(string filenameBase)
{
	_logger->save(filenameBase);
}
////////////////////////////////////////////////////////////

void PCManager::readFrame()
{
	if (state==STATE_CALIBRATING)
		FOREACH_CAMERA
			_decoder[iCam]->addCalibrationFrame();
	
	else if(state==STATE_SCANNING)
	{
		FOREACH_CAMERA
			_decoder[iCam]->addScanFrame(_payload->iScanInterleaveFrame(iFrame),
									 _payload->iInterleave(iFrame));
		
		ofLog(OF_LOG_VERBOSE, "PCManager: Read interleave frame " + ofToString(_payload->iScanInterleaveFrame(iFrame), 0));

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
				if (scanForever)
					start();
			}
			break;
			
		case STATE_SCANNING:
			if (iFrame >= _payload->totalFrames)
			{
				stop();
				updateProjectorMask();
				ofLog(OF_LOG_VERBOSE, "PCManager: end scan");
				if (scanForever)
					calibrate();
			}
			break;
	}
//	ofLog(OF_LOG_VERBOSE, "PCManager: Advance frame to " + ofToString(iFrame, 0));
}

void PCManager::writeFrame()
{
	if (state==STATE_CALIBRATING)
		_encoder->updateCalibrationFrame(iFrame);
	
	else if(state==STATE_SCANNING)
	{
		_encoder->updateScanFrame(_payload->iScanInterleaveFrame(iFrame),
								  _payload->iInterleave(iFrame));

		ofLog(OF_LOG_VERBOSE, "PCManager: Write interleave frame " + ofToString(_payload->iScanInterleaveFrame(iFrame), 0));
	}	
	
	if (state>0)
		FOREACH_CAMERA
			_camera[iCam]->clearTimer();
}

void PCManager::clearProjectorMask()
{
	for (int iPP=0; iPP<projWidth*projHeight; iPP++)
	{
		_boolProjectorMask[iPP]=true;
		_charProjectorMask[iPP]=255;
	}
	_texProjectorMask.loadData(_charProjectorMask,projWidth,projHeight,GL_LUMINANCE);
	
	FOREACH_CAMERA
		_decoder[iCam]->clear(true);
	
}

void PCManager::updateProjectorMask()
{
	bool current;
	
	for (int iPP=0; iPP<projWidth*projHeight; iPP++)
	{
		current = true;
		FOREACH_CAMERA
			current &= _decoder[iCam]->projPixels[iPP]->nFinds>0;
		
		_boolProjectorMask[iPP] = !current;
		_charProjectorMask[iPP] = (current ? 0 : 255);
	}
	
	_texProjectorMask.loadData(_charProjectorMask, projWidth, projHeight, GL_LUMINANCE);
}