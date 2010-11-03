/*
 *  CorrelateMain.cpp
 *  PC Encode
 *
 *  Created by Elliot Woods on 03/11/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "CorrelateMain.h"

CorrelateMain::CorrelateMain() :
scrControl("Calibrate control")
{
	////////////////////////////
	// INITIALISE VARIABLES
	////////////////////////////
	nCameras = 0;
	////////////////////////////
	
	////////////////////////////
	// BUILD INTERFACE
	////////////////////////////
	wdgCounter *wdgCameraCount = new wdgCounter("Number of cameras",
										nCameras,
										  0);
	testBang = new wdgButton("test bang");
	testToggle = new wdgButton("test toggle", testBool);
	
	scrControl.push(wdgCameraCount);
	scrControl.push(testBang);
	scrControl.push(testToggle);
	////////////////////////////
}