/*
 *  scrTexture.cpp
 *  pixel correlation
 *
 *  Created by Elliot Woods on 27/01/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "ofxKCScreensGUI.h"

scrTexture::scrTexture(enumShowCursor showCursor, bool hasCursorEvents, ofTexture &texture, string caption)
: scrBase(showCursor, hasCursorEvents, enum_texture, caption)
{
	_texture = &texture;
	_hasTwoTextures=false;
}

scrTexture::scrTexture(enumShowCursor showCursor, bool hasCursorEvents, ofTexture &texture, ofTexture &texture2, string caption)
: scrBase(showCursor, hasCursorEvents, enum_texture, caption
		  )
{
	_texture = &texture;
	_texture2 = &texture2;
	
	_hasTwoTextures=true;
	_selectedTexture=false;
	
	_btnSwitchTexture = new btnBase(button_toggle, *_assetButtonSwitch,*_assetButtonSwitch_over);
	_vecInterfaceButtons.push_back(_btnSwitchTexture);	
	ofAddListener(_btnSwitchTexture->buttonHit, this, &scrTexture::doSwitchTexture);
}

void scrTexture::draw(int x, int y, int width, int height)
{
	chooseTextureToDraw()->draw(x, y, width, height);
}

void scrTexture::doSwitchTexture(int &dummyval)
{
	_selectedTexture  = !_selectedTexture;
}

ofTexture *scrTexture::chooseTextureToDraw()
{
	if (_hasTwoTextures)
		if (!_selectedTexture) {
			return _texture;
		} else {
			return _texture2;
		}
	else
		return _texture;

}