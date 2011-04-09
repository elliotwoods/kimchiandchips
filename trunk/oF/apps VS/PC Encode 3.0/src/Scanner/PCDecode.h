#ifndef _PCDecode
#define _PCDecode
/*
 *  PCDecode.h
 *  PCDecode
 *
 *  Created by Elliot Woods on 25/10/2009.
 *  Copyright 2009 Kimchi and Chips. All rights reserved.
 *
 */

#include "CameraBase.h"
#include "PCconstants.h"
#include "PCConfig.h"

#include "ofMain.h"
//#include "ofxShader.h"

#include "PCEncode.h"
#include "ofxKCScreensGUI.h"

#include "PCPixel.h"

#include "PayloadBase.h"

#include <algorithm>

class PCDecode : public PCConfig 
{
	public:
		PCDecode(PayloadBase *payload, CameraBase *camera, bool *boolProjectorMask);
		~PCDecode();
	
		bool capture(bool needFreshFrame=false);
	
		void updateCameraSpacePreview();
		void updateProjectorSpacePreview();
	
		void resetCalibration();
		void resetData();
	
		void addCalibrationFrame();
		void addScanFrame(int iScanInterleaveFrame, int iInterleave);
		
		void calcThreshold();
		void calcInterleave(int iInterleave);
		
		void clear(bool clearFinds);
		void clearInterleave();
	
		int	getFrameData(int iCameraPixel);
		int getFrameParity(int iCameraPixel);
		int	getNFinds(int iProjectorPixel);
		void getFoundPixelData(int iProjectorPixel, float &CamMeanX, float &CamMeanY, int &iLastFoundCameraPixel);
		void getFoundPixelData(int iProjectorPixel, float &CamMeanX, float &CamMeanY, float &CamSigmaX, float &CamSigmaY, int &iLastFoundCameraPixel);

		unsigned char *			_charCameraSpacePreview;
		unsigned char *			_charProjectorSpacePreview;
	
		scrBase					*_scrFrameData, *_scrBinary, *_scrCamera;
		scrBase					*_scrThreshold, *_scrThresholdMask;
		scrBase					*_scrProjectorSpace, *_scrCameraSpace;
		scrBase					*_scrCameraNFinds, *_scrProjectorNFinds;
		scrHistograms			*_scrHistograms;
	
		std::vector<PCPixel*>	projPixels;
		std::vector<PCPixel*>	camPixels;
	
	private: 
		void					checkParity();
		void					updateThresholdSelection(int &iSelectionClass);
		void					renderThresholdMask();
		void					updateSpacePreview(int width, int height, unsigned char data[], ofTexture &texture, std::vector<PCPixel*> &foundPixels);
		
		void					moveSendCursor(ofPoint &ptCursorPosition);
	
		PayloadBase				*_payload;
		CameraBase				*_camera;
		
		ofTexture				*_texBinary, *_texFrameDataPreview;
		ofTexture				*_texThreshold, *_texThresholdMasked, *_texThresholdMask;
		ofTexture				*_texCamera;
		ofTexture				_texCameraSpacePreview, _texProjectorSpacePreview;
		ofTexture				_texCameraSpaceNFinds, _texProjectorSpaceNFinds;
	
		//ofxShader				_shadLookup;
		
		Histogram	 *			_histThresholdRange;
		Histogram	 *			_histNFinds;
		Histogram	 *			_histDeviation;
		
		unsigned char *			_charCamera;
	
		unsigned char *			_charBinary;
		unsigned char *			_charFrameDataPreview;
		unsigned char *			_charCameraSpaceNFinds, *_charProjectorSpaceNFinds;
		bool *					_boolBinary;
		
		int						_intThresholdSelection;
		unsigned char *			_charCalibrateMin;
		unsigned char *			_charCalibrateMax;
		unsigned char *			_charThreshold;
		unsigned char *			_charThresholdRange;
		bool *					_boolThresholdMask;
		unsigned char *			_charThresholdMasked;
		unsigned char *			_charThresholdMask;
	
		unsigned long *			_intFrameData;
		unsigned long *			_intFrameParity;
		bool *					_boolFrameValid;
	
		bool *					_boolProjectorMask;
	
};

#endif