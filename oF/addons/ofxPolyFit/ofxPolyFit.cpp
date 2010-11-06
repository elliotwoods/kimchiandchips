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

void ofxPolyFit::save(string filename)
{
#ifdef TARGET_WIN32
	filename = ".\\data\\" + filename;
#else
	filename = "../../../data/" + filename;
#endif
	ofstream fileout(filename.c_str(), ios::out | ios::binary);
	
	//write:
	//order, dimensionsIn, dimensionsOut, basisType, nBases
	//bases[iBasis,iDimensionIn]
	//coefficients[iDimensionOut,
	
	if (!fileout.is_open())
	{
		ofLog(OF_LOG_ERROR, "ofxPolyFit: cannot open output file " + filename);
		return;
	}
	
	fileout.write((char*) &_fit->_order, 2);
	fileout.write((char*) &_fit->_indim, 2);
	fileout.write((char*) &_fit->_outdim, 2);
	fileout.write((char*) &_fit->_basesShape, 1);
	fileout.write((char*) &nBases, 4);
	
	
	for (int iBasis=0; iBasis<nBases; iBasis++)
		fileout.write((char*) basisIndicies->at(iBasis),
					  _fit->_indim * 4);
	
	for (int iDimOut=0; iDimOut<_fit->_outdim; iDimOut++)
		fileout.write((char*) coefficients[iDimOut],
					  sizeof(double) * nBases);
	
}

void ofxPolyFit::load(string filename)
{
#ifdef TARGET_WIN32
	filename = ".\\data\\" + filename;
#else
	filename = "../../../data/" + filename;
#endif
	ifstream filein(filename.c_str(), ios::binary | ios::in);
	
	if (!filein.is_open())
	{
		ofLog(OF_LOG_ERROR, "ofxPolyFit: failed to load file " + filename);
		return;
	}
	
	filein.read((char*) &_fit->_order, 2);
	filein.read((char*) &_fit->_indim, 2);
	filein.read((char*) &_fit->_outdim, 2);
	filein.read((char*) &_fit->_basesShape, 1);
	filein.read((char*) &nBases, 4);
	
	for (int iBasis=0; iBasis<nBases; iBasis++)
		filein.read((char*) basisIndicies->at(iBasis),
					  _fit->_indim * 4);
	
	for (int iDimOut=0; iDimOut<_fit->_outdim; iDimOut++)
		filein.read((char*) coefficients[iDimOut],
					  sizeof(double) * nBases);
}

