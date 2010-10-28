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
	groupType = SCRGROUP_TYPE_GRID;
	
	gridWidth = -1;
	iScreenMaximised = -1;
}

//////////////////////////////////////////////////////

void scrGroup::drawContent()
{
	for (int i=0; i<screens.size(); i++)
		screens[i]->draw();
}

//////////////////////////////////////////////////////

void scrGroup::doResize()
{
	switch (groupType) {
		case SCRGROUP_TYPE_GRID:
			arrangeGrid();
			break;
		default:
			break;
	}	
}

void scrGroup::arrangeGrid()
{	
	double nScreens = screens.size();
	
	if (gridWidth==-1)
		gridWidth = ceil(sqrt(double(nScreens)));
	
	int gridHeight = ceil(nScreens / double(gridWidth));
	
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

int scrGroup::findGrid(int &x, int &y)
{
	int iScreen; // return value
	
	int w = _width / gridWidth;
	int h = _height / gridHeight;
	
	
	if (iScreenMaximised>=0)
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

void scrGroup::mouseOver(int x, int y)
{
	FOREACH_SCREEN
		if (screens[iScreen]->isHit(x,y))
			screens[iScreen]->mouseOver(x,y);
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