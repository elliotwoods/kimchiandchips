/*
 *  Camera.cpp
 *  PCEncode
 *
 *  Created by Elliot Woods on 28/12/2009.
 *  Copyright 2009 Kimchi and Chips. All rights reserved.
 *
 */

#include "Camera.h"

Camera::Camera()
{
}

Camera::~Camera()
{
	close();
}

void Camera::close()
{
#ifdef PCENCODE_CAM_DEFAULT
	_grabber.close();
#endif

#ifdef PCENCODE_CAM_CLEYE
	_grabber.close();
#endif

#ifdef PCENCODE_CAM_DC1394
	dc1394_video_set_transmission(_camera, DC1394_OFF);
	dc1394_capture_stop(_camera);
	dc1394_camera_free(_camera);
#endif

}

bool Camera::init()
{
	
	clear();

#ifdef PCENCODE_CAM_DEFAULT
	string input;
	int deviceid;
	bool validinput=false;
	
	_grabber.setVerbose(true);
	
//	while (!validinput)
//	 {
//	 ofSetLogLevel(OF_LOG_VERBOSE);
//	 _grabber.listDevices();
//	 ofSetLogLevel(OF_LOG_WARNING);
//	 
//	 cout << "Which devide do you want?";
//	 getline(cin, input);
//	 stringstream streamin(input);
//	 if (streamin >> deviceid)
//	 validinput=true;
//	 }
	
	_grabber.listDevices();
	
	_grabber.setDeviceID(ID);
	_grabber.videoSettings();
	
	_grabber.initGrabber(camWidth,camHeight);
	
	_charGrabFrame = new unsigned char [camWidth*camHeight];

#endif

#ifdef PCENCODE_CAM_CLEYE

	_grabber.setDeviceID(0);
	bool success = _grabber.initGrabber(camWidth,camHeight);

	_grabber.setAutoExposure(false);
	_grabber.setAutoGain(false);
	_grabber.setGain(gain);
	_grabber.setExposure(exposure);

	//this is to detect/circumvent bugs
	//that i'm having with this driver
	continuous=true;


	return success;

#endif

#ifdef PCENCODE_CAM_DC1394
	//create camera object
	d = dc1394_new ();
	if (!d)
		return false;
	
	//find cameras
	err = dc1394_camera_enumerate (d, &list);
	if (list->num==0)
	{
		cout <<"No cameras found, exiting\n";
		return false;
	}
	
	//initialise camera
	_camera = dc1394_camera_new (d, list->ids[0].guid);
    if (!_camera) {
        cout << "Failed to initialize camera with guid" << list->ids[0].guid << "\n";
        return false;
    }
	cout << "Using camera with GUID " << _camera->guid << "\n";
	
	list_modes();
	
	list_features();
	
	setupCameraParams();
	
	startTransmission();
	
	cout << "Camera is one shot capable: " << (_camera->one_shot_capable ? "true" : "false") << "\n";
	
	dc1394_capture_dequeue(_camera, DC1394_CAPTURE_POLICY_WAIT, &frame);
	
	return true;
#endif

}

void Camera::videoSettings()
{
	_grabber.videoSettings();
}

void Camera::clear()
{
	ofLog(OF_LOG_VERBOSE, "Camera: clear");
	_timeLastCapture = ofGetElapsedTimef();
}

bool Camera::capture(unsigned char *&pixels)
{
	bool hasWaited = ofGetElapsedTimef()-_timeLastCapture > float(captureDelay)/1000;
	
#ifdef PCENCODE_CAM_DEFAULT

	//grab frame
		
	_grabber.grabFrame();
	unsigned char * rgbPixels = _grabber.getPixels();
	
	for (int iPixel=0; iPixel<camWidth*camHeight; iPixel++)
	{
		_charGrabFrame[iPixel] = 0;
		for (int iColour=0; iColour<3; iColour++)
			_charGrabFrame[iPixel] += rgbPixels[iPixel*3+iColour]/3;
	}
	
	pixels = _charGrabFrame;
	
	//hasWaited &= _grabber.isFrameNew();

#endif

#ifdef PCENCODE_CAM_CLEYE

	_grabber.update();
	pixels = _grabber.getPixels();

#endif

#ifdef PCENCODE_CAM_DC1394

	if (_camera)
	{
		dc1394_capture_enqueue(_camera, frame);
		dc1394_capture_dequeue(_camera, DC1394_CAPTURE_POLICY_WAIT, &frame);
	}
	pixels = frame->image;

#endif

	if (hasWaited)
		_timeLastCapture = ofGetElapsedTimef();

	return hasWaited;
	
}

#ifdef PCENCODE_CAM_DC1394
void Camera::list_modes()
{

	dc1394video_modes_t video_modes;
    dc1394video_mode_t video_mode;
	
    dc1394color_coding_t coding;
    dc1394framerates_t framerates;	
	
    err=dc1394_video_get_supported_modes(_camera,&video_modes);
	
	for (int iVideoMode=0; iVideoMode<video_modes.num;iVideoMode++) {

		video_mode = video_modes.modes[iVideoMode];
		
		if (video_mode>=DC1394_VIDEO_MODE_FORMAT7_0)
		{
			cout << "mode=Format 7\n";
			continue;
		}
		
		dc1394_get_color_coding_from_video_mode(_camera,video_mode, &coding);
		dc1394_video_get_supported_framerates(_camera,video_mode,&framerates);
		cout <<
		"mode=" << lookup_mode(video_mode) <<
		" scalable=" << dc1394_is_video_mode_scalable(video_mode) <<
		" colour coding=" << coding <<
		" framerates=";
		
		for (int iFramerate = 0; iFramerate< framerates.num; iFramerate++)
			cout << lookup_fps(framerates.framerates[iFramerate]) << "fps, ";

		cout << "\n";
    }
}


int Camera::lookup_fps(int framerate)
{
	switch (framerate) {
		case DC1394_FRAMERATE_1_875:
			return 1.875;
		case DC1394_FRAMERATE_3_75:
			return 3.75;
		case DC1394_FRAMERATE_7_5:
			return 7.5;
		case DC1394_FRAMERATE_15:
			return 15;
		case DC1394_FRAMERATE_30:
			return 30;
		case DC1394_FRAMERATE_60:
			return 60;
		case DC1394_FRAMERATE_120:
			return 120;
		case DC1394_FRAMERATE_240:
			return 240;
	}
	return 0;
}

string Camera::lookup_mode(int video_mode)
{
	switch (video_mode) {
		case DC1394_VIDEO_MODE_160x120_YUV444:
			return "DC1394_VIDEO_MODE_160x120_YUV444";
		case DC1394_VIDEO_MODE_320x240_YUV422:
			return "DC1394_VIDEO_MODE_320x240_YUV422";
		case DC1394_VIDEO_MODE_640x480_YUV411:
			return "DC1394_VIDEO_MODE_640x480_YUV411";
		case DC1394_VIDEO_MODE_640x480_YUV422:
			return "DC1394_VIDEO_MODE_640x480_YUV422";
		case DC1394_VIDEO_MODE_640x480_RGB8:
			return "DC1394_VIDEO_MODE_640x480_RGB8";
		case DC1394_VIDEO_MODE_640x480_MONO8:
			return "DC1394_VIDEO_MODE_640x480_MONO8";
		case DC1394_VIDEO_MODE_640x480_MONO16:
			return "DC1394_VIDEO_MODE_640x480_MONO16";
	}
	return "couldn't find video mode, likely outside lookup table";
}

void Camera::list_features()
{
	//lists what settings the camera has, e.g. shutter, gain, zoom
	dc1394featureset_t features;
	
	err=dc1394_feature_get_all(_camera,&features);
    if (err!=DC1394_SUCCESS) {
        dc1394_log_warning("Could not get feature set");
    }
    else {
        dc1394_feature_print_all(&features, stdout);
    }
}

int Camera::setupCameraParams()
{
    err=dc1394_video_set_iso_speed(_camera, DC1394_ISO_SPEED_400);
    DC1394_ERR_CLN_RTN(err,shutdown(),"Could not set iso speed");
	
	//mono16 is better quality. but for development easier to stick with 8
    err=dc1394_video_set_mode(_camera, DC1394_VIDEO_MODE_640x480_MONO8);
    DC1394_ERR_CLN_RTN(err,shutdown(),"Could not set video mode");
	
    err=dc1394_video_set_framerate(_camera, DC1394_FRAMERATE_30);
    DC1394_ERR_CLN_RTN(err,shutdown(),"Could not set framerate");
	
    err=dc1394_capture_setup(_camera,2, DC1394_CAPTURE_FLAGS_DEFAULT);
    DC1394_ERR_CLN_RTN(err,shutdown(),"Could not setup camera-\nmake sure that the video mode and framerate are\nsupported by your camera");
	
	dc1394_feature_set_mode(_camera, DC1394_FEATURE_SHUTTER,DC1394_FEATURE_MODE_MANUAL);
	dc1394_feature_set_mode(_camera, DC1394_FEATURE_BRIGHTNESS,DC1394_FEATURE_MODE_MANUAL);
	dc1394_feature_set_mode(_camera, DC1394_FEATURE_EXPOSURE,DC1394_FEATURE_MODE_MANUAL);
	dc1394_feature_set_mode(_camera, DC1394_FEATURE_GAIN,DC1394_FEATURE_MODE_MANUAL);
	
}

int Camera::startTransmission()
{
	err=dc1394_video_set_transmission(_camera, DC1394_ON);
	DC1394_ERR_CLN_RTN(err,shutdown(),"Could not start camera iso transmission");
	
	//err=dc1394_video_set_one_shot(camera, DC1394_ON);
	//DC1394_ERR_CLN_RTN(err,shutdown(),"Could not set one shot mode on camera");
	

}

#endif