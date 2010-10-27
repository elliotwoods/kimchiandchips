#pragma once
/*
 *  ofxKCScreensGUI.h
 *  Kimchi and Chips' Screens GUI
 *
 *  Created by Elliot Woods on 27/01/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */
//#include "PCconstants.h"
//#include "PCAssets.h"
#include "GUIGlobal.h"
#include "GUIConstants.h"
#include "GUIAssets.h"

#include "Histogram.h"

#include "scrTexture.h"
#include "scrHistograms.h"

#include "ofMain.h"


class ofxKCScreensGUI : public GUIGlobal, public GUIAssets
{
public:
							ofxKCScreensGUI(int nScreenSlotsX, int nScreenSlotsY, float width, float height);
							ofxKCScreensGUI();
	void					arrange();
	
	void					doFullscreen();
	void					doSwitchTexture() { };
	
	void					update();
	void					draw();
	
	void					mouseOver(int x, int y);
	void					mouseDown(int x, int y);
	void					mouseReleased(int x, int y);
	void					findScreen(int &x, int &y, int &iScreen, float &xX, float &xY);
	
	void					addScreen(scrBase *Screen);
	
	void					interfaceOn() { g_enableUserInterface(); }
	void					interfaceOff() { g_disableUserInterface(); }
	void					interfaceNudge() {g_userAction(); }
	
	int						nScreens()  { return _vecGUI.size(); }
	
	string					status;
	
private:
	void					checkMaximised();
	
	std::vector<scrBase*>	_vecGUI;
	
	int						_iScreenHover;
	int						_iScreenMaximised;
	
	int						dummyval; //for event

};