/*
 *  Button.cpp
 *  nespresso sculpture
 *
 *  Created by Elliot Woods on 29/09/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "Button.h"

Button::Button(float _x, float _y, float _w, float _h, string _caption, int _iAction) :
GUI_element(x, y, w, h)
{
	caption = _caption;
	iAction = _iAction;
}

Button::Button(float _x, float _y, float _w, float _h, string _caption) :
GUI_element(_x, _y, _w, _h)
{
	caption = _caption;
	iAction = 0;
}

void Button::update()
{
}

void Button::draw()
{
	bool isDown = false;
	
	for (int iTouch=0; iTouch<MAX_TOUCHES; iTouch++)
		isDown |= _Touches[iTouch].isDown;
	
	float fcol = (isDown ? 255 : 50);
	float lcol = (isDown ? 0 : 255);
	
	ofPushStyle();
	
	//fill
	ofSetColor(fcol, fcol, fcol, 200);
	ofFill();
	ofSetLineWidth(0);
	ofRect(x, y, w, h);
	
	//outline
	ofNoFill();
	ofSetColor(lcol, lcol, lcol, 255);
	ofSetLineWidth(2);
	ofRect(x, y, w, h);
	
	ofPopStyle();
	
	ofDrawBitmapString(caption, x+(w-8*caption.length())/2, y+h/2);
}

void Button::touchUp(Touch *_touch)
{
	GUI_element::touchUp(_touch);
	action.notify(this,iAction);
}

void Button::touchDown(Touch *_touch)
{
	GUI_element::touchDown(_touch);
}
