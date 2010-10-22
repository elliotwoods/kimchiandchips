#pragma once
/*
 *  scrTexture.h
 *  pixel correlation
 *
 *  Created by Elliot Woods on 27/01/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "scrBase.h"
#include "ofEvents.h"

class scrTexture : public scrBase {

public:
	scrTexture(int iScreen, enumShowCursor showCursor, bool hasCursorEvents, int screenOrder, ofTexture &texture, string caption);
	scrTexture(int iScreen, enumShowCursor showCursor, bool hasCursorEvents, int screenOrder, ofTexture &texture, ofTexture &texture2, string caption);

	void		draw();
	void		draw(int x, int y, int width, int height);
	void		doSwitchTexture(int &dummyval);
	
protected:
	ofTexture		*chooseTextureToDraw();
	ofTexture		* _texture, *_texture2;
	bool			_hasTwoTextures;
	bool			_selectedTexture;
	
	btnBase			*_btnSwitchTexture;
	
	void			* _onMouseOver(float x, float y);
	
};