#pragma once
/*
 *  GUI_element.h
 *  playces-iphone
 *
 *  Created by Elliot Woods on 22/07/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "ofMain.h"
#include "Touch.h"
#include "GUI_constants.h"

class GUI_element
{
public:
	GUI_element(float inx, float iny, float inw, float inh);
	ofPoint				transformTouch(ofPoint touch);
	
	virtual void		draw();
	virtual void		update() = 0;
	
	virtual void		touchDown(Touch *_touch);
	virtual void		touchUp(Touch *_touch);
	virtual void		touchMoved(Touch *_touch);
	
	virtual bool		isHit(Touch *_touch);
	void				nudgeInterface();
	
	Touch				_Touches[MAX_TOUCHES];
	Touch				*_downTouches[MAX_TOUCHES];
	int					_nTouchesDown;
	
	bool				enabled;
protected:
	void				countTouches();
	ofPoint				position;
	float				x, y, w, h;	
	
	float				_lastTouch;

};