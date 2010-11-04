#pragma once
/*
 *  wdgButton.h
 *  PC Encode
 *
 *  Created by Elliot Woods on 03/11/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "wdgBase.h"

#define BANG_FRAMES 2

class wdgButton : public wdgBase
{
public:	
	//toggle constructor
	wdgButton(string caption,
			  bool &myValue);
	
	//bang constructor
	wdgButton(string caption);
	
	void		draw();
	void		mouseDown(int x, int y);
	
	bool		getBang();
	
protected:
	bool		_isBang;
	bool		*_valueToggle;
	bool		_valueBang;
	
	int			_wasBang;
};
