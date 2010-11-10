#pragma once
/*
 *  Button.h
 *  nespresso sculpture
 *
 *  Created by Elliot Woods on 29/09/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "GUI_element.h"

//events
#include "Poco/BasicEvent.h"
#include "Poco/Delegate.h"
using Poco::BasicEvent;
using Poco::Delegate;


class Button : public GUI_element
{
public:
	Button(float _x, float _y, float _w, float _h, string _caption, int _iAction);
	Button(float _x, float _y, float _w, float _h, string _caption);
	
	void	draw();
	void	update();
	
	void        touchUp(Touch *_touch);
	void        touchDown(Touch *_touch);
	
	string			caption;
	int				iAction;
	BasicEvent<int>	action;
};