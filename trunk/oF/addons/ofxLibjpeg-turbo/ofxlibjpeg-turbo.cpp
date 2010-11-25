/*
 *  ofxlibjpeg-turbo.cpp
 *  camtest
 *
 *  Created by Elliot Woods on 25/11/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "ofxlibjpeg-turbo.h"

ofxLibjpeg::ofxLibjpeg()
{
	//define error object
	infCompress.err = jpeg_std_error(&errCompress);
	
	//instatiate compression object
	jpeg_create_compress(&infCompress);
}

ofxLibjpeg::~ofxLibjpeg()
{
	//destroy compression object
	jpeg_destroy_compress(&infCompress);
}

void ofxLibjpeg::save(unsigned char * pixels, int w, int h, string filename, int quality)
{
	string fullpath = ofToDataPath(filename);
	char * chrFilename;
	strcpy(chrFilename, fullpath.c_str());
	FILE * fileOut;
	
	////////////////////////////////////
	// SET COMPRESSION PARAMETERS
	////////////////////////////////////
	infCompress.image_width = w;
	infCompress.image_height = h;
	infCompress.input_components = 3;
	infCompress.in_color_space = JCS_RGB;
	jpeg_set_defaults(&infCompress);
	jpeg_set_quality(&infCompress, quality, TRUE);
	////////////////////////////////////
	
	
	////////////////////////////////////
	// OPEN FILE
	////////////////////////////////////
	if ((fileOut = fopen(chrFilename, "wb")) == NULL) {
		ofLog(OF_LOG_ERROR, "ofxLibjpeg: cant open file for saving " + fullpath);
		return;
	}
	jpeg_stdio_dest(&infCompress, fileOut);	
	////////////////////////////////////
	
	
	////////////////////////////////////
	// PERFORM COMPRESSION
	////////////////////////////////////
	jpeg_start_compress(&infCompress, TRUE);
	rowStride = w * 3;
	
	while (infCompress.next_scanline < h) {
		/* jpeg_write_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could pass
		 * more than one scanline at a time if that's more convenient.
		 */
		row_pointer[0] = &pixels[infCompress.next_scanline * rowStride];
		(void) jpeg_write_scanlines(&infCompress, row_pointer, 1);
	}
	////////////////////////////////////
	
	
	////////////////////////////////////
	// CLOSE UP SHOP
	////////////////////////////////////	
	jpeg_finish_compress(&infCompress);
	fclose(fileOut);
	////////////////////////////////////
}