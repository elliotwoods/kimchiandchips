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
	
	void			saveProjector();
	
	//
	
	wdgButton		*bangLoad, *bangCorrelate, *bangTestData, *bangLoadFit, *bangSaveFit;
	wdgButton		*bangSaveProjectionXYZ;
	bool			invertXY;
	bool			newFormat;

	int				nCameras;
	int				nDatasets;
	int				nPoints;
	float			screenWidth, screenHeight;
	bool			swapCameras;
	
	// input point cloud
	float			input_pos[MAXPOINTS][3];
	float			input_col[MAXPOINTS][3];

	// output point cloud
	float			test_pos[MAXPOINTS][3];
	//use same colour as input
	
	// projector index data
	unsigned short	projWidth, projHeight;
	unsigned int	dataset_iPX[MAXPOINTS];
	unsigned int	dataset_iPY[MAXPOINTS];
	
	// fit data
	ofxPolyFit		fit;
	float			polyOrder; //it's a float because of slider
	vector<vector<double> >	polyInput;
	vector<vector<double> >	polyOutput;

	
};