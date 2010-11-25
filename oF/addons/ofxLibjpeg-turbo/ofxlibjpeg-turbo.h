/*
 *  ofxlibjpeg-turbo.h
 *  camtest
 *
 *  Created by Elliot Woods on 25/11/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "jpeglib.h"
#include "ofMain.h"

class ofxLibjpeg
{
public:
	ofxLibjpeg();
	~ofxLibjpeg();
	
	void save(unsigned char * pixels, int w, int h, string filename, int quality=80);
	
protected:
	struct jpeg_compress_struct			infCompress;
	struct jpeg_error_mgr				errCompress;
	
	int			rowStride;
	JSAMPROW	row_pointer[1];
};