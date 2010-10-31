/*
 *  scrGroupBase.cpp
 *  PC Encode
 *
 *  Created by Elliot Woods on 27/10/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "scrGroupBase.h"

#define FOREACH_SCREEN for (int iScreen=0; iScreen<screens.size(); iScreen++)

//////////////////////////////////////////////////////

scrGroupBase::scrGroupBase() :
scrBase(cursor_none, true, "")
{
	iScreenMaximised = -1;
	_hasInterface = false;
}

//////////////////////////////////////////////////////

void scrGroupBase::push(scrBase *addScreen)
{
	screens.push_back(addScreen);
}

//////////////////////////////////////////////////////

void scrGroupBase::mouseMoved(int x, int y)
{
	int iScreen = findScreen(x,y);
	
	if (iScreen==-1)
		return;

	screens[iScreen]->mouseMoved(x,y);
}

void scrGroupBase::mouseDown(int x, int y)
{
	int iScreen = findScreen(x,y);
	
	if (iScreen==-1)
		return;
	
	screens[iScreen]->mouseDown(x,y);	
}

void scrGroupBase::mouseReleased(int x, int y)
{
	FOREACH_SCREEN
		if (screens[iScreen]->hasCursorAttached)
			screens[iScreen]->mouseDown(x,y);		
}

//////////////////////////////////////////////////////

bool scrGroupBase::hitMaximise(int x, int y)
{
	if (!_isFullscreen)
	{
		int iScreen = findScreen(x, y);

		if (iScreen != -1)

			if (screens[iScreen]->hitMaximise(x,y))
			{
				iScreenMaximised = iScreen;
				_isFullscreen = true;
				return true;
			}
		else
			return false;
	}
	
	screens[iScreenMaximised]->hitMaximise(x,y,false);
	iScreenMaximised = -1;
	_isFullscreen=false;
	return false;
	
}
