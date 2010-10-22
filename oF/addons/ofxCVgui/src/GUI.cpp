/*
 *  GUI.cpp
 *  pixel correlation
 *
 *  Created by Elliot Woods on 27/01/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "GUI.h"

GUI::GUI(string &strStatus, float y_status, int nScreenSlotsX, int nScreenSlotsY, float resScreenX, float resScreenY)
{
	_strStatus = &strStatus;
	_iScreenHover = -1;
	_iScreenMaximised = -1;
	
	_y_status = y_status  + GUI_FONT_SIZE*GUI_STATUS_LINES*2/3;
	
	_nScreenSlotsX = nScreenSlotsX;
	_nScreenSlotsY = nScreenSlotsY;
	
	_resScreenX = resScreenX;
	_resScreenY = resScreenY;
	
	assetLoad();
}

GUI::~GUI()
{
	for (int iScreen=0; iScreen<_vecGUI.size(); iScreen++) {
		delete _vecGUI.at(iScreen);
	}
	_vecGUI.clear();
	
	assetUnload();
}

void GUI::sortGUI()
{
	sort(_vecGUI.begin(), _vecGUI.end());
	
	for (int iScreen=0; iScreen<_vecGUI.size(); iScreen++) {
		_vecGUI.at(iScreen)->moveScreen(iScreen);
	}
}

void GUI::doFullscreen()
{
	if (_iScreenHover>=0 && _iScreenHover<_vecGUI.size())
		_vecGUI.at(_iScreenHover)->hitMaximise(dummyval);
	checkMaximised();
}

void GUI::update()
{
	g_updateIsUserActive();
}

void GUI::draw()
{
	if (_iScreenMaximised>=0)
	{
		_vecGUI.at(_iScreenMaximised)->draw(0,0,ofGetScreenWidth(),ofGetScreenHeight());
		
		if (g_isUserActive && g_isInterfaceEnabled)
		{
			_vecGUI.at(_iScreenMaximised)->drawInterface(0,0,ofGetScreenWidth(),ofGetScreenHeight());
			
			ofShowCursor(); // doing this every frame may be expensive. see if we can cache this effectively
		} else {
			ofHideCursor(); // doing this every frame may be expensive. see if we can cache this effectively
		}

		
	} else {
		ofShowCursor(); // doing this every frame may be expensive. see if we can cache this effectively
		
		for (int iScreen=_vecGUI.size()-1; iScreen >=0 ; iScreen--)
		{
			_vecGUI.at(iScreen)->draw();
			
			if ((iScreen==_iScreenHover && g_isUserActive) && g_isInterfaceEnabled)
				_vecGUI.at(iScreen)->drawInterface();
		}
		
		_typer->drawString(status, 0, _y_status);
	}

}

//------------------------------------------------------------------------------------------------

void GUI::mouseOver(int x, int y)
{
	
	float xX, xY;
	int iScreen;
	
	findScreen(x,y,iScreen,xX,xY);
	
	if (iScreen>=0 && iScreen<_vecGUI.size())
	{
		_vecGUI.at(iScreen)->mouseOver(xX, xY, x, y);
		_vecGUI.at(iScreen)->getStatus(*_strStatus);
		_iScreenHover = iScreen;
	}
	
	//probably move this line into scrBase
	char strMouseCoords[50];
	sprintf(strMouseCoords, "Mouse=(%1.3f, %1.3f)", xX, xY);

	interfaceNudge();
	
}

void GUI::mouseDown(int x, int y)
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

void GUI::mouseReleased(int x, int y)
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

void GUI::findScreen(int &x, int &y, int &iScreen, float &xX, float &xY)
{
	if (_iScreenMaximised>=0)
	{
		iScreen=_iScreenMaximised;
		xX = float(x)/ofGetScreenWidth();
		xY = float(y)/ofGetScreenHeight();
	} else {
		int iScreenX, iScreenY;
		
		iScreenX = x / _resScreenX;
		iScreenY = y / _resScreenY;
		iScreen = iScreenY * _nScreenSlotsX+ iScreenX;
		
		x %= (int)_resScreenX;
		y %= (int)_resScreenY;
		
		xX = float(x)/_resScreenX;
		xY = float(y)/_resScreenY;
	}
}

//------------------------------------------------------------------------------------------------

void GUI::addScreen(scrBase &Screen)
{
	_vecGUI.push_back(&Screen);
	sortGUI();
}
//------------------------------------------------------------------------------------------------

void GUI::checkMaximised()
{
	bool foundMaximised=false;
	
	for (int iScreen=0; iScreen<_vecGUI.size(); iScreen++)
	{
		if (_vecGUI.at(iScreen)->_isMaximised)
		{
			if (foundMaximised==true)
				_vecGUI.at(iScreen)->_isMaximised=false;
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
