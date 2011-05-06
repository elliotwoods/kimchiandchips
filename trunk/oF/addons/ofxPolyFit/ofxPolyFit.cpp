/*
 *  ofxPolyFit.cpp
 *  ofxPolyFit example
 *
 *  Created by Elliot Woods on 03/11/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "ofxPolyFit.h"

ofxPolyFit::ofxPolyFit() : 
bestModel(0)
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
		uninitialise();
	
	_fit = new polyNfit(order, dimensionsIn, dimensionsOut, basisType);
	
	basisIndicies = &(_fit->vecBasisIndices);
	
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

void ofxPolyFit::uninitialise()
{
    for (int i=0; i<_fit->_outdim; i++)
        delete[] coefficients[i];
    coefficients.clear();
    delete _fit;
    nBases = 0;
    
    _isInitialised = false;
    _success = false;
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

void ofxPolyFit::correlate(double* input, double* output, int nDataPoints)
{
    unsigned short &indim(_fit->_indim);
    unsigned short &outdim(_fit->_outdim);
    
    vector<double> inpoint;
    vector<double> outpoint;
    vector< vector<double> > vecInput, vecOutput;
    
    inpoint.resize(indim);
    outpoint.resize(outdim);
    vecInput.resize(nDataPoints);
    vecOutput.resize(nDataPoints);
    
    for (int i=0; i<nDataPoints; i++)
    {
        memcpy(&inpoint[0], input + indim * i, indim * sizeof(double));
        memcpy(&outpoint[0], output + outdim * i, outdim * sizeof(double));
        
        vecInput[i] = inpoint;
        vecOutput[i] = outpoint;
    }
    
    correlate(vecInput, vecOutput);
}

void ofxPolyFit::correlate(double* input, double* output, set<int> activeIndices)
{
    unsigned short &indim(_fit->_indim);
    unsigned short &outdim(_fit->_outdim);
    
    vector<double> inpoint;
    vector<double> outpoint;
    vector< vector<double> > vecInput, vecOutput;
    
    int nDataPoints = activeIndices.size();
    
    inpoint.resize(indim);
    outpoint.resize(outdim);
    
    vecInput.resize(nDataPoints);
    vecOutput.resize(nDataPoints);
    
    set<int>::iterator it;
    int i = 0;
    
    for (it = activeIndices.begin(); it != activeIndices.end(); it++)
    {
        memcpy(&inpoint[0], input + indim * *it, indim * sizeof(double));
        memcpy(&outpoint[0], output + outdim * *it, outdim * sizeof(double));
        
        vecInput[i] = inpoint;
        vecOutput[i] = outpoint;
        
        i++;
    }
    
    correlate(vecInput, vecOutput);    
}

vector<double> ofxPolyFit::evaluate(vector<double> &input)
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

void ofxPolyFit::evaluate(double* input, double* output)
{
    float basis, coeff;
    
    vector<double> inputVec(input, input + _fit->_indim);

	for (int iDimOut=0; iDimOut< _fit->_outdim; iDimOut++)
	{
		output[iDimOut] = 0;
		
		for (int iBasis=0; iBasis < nBases; iBasis++)
		{
			basis=_fit->basis(iBasis, inputVec);
			
			output[iDimOut]+= basis * coefficients[iDimOut][iBasis];
		}
		
	}    
}

void ofxPolyFit::evaluate(float* input, float* output)
{
    float basis, coeff;
    
    vector<double> inputVec(input, input + _fit->_indim);
    
	for (int iDimOut=0; iDimOut< _fit->_outdim; iDimOut++)
	{
		output[iDimOut] = 0;
		
		for (int iBasis=0; iBasis < nBases; iBasis++)
		{
			basis=_fit->basis(iBasis, inputVec);
			
			output[iDimOut]+= basis * coefficients[iDimOut][iBasis];
		}
		
	}    
}

float ofxPolyFit::evaluate(float input)
{
    if (_fit->_outdim != 1 || _fit->_indim != 1)
    {
        ofLog(OF_LOG_WARNING, "Cannot use ofxPolyFit::evaluate(float) unless indim=1, outdim=1");
        return 0;
    }
    
    float output = 0;
    float basis;
    
    vector<double> inputVec(1);
    inputVec[0] = input;
    
    for (int iBasis=0; iBasis < nBases; iBasis++)
    {
        basis=_fit->basis(iBasis, inputVec);
        
        output += basis * coefficients[0][iBasis];
    }
    
    return output;
    
}

double ofxPolyFit::residual(vector<double> input, vector<double> output)
{
    if (input.size() != _fit->_indim || output.size() != _fit->_outdim)
    {
        ofLog(OF_LOG_ERROR, "ofxPolyFit::residual(vector<double> input, vector<double> output) input dimensions do not match my local dimensions");
        return 0;
    }
    vector<double> evaluatedOutput = evaluate(input);
    vector<double>::iterator it;
    
    double error = 0;
    
    for (int i=0; i<output.size(); i++)
        error += pow(output[i] - evaluatedOutput[i], 2);

    return sqrt(error);
}

double ofxPolyFit::residual(double* input, double* output)
{
    //residual for an individual point
    double *evaluatedOutput = new double[_fit->_outdim];
    
    evaluate(input, evaluatedOutput);
    
    double error = 0;
    
    for (int i=0; i<_fit->_outdim; i++)
        error += pow(output[i] - evaluatedOutput[i], 2);
    
    return sqrt(error);
    
}

double ofxPolyFit::residual(double* input, double* output, set<int> activeIndices)
{
    //calculae the mean error
    double error = 0;
    set<int>::iterator it;
    
    for (it = activeIndices.begin(); it != activeIndices.end(); it++)
        error += residual(input + _fit->_indim * (*it), output + _fit->_outdim* (*it));
    
    error /= double(activeIndices.size());
    
    return error;
    
}


void ofxPolyFit::save(string filename)
{
#ifdef TARGET_WIN32
	filename = ".\\data\\" + filename;
#else
	filename = "../../../data/" + filename;
#endif
	ofstream fileout(filename.c_str(), ios::out | ios::binary | ios::trunc);
	
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
					  sizeof(unsigned int) * _fit->_indim);
	
	for (int iDimOut=0; iDimOut<_fit->_outdim; iDimOut++)
		fileout.write((char*) coefficients[iDimOut],
					  sizeof(double) * nBases);
    
	ofLog(OF_LOG_VERBOSE, "ofxPolyFit: Fit saved");
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
	unsigned short order, indim, outdim;
    unsigned char basesShape;
    
	filein.read((char*) &order, 2);
	filein.read((char*) &indim, 2);
	filein.read((char*) &outdim, 2);
	filein.read((char*) &basesShape, 1);
    
    init(order, indim, outdim, basesShape);
    
	filein.read((char*) &nBases, 4);
	
	for (int iBasis=0; iBasis<nBases; iBasis++)
		filein.read((char*) basisIndicies->at(iBasis),
					  _fit->_indim * 4);
	
	for (int iDimOut=0; iDimOut<_fit->_outdim; iDimOut++)
		filein.read((char*) coefficients[iDimOut],
					  sizeof(double) * nBases);
}



void ofxPolyFit::RANSAC(double* input, double* output, int nDataPoints, int maxIterations, float selectionProbability, float residualThreshold, float inclusionThreshold)
{
    //////////////////////////////////////////////////////////////////
    //taken from pseudocode at http://en.wikipedia.org/wiki/RANSAC
    //////////////////////////////////////////////////////////////////
    //
    // selectionProbability = 0.0..1.0  ;   Possibility of each data
    //                                      point being within
    //                                      "maybeInlier" set at
    //                                      the start of each
    //                                      iteration.
    //
    // residualThreshold = 0.0..+INF    ;   Maximum residual a data
    //                                      point can have to be
    //                                      added to the consensus
    //                                      set.
    //
    // inclusionThreshold = 0.0..1.0    ;   What percentage of data
    //                                      points must be included
    //                                      in the consenus set.
    //////////////////////////////////////////////////////////////////
    
    bestError = + 1e37;
    bestConsensus.clear();
    
    if (bestModel != 0)
        delete[] bestModel;
    bestModel = new double[_fit->_outdim * nBases];
    
    set<int> maybeInlierIndices;
    set<int> currentConsensus;
    set<int>::iterator idxIt;
    double currentError;
    
    vector<double> vecInputPoint(_fit->_indim);
    vector<double> vecOutputPoint(_fit->_outdim);
    
    float startTime;
    
    //loop through allowed number of iterations
    for (int iteration=0; iteration<maxIterations; iteration++)
    {
        startTime = ofGetElapsedTimef();
        
        //////////////////////////////////
        // Randomly select maybe inliers
        //////////////////////////////////
        //
        maybeInlierIndices.clear();
        for (int iPoint=0; iPoint<nDataPoints; iPoint++)
            if (ofRandomuf() < selectionProbability)
                maybeInlierIndices.insert(iPoint);
        
        if (maybeInlierIndices.size() < nBases)
            continue;
        //
        //////////////////////////////////
        
        
        ////////////////////////////////////
        // Perform a fit with maybe inliers
        ////////////////////////////////////
        //
        correlate(input, output, maybeInlierIndices);
        //
        ////////////////////////////////////
        
        
        ////////////////////////////////////
        // Build consensus set
        ////////////////////////////////////
        //
        currentConsensus = maybeInlierIndices;
        
        for (int iPoint=0; iPoint<nDataPoints; iPoint++)
        {
            //first check if already in set
            if (currentConsensus.count(iPoint) != 0)
                continue;
            
            memcpy(&vecInputPoint[0], input + (iPoint * _fit->_indim), _fit->_indim * sizeof(double)); 
            memcpy(&vecOutputPoint[0], output + (iPoint * _fit->_outdim), _fit->_outdim * sizeof(double)); 
            
            //if residual for this point is less than threshold
            //then add it to the consensus set
            if (residual(vecInputPoint, vecOutputPoint) < residualThreshold)
                currentConsensus.insert(iPoint);
            
        }
        //
        ////////////////////////////////////
        
        
        ////////////////////////////////////////
        // Check if we meet inclusion threshold
        ////////////////////////////////////////
        //
        if (currentConsensus.size() < inclusionThreshold * float(nDataPoints))
            continue;
        //
        ////////////////////////////////////////
        
        
        ////////////////////////////////////
        // Check to see if this iteration
        // is best so far
        ////////////////////////////////////
        //
        correlate(input, output, currentConsensus);
        currentError = residual(input, output, currentConsensus);
        
        if (currentError < bestError)
        {
            //we've got a better fit
            bestError = currentError;
            bestConsensus = currentConsensus;
            
            for (int iOutDim=0; iOutDim<_fit->_outdim; iOutDim++)
                for (int iBasis=0; iBasis<nBases; iBasis++)
                {
                    bestModel[iOutDim * nBases + iBasis] = coefficients[iOutDim][iBasis];
                }
        }
        //        
        ////////////////////////////////////
        
        cout << "Iteration #" << iteration << " took " << (ofGetElapsedTimef() - startTime) << "s to complete. bestError=" << bestError << " bestConsensus count=" << bestConsensus.size() << "\n";
    }
    
    
    ////////////////////////////////////
    // Reload best model as current fit
    ////////////////////////////////////
    //
    for (int iOutDim=0; iOutDim<_fit->_outdim; iOutDim++)
        for (int iBasis=0; iBasis<nBases; iBasis++)
        {
            coefficients[iOutDim][iBasis] = bestModel[iOutDim * nBases + iBasis];
        }    
    //
    ////////////////////////////////////
}
                   

