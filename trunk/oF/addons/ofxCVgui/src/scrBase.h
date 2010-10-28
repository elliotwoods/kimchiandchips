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
	scrBase(enumShowCursor showCursor, bool showMarkers, string caption);
	~scrBase();
	
	void					draw(); // please override the private one below
	
	bool					isHit(int x, int y);
	
	virtual void			mouseOver(int x, int y) { };
	virtual void			mouseDown(int x, int y) { };
	virtual void			mouseReleased(int x, int y) { };
	
	void					hitMaximise();
	void					setBounds(int x, int y, int w, int h);
	
	void					getStatus(string &strStatus) { };
	
	void					moveCursor(float x, float y);
	
	void					updateInterface();
	bool					isUserActive();
		
	bool					isFullscreen;
	
	bool					hasCursorAttached;
	
	enumShowCursor			_showCursor;
	ofEvent<ofPoint>		evtCursorMove;
	
	string					_strCaption;
	
protected:
	virtual void			doResize() { };
	void					getLiveBounds(int &x, int &y, int &w, int &h); //also considers fullscreen
	
	virtual void			drawContent()=0;
	void					drawInterface();
	
	bool					transformMouse(float mouseX, float mouseY, float &screenX, float &screenY);
	
	int						_x, _y, _width, _height;
	
	ofPoint					_ptCursorPosition;
	
	string					_strStatus;

	std::vector<btnBase*>	_vecInterfaceButtons;
	btnBase					*_btnMaximise;
	
	float					_lastLocalInterfaceUpdate;

};


