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

void PCLogger::save()
{
	if (!isLogging)
		return;
	
	string strFilename = getDateString();
	
	//////////////////////////////////////////////////////
	// SAVE IMAGES
	//////////////////////////////////////////////////////
	for (int iDecoder=0; iDecoder<_decoders->size(); iDecoder++)
	{
		
		string strExtension = ".png";
	#ifdef TARGET_WIN32
		strFilename = "logs\\" + strFilename;
	#else
		strFilename = "logs/" + strFilename;
	#endif
		
		
		_imgCameraSpacePreview.setFromPixels(_decoders->at(iDecoder)->_charCameraSpacePreview,
											 projWidth, projHeight,
											 OF_IMAGE_COLOR, true);
		
		_imgProjectorSpacePreview.setFromPixels(_decoders->at(iDecoder)->_charProjectorSpacePreview,
												camWidth, camHeight,
												OF_IMAGE_COLOR, true);
		
		_imgCameraSpacePreview.saveImage(strFilename + "-camera" + strExtension);
		_imgProjectorSpacePreview.saveImage(strFilename + "-projector" + strExtension);
	}
	//////////////////////////////////////////////////////

	//////////////////////////////////////////////////////
	// SAVE DATA
	//////////////////////////////////////////////////////
	//first we have to create the filename
	//with path for iostream to access file
	string strFileWithPath;
#ifdef TARGET_WIN32
	strFileWithPath = ".\\data\\" + strFilename;
#else
	strFileWithPath = "../../../data/" + strFilename;
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
    
	for (int iPP=0; iPP<projWidth*projHeight; iPP++)
		for (int iDec=0; iDec<_decoders->size(); iDec++)
			
			iofOutput.write((char*) (PCPixelSlim*)
							_decoders->at(iDec)->projPixels[iPP],
							sizeof(PCPixelSlim));
	
    iofOutput.close();
}

void PCLogger::savePixelsText(string filename)
{
	//save in row format:
	
	//	i	ix	iy	meanx	meany	sigmax	sigmay	iLastFind	nFinds
	
	ofstream iofOutput(filename.c_str(), ios::out);
	
	iofOutput.precision(20);
	
	PCPixelSlim *pixelFinds;
	
	for (int iPP=0; iPP<projWidth*projHeight; iPP++)
		for (int iDec=0; iDec<_decoders->size(); iDec++)
		{
			iofOutput << iPP << "\t" <<
						(iPP % projWidth) << "\t" <<
						int(iPP / projWidth) << "\t";
			
			for (int iDec=0; iDec<_decoders->size(); iDec++)
			{
				pixelFinds = _decoders->at(iDec)->projPixels[iPP];
		
				iofOutput <<	pixelFinds->_meanXdash.x << "\t" << pixelFinds->_meanXdash.y << "\t" <<
								pixelFinds->_sigmaXdash.x << "\t" <<  pixelFinds->_sigmaXdash.y << "\t" <<
								pixelFinds->_sigmaRdash << "\t" << 
								pixelFinds->_iLastFoundPixel << "\t" << 
								pixelFinds->_nFinds;
			}
			
			iofOutput << endl;
		}
	
    iofOutput.close();
}

string PCLogger::getDateString()
{
	char datestring[10];

	time_t now = time(0);
	tm hereandnow=*localtime(&now);

	const char format[]="%Y%m%d";
	strftime(datestring, 50, format, &hereandnow);
	
	return string(datestring);
}