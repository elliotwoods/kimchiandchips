/*
 *  Selector.cpp
 *  Quad Mapper app
 *
 *  Created by Elliot Woods on 28/09/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "Selector.h"

Selector::Selector(float inx, float iny, float inw, float inh,
		int nItems,
		string caption)
:  GUI_element(inx,iny,inw,inh)
{
	_caption = caption;
	_nItems = nItems;
	
	for (int i=0; i< nItems; i++)
		_items.push_back(ofToString(i, 0));
}

void Selector::draw()
{
	GUI_element::draw();
	
	float xStep = w / _nItems;
	
	ofPushStyle();
	ofEnableAlphaBlending();
	
	for (int i=0; i< _nItems; i++)
	{
		if (selection==i)
			ofSetColor(255, 255, 255, 200);
		else
			ofSetColor(50, 50, 50, 200);
		
		//fill
		ofFill();
		ofSetLineWidth(0);
		
		ofRect(x+xStep*i, y, xStep, h);
		
		//outline
		ofNoFill();
		ofSetColor(255, 255, 255);
		ofSetLineWidth(2);
		ofRect(x+xStep*i, y, xStep, h);
		
		//text
		if (selection==i)
			ofSetColor(0, 0, 0);
		else
			ofSetColor(255, 255, 255);
		ofDrawBitmapString(_items[i], x+xStep*(float(i)+0.5) - (8*_items[i].length()/2), y+0.5*h);
	}
	ofPopStyle();
	
	
}

void Selector::update()
{
}

void Selector::touchMoved(Touch *_touch)
{
    GUI_element::touchMoved(_touch);
}

void Selector::touchUp(Touch *_touch)
{
    GUI_element::touchUp(_touch);
	
}

void Selector::touchDown(Touch *_touch)
{
    GUI_element::touchDown(_touch);
	
	selection = _nItems * (_touch->x - x) / w;
}