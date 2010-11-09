#include "PCApp.h"

#include <cmath>

PCApp::PCApp()
{
	_screens = new ofxKCScreensGUI(0,0,ofGetWidth(),ofGetHeight());

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
	
	//add the send message group
	scrGroupTabbed *scrMessageGroup = new scrGroupTabbed();
	scrMessageGroup->push(&_scanner._encoder->scrSend);
	scrMessageGroup->push(&_scanner._scrProjectorMask);
	gridScan->push(scrMessageGroup);
	
	gridScan->push(&_scanner._scrControls);
	
	for (int iCam=0; iCam<PCConfig().nCameras; iCam++)
	{
		//add projection space preview to the send group
		scrGroupTabbed *scrProjDataGroup = new scrGroupTabbed();
		scrProjDataGroup->push(_scanner._decoder[iCam]->_scrProjectorSpace);
		scrProjDataGroup->push(_scanner._decoder[iCam]->_scrProjectorNFinds);
		gridScan->push(scrProjDataGroup);
		
		scrGroupTabbed *scrCamDataGroup = new scrGroupTabbed();
		scrCamDataGroup->push(_scanner._decoder[iCam]->_scrFrameData);
		scrCamDataGroup->push(_scanner._decoder[iCam]->_scrCameraSpace);
		scrCamDataGroup->push(_scanner._decoder[iCam]->_scrCameraNFinds);
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
	// MAIN TAB GROUP
	/////////////////////////////////////////////////////////
	_scrTabMain = new scrGroupTabbed(32);
	
	_scrTabMain->push(gridScan);
	_scrTabMain->push(&_Correlator.scrGridMain);
	
	_screens->mainScreen = _scrTabMain;
	/////////////////////////////////////////////////////////
	
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
	
	_screens->showInterface(_scanner.state==0 || _scrTabMain->iSelection>0);
}

//--------------------------------------------------------------

void PCApp::draw(){

	if (_scanner.state==PC_STATE_SCANNING)
		ofLog(OF_LOG_VERBOSE, "PCApp: drawing interleave frame " + ofToString(_scanner._payload->iScanInterleaveFrame(_scanner.iFrame)));

	if (_scanner.state==PC_STATE_CALIBRATING)
		ofLog(OF_LOG_VERBOSE, "PCApp: drawing calibration frame " + ofToString(_scanner.iFrame, 0));
	
	_screens->draw();
	
	
}
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
		break;
			
		case 'c': // c = calibrate threshold
			_scanner.calibrate();
			break;

		case 's': // s = save current activity
			//_scanner.save(getDateString());
			_scanner.save(ofToString(_scanner.screenDistance, 2));
			break;
		
		case 'd': // d = save as 'data'
			_scanner.save(ofToString(PCConfig.projWidth) + "," + ofToString(_scanner.interleaveWidth) + "x" +
				ofToString(PCConfig.projHeight) + "," + ofToString(_scanner.interleaveHeight));
			break;
		

		case 'v': // v = video settings
			_scanner.videoSettings();
			break;

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