#pragma once
/*
 *  scrBase.h
 *  screen base class
 *	designed as part of pixel correlation project
 *
 *  Created by Elliot Woods on 27/01/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "GUIAssets.h"

#include "ofMain.h"
#include "ofEvents.h"

#include "scrBaseMarkers.h"

#include "Histogram.h"

#include "btnBase.h"

class scrBase : public scrBaseMarkers, public GUIAssets
{
public:
	scrBase(enumShowCursor showCursor, bool showMarkers, enumScreenType screenType, string caption);
	~scrBase();
	
	virtual void			draw();
	
	virtual void			mouseOver(float xX, float xY, int x, int y);
	virtual void			mouseDown(float xX, float xY, int x, int y);
	virtual void			mouseReleased(float xX, float xY, int x, int y);
	
	virtual void			mouseOverInterface(int x, int y);
	virtual void			mouseDownInterface(int x, int y);
	virtual void			mouseReleasedInterface(int x, int y);
	
	void					hitMaximise(int &dummyval);
	
	void					getStatus(string &strStatus) { };
	
	void					moveCursor(float x, float y);
	
	void					updateInterface();
	bool					isUserActive();
	ofRectangle				bounds;
	
	virtual void			addHistogram(Histogram &histogram) { }
	
	bool					operator < ( const scrBase &cmp );
	
	bool					maximised;
	
	enumScreenType			_screenType;
	
	enumShowCursor			_showCursor;
	ofEvent<ofPoint>		evtCursorMove;
	
	string					_strCaption;
	
protected:
	void					drawInterface(int x, int y, int width, int height);
	virtual void			draw(int x, int y, int width, int height) = 0;
	
	bool					transformMouse(float mouseX, int mouseY, float &screenX, float &screenY);
	
	int						_x, _y, _width, _height;
	
	ofPoint					_ptCursorPosition;
	
	string					_strStatus;

	std::vector<btnBase*>	_vecInterfaceButtons;
	btnBase					*_btnMaximise;
	
	float					_lastLocalInterfaceUpdate;

};


