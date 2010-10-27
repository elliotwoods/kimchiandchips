/*
 *  Camera.h
 *  PCEncode
 *
 *  Created by Elliot Woods on 28/12/2009.
 *  Copyright 2009 Kimchi and Chips. All rights reserved.
 *
 */
#define PCENCODE_CAM_DEFAULT

#ifdef PCENCODE_CAM_DEFAULT
#endif

#ifdef PCENCODE_CAM_CLEYE
	#include "ofxCLeye.h"
#endif

#ifdef PCENCODE_CAM_DC1394
	#include <dc1394/dc1394.h>
#endif


#include "ofMain.h"
#include "PCconstants.h"
#include "PCConfig.h"


class Camera : public PCConfig 
{
	public:
		Camera();
		~Camera();
		
		bool init();
		void close();
	
		void clear();
		bool capture(unsigned char *&pixels);
	
		void	videoSettings();
	
		int		ID;

#ifdef PCENCODE_CAM_DEFAULT
		ofVideoGrabber			_grabber;
		unsigned char *			_charGrabFrame;
#endif

#ifdef PCENCODE_CAM_CLEYE
		ofxCLeye				_grabber;
#endif

#ifdef PCENCODE_CAM_DC1394
	
		void list_modes();
		void list_features();
		int setupCameraParams();
		int startTransmission();

		int lookup_fps(int framerate);
		string lookup_mode(int video_mode);

		dc1394error_t err;
		dc1394_t * d;
		dc1394camera_t *_camera;
		dc1394camera_list_t * list;
		dc1394video_frame_t *frame;	
		unsigned char *			_charGrabFrame;

#endif
	
protected:
	float	_timeLastCapture;

		

};