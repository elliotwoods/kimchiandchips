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
scrTestCorrelate("Test correlation"),

scrGridMain("Correlate"),
scrGridData("Data pointclouds")
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
	wdgSlider *wdgPolyOrder = new wdgSlider("Polynomial order",
											polyOrder,
											1, 10,
											1);
	bangLoad = new wdgButton("Load data");
	bangCorrelate = new wdgButton("Run polyfit");
	bangTestData = new wdgButton("Test dataset");
	bangLoadFit = new wdgButton("Load fit");
	bangSaveFit = new wdgButton("Save fit");
	bangSaveProjectionXYZ = new wdgButton("Save projection space XYZ");
	
	bangCorrelate->enabled=false;
	bangTestData->enabled=false;
	bangSaveFit->enabled=false;
	bangSaveProjectionXYZ->enabled=false;
	
	scrControl.push(wdgCameraCount);
	scrControl.push(wdgDatasetCount);
	scrControl.push(bangLoad);
	scrControl.push(bangCorrelate);
	scrControl.push(wdgScreenWidth);
	scrControl.push(wdgScreenHeight);
	scrControl.push(wdgPolyOrder);
	scrControl.push(bangTestData);
	scrControl.push(bangLoadFit);
	scrControl.push(bangSaveFit);
	scrControl.push(new wdgButton("Invert XY", invertXY));
	scrControl.push(new wdgButton("File format v0.2", newFormat));
	scrControl.push(new wdgButton("Swap cameras", swapCameras));
	scrControl.push(bangSaveProjectionXYZ);
	
	
	scrGridData.push(&scrInputPoints);
	scrGridData.push(&scrTestCorrelate);
	scrGridData.setGridWidth(1);
	
	scrGridMain.push(&scrControl);
	scrGridMain.push(&scrFileSelection);
	scrGridMain.push(&scrGridData);
	scrGridMain.setGridWidth(3);
	////////////////////////////

	
	////////////////////////////
	// Initialise some values
	////////////////////////////
	// from the samsung monitor in the studio
	screenWidth = 0.598;
	screenHeight = 0.336;
	//
	polyOrder = 4;
	nDatasets = 0;
	//
	invertXY = true;
	newFormat = false;
	////////////////////////////
	
}

void CorrelateMain::update()
{
	if (bangLoad->getBang())
		loadData();
	
	if (bangCorrelate->getBang())
		runPolyfit();

	if (bangTestData->getBang())
		runTestSet();
	
	if (bangLoadFit->getBang())
	{
		string fname = "calib.fit";
		fit.load(fname);
		bangTestData->enabled = true;
	}

	if (bangSaveFit->getBang())
	{
		//string fname =	"order=" + ofToString(polyOrder, 0) +
		//				",nSets=" + ofToString(nDatasets, 0);
		string fname = "calib.fit";
		fit.save(fname);
	}
	
	if (bangSaveProjectionXYZ->getBang())
		saveProjector();
}

void CorrelateMain::loadData()
{
	polyInput.clear();
	polyOutput.clear();

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
			if (newFormat)
			{
				inFile.read((char*) &projWidth, 2);
				inFile.read((char*) &projHeight, 2);
			}
			if (nDatasets==0)
			{
				//first file, so initalise rows
				nCameras = thisNCameras;
				inputRow.resize(2*nCameras);
				outputRow.resize(3);
			} else
				if (nCameras != thisNCameras)
					ofLog(OF_LOG_ERROR, "CorrelateMain: mismatched number of cameras between files");
			///////////////////////////////
			
			
			///////////////////////////////
			// Read data
			///////////////////////////////
			thisNPoints = (thisFileSize-1) / ((newFormat ? 4 : 0)+8+8*nCameras);
			//
			for (int iPoint=0; iPoint<thisNPoints; iPoint++)
			{
				if (newFormat)
				{
					//read indicies
					inFile.read((char*) &dataset_iPX[iPoint], 2);
					inFile.read((char*) &dataset_iPY[iPoint], 2);
				}

				//read positions
				inFile.read((char*) &thisvalx, 4);
				inFile.read((char*) &thisvaly, 4);
				
				outputRow[0] = screenWidth * (thisvalx-0.5);
				outputRow[1] = screenHeight * (thisvaly-0.5);
				outputRow[2] = getDepthFromFilename(scrFileSelection.getName(iFile));
				
				for (int iCam=0; iCam<nCameras; iCam++)
				{					
					//read positions
					inFile.read((char*) &thisvalx, 4);
					inFile.read((char*) &thisvaly, 4);
					
					if (!swapCameras)
					{
						inputRow[0 + iCam*2] = thisvalx;
						inputRow[1 + iCam*2] = thisvaly;
					} else {
						inputRow[0 + (nCameras-iCam-1)*2] = thisvalx;
						inputRow[1 + (nCameras-iCam-1)*2] = thisvaly;
					}

				}
				
				nPoints++;
				
				polyInput.push_back(inputRow);
				polyOutput.push_back(outputRow);
			}
			///////////////////////////////
			
			
			///////////////////////////////
			// Close up the shop
			///////////////////////////////
			inFile.close();
			nDatasets++;
			///////////////////////////////			
		}
	
	//load input window
	copyToInputScreen();
	
	//clear test set window
	scrTestCorrelate.setWith(*test_pos, *input_col, 0);
	
	bangCorrelate->enabled=true;
	bangSaveFit->enabled=false;
}

float CorrelateMain::getDepthFromFilename(string filename)
{
	return ofToFloat(filename.substr(0,filename.length()-4));
}

void CorrelateMain::copyToInputScreen()
{
	double inv = (invertXY ? -1 : 1);
	
	if (nPoints>MAXPOINTS)
		ofLog(OF_LOG_WARNING, "CorrelateMain: nPoints > MAXPOINTS. only drawing first MAXPOINTS");
	
	for (int iPoint = 0; iPoint<min(nPoints,MAXPOINTS); iPoint++)
	{
		input_pos[iPoint][0] = inv * polyOutput[iPoint][0];
		input_pos[iPoint][1] = inv * polyOutput[iPoint][1];
		input_pos[iPoint][2] = polyOutput[iPoint][2];
		
		input_col[iPoint][0] = polyOutput[iPoint][0] / screenWidth + 0.5;
		input_col[iPoint][1] = polyOutput[iPoint][1] / screenHeight + 0.5;
		input_col[iPoint][2] = 0;//float(iPoint) / float(nDatasets-1);
	}
	
	scrInputPoints.setWith(&input_pos[0][0], &input_col[0][0], nPoints);	

}

void CorrelateMain::runPolyfit()
{
	fit.init(polyOrder, 2*nCameras, 3, BASIS_SHAPE_TRIANGLE);
	fit.correlate(polyInput, polyOutput);
	
	bangTestData->enabled=true;
	bangSaveFit->enabled=true;
}

void CorrelateMain::runTestSet()
{
	vector<double> *input;
	vector<double> output(3);
	
	double inv = (invertXY ? -1 : 1);
	
	for (int iPoint = 0; iPoint<min(nPoints,MAXPOINTS); iPoint++)
	{
		input = &polyInput[iPoint];
		
		output = fit.evaluate(*input);
		
		test_pos[iPoint][0] = inv * output[0];
		test_pos[iPoint][1] = inv * output[1];
		test_pos[iPoint][2] = output[2];
	}
	
	scrTestCorrelate.setWith(*test_pos, *input_col, nPoints);
	
	//we'll enable the save xyz, if we're in new format
	bangSaveProjectionXYZ->enabled = newFormat;
}

void CorrelateMain::saveProjector()
{
	ofImage imgSave;
	imgSave.setImageType(OF_IMAGE_COLOR);
	imgSave.allocate(projWidth, projHeight, OF_IMAGE_COLOR);
	
	ofPoint lbr = ofPoint(-1,-1,0);
	ofPoint rtb = ofPoint(1,1,2);
	
	//clear all values out to black
	memset(imgSave.getPixels(), 0, projWidth*projHeight*3);
	
	int iPP;
	unsigned char col[3];
	
	for (int iPoint=0; iPoint<nPoints; iPoint++)
	{
		//convert position to colour values
		col[0] = ofMap(test_pos[iPoint][0],lbr.x,rtb.x,0,255,true);
		col[1] = ofMap(test_pos[iPoint][1],lbr.y,rtb.y,0,255,true);
		col[2] = ofMap(test_pos[iPoint][2],lbr.z,rtb.z,0,255,true);
		
		iPP = dataset_iPX[iPoint] + projWidth * dataset_iPY[iPoint];
		if (iPP<projWidth*projHeight && iPP>=0)
			memcpy(imgSave.getPixels()+3*iPP, col, 3);
	}
	
	imgSave.saveImage("projector.bmp");
}

