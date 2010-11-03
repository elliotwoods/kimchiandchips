/*
 *  PCConfig.cpp
 *  PCEncode
 *
 *  Created by Elliot Woods on 16/01/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "PCConfig.h"

bool PCConfig::sdev = false;
bool PCConfig::isLogging=true;

float PCConfig::thresholdPercentile=0.5;
int PCConfig::captureDelay=200;

int PCConfig::gain=CONFIG_DEFAULT_GAIN;
int PCConfig::exposure=CONFIG_DEFAULT_EXPOSURE;

char PCConfig::nCameras=0;
vector<int> PCConfig::camIDs = vector<int>();

int PCConfig::camWidth=0;
int PCConfig::camHeight=0;

int PCConfig::projWidth=0;
int PCConfig::projHeight=0;

int PCConfig::interleaveWidth=0;
int PCConfig::interleaveHeight=0;

int PCConfig::errorBits=0;

string PCConfig::_configFilename="";
bool PCConfig::_loaded=false;

PCConfig::PCConfig()
{
	if (_loaded)
		return;

	_configFilename = "settings.xml";

	captureDelay = 100; //ms
	thresholdPercentile = 0.5;
	
	_loaded = configLoad();

	if (!_loaded)
	{
		cout << "load settings failed";
		std::exit(1);
	}
	
}

bool PCConfig::configLoad()
{
	return configLoad(_configFilename);
}

bool PCConfig::configLoad(string filename)
{
	bool success;
	
	_configFileXML.clear();
	
	success = _configFileXML.loadFile(filename);
		
	if (success)
	{
		try
		{
			if (!_configFileXML.pushTag("settings", 0))
				throw;
			
			if (_configFileXML.pushTag("encoding", 0))
			{
				projWidth = _configFileXML.getAttribute("projector", "width", PC_SCREEN_RESOLUTION_X, 0);
				projHeight = _configFileXML.getAttribute("projector", "height", PC_SCREEN_RESOLUTION_Y, 0);

				interleaveWidth = _configFileXML.getAttribute("interleave", "width", 1, 0);
				interleaveHeight = _configFileXML.getAttribute("interleave", "height", 1, 0);
				
				errorBits = _configFileXML.getAttribute("errorcheck", "nFrames", 4, 0);
				
				_configFileXML.popTag();
			}
			
			if (_configFileXML.pushTag("decoding", 0))
			{
				camWidth = _configFileXML.getAttribute("cameras", "width", 640, 0);
				camHeight = _configFileXML.getAttribute("cameras", "height", 480, 0);
				captureDelay = _configFileXML.getAttribute("cameras", "delay", 100, 0);
				exposure = _configFileXML.getAttribute("cameras", "exposure", CONFIG_DEFAULT_EXPOSURE, 0);
				gain = _configFileXML.getAttribute("cameras", "gain", CONFIG_DEFAULT_GAIN, 0);
				
				if (_configFileXML.pushTag("cameras", 0))
				{
					nCameras = _configFileXML.getNumTags("camera");
					
					//read camera IDs into vector
					camIDs.clear();
					for (int iCam=0; iCam<nCameras; iCam++)
						camIDs.push_back(_configFileXML.getAttribute("camera", "id", 0, iCam));
					
					_configFileXML.popTag();
				}
				
				thresholdPercentile = _configFileXML.getAttribute("threshold", "percentile", 0.5, 0);
				sdev = _configFileXML.getAttribute("data", "sdev", 0, 0);
				
				_configFileXML.popTag();
			}
			
			isLogging = _configFileXML.getAttribute("logging", "on", 0, 0) > 0;
			
		}
		catch(...)
		{
			success=false;
		}
	}

	if (!success)
		cout << "Loading settings file " << _configFilename << " failed.\n";
	else
		cout << "Loading settings file " << _configFilename << " success.\n";
	
	return success;
}