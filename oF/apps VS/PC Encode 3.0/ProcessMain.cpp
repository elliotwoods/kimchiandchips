//
//  ProcessMain.cpp
//  PC Encode
//
//  Created by Elliot Woods on 06/05/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ProcessMain.h"

ProcessMain::ProcessMain() :
scrGridMain("Process"),
scrControl("Controls"),
scrData("Data points"),
scrRaw("Raw data"),
scrFiltered("Filtered data"),
allocated(false),
allocatedFiltered(false),
nPoints(0),
nPointsFiltered(0),
ransac_residual(0.03),
ransac_selection(0.1)
{
    scrGridMain.push(&scrControl);
    scrGridMain.push(&scrData);
    
    scrData.setGridWidth(1);
    scrData.push(&scrRaw);
    scrData.push(&scrFiltered);
    
    bangLoadScan = new wdgButton("Load scan");
    bangRANSAC = new wdgButton("Perform RANSAC filter");
    bangSaveFiltered = new wdgButton("Save filtered scan");
    
    wdgSlider *sliderResidual = new wdgSlider("Residual threshold", ransac_residual, 0, 0.1, 0.005);
    wdgSlider *sliderSelection = new wdgSlider("Initial selection", ransac_selection, 0, 1, 0.05);    
    
    scrControl.push(bangLoadScan);
    scrControl.push(bangRANSAC);
    scrControl.push(bangSaveFiltered);
    scrControl.push(sliderResidual);
    scrControl.push(sliderSelection);
    
}

ProcessMain::~ProcessMain()
{
    deAllocate();
}

void ProcessMain::update()
{
    if (bangLoadScan->getBang())
        load3DScan();
    
    if (bangRANSAC->getBang())
        filterRANSAC();
 
    if (bangSaveFiltered->getBang())
        saveFiltered();
}

void ProcessMain::deAllocate()
{
    if (allocated)
    {
        delete[] iX;
        delete[] iY;
        delete[] xyz;
        delete[] rgb;
        
        scrRaw.clear();
        
        allocated = false;
    }
}

void ProcessMain::deAllocateFiltered()
{
    
    if (allocatedFiltered)
    {
        delete[] xyzFiltered;
        delete[] rgbFiltered;
        delete[] iXFiltered;
        delete[] iYFiltered;
        
        scrFiltered.clear();
        allocatedFiltered = false;
    }
}

void ProcessMain::allocate()
{
    deAllocate();
    
    iX = new unsigned int[nPoints];
    iY = new unsigned int[nPoints];
    xyz = new float[nPoints*3];
    rgb = new float[nPoints*3];
    
    allocated = true;
}


void ProcessMain::allocateFiltered()
{
    deAllocateFiltered();
    
    iXFiltered = new unsigned int[nPointsFiltered];
    iYFiltered = new unsigned int[nPointsFiltered];
    xyzFiltered = new float[nPointsFiltered*3];
    rgbFiltered = new float[nPointsFiltered*3];
    
    allocatedFiltered = true;
}

void ProcessMain::load3DScan()
{
    //////////////////////////
    // Load binary data
    //////////////////////////
    //
    string filename = "3DScan.bin";
#ifdef TARGET_WIN32
	filename = ".\\data\\" + filename;
#else
	filename = "../../../data/" + filename;
#endif
    ifstream inFile(filename.c_str(), ios::out | ios::binary);
    
    char header[2];
    
    //read overall data
    inFile.read(header, 2);
    
    if (header[0] != '3' || header[1] != 'D')
    {
        ofLog(OF_LOG_ERROR, "ProcessMain: Failed to load scan, incorrect header");
        return;
    }
    
    inFile.read((char*) &nPoints, 4);
    inFile.read((char*) &projWidth, 2);
    inFile.read((char*) &projHeight, 2);
    inFile.read((char*) lbf, 4 * 3);
    inFile.read((char*) rtb, 4 * 3);
    
    allocate();    
    
    for (int i=0; i<nPoints; i++)
    {
        inFile.read((char*) &iX[i], 4);
        inFile.read((char*) &iY[i], 4);
        inFile.read((char*) &xyz[i*3], 4 * 3);
    }
    
    calcRGB();
    
    inFile.close();
    //
    //////////////////////////
    
    scrRaw.setWith(xyz, rgb, nPoints);
}

void ProcessMain::calcRGB()
{
    if (!allocated)
        return;
    
    for (int i=0; i<nPoints; i++)
    {
        rgb[i*3 + 0] = (xyz[i*3 + 0] - lbf[0]) / (rtb[0] - lbf[0]);
        rgb[i*3 + 1] = (xyz[i*3 + 1] - lbf[1]) / (rtb[1] - lbf[1]);
        rgb[i*3 + 2] = (xyz[i*3 + 2] - lbf[2]) / (rtb[2] - lbf[2]);
    }
}

void ProcessMain::filterRANSAC()
{
    RANSAC.init(1, 4, 2, BASIS_SHAPE_PADE_FIRST);
    
    double* input = new double[nPoints * 4];
    double* output = new double[nPoints * 2];

    double ProjectorX, ProjectorY;
    
    double* inputMove = input;
    double* outputMove = output;
    
    //fill RANSAC
    for (int i=0; i<nPoints; i++)
    {
        ProjectorX = 2 * (double(iX[i]) / double(projWidth)) - 1;
        ProjectorY = 1 - 2 * (double(iY[i]) / double(projHeight));
        
        
        inputMove[0] = xyz[i*3 + 0];
        inputMove[1] = xyz[i*3 + 1];
        inputMove[2] = xyz[i*3 + 2];        
        inputMove[3] = ProjectorX;
        
        outputMove[0] = ProjectorX;
        outputMove[1] = ProjectorY;
        
        inputMove += 4;
        outputMove += 2;
    }
    
    RANSAC.RANSAC(input, output, nPoints, 20, ransac_selection, ransac_residual, 0.5);
    
    set<int>::iterator it;
    
    nPointsFiltered = RANSAC.bestConsensus.size();
    
    deAllocateFiltered();
    allocateFiltered();

    
    
    int i=0;
    for (it = RANSAC.bestConsensus.begin(); it != RANSAC.bestConsensus.end(); it++)
    {
        for (int iDim=0; iDim<3; iDim++)
        {
            xyzFiltered[i*3 + iDim] = xyz[*it * 3 + iDim];
            rgbFiltered[i*3 + iDim] = rgb[*it * 3 + iDim];
        }
        
        iXFiltered[i] = iX[*it];
        iYFiltered[i] = iY[*it];
        
        i++;
    }
    
    scrFiltered.setWith(xyzFiltered, rgbFiltered, nPointsFiltered);
    
}

void ProcessMain::saveFiltered()
{
    //////////////////////////
    // BMP file
    //////////////////////////
    //
	ofImage imgSave;
	imgSave.setImageType(OF_IMAGE_COLOR);
	imgSave.allocate(projWidth, projHeight, OF_IMAGE_COLOR);
	
	//clear all values out to black
	memset(imgSave.getPixels(), 0, projWidth*projHeight*3);
    
	int iPP;
	unsigned char col[3];
    float* point;
	
	for (int iPoint=0; iPoint<nPointsFiltered; iPoint++)
	{
        point = &xyzFiltered[iPoint];
        
        //check if not within selected bounds
        if (point[0] < lbf[0] || point[1] < lbf[1] || point[2] < lbf[2] || point[0] > rtb[0] || point[1] > rtb[1] || point[2] > rtb[2])
            continue;
        
		//convert position to colour values
		col[0] = ofMap(point[0],lbf[0],rtb[0],0,255,true);
		col[1] = ofMap(point[1],lbf[1],rtb[1],0,255,true);
		col[2] = ofMap(point[2],lbf[2],rtb[2],0,255,true);
		
		iPP = iXFiltered[iPoint] + projWidth * iYFiltered[iPoint];
        
		if (iPP<int(projWidth*projHeight) && iPP>=0)
			memcpy(imgSave.getPixels()+3*iPP, col, 3);
	}
	
	imgSave.saveImage("filtered.bmp");
    //
    //////////////////////////
    
    
    
    //////////////////////////
    // Binary data
    //////////////////////////
    //
    int nPointsSelected = 0, null;
    
    string filename = "filtered.bin";
#ifdef TARGET_WIN32
	filename = ".\\data\\" + filename;
#else
	filename = "../../../data/" + filename;
#endif
    ofstream outFile(filename.c_str(), ios::out | ios::binary);
    
    //write overall data
    
    outFile << "3D";
    outFile.write((char*) &null, 4);
    outFile.write((char*) &projWidth, 2);
    outFile.write((char*) &projHeight, 2);
    outFile.write((char*) lbf, 4 * 3);
    outFile.write((char*) rtb, 4 * 3);
    
    
    for (int i=0; i<nPoints; i++)
    {
        point = &xyz[i];
        
        //check if not within selected bounds
        if (point[0] < lbf[0] || point[1] < lbf[1] || point[2] < lbf[2] || point[0] > rtb[0] || point[1] > rtb[1] || point[2] > rtb[2])
            continue;
        
        outFile.write((char*) &iXFiltered[i], 4);
        outFile.write((char*) &iYFiltered[i], 4);
        outFile.write((char*) point, 4 * 3);
        
        nPointsSelected++;
    }
    
    outFile.seekp(2);
    outFile.write((char*) &nPointsSelected, 4);    
    outFile.close();
    //
    //////////////////////////    
}






