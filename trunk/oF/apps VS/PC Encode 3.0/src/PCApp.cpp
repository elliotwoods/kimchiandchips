#include "PCApp.h"

#include <cmath>

PCApp::PCApp()
{
	_screens = new ofxKCScreensGUI(0,0,ofGetWidth(),ofGetHeight());
	_screenDistance = 0.5;

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
	
	/////////////////////////////////////////////////////////
	// SCAN GROUP
	/////////////////////////////////////////////////////////
	scrGroupGrid *gridScan = new scrGroupGrid("Scan");
	scrGroupTabbed *scrSendGroup = new scrGroupTabbed();
	
	//add the projection space group first to the grid
	gridScan->push(scrSendGroup);
	
	//add the send message to the send group (first screen)
	scrSendGroup->push(&_scanner._encoder->scrSend);
	
	for (int iCam=0; iCam<PCConfig().nCameras; iCam++)
	{
		//add projection space preview to the send group
		scrSendGroup->push(_scanner._decoder[iCam]->_scrProjectorSpace);
		
		scrGroupTabbed *scrCamDataGroup = new scrGroupTabbed();
		scrCamDataGroup->push(_scanner._decoder[iCam]->_scrFrameData);
		scrCamDataGroup->push(_scanner._decoder[iCam]->_scrCameraSpace);
		gridScan->push(scrCamDataGroup);
		
		scrGroupTabbed *scrCamSourceGroup = new scrGroupTabbed();
		scrCamSourceGroup->push(_scanner._decoder[iCam]->_scrCamera);
		scrCamSourceGroup->push(_scanner._decoder[iCam]->_scrBinary);
		gridScan->push(scrCamSourceGroup);
		
		scrGroupTabbed *scrThresoldGroup = new scrGroupTabbed();
		scrThresoldGroup->push(_scanner._decoder[iCam]->_scrThreshold);
		scrThresoldGroup->push(_scanner._decoder[iCam]->_scrThresholdMask);
		gridScan->push(scrThresoldGroup);
		
		gridScan->screens.push_back(_scanner._decoder[iCam]->_scrHistograms);
	}
	/////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////
	// CORRELATE GROUP
	/////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////
	// MAIN TAB GROUP
	/////////////////////////////////////////////////////////
	_scrTabMain = new scrGroupTabbed(32);
	
	_scrTabMain->push(gridScan);
	_scrTabMain->push(&_Correlator.scrGrid);
	
	_screens->mainScreen = _scrTabMain;
	/////////////////////////////////////////////////////////
	
	
	/////////////////////////////////////////////
	// ADD USER CONTROLS
	/////////////////////////////////////////////
	scrWidgets *scrControl = new scrWidgets("User controls");
	wdgBase *sliderDistance = new wdgSlider("Screen distance",
										  _screenDistance,
										  0, 1,
										  0.01,
										  "meters"); 
	
	_wdgFrameCounter= new wdgCounter("iFrame",
								  _scanner.iFrame,
								  0);
	
	scrControl->push(sliderDistance);
	scrControl->push(_wdgFrameCounter);
	gridScan->screens.push_back(scrControl);
	/////////////////////////////////////////////
	
//	tabMain->setBounds(0, 0, ofGetWidth(), ofGetHeight());
	
	ofBackground(20, 20, 20);
}

//--------------------------------------------------------------

void PCApp::update(){
	
	switch (_scrTabMain->iSelection) {
		case 0:
			//we're looking at scan
			_scanner.update();
			break;
			
		case 1:
			//we're looking at correlate
			_Correlator.update();
			break;
			
		default:
			break;
	}
	_scanner.update();
	
	_screens->showInterface(_scanner.state==0 || _scrTabMain->iSelection>0);
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

	if (_scanner.state>0)
		ofLog(OF_LOG_VERBOSE, "PCApp: drawing interleave frame " + ofToString(_scanner._payload->iScanInterleaveFrame(_scanner.iFrame)));

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
			ofSetFullscreen(_screens->doFullscreen());
			break;

		case 32: // SPACE = run system
			if (_scanner.state==0)
				_scanner.start();
			else
				_scanner.stop();
			
			_wdgFrameCounter->setMax(_scanner._payload->totalFrames);
			
			break;
			
		case 'c': // c = calibrate threshold
			_scanner.calibrate();
			_wdgFrameCounter->setMax(_scanner._payload->interleaves+1);			
			break;

		case 's': // s = save current activity
			//_scanner.save(getDateString());
			_scanner.save(ofToString(_screenDistance, 2));
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
	
	_screens->mouseMoved(x, y);

}

//--------------------------------------------------------------
void PCApp::mouseDragged(int x, int y, int button){
	_screens->mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void PCApp::mousePressed(int x, int y, int button){
	
	_screens->interfaceNudge();
	
	if (button==0)
	{
		int ScreenX, ScreenY;
		
//		_MouseStartX = x;
//		_MouseStartY = y;
//		
//		_MouseStartScreenPixX = x % PC_SCREEN_RESOLUTION_X;
//		_MouseStartScreenPixY = y % PC_SCREEN_RESOLUTION_Y;
//		
//		_MouseStartScreenFloatX = float(_MouseStartScreenPixX)/ float(PC_SCREEN_RESOLUTION_X);
//		_MouseStartScreenFloatY = float(_MouseStartScreenPixY)/ float(PC_SCREEN_RESOLUTION_Y);
		
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
	_screens->resize(w, h);
}


string PCApp::getDateString()
{
	char datestring[10];
	
	time_t now = time(0);
	tm hereandnow=*localtime(&now);
	
	const char format[]="%Y%m%d";
	strftime(datestring, 50, format, &hereandnow);
	
	return string(datestring);
}