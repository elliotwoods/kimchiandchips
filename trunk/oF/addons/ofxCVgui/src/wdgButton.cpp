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
}

wdgButton::wdgButton(string caption) :
wdgBase(caption)
{
	//bang constructor
	_isBang = true;
	_valueBang = false;
}

void wdgButton::draw()
{
	bool currentValue;
	
	currentValue = (_isBang ? _valueBang : *_valueToggle);
	
	ofPushStyle();
	ofSetLineWidth(1);
	ofFill();
	if (currentValue)
		ofSetColor(255, 255, 255);
	else
		ofSetColor(0, 0, 0);
	ofRect(_x, _y, _width, _height);
	
	if (currentValue)
		ofSetColor(0, 0, 0);
	else
		ofSetColor(255, 255, 255);
	
	ofDrawBitmapString(caption, _x+_width/2-4*caption.length(),
					   _y+_height/2+5);
	ofPopStyle();

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
		bool tempval = _valueBang;
		_valueBang=false;
		return tempval;
	} else {
		return false;
	}
		
}