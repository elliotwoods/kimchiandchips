/*
 *  PCLogger.cpp
 *  pixel correlation
 *
 *  Created by Elliot Woods on 26/01/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "PCincludes.h"

PCLogger::PCLogger(PCEncode *encoder, vector<PCDecode*> *decoders)
{
	_encoder = encoder;
	_decoders = decoders;
	
	_imgCameraSpacePreview.setUseTexture(false);
	_imgProjectorSpacePreview.setUseTexture(false);
	
}

void PCLogger::save(string filename)
{
	// ** TODO ** 
	//this needs to be replaced with a kind
	//'what to log' rather than
	//'whether to log'
	if (!isLogging)
		return;
	
	#ifdef TARGET_WIN32
		filename = "logs\\" + filename;
	#else
		filename = "logs/" + filename;
	#endif

	//////////////////////////////////////////////////////
	// SAVE IMAGES
	//////////////////////////////////////////////////////
	for (int iDecoder=0; iDecoder<_decoders->size(); iDecoder++)
	{
		
		string strExtension = ".png";
		string strIndex = "[" + ofToString(iDecoder,0) + "]";
		
		
		_imgCameraSpacePreview.setFromPixels(_decoders->at(iDecoder)->_charCameraSpacePreview,
											 projWidth, projHeight,
											 OF_IMAGE_COLOR, true);
		
		_imgProjectorSpacePreview.setFromPixels(_decoders->at(iDecoder)->_charProjectorSpacePreview,
												camWidth, camHeight,
												OF_IMAGE_COLOR, true);
		
		_imgCameraSpacePreview.saveImage(filename + "-camera" + strIndex + strExtension);
		_imgProjectorSpacePreview.saveImage(filename + "-projector" + strIndex + strExtension);
	}
	//////////////////////////////////////////////////////

	//////////////////////////////////////////////////////
	// SAVE DATA
	//////////////////////////////////////////////////////
	//first we have to create the filename
	//with path for iostream to access file
	string strFileWithPath;
#ifdef TARGET_WIN32
	strFileWithPath = ".\\data\\" + filename;
#else
	strFileWithPath = "../../../data/" + filename;
#endif
	
	
	saveConfig(strFileWithPath + "-config.txt");
	savePixelsText(strFileWithPath + "-data.txt");
	savePixelsBinary(strFileWithPath + ".bin");
	//////////////////////////////////////////////////////

}

void PCLogger::saveConfig(string filename)
{
	ofstream iofOutput(filename.c_str(), ios::out);
	iofOutput	<< projWidth << "\t"
				<< projHeight << "\t"
				<< interleaveWidth << "\t"
				<< camWidth << "\t"
				<< camHeight << "\t"
				<< _decoders->size() << endl;
	
	iofOutput.close();
}

void PCLogger::savePixelsBinary(string filename)
{
	ofstream iofOutput(filename.c_str(), ios::out | ios::binary);
    
	bool hasAllFinds;
	PCPixelSlim *currentPixel;
	
	int iPX, iPY;
	float fPX, fPY;
	
	//write config
	iofOutput.write((char*) &nCameras, 1);
	//could write rest of config here if it's useful..
	
	for (int iPP=0; iPP<projWidth*projHeight; iPP++)
	{
		//////////////////////////////
		//check exists in all cameras
		//////////////////////////////
		hasAllFinds = true;
		for (int iDec=0; iDec<_decoders->size(); iDec++)
		{
			currentPixel = _decoders->at(iDec)->projPixels[iPP];
			hasAllFinds &= currentPixel->_iLastFoundPixel != -1;
		}
		if (!hasAllFinds)
			continue;
		//////////////////////////////
		
		
		//////////////////////////////
		//write projector data
		//////////////////////////////
		iPX = iPP % projWidth;
		iPY = iPP / projHeight;
		
		fPX = float(iPX) / float (projWidth);
		fPY = float(iPY) / float (projHeight);
		
		//write the projector pixels' ID
		iofOutput.write((char*) &fPX, 4);
		iofOutput.write((char*) &fPY, 4);
		//////////////////////////////
								
		
		//////////////////////////////
		//write camera data
		//////////////////////////////
		for (int iDec=0; iDec<_decoders->size(); iDec++)
		{
			currentPixel = _decoders->at(iDec)->projPixels[iPP];
			iofOutput.write((char*) &(currentPixel->_meanXdash.x), 4);
			iofOutput.write((char*) &(currentPixel->_meanXdash.y), 4);
		}
		//////////////////////////////

}
	
    iofOutput.close();
}

void PCLogger::savePixelsText(string filename)
{
	//save in row format:
	
	//	i	ix	iy
	//				meanX	meanY	sigmaX	sigmaY sigmaR	iLastFind	nFinds
	
	ofstream iofOutput(filename.c_str(), ios::out);
	stringstream dataRow;
	
	dataRow.precision(10);
	
	PCPixelSlim *pixelFinds;
	
	bool hasAllFinds;

	for (int iPP=0; iPP<projWidth*projHeight; iPP++)
	{
		dataRow.str("");

		dataRow << iPP << "\t" <<
			(iPP % projWidth) << "\t" <<
			int(iPP / projWidth);

		hasAllFinds = true;

		for (int iDec=0; iDec<_decoders->size(); iDec++)
		{
			pixelFinds = _decoders->at(iDec)->projPixels[iPP];
	
			dataRow << "\t"<< pixelFinds->_meanXdash.x << "\t" << pixelFinds->_meanXdash.y << "\t" <<
								pixelFinds->_sigmaXdash.x << "\t" <<  pixelFinds->_sigmaXdash.y << "\t" <<
								pixelFinds->_sigmaRdash << "\t" << 
								pixelFinds->_iLastFoundPixel << "\t" << 
								pixelFinds->_nFinds;

			hasAllFinds &= (pixelFinds->_iLastFoundPixel != -1);
		}
			
		dataRow << endl;

		if (hasAllFinds)
			iofOutput << dataRow.str();
	}

    iofOutput.close();
}