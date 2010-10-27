/*
 *  ofxKCScreensGUI.cpp
 *  Kimchi and Chips' Screens GUI
 *
 *  Created by Elliot Woods on 27/01/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "ofxKCScreensGUI.h"

ofxKCScreensGUI::ofxKCScreensGUI(int nScreenSlotsX, int nScreenSlotsY, float width, float height)
{
//	_strStatus = &strStatus;
	_iScreenHover = -1;
	_iScreenMaximised = -1;
	
//	_y_status = y_status  + GUI_FONT_SIZE*GUI_STATUS_LINES*2/3;
	
	_nScreenSlotsX = nScreenSlotsX;
	_nScreenSlotsY = nScreenSlotsY;
	
	_width = width;
	_height = height;
	
	assetLoad();
}

ofxKCScreensGUI::ofxKCScreensGUI()
{
	for (int iScreen=0; iScreen<_vecGUI.size(); iScreen++) {
		delete _vecGUI.at(iScreen);
	}
	_vecGUI.clear();
	
	assetUnload();
}

void ofxKCScreensGUI::arrange()
{
	int ix, iy;
	float x, y, w, h;
	
	w = _width/_nScreenSlotsX;
	h = _height/_nScreenSlotsY;
	
	for (int i=0; i<_vecGUI.size(); i++)
	{
		ix = i % _nScreenSlotsX;
		iy = i / _nScreenSlotsX;
		
		x = ix * w;
		y = iy * h;
		
		_vecGUI[i]->bounds = ofRectangle(x, y, w, h);
	}
}

void ofxKCScreensGUI::doFullscreen()
{
	if (_iScreenHover>=0 && _iScreenHover<_vecGUI.size())
		_vecGUI.at(_iScreenHover)->hitMaximise(dummyval);
	checkMaximised();
}

void ofxKCScreensGUI::update()
{
	g_updateIsUserActive();
}

void ofxKCScreensGUI::draw()
{
	if (_iScreenMaximised>=0)
	{
		_vecGUI.at(_iScreenMaximised)->draw();
		
		if (g_isUserActive && g_isInterfaceEnabled)			
			ofShowCursor(); // doing this every frame may be expensive. see if we can cache this effectively
		else
			ofHideCursor(); // doing this every frame may be expensive. see if we can cache this effectively
		
	} else {
		ofShowCursor(); // doing this every frame may be expensive. see if we can cache this effectively
		//but if it's only once per frame, is it really expensive?
		
		for (int iScreen=_vecGUI.size()-1; iScreen >=0 ; iScreen--)
			_vecGUI.at(iScreen)->draw();
		
//		_typer->drawString(status, 0, _y_status);
	}

}

//------------------------------------------------------------------------------------------------

void ofxKCScreensGUI::mouseOver(int x, int y)
{
	
	float xX, xY;
	int iScreen;
	
	findScreen(x,y,iScreen,xX,xY);
	
	if (iScreen>=0 && iScreen<_vecGUI.size())
	{
		_vecGUI.at(iScreen)->mouseOver(xX, xY, x, y);
//		_vecGUI.at(iScreen)->getStatus(*_strStatus);
		_iScreenHover = iScreen;
	}
	
	//probably move this line into scrBase
	char strMouseCoords[50];
	sprintf(strMouseCoords, "Mouse=(%1.3f, %1.3f)", xX, xY);

	interfaceNudge();
	
}

void ofxKCScreensGUI::mouseDown(int x, int y)
{
	float xX, xY;
	int iScreen;
	
	findScreen(x,y,iScreen,xX,xY);
	
	if (iScreen>=0 && iScreen<_vecGUI.size())
	{
		_vecGUI.at(iScreen)->mouseDown(xX, xY, x, y);
	}
	
	interfaceNudge();
}

void ofxKCScreensGUI::mouseReleased(int x, int y)
{
	float xX, xY;
	int iScreen;
	
	findScreen(x,y,iScreen,xX,xY);
	
	if (iScreen>=0 && iScreen<_vecGUI.size())
	{
		_vecGUI.at(iScreen)->mouseReleased(xX, xY, x, y);
	}
	
	checkMaximised();
}

void ofxKCScreensGUI::findScreen(int &x, int &y, int &iScreen, float &xX, float &xY)
{
	float w = _width / _nScreenSlotsX;
	float h = _height / _nScreenSlotsY;
	
	if (_iScreenMaximised>=0)
	{
		iScreen=_iScreenMaximised;
		xX = float(x)/ofGetScreenWidth();
		xY = float(y)/ofGetScreenHeight();
	} else {
		int iScreenX, iScreenY;
		
		iScreenX = x / w;
		iScreenY = y / h;
		iScreen = iScreenY * _nScreenSlotsX+ iScreenX;
		
		x %= (int)w;
		y %= (int)h;
		
		xX = float(x)/w;
		xY = float(y)/h;
	}
}

//------------------------------------------------------------------------------------------------

void ofxKCScreensGUI::addScreen(scrBase *Screen)
{
	_vecGUI.push_back(Screen);
}
//------------------------------------------------------------------------------------------------

void ofxKCScreensGUI::checkMaximised()
{
	bool foundMaximised=false;
	
	for (int iScreen=0; iScreen<_vecGUI.size(); iScreen++)
	{
		if (_vecGUI.at(iScreen)->maximised)
		{
			if (foundMaximised==true)
				_vecGUI.at(iScreen)->maximised=false;
			else {
				foundMaximised=true;
				_iScreenMaximised=iScreen;
			}
		}
	}
	
	if (!foundMaximised)
	{
		_iScreenMaximised=-1;
		ofSetFullscreen(false);
	} else {
		ofSetFullscreen(true);
	}
	
}
