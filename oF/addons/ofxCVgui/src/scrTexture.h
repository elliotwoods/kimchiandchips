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
	scrTexture(enumShowCursor showCursor, bool hasCursorEvents, ofTexture &texture, string caption);
	scrTexture(enumShowCursor showCursor, bool hasCursorEvents, ofTexture &texture, ofTexture &texture2, string caption);

	void		doSwitchTexture(int &dummyval);
	
protected:
	void		draw(int x, int y, int width, int height);

	ofTexture		*chooseTextureToDraw();
	ofTexture		* _texture, *_texture2;
	bool			_hasTwoTextures;
	bool			_selectedTexture;
	
	btnBase			*_btnSwitchTexture;
	
	void			* _onMouseOver(float x, float y);
	
};