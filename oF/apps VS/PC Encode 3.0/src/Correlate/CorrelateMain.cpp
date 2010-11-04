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
scrControl("Calibrate control"),
scrFileSelection("Select data", "./Logs/", "bin"),
scrInputPoints("Input data"),
scrGrid("Correlate")
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
	wdgCounter *wdgDatasetCount = new wdgCounter("Datasets loaded",
												 nDatasets,
												 0);
	wdgSlider *wdgScreenWidth = new wdgSlider("Screen width",
											   screenWidth,
											   0, 5,
											   0.001,
											   "meters");
	wdgSlider *wdgScreenHeight = new wdgSlider("Screen height",
											  screenHeight,
											  0, 4,
											  0.001,
											  "meters");
	bangLoad = new wdgButton("Load data");
	bangCorrelate = new wdgButton("Run polyfit");
	
	scrControl.push(wdgCameraCount);
	scrControl.push(bangLoad);
	scrControl.push(bangCorrelate);
	scrControl.push(wdgScreenWidth);
	scrControl.push(wdgScreenHeight);
	
	scrGrid.push(&scrControl);
	scrGrid.push(&scrFileSelection);
	scrGrid.push(&scrInputPoints);
	////////////////////////////
	
	////////////////////////////
	// Clear points
	////////////////////////////
//	nPoints=0;
//	for (float x=-1; x<1; x+=0.1)
//		for (float y=-1; y<1; y+=0.1)
//			for (float z=-1; z<1; z+=0.1)
//			{
//				input_pos[nPoints][0] = x;
//				input_pos[nPoints][1] = y;
//				input_pos[nPoints][2] = z;
//				
//				input_col[nPoints][0] = x/2+0.5f;
//				input_col[nPoints][1] = y/2+0.5f;
//				input_col[nPoints][2] = z/2+0.5f;
//				nPoints++;
//			}
//	//
//	float *inpos = input_pos[0];
//	float *incol = input_col[0];
//	//
	scrInputPoints.setWith(*input_pos, *input_col, 0);	
	////////////////////////////
	
	////////////////////////////
	// Initialise some values
	////////////////////////////
	// from the samsung monitor in the studio
	screenWidth = 0.598;
	screenHeight = 0.336;
	////////////////////////////
	
}

void CorrelateMain::update()
{
	if (bangLoad->getBang())
		loadData();
	
	if (bangCorrelate->getBang())
		runPolyfit();

}

void CorrelateMain::loadData()
{
	input.clear();
	output.clear();

	nDatasets = 0;
	nPoints = 0;
	
	vector<double> inputRow;
	vector<double> outputRow;
	
	char	thisNCameras;
	int		thisFileSize;
	string	thisFilename;
	int		thisNPoints;
	
	int		thisStart, thisEnd;
	float	thisvalx, thisvaly; //for reading from file
	
	for (int iFile=0; iFile<scrFileSelection.nFiles; iFile++)
		if (scrFileSelection.selected[iFile])
		{
			///////////////////////////////
			// Open file
			///////////////////////////////
			thisFilename = scrFileSelection.getPath(iFile);
			ifstream inFile(thisFilename.c_str(),
							ios::in | ios::binary);
			//
			if (!inFile.is_open())
			{
				ofLog(OF_LOG_WARNING, "CorrelateMain: failed to load file " + thisFilename);
				continue;
			}
			///////////////////////////////
			
			
			///////////////////////////////
			// Determine size
			///////////////////////////////
			thisStart = inFile.tellg();
			inFile.seekg (0, ios::end);
			thisEnd = inFile.tellg();
			//
			thisFileSize = thisEnd - thisStart;
			//
			inFile.seekg(0);
			///////////////////////////////
			
			
			///////////////////////////////
			// Read in number of cameras
			///////////////////////////////
			inFile.read(&thisNCameras, 1);
			if (nDatasets==0)
			{
				//first file, so initalise rows
				nCameras = thisNCameras;
				inputRow.resize(2*nCameras);
				outputRow.resize(3);
				//
			} else
				if (nCameras != thisNCameras)
					ofLog(OF_LOG_ERROR, "CorrelateMain: mismatched number of cameras between files");
			///////////////////////////////
			
			
			///////////////////////////////
			// Read data
			///////////////////////////////
			thisNPoints = (thisFileSize-1) / (8+8*nCameras);
			//
			for (int iPoint=0; iPoint<thisNPoints; iPoint++)
			{
				inFile.read((char*) &thisvalx, 4);
				inFile.read((char*) &thisvaly, 4);
				
				outputRow[0] = screenWidth * (thisvalx-0.5);
				outputRow[1] = screenHeight * (thisvaly-0.5);
				outputRow[2] = getDepthFromFilename(scrFileSelection.getName(iFile));
				
				for (int iCam=0; iCam<nCameras; iCam++)
				{
					inFile.read((char*) &thisvalx, 4);
					inFile.read((char*) &thisvaly, 4);
					
					inputRow[0 + iCam*2] = thisvalx;
					inputRow[1 + iCam*2] = thisvaly;
				}
				
				nPoints++;
				
				input.push_back(inputRow);
				output.push_back(outputRow);
			}
			///////////////////////////////
			
			
			///////////////////////////////
			// Close up the shop
			///////////////////////////////
			inFile.close();
			nDatasets++;
			///////////////////////////////			
		}
	
	copyToPointCloud();
}

float CorrelateMain::getDepthFromFilename(string filename)
{
	return ofToFloat(filename.substr(0,filename.length()-4));
}

void CorrelateMain::copyToPointCloud()
{
	if (nPoints>MAXPOINTS)
		ofLog(OF_LOG_WARNING, "CorrelateMain: nPoints > MAXPOINTS. only drawing first MAXPOINTS");
	
	for (int iPoint = 0; iPoint<min(nPoints,MAXPOINTS); iPoint++)
	{
		input_pos[iPoint][0] = output[iPoint][0];
		input_pos[iPoint][1] = output[iPoint][1];
		input_pos[iPoint][2] = output[iPoint][2];
		
		input_col[iPoint][0] = output[iPoint][0] / screenWidth + 0.5;
		input_col[iPoint][1] = output[iPoint][1] / screenHeight + 0.5;
		input_col[iPoint][2] = 0;//float(iPoint) / float(nDatasets-1);
	}
	
	scrInputPoints.setWith(&input_pos[0][0], &input_col[0][0], nPoints);	

}

void CorrelateMain::runPolyfit()
{
}