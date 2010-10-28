#include "PCApp.h"

#include <cmath>

PCApp::PCApp()
{
	_screens = new ofxKCScreensGUI(4,3,ofGetWidth(),ofGetHeight());

}

PCApp::~PCApp()
{
	delete _screens;
}

//--------------------------------------------------------------

void PCApp::setup(){
	ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	//setup scanner
	_scanner.setup();
	
	//add screens
	scrGroup *grid = new scrGroup();
	_screens->mainScreen = grid;
	
	for (int iCam=0; iCam<PCConfig().nCameras; iCam++)
	{
 		grid->screens.push_back(_scanner._decoder[iCam]->_scrSend);
		grid->screens.push_back(_scanner._decoder[iCam]->_scrFrameData);
		grid->screens.push_back(_scanner._decoder[iCam]->_scrBinary);
		grid->screens.push_back(_scanner._decoder[iCam]->_scrThreshold);
		grid->screens.push_back(_scanner._decoder[iCam]->_scrHistograms);
		grid->screens.push_back(_scanner._decoder[iCam]->_scrCamera);
	}
	
	grid->setBounds(0, 0, ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------

void PCApp::update(){
	_scanner.update();
}

//--------------------------------------------------------------

void PCApp::draw(){
	
//	if (_scanner.state>0)
//		ofLog(OF_LOG_VERBOSE, "PCApp: Draw iFrame " + 
//			  ofToString(_scanner.iFrame, 0));
//	
	//reset frame variables
	//move these to a seperate function if gets too large
//	sprintf(_strDebug, "");
	
//	drawStatus();

	_screens->draw();
	
	
}

//void PCApp::drawStatus()
//{
//	string strStatus, strState;
//	string strNewLine = "\n";
//	
//	char strIFrame[100], strInterleaves[100], strICalibrationFrame[100];
//	char strNParityBits[100];
//	char strIInterleaveFrame[100];
//	char strFPS[100], strContinousCapture[100], strCaptureDelay[100];
//	
//	strState = "state=";
//	switch (_PC_State) {
//		case PC_STATE_STANDBY:
//			strState += "standby";
//			break;
//		case PC_STATE_CALIBRATING:
//			strState += "calibrating";
//			break;
//		case PC_STATE_SCANNING:
//			strState += "scanning";
//		default:
//			break;
//	}
//	
//	if (iScanInterleaveFrame(_iScanFrame) < _payload->dataFramesPerInterleave)
//		sprintf(strIInterleaveFrame, " iInterleaveFrame=%d/%d", iScanInterleaveFrame(_iScanFrame), _payload->totalFramesPerInterleave);
//	else 
//		sprintf(strIInterleaveFrame, " iInterleaveFrame=%d/%d iParityFrame=%d/%d",
//				iScanInterleaveFrame(_iScanFrame), _payload->totalFramesPerInterleave,
//				(iScanInterleaveFrame(_iScanFrame) - _payload->dataFramesPerInterleave),
//				errorBits);
//
//	
//	
//	sprintf(strIFrame, " iScanFrame=%d/%d", _iScanFrame, _payload->totalFrames);
//	sprintf(strInterleaves, " iInterleave=%d/%d", iInterleave(_iScanFrame), _payload->interleaves);
//	sprintf(strNParityBits, " errorBits=%d", errorBits);
//	sprintf(strICalibrationFrame, " iCalibrationFrame=%d/%d", _iCalibrationFrame, _payload->calibrateFrames);
//	sprintf(strContinousCapture, " continuousShoot=%s", (_camera->continuous ? "true" : "false"));
//	sprintf(strFPS, " fps=%3d", int(ofGetFrameRate()));
//	sprintf(strCaptureDelay, " captureDelay=%dms", captureDelay);
//	
//	strStatus = strState + strIFrame + strInterleaves + strNParityBits +
//				strIInterleaveFrame + strICalibrationFrame +
//				strContinousCapture + strFPS + strCaptureDelay;
//	
//	strStatus += strNewLine + _strDebug;
//	
//	ofPushStyle();
//	ofSetColor(255, 255, 255);
//	_screens->status = strStatus;
//	
//	ofPopStyle();
//}
//--------------------------------------------------------------
void PCApp::keyPressed(int key){
	
	_screens->interfaceNudge();
	
	switch (key)
	{
		case 'f': // f = fullscreen
			_screens->doFullscreen();
			break;

		case 32: // SPACE = run system
			_scanner.start();
			break;
			
		case 'c': // c = calibrate threshold
			_scanner.calibrate();
			break;

		case 's': // s = stop current activity and standby
			_scanner.stop();
			break;

// currently out of order
// causes a GDB crash, might not crash without GDB
//		case 'v': // v = video settings
//			_scanner.videoSettings();
//			break;

// currently out of order
//		case 'l': // l = load
//			load();
//			break;

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