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
#include "ofxPolyfit.h"

#include <fstream>
#include <iostream>

#define MAXPOINTS 10000000

class CorrelateMain
{
public:
	CorrelateMain();
	void			update();
	
	//
	
	scrGroupGrid	scrGridMain, scrGridData;
	scrWidgets		scrControl;
	scrFileSelect	scrFileSelection;
	scrPointCloud	scrInputPoints, scrTestCorrelate;
	
protected:
	void			loadData();	
	float			getDepthFromFilename(string filename);
	void			copyToInputScreen();
	
	void			runPolyfit();
	void			runTestSet();
	//
	
	wdgButton		*bangLoad, *bangCorrelate, *bangTestData, *bangWrite;
	bool			invertXY;

	int				nCameras;
	int				nDatasets;
	int				nPoints;
	float			screenWidth, screenHeight;
	
	// input point cloud
	float			input_pos[MAXPOINTS][3];
	float			input_col[MAXPOINTS][3];

	// output point cloud
	float			test_pos[MAXPOINTS][3];
	//use same colour as input
	
	// fit data
	ofxPolyFit		fit;
	float			polyOrder; //it's a float because of slider
	vector<vector<double> >	polyInput;
	vector<vector<double> >	polyOutput;

	
};