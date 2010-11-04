/*
 *  CorrelateMain.h
 *  PC Encode
 *
 *  Created by Elliot Woods on 03/11/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "ofMain.h"

#include "ofxKCScreensGUI.h"

#include <fstream>
#include <iostream>

#define MAXPOINTS 10000000

class CorrelateMain
{
public:
	CorrelateMain();
	void			update();
	
	//
	
	scrGroupGrid	scrGrid;
	scrWidgets		scrControl;
	scrFileSelect	scrFileSelection;
	scrPointCloud	scrInputPoints;
	
protected:
	void			loadData();	
	float			getDepthFromFilename(string filename);
	void			copyToPointCloud();
	
	void			runPolyfit();
	//
	
	wdgButton		*bangLoad, *bangCorrelate;
	bool			testBool;

	int				nCameras;
	int				nDatasets;
	int				nPoints;
	float			screenWidth, screenHeight;
	
	// input point cloud
	float			input_pos[MAXPOINTS][3];
	float			input_col[MAXPOINTS][3];
	
	// fit data
	vector<vector<double> >	input;
	vector<vector<double> >	output;

	
};