/*
 *  ofxPolyFit.h
 *  ofxPolyFit example
 *
 *  Created by Elliot Woods on 03/11/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "polyNfit.h"
#include "ofMain.h"
#include <fstream>

class ofxPolyFit
{
public:
	ofxPolyFit();
	~ofxPolyFit();
	
	void								init(int order, int dimensionsIn, int dimensionsOut, int basisType);
	
	void								correlate(vector<vector<double> > &input, std::vector<std::vector<double> > &output);
	vector<double>						evaluate(vector<double> input);
	
	void								save(string filename);
	void								load(string filename);
	
	vector<unsigned int*>				*basisIndicies;
	vector<double*>						coefficients;
	
	unsigned int						nBases;
protected:

	polyNfit		*_fit;
	
	bool			_isInitialised;
	bool			_success;
};