/*
 *  scrBase.cpp
 *  screen base class
 *	designed as part of pixel correlation project
 *
 *  Created by Elliot Woods on 27/01/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "GUI.h"

scrBase::scrBase(int iScreen, enumShowCursor showCursor, bool showMarkers, int screenOrder, enumScreenType screenType, string caption)
{

	moveScreen(iScreen);
	
	_showCursor = showCursor;
	_showMarkers = showMarkers;
	
	_screenOrder = screenOrder;
	
	_screenType = screenType;
	
	_btnMaximise = new btnBase(button_toggle, *_assetButtonPlus, *_assetButtonPlus_over);
	_vecInterfaceButtons.push_back(_btnMaximise);
	ofAddListener(_btnMaximise->buttonHit, this, &scrBase::hitMaximise);
	_isMaximised = false;
	
	_strCaption = caption;

}

scrBase::~scrBase()
{
	for (int iButton=0; iButton<_vecInterfaceButtons.size(); iButton++) {
		delete _vecInterfaceButtons.at(iButton);
	}
	
	_vecInterfaceButtons.clear();
}

void scrBase::moveScreen(int iScreen)
{
	int iScreenX, iScreenY;
	
	iScreenX = iScreen % _nScreenSlotsX;
	iScreenY = iScreen / _nScreenSlotsX;
	
	_width = _resScreenX;
	_height = _resScreenY;
	_x = iScreenX * _width;
	_y = iScreenY * _height;
	
}

bool scrBase::operator < ( const scrBase &cmp )
{
	return _screenOrder < cmp._screenOrder;
}

bool scrBase::transformMouse(float mouseX, int mouseY, float &screenX, float &screenY)
// transforms the mouse into screen coords
// returns 'true' if mouse is inside this screen
{
	screenX = (float(mouseX) - float(_x)) / _width;
	screenY = (float(mouseY) - float(_y)) / _height;
	
	if (screenX >= 0 && screenX <= 1)
		if (screenY >= 0 && screenY <= 1)
			return true;
	
	return false;
}

void scrBase::drawInterface()
{
	if (_isMaximised)
		drawInterface(0,0,ofGetScreenWidth(),ofGetScreenHeight());
	else
		drawInterface(_x,_y,_width,_height);
}

void scrBase::drawInterface(int x, int y, int width, int height)
{
	
	//
	// MARKERS
	//
	
	if (_showMarkers)
	{
		drawMarkers(x,y,width,height);
	}
	
	
	//
	// BORDER
	//
	ofPushStyle();
	ofNoFill();
	ofSetLineWidth(GUI_INTERFACE_BORDER_LINE_WIDTH);
	ofSetColor(255,255,255);
	
	ofRect(x+(GUI_INTERFACE_BORDER_LINE_WIDTH/2),
		   y+(GUI_INTERFACE_BORDER_LINE_WIDTH/2),
		   width-GUI_INTERFACE_BORDER_LINE_WIDTH,
		   height-GUI_INTERFACE_BORDER_LINE_WIDTH);
	
	ofPopStyle();
	
	
	//
	// BUTTONS
	//
	int button_x, button_y;
	button_y = y+height-GUI_INTERFACE_BUTTON_SIZE;
	
	for (int iButton = 0; iButton < _vecInterfaceButtons.size(); iButton++)
	{
		button_x = x+width-(GUI_INTERFACE_BUTTON_SIZE*(iButton+1));
		_vecInterfaceButtons.at(iButton)->draw(button_wait, button_x, button_y);
	}
	
	
	if (_showCursor > cursor_none)
	{
		ofPushStyle();
		ofSetColor(255, 255, 255);		
		ofSetLineWidth(2);
		
		float fCursorX, fCursorY;
		fCursorX = width*_ptCursorPosition.x + x;
		fCursorY = height*_ptCursorPosition.y + y;


		
		if (_showCursor==cursor_x ||  _showCursor==cursor_xy)
			ofLine(fCursorX, y, fCursorX, y+height);
			
		if (_showCursor==cursor_y ||  _showCursor==cursor_xy)
			ofLine(x, fCursorY, x+width, fCursorY);

		ofPopStyle();
	}	
	
	//
	//CAPTION
	//
	
	ofPushStyle();
	ofSetColor(255,255,255);
	ofSetLineWidth(0);
	ofFill();
	ofRectangle boundBox = _typer->getStringBoundingBox(_strCaption, x, y+GUI_FONT_SIZE);
	ofRect(boundBox.x, boundBox.y, boundBox.width, boundBox.height);
	ofPopStyle();
	
	ofPushStyle();
	ofSetColor(0, 0, 0);
	_typer->drawString(_strCaption, x, y+GUI_FONT_SIZE);
	ofPopStyle();
}


//---------------------------------------------------------------------------------

void scrBase::mouseOver(float xX, float xY, int x, int y)
{
	mouseOverInterface(x, y);
}

void scrBase::mouseDown(float xX, float xY, int x, int y)
{
	mouseDownInterface(x, y);
}

void scrBase::mouseReleased(float xX, float xY, int x, int y)
{
	mouseReleasedInterface(x, y);
}

void scrBase::mouseOverInterface(int x, int y)
{
	if (y>_height-GUI_INTERFACE_BUTTON_SIZE)
	{
		int iButton = (_width-x)/GUI_INTERFACE_BUTTON_SIZE;
		if (iButton<_vecInterfaceButtons.size())
		{
		}		
	}
	
}

void scrBase::mouseDownInterface(int x, int y)
{
	//cursor
	if (_showCursor > cursor_none)
	{
		moveCursor(float(x)/float(_width),
				   float(y)/float(_height));
	}
}

void scrBase::mouseReleasedInterface(int x, int y)
{
	int width, height;
	
	//buttons
	if (_isMaximised == true)
	{
		width = ofGetScreenWidth();
		height = ofGetScreenHeight();
	} else {
		width = _width;
		height = _height;
	}

	if (y>height-GUI_INTERFACE_BUTTON_SIZE)
	{
		int iButton = (width-x)/GUI_INTERFACE_BUTTON_SIZE;
		
		if (iButton<_vecInterfaceButtons.size())
			_vecInterfaceButtons.at(iButton)->mouseReleased();
	}
	
	
	
	
}

void scrBase::hitMaximise(int &dummyval)
{
	_isMaximised = !_isMaximised;
}

//---------------------------------------------------------------------------------

void scrBase::moveCursor(float x, float y)
{
	
	_ptCursorPosition.x = x;
	_ptCursorPosition.y = y;

	ofNotifyEvent(evtCursorMove, _ptCursorPosition, this);
}


//---------------------------------------------------------------------------------
void scrBase::updateInterface()
{
	_lastLocalInterfaceUpdate = ofGetElapsedTimef();
}

bool scrBase::isUserActive()
{
	return (_lastLocalInterfaceUpdate + GUI_INTERFACE_DISPLAY_TIME) > ofGetElapsedTimef();
}

ofRectangle scrBase::getBounds()
{
	if (_isMaximised)
		return ofRectangle(0,0,ofGetWidth(),ofGetHeight());
	else
		return ofRectangle(_x,_y,_width,_height);
}
