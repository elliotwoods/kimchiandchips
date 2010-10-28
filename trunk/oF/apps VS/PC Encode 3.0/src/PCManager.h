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
	void	clear();
	
	bool	isInitialised;
	bool	hasData;
	int		state;
	int		iFrame;

	PCEncode			*_encoder;
	vector<PCDecode*>	_decoder;
	PayloadBase			*_payload;
	
	PCLogger			*_logger;
	
	vector<Camera*>		_camera;

protected:
	void	readFrame();
	void	writeFrame();
	void	advanceFrame();

	bool	_firstFrame;
	
};
