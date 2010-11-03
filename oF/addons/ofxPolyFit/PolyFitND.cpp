
//
//
//		P O L F I T
//
//

#include "stdafx.h"

#include "PolyFitND.h"

using namespace System;
using namespace VVVV::PluginInterfaces::V1;
using namespace std;

namespace PolyFitND 
{

	PolyFitND::PolyFitND()
	{
		_boolConfigChanged=true;

	}

	PolyFitND::~PolyFitND()
	{
		delete _fit;
	}


	void PolyFitND::SetPluginHost(IPluginHost ^ Host) 
	{
		Host->Log(TLogType::Debug,"Here");
		this->FHost = Host;

		array<String ^> ^ arr = gcnew array<String ^>(1);

		FHost->CreateValueInput("x ",1,arr,TSliceMode::Dynamic,TPinVisibility::True,this->vPinInX);
		FHost->CreateValueInput("x' ",1,arr,TSliceMode::Dynamic,TPinVisibility::True,this->vPinInXDash);
		
		

		FHost->CreateValueInput("Order",1,arr,TSliceMode::Single,TPinVisibility::True,this->vPinInOrder);
		FHost->CreateValueInput("Input Dimensions", 1, arr, TSliceMode::Dynamic, TPinVisibility::True,this->vPinDimensionsIn);
		FHost->CreateValueInput("Output Dimensions", 1, arr, TSliceMode::Dynamic, TPinVisibility::True,this->vPinDimensionsOut);
		vPinInOrder->SetSubType(1,MAX_ORDER,1,3,false,false,true);
		vPinDimensionsIn->SetSubType(1,MAX_DIMENSIONS,1,3,false,false,true);
		vPinDimensionsOut->SetSubType(1,MAX_DIMENSIONS,1,3,false,false,true);

		FHost->CreateValueOutput("Coefficients",1,arr,TSliceMode::Dynamic,TPinVisibility::True,this->vPinOutCoefficients);
		FHost->CreateValueOutput("Bases indicies",1,arr,TSliceMode::Dynamic,TPinVisibility::True,vPinOutBasisIndicies);
		vPinOutBasisIndicies->SetSubType(0,MAX_ORDER,1,0,false,false,true);

		FHost->CreateStringOutput("Message",TSliceMode::Single,TPinVisibility::True,this->vPinOutMessage);
	}

	void PolyFitND::Configurate(IPluginConfig^ Input) 
	{	}

	void PolyFitND::Evaluate(int SpreadMax) 
	{
		
		if (vPinDimensionsIn->PinIsChanged || vPinDimensionsOut->PinIsChanged)
		{
			_boolConfigChanged=true;
			changeDimensions();
		}
		
		if (vPinInOrder->PinIsChanged)
		{
			_boolConfigChanged=true;
			changeOrder();
		}

		if (_boolConfigChanged || vPinInX->PinIsChanged || vPinInXDash->PinIsChanged)
		{
			_boolConfigChanged=false;
			evalPoly();
		}

		//move this into the change bit?
		returnBasisIndicies();


	}	

	void PolyFitND::changeDimensions()
	{
		if (_boolFitCreated)
		{			
			delete _fit;
		}

		double currentDimensionsIn, currentDimensionsOut;
		double currentOrder;

		vPinDimensionsIn->GetValue(0,currentDimensionsIn);
		vPinDimensionsOut->GetValue(0,currentDimensionsOut);
		vPinInOrder->GetValue(0,currentOrder);

		dimensionsIn = int(currentDimensionsIn);
		dimensionsOut = int(currentDimensionsOut);
		order = int(currentOrder);

		_fit = new polyNfit(order, dimensionsIn, dimensionsOut);
		
		_boolFitCreated=true;
	}

	void PolyFitND::changeOrder()
	{

		double currentOrder;
		vPinInOrder->GetValue(0,currentOrder);
		order=int(currentOrder);

		delete _fit;
		_fit = new polyNfit(order,dimensionsIn,dimensionsOut);

		returnBasisIndicies();
	}

	void PolyFitND::evalPoly()
	{
		
		// reset error message
		vPinOutMessage->SetString(0, "");


		// --------------
		// FILL VECTORS
		// --------------

		_nDataPoints = vPinInX->SliceCount  / dimensionsIn;
		if (_nDataPoints>0)
			_nDataSets = max(vPinInXDash->SliceCount / (_nDataPoints*dimensionsOut), 1);
		else
			_nDataSets = 0;

		vector <vector<vector<double> > > vecSetDataIn;
		vector <vector<vector<double> > > vecSetDataOut;
	
		vector<vector<double> > vecDataIn, vecDataOut;
		vector<double> vecDummy;

		for (int iDataSet=0; iDataSet < _nDataSets; iDataSet++)
		{
			vecDataIn.clear();
			vecDataOut.clear();

			vecDummy.resize(dimensionsIn);
			for (int iDataPoint = 0; iDataPoint<_nDataPoints; iDataPoint++)
			{
				vecDataIn.push_back(vecDummy);
			}

			vecDummy.resize(dimensionsOut);
			for (int iDataPoint = 0; iDataPoint<_nDataPoints; iDataPoint++)
			{
				vecDataOut.push_back(vecDummy);
			}

			for (int iDataPoint = 0; iDataPoint<_nDataPoints; iDataPoint++)
			{
				for (int iDimension=0; iDimension<dimensionsIn; iDimension++)
					vPinInX->GetValue(iDataSet*_nDataPoints*dimensionsIn +
									iDataPoint*dimensionsIn +
									iDimension,
									vecDataIn[iDataPoint][iDimension]);

				for (int iDimension=0; iDimension<dimensionsOut; iDimension++)
					vPinInXDash->GetValue(iDataSet*_nDataPoints*dimensionsOut +
									iDataPoint*dimensionsOut +
									iDimension,
									vecDataOut[iDataPoint][iDimension]);

			}

			vecSetDataIn.push_back(vecDataIn);
			vecSetDataOut.push_back(vecDataOut);

		}
		try {
			if (_nDataSets>0 && _nDataPoints>0)
				_fit->init(vecSetDataIn[0], vecSetDataIn[0], _nDataPoints);
		}

		catch(char * message)
		{
			String^ clistr = gcnew String(message);
			vPinOutMessage->SetString(0, clistr);
		}

		// ----------------------------
		// RETURN COEFFICIENTS
		// ----------------------------
		// would prefer to split this into 2 functions
		// but managed c++ wont let me have vector as a member of the class
		// so need to transfer the data across internal to the function
		// or fiddle with using lists and converting back to vectors to pass to poly class


		double *coefficients;
		int nBases;
		int outputOffset=0;
		double *vOutPointer;
		int nCoefficients = _fit->nBases() * dimensionsOut; // per dataset

		coefficients = new double[nCoefficients]; //set to maximum size of coefficients (not truncated)
																	// this is coefficients per data set

		vPinOutCoefficients->SliceCount = nCoefficients * _nDataSets;

		for (int iDataSet=0; iDataSet < _nDataSets; iDataSet++)
		{
			for (int iDimensionOut=0; iDimensionOut < dimensionsOut; iDimensionOut++)
			{
				vPinOutCoefficients->GetValuePointer(vOutPointer);
				_fit->init(vecSetDataIn[iDataSet], vecSetDataOut[iDataSet], _nDataPoints);
				_fit->solve(iDimensionOut, coefficients + (iDimensionOut*nBases), nBases);
				//_fit->solve(iDimensionOut, vOutPointer + (iDimensionOut*nCoefficients), nCoefficients);

				for (int iBasis=0; iBasis < nBases; ++iBasis)
					vOutPointer[iDataSet * nCoefficients +
								iBasis * dimensionsOut +
								iDimensionOut] = coefficients[iDimensionOut*nBases + iBasis];
			}
		}
	}

	void PolyFitND::returnBasisIndicies()
	{
		double *vOutPointer;
		int nBasisIndicies = _fit->vecBasisIndicies.size();
		
		vPinOutBasisIndicies->SliceCount = dimensionsIn * nBasisIndicies;
		

		for (int iIndicies = 0; iIndicies < nBasisIndicies; ++iIndicies)
		{
			for (int iDimension = 0; iDimension < dimensionsIn; ++iDimension)
			{
				vPinOutBasisIndicies->SetValue(iIndicies*dimensionsIn+iDimension,
										_fit->vecBasisIndicies[iIndicies][iDimension]);
			}
		}
	}

}