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
#include <set>

class ofxPolyFit
{
public:
	ofxPolyFit();
	~ofxPolyFit();
	
	void								init(int order, int dimensionsIn, int dimensionsOut, int basisType);
	void                                uninitialise();
    
	void								correlate(vector<vector<double> > &input, std::vector<std::vector<double> > &output);
    void                                correlate(double* input, double* output, int nDataPoints); //presume xyzxyzxyz format
    void                                correlate(double* input, double* output, set<int> activeIndices);
    
    
    
    
	vector<double>						evaluate(vector<double> &input);
    void                                evaluate(double* input, double* output);
    void                                evaluate(float* input, float* output);	
    
    //special 1D case
    float                               evaluate(float input);
    
    double                              residual(vector<double> input, vector<double> output);
    double                              residual(double* input, double* output);
    double                              residual(double* input, double* output, set<int> activeIndices);
    
	void								save(string filename);
	void								load(string filename);
	
    //experimental RANSAC function
    void                                RANSAC(double* input, double* output, int nDataPoints, int maxIterations, float selectionProbability, float residualThreshold, float inclusionThreshold);
    
	vector<unsigned int*>				*basisIndicies;
	vector<double*>						coefficients;
	
	unsigned int						nBases;
protected:

	polyNfit		*_fit;
	
	bool			_isInitialised;
	bool			_success;
};