#include "PCApp.h"

#include <cmath>

PCApp::PCApp()
{
	_camera = new Camera();
	_camera->init();
	initialise();
}

PCApp::~PCApp()
{
	_camera->close();
	delete _camera;
}

void PCApp::load()
{
	if (_isInitialised)
		uninitialse();
	initialise();
	setup();
}

void PCApp::initialise()
{
	_payload = new PayloadGraycode();
	_payload->setup();

	_screens = new GUI(_strStatus, PC_SCREENS_Y * PC_SCREEN_RESOLUTION_Y,
					   PC_SCREENS_X, PC_SCREENS_Y,
					   PC_SCREEN_RESOLUTION_X, PC_SCREEN_RESOLUTION_Y);
	_encoder = new PCEncode(_screens, _payload);
	_decoder = new PCDecode(_screens, _payload, _camera, _encoder->_texOutput);
	_logger = new PCLogger(*_encoder, *_decoder);

	_isInitialised = true;
}

void PCApp::uninitialse()
{
	delete _screens;
	delete _decoder;
	delete _encoder;
	delete _logger;
	delete _payload;

	_isInitialised = false;
}

//--------------------------------------------------------------
void PCApp::setup(){
	
	ofSetVerticalSync(true);
	
	_PC_State = PC_STATE_STANDBY;
	_iScanFrame = -1;
	_iCalibrationFrame = -1;
	
	_lastCaptureTime = ofGetElapsedTimeMillis();
	
	_manualShot=-1;

}
//--------------------------------------------------------------
void PCApp::update(){
	int * iFrame;
	int * nFrames;
	
	_screens->update();
	
	ofBackground(0,0,0);
	
	switch (_PC_State) {

		case PC_STATE_STANDBY:
			_screens->interfaceOn();
			break;
		
		case PC_STATE_CALIBRATING:
			_screens->interfaceOff();
			
			iFrame = &_iCalibrationFrame;
			nFrames = &_payload->calibrateFrames;
			
			break;
		
		case PC_STATE_SCANNING:
			_screens->interfaceOff();
			
			iFrame = &_iScanFrame;
			nFrames = &_payload->totalFrames;
			
			break;

		default:
			break;
	}
	
	if (_PC_State==PC_STATE_CALIBRATING || _PC_State==PC_STATE_SCANNING)
	{
		if (hasWaitedForCapture())
		{
			captureFrame(*iFrame, *nFrames);
			
			if (*iFrame == *nFrames-1)
			{
				
				_PC_State=PC_STATE_STANDBY;
				*iFrame=*nFrames;
				_screens->interfaceNudge();
				
			} else {
				
				(*iFrame)++;
				sendFrame(*iFrame);
				
			}
		}
	} else if(_camera->continuous)
		_decoder->capture();
	
}
//--------------------------------------------------------------

void PCApp::captureFrame(int iFrame, int nFrames)
{
	_decoder->capture();
	switch (_PC_State) {
		case PC_STATE_CALIBRATING:
			
			_decoder->addCalibrationFrame();
			
			if (iFrame >= nFrames-1)
				_decoder->calcThreshold();
			break;
		case PC_STATE_SCANNING:
			
			_decoder->addScanFrame(iScanInterleaveFrame(iFrame), iInterleave(iFrame));
			
			if (iInterleave(iFrame+1) != iInterleave(iFrame))
				_decoder->calcInterleave(iInterleave(iFrame));
			
			if (iFrame==_payload->totalFrames -1)
			{
				_decoder->updateCameraSpacePreview();
				_logger->save();
			}
			break;

		default:
			break;
	}
}

void PCApp::sendFrame(int iFrame)
{
	
	switch (_PC_State) {
		case PC_STATE_CALIBRATING:
			
			_encoder->updateCalibrationFrame(iFrame);
			
			break;
		case PC_STATE_SCANNING:
			
			if (iScanInterleaveFrame(iFrame)==0)
				_decoder->clearInterleave();
				
			
			_encoder->updateScanFrame(iScanInterleaveFrame(iFrame), iInterleave(iFrame));

			break;
			
		default:
			break;
	}
	_lastCaptureTime=ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------

void PCApp::draw(){
	
	//reset frame variables
	//move these to a seperate function if gets too large
	sprintf(_strDebug, "");
	
	drawStatus();

	_screens->draw();
	
	
}

void PCApp::drawStatus()
{
	string strStatus, strState;
	string strNewLine = "\n";
	
	char strIFrame[100], strInterleaves[100], strICalibrationFrame[100];
	char strNParityBits[100];
	char strIInterleaveFrame[100];
	char strFPS[100], strManualShot[100],strContinousCapture[100], strCaptureDelay[100];
	
	strState = "state=";
	switch (_PC_State) {
		case PC_STATE_STANDBY:
			strState += "standby";
			break;
		case PC_STATE_CALIBRATING:
			strState += "calibrating";
			break;
		case PC_STATE_SCANNING:
			strState += "scanning";
		default:
			break;
	}
	
	if (iScanInterleaveFrame(_iScanFrame) < _payload->dataFramesPerInterleave)
		sprintf(strIInterleaveFrame, " iInterleaveFrame=%d/%d", iScanInterleaveFrame(_iScanFrame), _payload->totalFramesPerInterleave);
	else 
		sprintf(strIInterleaveFrame, " iInterleaveFrame=%d/%d iParityFrame=%d/%d",
				iScanInterleaveFrame(_iScanFrame), _payload->totalFramesPerInterleave,
				(iScanInterleaveFrame(_iScanFrame) - _payload->dataFramesPerInterleave),
				errorBits);

	
	
	sprintf(strIFrame, " iScanFrame=%d/%d", _iScanFrame, _payload->totalFrames);
	sprintf(strInterleaves, " iInterleave=%d/%d", iInterleave(_iScanFrame), _payload->interleaves);
	sprintf(strNParityBits, " errorBits=%d", errorBits);
	sprintf(strICalibrationFrame, " iCalibrationFrame=%d/%d", _iCalibrationFrame, _payload->calibrateFrames);
	sprintf(strManualShot, " manualShot=%d", _manualShot);
	sprintf(strContinousCapture, " continuousShoot=%s", (_camera->continuous ? "true" : "false"));
	sprintf(strFPS, " fps=%3d", int(ofGetFrameRate()));
	sprintf(strCaptureDelay, " captureDelay=%dms", captureDelay);
	
	strStatus = strState + strIFrame + strInterleaves + strNParityBits +
				strIInterleaveFrame + strICalibrationFrame +
				strManualShot + strContinousCapture + strFPS + strCaptureDelay;
	
	strStatus += strNewLine + _strDebug;
	
	ofPushStyle();
	ofSetColor(255, 255, 255);
	_screens->status = strStatus;
	
	ofPopStyle();
}
//--------------------------------------------------------------
void PCApp::keyPressed(int key){
	
	_screens->interfaceNudge();
	
	switch (key)
	{
		case 'f': // f = fullscreen
			
			_screens->doFullscreen();
			break;

		case 32: // SPACE = run system
			_PC_State=PC_STATE_SCANNING;
			_decoder->clear();
			_iScanFrame=0;
			sendFrame(0);
			
			break;
			
		case 'c': // c = calibrate threshold
			_PC_State=PC_STATE_CALIBRATING;
			_decoder->resetCalibration();
			_iCalibrationFrame=0;
			sendFrame(0);
			
			break;

		case 'd': // d = capture one frame (to preview camera image)
			_decoder->capture();
			break;
		
		case 'e': // e = start/stop continuous capture. stopping continuous capture also resets manual shot mode
			if (_camera->continuous)
				_manualShot=-1;
			
			_camera->continuous = !_camera->continuous;
			break;
			
		case 'm':	// m = manual shot. pressing once starts manual shot mode.
					//every time press m after takes 1 shot.
					//start+stop continuous capture to reset manual shot mode 
			
			if (_PC_State>PC_STATE_STANDBY)
				_manualShot=1;
			else
				_manualShot=0;
			break;
		
		case 's': // s = stop current activity and standby
			_PC_State=PC_STATE_STANDBY;
			break;
			
		case 'p': // p = toggle camera space, projector space previews
			_screens->doSwitchTexture();
			break;
		
		case 'l': // l = load
			load();
			break;

	}
}

//--------------------------------------------------------------
void PCApp::keyReleased(int key){
	
	_screens->interfaceNudge();
	
}

//--------------------------------------------------------------
void PCApp::mouseMoved(int x, int y ){
	
	_screens->interfaceNudge();
	
	_screens->mouseOver(x, y);

}

//--------------------------------------------------------------
void PCApp::mouseDragged(int x, int y, int button){
	
	if (button==0)
	{
		_screens->mouseDown(x, y);
	}
	
	_screens->mouseOver(x, y);
	
}

//--------------------------------------------------------------
void PCApp::mousePressed(int x, int y, int button){
	
	_screens->interfaceNudge();
	
	if (button==0)
	{
		int ScreenX, ScreenY;
		
		_MouseStartX = x;
		_MouseStartY = y;
		
		_MouseStartScreenPixX = x % PC_SCREEN_RESOLUTION_X;
		_MouseStartScreenPixY = y % PC_SCREEN_RESOLUTION_Y;
		
		_MouseStartScreenFloatX = float(_MouseStartScreenPixX)/ float(PC_SCREEN_RESOLUTION_X);
		_MouseStartScreenFloatY = float(_MouseStartScreenPixY)/ float(PC_SCREEN_RESOLUTION_Y);
		
		mouseDown(x,y);
	}

}

//--------------------------------------------------------------
void PCApp::mouseReleased(int x, int y, int button){
	
	_screens->mouseReleased(x, y);
}

void PCApp::mouseDown(int x, int y)
{
	_screens->mouseDown(x, y);
}

//--------------------------------------------------------------
void PCApp::windowResized(int w, int h){

}
//--------------------------------------------------------------

bool PCApp::hasWaitedForCapture()
{
	if (_manualShot>-1)
	{
		if (_manualShot>0)
		{
			_manualShot=0;
			return true;
		} else
			return false;

	} else
		return ofGetElapsedTimeMillis()-_lastCaptureTime > captureDelay;
}

int	PCApp::iScanInterleaveFrame(int iScanFrame)
{
	return iScanFrame % _payload->totalFramesPerInterleave;
}

int PCApp::iInterleave(int iScanFrame)
{
	return iScanFrame / _payload->totalFramesPerInterleave;
}