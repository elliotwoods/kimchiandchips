/*
 *  scrGroupTabbed.cpp
 *  PC Encode
 *
 *  Created by Elliot Woods on 31/10/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "scrGroupTabbed.h"

////////////////////////////////////////////////////////////

scrGroupTabbed::scrGroupTabbed(int _tabRegionHeight)
{
	tabRegionHeight = _tabRegionHeight;
}

////////////////////////////////////////////////////////////

void scrGroupTabbed::drawContent()
{
	bool selected;
	
	for (int i=0; i<screens.size(); i++)
	{
		selected = (i==iSelection);

		ofPushStyle();
		
		//draw background
		if (selected)
			ofSetColor(255, 255, 255);
		else
			ofSetColor(0, 0, 0);
		ofFill();
		ofSetLineWidth(1);

		ofRect(_x+i*tabWidth, _y,
			   tabWidth, tabRegionHeight);
		
		
		//draw text
		int texty = tabRegionHeight/2 + TEXT_HEIGHT/2;
		texty += _y;
		
		if (selected)
			ofSetColor(0, 0, 0);
		else
			ofSetColor(255, 255, 255);
		ofDrawBitmapString(screens[i]->caption,
						   _x+i*tabWidth+1, texty);
		
		ofPopStyle();
	}
	
	screens[iSelection]->draw();
	
}

////////////////////////////////////////////////////////////

void scrGroupTabbed::mouseDown(int x, int y)
{
	//check to see whether our mouse is
	//hitting the tab region
	//otherwise pass through to normal group
	//action
	
	if (x>_x && x<_x+_width &&
		y>_y && y<_y+tabRegionHeight)
		iSelection = (x-_x)/tabWidth;
	else
		scrGroupBase::mouseDown(x,y);

}

////////////////////////////////////////////////////////////

int scrGroupTabbed::findScreen(int x, int y)
{
	if (x>_x && x<_x+_width &&
		y>_y+tabRegionHeight && y<_y+_height)
		return iSelection;
	else
		return -1;
}

void scrGroupTabbed::doResize()
{
	tabWidth = _width / screens.size();
	
	for (int i=0; i<screens.size(); i++)
		screens[i]->setBounds(_x,_y+tabRegionHeight,
							  _width, _height-tabRegionHeight);
}