/*
 *  GUI_element.cpp
 *  playces-iphone
 *
 *  Created by Elliot Woods on 22/07/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "GUI_element.h"

GUI_element::GUI_element(float inx, float iny, float inw, float inh)
{
	nudgeInterface();
    x=inx;
    y=iny;
    w=inw;
    h=inh;
	
	enabled = true;
	
	//setup some blank touches
	for (int iTouch=0; iTouch<MAX_TOUCHES; iTouch++)
		_Touches[iTouch].ID = iTouch;
	_nTouchesDown = 0;
}

ofPoint GUI_element::transformTouch(ofPoint touch)
{
	return ofPoint(ofMap(touch.x, x, x+w, -1, 1, true),
				   ofMap(touch.y, y, y+h, -1, 1, true));
}

bool GUI_element::isHit(Touch *_touch)
{
	bool output =	(_touch->x >= x) && (_touch->y >= y) &&
					(_touch->x <= x+w) && (_touch->y <= y+h);
	
	output &= enabled;
	
	if (output)
		nudgeInterface();
	
	return output;
}

void GUI_element::draw()
{
	if (ofGetElapsedTimef()-_lastTouch < GUI_OUTLINE_DECAY)
	{
		ofPushStyle();
		ofSetLineWidth(2);
		ofEnableAlphaBlending();
		ofSetColor(255, 255, 255, ofMap(ofGetElapsedTimef()-_lastTouch, 0, GUI_OUTLINE_DECAY, 255, 0, true));
		ofNoFill();
		
		ofRect(x, y, w, h);
		
		ofPopStyle();
	}
}

void GUI_element::nudgeInterface()
{
	_lastTouch = ofGetElapsedTimef();
}

void GUI_element::touchDown(Touch *_touch)
{
	_Touches[_touch->ID].isDown = true;
	_downTouches[_nTouchesDown++] = &_Touches[_touch->ID];
	countTouches();
	
	_Touches[_touch->ID].setXY(_touch->x, _touch->y);
	
	nudgeInterface();
}

void GUI_element::touchUp(Touch *_touch)
{
	_Touches[_touch->ID].isDown=false;
	countTouches();
}

void GUI_element::touchMoved(Touch *_touch)
{
	if (_Touches[_touch->ID].isDown)
		nudgeInterface();
	
	_Touches[_touch->ID].moveXY(_touch->x, _touch->y);
	
}

void GUI_element::countTouches()
{
	_nTouchesDown=0;
	
	for (int i=0; i<MAX_TOUCHES; i++)
		if (_Touches[i].isDown)
			_downTouches[_nTouchesDown++] = &_Touches[i];
}