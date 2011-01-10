
//
//
//		P O L Y A L
//
//

#include "stdafx.h"

#include "PolyValND.h"

using namespace System;
using namespace VVVV::PluginInterfaces::V1;
using namespace std;

namespace PolyFitND 
{

	PolyValND::PolyValND()
	{ }

	PolyValND::~PolyValND()
	{	}


	void PolyValND::SetPluginHost(IPluginHost ^ Host) 
	{
		this->FHost = Host;

		array<String ^> ^ arr = gcnew array<String ^>(1);		
		FHost->CreateValueInput("Input",1,arr,TSliceMode::Dynamic,TPinVisibility::True,this->vPinInInput);
		FHost->CreateValueOutput("Output",1,arr,TSliceMode::Dynamic,TPinVisibility::True,this->vPinOutOutput);

		FHost->CreateValueInput("Input Dimensions", 1, arr, TSliceMode::Dynamic, TPinVisibility::True,this->vPinDimensionsIn);
		vPinDimensionsIn->SetSubType(1,MAX_DIMENSIONS,1,3,false,false,true);

		FHost->CreateValueInput("Output Dimensions", 1, arr, TSliceMode::Dynamic, TPinVisibility::True,this->vPinDimensionsOut);
		vPinDimensionsOut->SetSubType(1,MAX_DIMENSIONS,1,3,false,false,true);

		FHost->CreateValueInput("Coefficients",1,arr,TSliceMode::Dynamic,TPinVisibility::True,this->vPinInCoefficients);
		FHost->CreateValueInput("Bases indicies",1,arr,TSliceMode::Dynamic,TPinVisibility::True,vPinInBasisIndicies);
		vPinInBasisIndicies->SetSubType(0,MAX_ORDER,1,0,false,false,true);

		_boolConfigChanged=true;
	}

	void PolyValND::Configurate(IPluginConfig^ Input) 
	{	}

	void PolyValND::Evaluate(int SpreadMax) 
	{

		if (vPinDimensionsIn->PinIsChanged || vPinDimensionsOut->PinIsChanged)
		{
			changeDimensions();
			_boolConfigChanged=true;
		}

		// we should change this so poly isn't forced to evaluate
		if (_boolConfigChanged || vPinInInput->PinIsChanged || vPinInBasisIndicies->PinIsChanged || vPinInCoefficients->PinIsChanged)
		{
			evaluatePoly();
			_boolConfigChanged=false;
		}


	}	

	void PolyValND::changeDimensions()
	{
		double currentDimensionsIn, currentDimensionsOut;

		vPinDimensionsIn->GetValue(0,currentDimensionsIn);
		vPinDimensionsOut->GetValue(0,currentDimensionsOut);

		dimensionsIn = int(currentDimensionsIn);
		dimensionsOut = int(currentDimensionsOut);
	}

	void PolyValND::evaluatePoly()
	{
		double output;
		double basis;
		double coefficient;
		double *ptDataIn;
		double *ptBasesIn;

		int nDataPoints = vPinInInput->SliceCount / dimensionsIn;
		int nBases = vPinInBasisIndicies->SliceCount / dimensionsIn;
		int nDataSets;
		
		if (nBases*dimensionsOut!=0)
			nDataSets = vPinInCoefficients->SliceCount / (nBases * dimensionsOut);
		else
			nDataSets = 0;

		//temporary variables per basis inner loop
		double dataIn;
		int basisIn;

		//set output slicecount
		vPinOutOutput->SliceCount = nDataPoints * dimensionsOut * nDataSets;

		//get pointers to data
		vPinInBasisIndicies->GetValuePointer(nBases * dimensionsIn, ptBasesIn);
		vPinInInput->GetValuePointer(nDataSets * nDataPoints * dimensionsIn, ptDataIn);

		for (int iDataSet=0; iDataSet<nDataSets; ++iDataSet)
		{
			for (int iDataPoint=0; iDataPoint<nDataPoints; ++iDataPoint)
			{
				
				// render slice for each output dimension (loop through output dimensions)
				for (int iDimensionOut=0; iDimensionOut<dimensionsOut; ++iDimensionOut)
				{
					output=0;

					for (int iBasis = 0; iBasis<nBases; iBasis++)
					{
						basis=1;
						
						// build up basis (loop through input dimensions
						for (int iDimensionIn=0; iDimensionIn<dimensionsIn; ++iDimensionIn)
						{
							dataIn = *(ptDataIn + iDimensionIn + (iDataPoint*dimensionsIn));
							basisIn = *(ptBasesIn + (iBasis*dimensionsIn+iDimensionIn));
							basis *= pow(dataIn, basisIn);
						}
						
						// read in coefficients
						vPinInCoefficients->GetValue(iDataSet*nBases*dimensionsOut +
													iBasis*dimensionsOut + 
													iDimensionOut, coefficient);

						output += basis * coefficient;

					}
					
					vPinOutOutput->SetValue(dimensionsOut*nDataPoints*iDataSet +
											dimensionsOut*iDataPoint +
											iDimensionOut, output);

				}
			}
		}
	}

}