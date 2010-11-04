/*
 *  wdgButton.cpp
 *  PC Encode
 *
 *  Created by Elliot Woods on 03/11/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "wdgButton.h"

wdgButton::wdgButton(string caption,
					 bool &myValue) :
wdgBase(caption)
{
	//toggle constructor
	_isBang = false;
	_valueToggle = &myValue;
	
	_wasBang = 0;
}

wdgButton::wdgButton(string caption) :
wdgBase(caption)
{
	//bang constructor
	_isBang = true;
	_valueBang = false;

	_wasBang = 0;
}

void wdgButton::draw()
{
	bool currentValue;
	
	currentValue = (_isBang ? _valueBang : *_valueToggle);
	currentValue |= _wasBang > 0;
	
	ofPushStyle();
	ofSetLineWidth(1);
	ofFill();
	if (currentValue)
		ofSetColor(255, 255, 255);
	else
		ofSetColor(0, 0, 0);
	ofRect(_x, _y, _width, _height);
	
	//draw outline if not current value
	if (!currentValue)
	{
		ofPushStyle();
		ofSetLineWidth(1);
		ofNoFill();
		ofSetColor(255, 255, 255);
		ofRect(_x, _y, _width, _height);
		ofPopStyle();
	}
	
	if (currentValue)
		ofSetColor(0, 0, 0);
	else
		ofSetColor(255, 255, 255);
	
	ofDrawBitmapString(caption, _x+_width/2-4*caption.length(),
					   _y+_height/2+5);
	ofPopStyle();
	
	
	
	if (_wasBang>0)
		_wasBang--;

}
void wdgButton::mouseDown(int x, int y)
{
	if (_isBang)
		_valueBang = true;
	else
		*_valueToggle = !*_valueToggle;
}

bool wdgButton::getBang()
{
	if (_isBang)
	{
		if (_valueBang)
		{
			_valueBang=false;
			_wasBang=2;
			return true;
		} else
			return false;

	} else {
		return false;
	}
		
}