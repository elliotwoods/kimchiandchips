/*
 *  ofxPolyFit.cpp
 *  ofxPolyFit example
 *
 *  Created by Elliot Woods on 03/11/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "ofxPolyFit.h"

ofxPolyFit::ofxPolyFit()
{
	_isInitialised = false;
	_success = false;
	nBases = 0;
}

ofxPolyFit::~ofxPolyFit()
{
	if (_isInitialised)
		delete _fit;
}

void ofxPolyFit::init(int order, int dimensionsIn, int dimensionsOut, int basisType)
{
	if (_isInitialised)
	{
		for (int i=0; i<_fit->_outdim; i++)
			delete[] coefficients[i];
		coefficients.clear();
		delete _fit;
	}
	
	_fit = new polyNfit(order, dimensionsIn, dimensionsOut, basisType);
	
	basisIndicies = &(_fit->vecBasisIndicies);
	
	//create space for coefficients
	coefficients.resize(_fit->_outdim);
	for (int i=0; i<_fit->_outdim; i++)
	{
		coefficients[i] = new double[_fit->_nBases];
		for (int iBasis=0; iBasis<_fit->_nBases; iBasis++)
			coefficients[i][iBasis] = 0;
	}
	
	_success = false;
	_isInitialised = true;
}

void ofxPolyFit::correlate(std::vector<std::vector<double> > &input, std::vector<std::vector<double> > &output)
{
	if (_isInitialised)
	{
		try
		{
			//////////////////////////////////////////////////////
			// BASIC ERROR CHECKING
			//////////////////////////////////////////////////////
			if (input.size() != output.size())
				throw("number of input points doesn't match number of output points");
			
			//NB: there's no error check here
			//for every element to be the right dimension
			//one could be out!
			if (input[0].size() != _fit->_indim)
				throw("dimensions of input doesn't match fit");
			
			if (output[0].size() != _fit->_outdim)
				throw("dimensions of output doesn't match fit");
			//////////////////////////////////////////////////////
			
			
			//////////////////////////////////////////////////////
			// PERFORM FIT
			//////////////////////////////////////////////////////
			_fit->init(input, output, input.size());
			//////////////////////////////////////////////////////
			
			
			//////////////////////////////////////////////////////
			// READ IN COEFFICIENTS
			//////////////////////////////////////////////////////
			for (int iDimOut = 0; iDimOut < _fit->_outdim; iDimOut++)
				_fit->solve(iDimOut, coefficients[iDimOut], nBases);
			//////////////////////////////////////////////////////
			
			_success = true;
		}
		
		catch (char* message)
		{
			  ofLog(OF_LOG_ERROR, "ofxPolyFit: " + string(message));
			
			_success = false;
		}
	} else
		ofLog(OF_LOG_ERROR, "ofxPolyFit: Cannot evaluate, not yet initiliased");
}

vector<double> ofxPolyFit::evaluate(vector<double> input)
{
	vector<double> output;
	output.assign(_fit->_outdim, 0);
	
	if (input.size() != _fit->_indim)
	{
		ofLog(OF_LOG_ERROR, "ofxPolyFit: evalute() input dimensions do not match fit");
		return output;
	}
	
	float basis, coeff;
	for (int iDimOut=0; iDimOut< _fit->_outdim; iDimOut++)
	{
		output[iDimOut] = 0;
		
		for (int iBasis=0; iBasis<nBases; iBasis++)
		{
			basis=_fit->basis(iBasis, input);
			
			output[iDimOut]+= basis * coefficients[iDimOut][iBasis];
		}
			
		
	}
	return output;
}