/*
 *  scrGroupGrid.cpp
 *  PC Encode
 *
 *  Created by Elliot Woods on 29/10/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "scrGroupGrid.h"

scrGroupGrid::scrGroupGrid(string caption) :
scrGroupBase(caption)
{
	gridWidth = -1;
}
//////////////////////////////////////////////////////

void scrGroupGrid::drawContent()
{
	if (!_isFullscreen)
		for (int i=0; i<screens.size(); i++)
			screens[i]->draw();
	else
		screens[iScreenMaximised]->draw();
}

//////////////////////////////////////////////////////

int scrGroupGrid::findScreen(int x, int y)
{
	if (screens.size()==0)
		return -1;
	
	int iScreen; // return value
	
	int w = _width / gridWidth;
	int h = _height / gridHeight;
	
	
	if (_isFullscreen)
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

void scrGroupGrid::setGridWidth(int _gridWidth)
{
	gridWidth = _gridWidth;
	doResize();
}

void scrGroupGrid::doResize()
{
	double nScreens = screens.size();
	
	if (nScreens==0)
		return;
	
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
