/*
 *  scrGroup.cpp
 *  PC Encode
 *
 *  Created by Elliot Woods on 27/10/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "scrGroup.h"

#define FOREACH_SCREEN for (int iScreen=0; iScreen<screens.size(); iScreen++)

//////////////////////////////////////////////////////

scrGroup::scrGroup() :
scrBase(cursor_none, true, "")
{
	gridWidth = -1;
	iScreenMaximised = -1;
}

//////////////////////////////////////////////////////

void scrGroup::drawContent()
{
	if (!_isFullscreen)
		for (int i=0; i<screens.size(); i++)
			screens[i]->draw();
	else
		screens[iScreenMaximised]->draw();
}

//////////////////////////////////////////////////////

void scrGroup::doResize()
{
	double nScreens = screens.size();
	
	if (gridWidth==-1)
		gridWidth = ceil(sqrt(double(nScreens)));
	
	gridHeight = ceil(nScreens / double(gridWidth));
	
	//code for arranging grid
	int ix, iy;
	float x, y, w, h;
	
	w = _width/gridWidth;
	h = _height/gridHeight;
	
	for (int i=0; i<screens.size(); i++)
	{
		ix = i % gridWidth;
		iy = i / gridWidth;
		
		x = _x + ix * w;
		y = _y + iy * h;
		
		screens[i]->setBounds(x, y, w, h);
	}
}

int scrGroup::findScreen(int x, int y)
{
	int iScreen; // return value
	
	int w = _width / gridWidth;
	int h = _height / gridHeight;
	
	
	if (iScreenMaximised != -1)
		iScreen=iScreenMaximised;
	
	else
	{
		int iScreenX, iScreenY;
		
		iScreenX = (x - _x) / w;
		iScreenY = (y - _y) / h;
		
		iScreen = iScreenY * gridWidth + iScreenX;
	}
	
	if (iScreen<0 || iScreen>screens.size()-1)
		iScreen=-1;

	return iScreen;
	
}

//////////////////////////////////////////////////////

void scrGroup::mouseMoved(int x, int y)
{
	FOREACH_SCREEN
		if (screens[iScreen]->isHit(x,y))
			screens[iScreen]->mouseMoved(x,y);
}

void scrGroup::mouseDown(int x, int y)
{
	FOREACH_SCREEN
		if (screens[iScreen]->isHit(x,y))
			screens[iScreen]->mouseDown(x,y);	
}

void scrGroup::mouseReleased(int x, int y)
{
	FOREACH_SCREEN
		if (screens[iScreen]->hasCursorAttached)
			screens[iScreen]->mouseDown(x,y);		
}

//////////////////////////////////////////////////////

bool scrGroup::hitMaximise(int x, int y)
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
	}
	
	screens[iScreenMaximised]->hitMaximise(x,y,false);
	iScreenMaximised = -1;
	_isFullscreen=false;
	return false;
	
}
