#pragma once

/*
 *  PCManager.h
 *  PCDecode
 *
 *  Created by Elliot Woods on 25/10/2009.
 *  Copyright 2009 Kimchi and Chips. All rights reserved.
 *
 */


#include "ofMain.h"

#include "PCEncode.h"
#include "PCLogger.h"
#include "PCDecode.h"

#include "PayloadGraycode.h"
#include "PayloadBinary.h"

#include "ofxKCScreensGUI.h"

#define STATE_STANDBY				0
#define STATE_CALIBRATING			1
#define STATE_SCANNING				2

class PCManager : public PCConfig 
{
public:
	PCManager();
	~PCManager();
	
	void	setup();
	void	update();
	void	close();
	
	void	videoSettings();

	void	calibrate();
	void	start();
	void	stop(); //force early stop

	void	clear(bool clearMask=false);
	void	save(string filenameBase);
	
	bool	isInitialised;
	bool	hasData;
	int		state;
	int		iFrame;

	PCEncode			*_encoder;
	vector<PCDecode*>	_decoder;
	PayloadBase			*_payload;
	
	PCLogger			*_logger;
	
	vector<Camera*>		_camera;
		
	//interface
	scrTexture				_scrProjectorMask;
	scrWidgets				_scrControls;
	
	//cal vars
	float				screenDistance;

protected:
	void	readFrame();
	void	writeFrame();
	void	advanceFrame();

	bool	_firstFrame;
	
	//interface
	wdgButton			_wdgStartScan;
	wdgButton			_wdgClear;
	wdgSlider			_wdgDistance;
	wdgCounter			_wdgIFrame;
		
	//mask
	void					updateProjectorMask();
	void					clearProjectorMask();
	ofTexture				_texProjectorMask;
	bool *					_boolProjectorMask;
	unsigned char *			_charProjectorMask;
	
};
