/*
 *  PCDecode.cpp
 *  PCDecode
 *
 *  Created by Elliot Woods on 25/10/2009.
 *  Copyright 2009 Kimchi and Chips. All rights reserved.
 *
 */


#include "PCincludes.h"

PCDecode::PCDecode(PayloadBase *payload, Camera *camera)
{		
	_payload = payload;
	_camera = camera;

	int nProjPixels = projWidth*projHeight;
	int nCamPixels = camWidth*camHeight;

	_charCamera = new unsigned char[nCamPixels*3];
	_boolBinary = new bool[nCamPixels];
	_charBinary	= new unsigned char[nCamPixels];
	_charFrameDataPreview = new unsigned char[nCamPixels];
	
	_intFrameData = new unsigned long[nCamPixels];
	_intFrameParity = new unsigned long[nCamPixels];
	_boolFrameValid = new bool[nCamPixels];
	
	_charThreshold = new unsigned char[nCamPixels];
	_charCalibrateMin = new unsigned char[nCamPixels];
	_charCalibrateMax = new unsigned char[nCamPixels];
	_charThresholdRange = new unsigned char[nCamPixels];
	_boolThresholdMask = new bool[nCamPixels];
	_charThresholdMasked = new unsigned char[nCamPixels];
	_charThresholdMask = new unsigned char[nCamPixels];
	
	_charCameraSpacePreview = new unsigned char[nProjPixels*3];
	_charProjectorSpacePreview = new unsigned char[nCamPixels*3];

	_texCamera = new ofTexture();
	_texThreshold = new ofTexture();
	_texThresholdMasked = new ofTexture();
	_texThresholdMask = new ofTexture();
	
	_texBinary = new ofTexture();
	_texFrameDataPreview = new ofTexture();
	
	_texThreshold->allocate(camWidth,camHeight,GL_LUMINANCE);
	_texThresholdMasked->allocate(camWidth, camHeight, GL_LUMINANCE);
	_texThresholdMask->allocate(camWidth, camHeight, GL_LUMINANCE);
	_texBinary->allocate(camWidth,camHeight,GL_LUMINANCE);
	_texFrameDataPreview->allocate(camWidth,camHeight,GL_LUMINANCE);
	_texCamera->allocate(camWidth, camHeight, GL_LUMINANCE);
	
	_texCameraSpacePreview.allocate(projWidth, projHeight, GL_RGB);
	_texCameraSpacePreview.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);

	_texProjectorSpacePreview.allocate(camWidth, camHeight, GL_RGB);
	
	for (int iPixel=0; iPixel<nProjPixels; iPixel++)
		projPixels.push_back(new PCPixel());
	
	for (int iPixel=0; iPixel<nCamPixels; iPixel++)
		camPixels.push_back(new PCPixel());
	
	_histThresholdRange = new Histogram("Difference between MIN and MAX to calc threshold.", 256);
	_histNFinds = new Histogram("Camera pixels found per projector pixel.", nProjPixels);
	_histDeviation = new Histogram("Deviation of projector pixels in camera space.", 256);
	
	ofAddListener(_histThresholdRange->updateSelection,this,&PCDecode::updateThresholdSelection);


	_scrProjectorSpace = new scrTexture(cursor_xy, false, _texCameraSpacePreview, "Projector space preview");
	ofAddListener(_scrProjectorSpace->evtCursorMove, this, &PCDecode::moveSendCursor);
	
	_scrFrameData = new scrTexture(cursor_none, true, *_texFrameDataPreview, "Frame data");
	
	_scrCameraSpace = new scrTexture(cursor_none, true, _texProjectorSpacePreview, "Camera space preview");
	
	_scrBinary = new scrTexture(cursor_none, false, *_texBinary, "Binary image");
	
	_scrThreshold = new scrTexture(cursor_none, false,*_texThresholdMasked, "Threshold");
	
	_scrThresholdMask = new scrTexture(cursor_none, false,*_texThresholdMask, "Threshold mask");
	
	_scrHistograms = new scrHistograms(cursor_none, false, "Histograms");
	_scrHistograms->addHistogram(*_histThresholdRange);
	_scrHistograms->addHistogram(*_histNFinds);
	
	_scrCamera = new scrTexture(cursor_none, false,  *_texCamera, "Camera");
	
}

PCDecode::~PCDecode()
{

	delete _histThresholdRange;
	delete _histNFinds;
	
	delete _charBinary;
	delete _charFrameDataPreview;
	delete _boolBinary;
	
	delete _charCalibrateMin;
	delete _charCalibrateMax;
	delete _charThreshold;
	delete _charThresholdRange;
	delete _boolThresholdMask;
	delete _charThresholdMasked;
	delete _charThresholdMask;
	
	delete _charCameraSpacePreview;
	delete _charProjectorSpacePreview;
	
	delete _intFrameData;
	delete _intFrameParity;
	delete _boolFrameValid;
	
	delete _scrFrameData;
	delete _scrCameraSpace;
	delete _scrProjectorSpace;
	
}

bool PCDecode::capture()
{
	
	bool isFrameNew = _camera->capture(_charCamera);
	
	_texCamera->loadData(_charCamera, 640, 480, GL_LUMINANCE);


	int nCamPixels = camWidth*camHeight;

	for (int i=0; i<nCamPixels; i++)
	{
		_boolBinary[i] = _charCamera[i] > _charThreshold[i];
		_charBinary[i] = (_boolBinary[i] && _boolThresholdMask[i]) * 255;
	}
	
	_texBinary->loadData(_charBinary, camWidth, camHeight, GL_LUMINANCE);
	
	return isFrameNew;

}

void PCDecode::updateSpacePreview(int width, int height, unsigned char data[], ofTexture &texture, std::vector<PCPixel*> &foundPixels)
{
	float Xdash, Ydash;
	int nPixels = width * height;
	
	for (int iPixel=0; iPixel<nPixels; iPixel++)
	{
		foundPixels.at(iPixel)->getData(Xdash, Ydash);
		
		data[iPixel*3] = float(255) * Xdash;
		data[iPixel*3+1] = float(255) * Ydash;
		data[iPixel*3+2] = 0;
	}
	texture.loadData(data, width, height, GL_RGB);	
}

void PCDecode::updateCameraSpacePreview()
{
	updateSpacePreview(projWidth,projHeight,_charCameraSpacePreview,_texCameraSpacePreview, projPixels);
}

void PCDecode::drawCameraSpacePreview(int screenx, int screeny)
{
	_texCameraSpacePreview.draw(PC_SCREEN_RESOLUTION_X*screenx,PC_SCREEN_RESOLUTION_Y*screeny,PC_SCREEN_RESOLUTION_X,PC_SCREEN_RESOLUTION_Y);
}

void PCDecode::drawCameraSpacePreview()
{
	int resX, resY;
	resX = ofGetScreenWidth();
	resY = ofGetScreenHeight();
	
	_texCameraSpacePreview.draw(0,0,resX,resY);
	
}

void PCDecode::updateProjectorSpacePreview()
{
	updateSpacePreview(camWidth,camHeight,_charProjectorSpacePreview,_texProjectorSpacePreview, camPixels);
}

void PCDecode::drawProjectorSpacePreview(int screenx, int screeny)
{
	_texProjectorSpacePreview.draw(PC_SCREEN_RESOLUTION_X*screenx,PC_SCREEN_RESOLUTION_Y*screeny,PC_SCREEN_RESOLUTION_X,PC_SCREEN_RESOLUTION_Y);
}

void PCDecode::moveSendCursor(ofPoint &ptCursorPosition)
{
	float &x = ptCursorPosition.x;
	float &y = ptCursorPosition.y;
	std::vector<ofPoint> *vecXdash;
	
	int iPixelX, iPixelY, iPixel;
	
	iPixelX = x*projWidth;
	iPixelY = y*projHeight;
	iPixel = iPixelY * projWidth + iPixelX;
	
	if (iPixel>0 && iPixel < projWidth * projHeight)
	{
		projPixels.at(iPixel)->getData(&vecXdash);
		
		_scrFrameData->setMarkers(vecXdash);
		
		_scrFrameData->updateInterface();
	} else {
		_scrFrameData->clearMarkers();
	}

}

void PCDecode::resetCalibration()
{
	int nCamPixels = camWidth*camHeight;

	for (int i=0; i<nCamPixels; i++)
	{
		_charCalibrateMin[i]=255;
		_charCalibrateMax[i]=0;
	}
}

void PCDecode::addScanFrame(int iScanInterleaveFrame, int iInterleave)
{
	
	int nScanFramesPerInterleave = _payload->totalFramesPerInterleave;
	int nScanDataFramesPerInterleave = _payload->dataFramesPerInterleave;
	int binaryFrame;
	
	binaryFrame = 1 << (iScanInterleaveFrame % nScanDataFramesPerInterleave);
	
	int nCamPixels = camWidth*camHeight;
	for (int i=0; i<nCamPixels; i++)
	{
		if (_boolThresholdMask[i])
		{
			//store the values that we receive
			if (iScanInterleaveFrame < nScanDataFramesPerInterleave)
				_intFrameData[i] |= _boolBinary[i]  * binaryFrame;
			else
				_intFrameParity[i] |= _boolBinary[i] * binaryFrame;
			
			_charFrameDataPreview[i]=double(_intFrameData[i])/
									double(1<<nScanDataFramesPerInterleave)
									*255;
			
		} else {
			_intFrameData[i]=0;
			_charFrameDataPreview[i]=0;
		}
		
	}
	
	if(iScanInterleaveFrame == nScanFramesPerInterleave-1)
		checkParity();
	
	_texFrameDataPreview->loadData(_charFrameDataPreview, camWidth, camHeight, GL_LUMINANCE);
}

void PCDecode::addCalibrationFrame()
{
	int nCamPixels = camWidth*camHeight;
	for (int i=0; i<nCamPixels; i++)
	{
		_charCalibrateMin[i]=min(_charCamera[i],_charCalibrateMin[i]);
		_charCalibrateMax[i]=max(_charCamera[i],_charCalibrateMax[i]);
	}
}


void PCDecode::calcThreshold()
{
	
	_histThresholdRange->clear();
	
	int rangeMinMax;

	int nCamPixels = camWidth*camHeight;
	for (int i=0; i<nCamPixels; i++)
	{
		rangeMinMax = _charCalibrateMax[i] -_charCalibrateMin[i];
		
		_charThreshold[i] = rangeMinMax * thresholdPercentile
							+ _charCalibrateMin[i];
		
		_charThresholdRange[i] = rangeMinMax;
		
		_histThresholdRange->add(rangeMinMax);
		
	}
	
	renderThresholdMask();
	
	_texThreshold->loadData(_charThreshold, camWidth, camHeight, GL_LUMINANCE);
}

void PCDecode::calcInterleave(int iInterleave)
{
	int iCamPixelX, iCamPixelY, iCamPixel;
	float xCamX, xCamY;

	int iProjectorInterleavePixelX, iProjectorInterleavePixelY;
	
	int iProjectorPixel, iProjectorPixelX, iProjectorPixelY;

	float xProjectorX, xProjectorY;
	
	int iInterleaveX = iInterleave % interleaveWidth;
	int iInterleaveY = iInterleave / interleaveHeight;
	
	int interleavePixelsX = (projWidth / interleaveWidth);

	bool isValid;
	
	for (iCamPixelX = 0; iCamPixelX < camWidth; iCamPixelX++)
		for (iCamPixelY=0; iCamPixelY < camHeight; iCamPixelY++) {
			
			iCamPixel = iCamPixelY * camWidth + iCamPixelX;
			
			if (_boolThresholdMask[iCamPixel]) // DOES THIS PIXEL FIT WITHIN SELECTED THRESHOLD MIN RANGE
			{
				// calculate camera space floatwise
				xCamX = float(iCamPixelX)/float(camWidth-1);
				xCamY = float(iCamPixelY)/float(camHeight-1);			
				
				//use payload to decode reading into position indicies
				isValid = _payload->decode(_intFrameData[iCamPixel],iProjectorInterleavePixelX, iProjectorInterleavePixelY);

				// assign last found value indexwise				
				iProjectorPixelX = iProjectorInterleavePixelX * interleaveWidth + iInterleaveX;
				iProjectorPixelY = iProjectorInterleavePixelY * interleaveHeight + iInterleaveY;
				iProjectorPixel = iProjectorPixelY * projWidth + iProjectorPixelX;
				
				xProjectorX = float(iProjectorPixelX)/float(projWidth);
				xProjectorY = float(iProjectorPixelY)/float(projHeight);
				
				if (isValid)
				{
					projPixels.at(iProjectorPixel)->addFind(iCamPixel, xCamX, xCamY);
				
					camPixels.at(iCamPixel)->addFind(iProjectorPixel, xProjectorX, xProjectorY);
				}
			}
		}
	
	updateProjectorSpacePreview();
	updateCameraSpacePreview();

	if (iInterleave<_payload->interleaves-1)
		clearInterleave();
}

void PCDecode::checkParity()
{
	
	int iProjectorPixel;
	int intSentParity;
	
	int nCamPixels = camWidth*camHeight;
	for (int i=0; i<nCamPixels; i++)
	{
		iProjectorPixel = _intFrameData[i];
		
		if (iProjectorPixel< _payload->nPixelsPerInterleave && iProjectorPixel>0)
		{
			
			intSentParity = _payload->errorCheck[iProjectorPixel];
			
			_boolFrameValid[i] = (intSentParity == _intFrameParity[i]);
			
		} else
			_boolFrameValid[i] = false;
		
		_charFrameDataPreview[i] *= _boolFrameValid[i];			
		
	}
	
	_texFrameDataPreview->loadData(_charFrameDataPreview, camWidth, camHeight, GL_LUMINANCE);
	
}

void PCDecode::clearInterleave()
{
	int nCamPixels = camWidth*camHeight;
	for (int i=0; i<nCamPixels; i++)
	{
		_intFrameData[i]=0;
		_intFrameParity[i]=0;
	}
}

void PCDecode::clear()
{
	int nCamPixels = camWidth*camHeight;

	for (int iProjectorPixel = 0; iProjectorPixel<projWidth*projHeight; iProjectorPixel++)
		projPixels.at(iProjectorPixel)->clear();
	
	for (int iCameraPixel = 0; iCameraPixel < nCamPixels; iCameraPixel++)
		camPixels.at(iCameraPixel)->clear();
	
	clearInterleave();

}

void PCDecode::updateThresholdSelection(int &iSelectionClass)
{
	_intThresholdSelection=iSelectionClass;
	renderThresholdMask();
}

void PCDecode::renderThresholdMask()
{
	int nCamPixels = camWidth*camHeight;
	for (int i=0; i<nCamPixels; i++)
	{	
		_boolThresholdMask[i] = _charThresholdRange[i] > _intThresholdSelection;
		
		_charThresholdMask[i] = _boolThresholdMask[i] * 255;
		
		_charThresholdMasked[i] = _charThreshold[i] * _boolThresholdMask[i];
	}
	
	_texThresholdMasked->loadData(_charThresholdMasked, camWidth, camHeight, GL_LUMINANCE);
	
	_texThresholdMask->loadData(_charThresholdMask, camWidth,camHeight, GL_LUMINANCE);
}

int PCDecode::getFrameData(int iCameraPixel)
{
	return _intFrameData[iCameraPixel];
}

int PCDecode::getFrameParity(int iCameraPixel)
{
	return _intFrameParity[iCameraPixel];
}

int PCDecode::getNFinds(int iProjectorPixel)
{
	if (iProjectorPixel>=0 && iProjectorPixel<projWidth*projHeight)
		return projPixels.at(iProjectorPixel)->getNFinds();
	else
		return 0;
}

void PCDecode::getFoundPixelData(int iProjectorPixel, float &CamMeanX, float &CamMeanY, float &CamSigmaX, float &CamSigmaY, int &iLastFoundCameraPixel)
{
	
	if (iProjectorPixel>=0 && iProjectorPixel<projWidth*projHeight)
		projPixels.at(iProjectorPixel)->getData(CamMeanX, CamMeanY, CamSigmaX, CamSigmaY, iLastFoundCameraPixel);
	
}