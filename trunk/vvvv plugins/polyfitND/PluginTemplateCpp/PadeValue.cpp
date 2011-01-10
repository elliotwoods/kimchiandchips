
//
//
//		P A D E ( V a l u e )
//
//

//		vvvv plugin

#include "Stdafx.h"

#include "PadeValue.h"

using namespace System;
using namespace VVVV::PluginInterfaces::V1;
using namespace std;

namespace PolyFitND 
{

	PadeValue::PadeValue()
	{
		_hasSuccess=false;
		_debugMessage = gcnew String("");

		_fitX = new polyNfit(1, 4, 1, 3);
		_fitY = new polyNfit(1, 4, 1, 3);
	}

	PadeValue::~PadeValue()
	{
		delete _fitX, _fitY;
	}


	void PadeValue::SetPluginHost(IPluginHost ^ Host) 
	{
		this->FHost = Host;

		array<String ^> ^ arr = gcnew array<String ^>(1);

		FHost->CreateValueInput("World",3,arr,TSliceMode::Dynamic,TPinVisibility::True,this->vPinInWorld);
		FHost->CreateValueInput("Projection",2,arr,TSliceMode::Dynamic,TPinVisibility::True,this->vPinInProjection);
		
		FHost->CreateValueInput("Order", 1, arr, TSliceMode::Single, TPinVisibility::True, this->vPinInOrder);
		vPinInOrder->SetSubType(1, 2, 1, 1, false, false, true);


		FHost->CreateValueOutput("Output",1,arr,TSliceMode::Dynamic,TPinVisibility::True,this->vPinOutOutput);
		FHost->CreateValueOutput("Sucess",1,arr,TSliceMode::Single,TPinVisibility::True,this->vPinOutSuccess);
		vPinOutSuccess->SetSubType(0,1,1,0,false,false,true);
	}

	void PadeValue::Configurate(IPluginConfig^ Input) 
	{	}

	void PadeValue::Evaluate(int SpreadMax) 
	{
		if (vPinInOrder->PinIsChanged)
		{
			double currentOrder;
			vPinInOrder->GetValue(0, currentOrder);
			currentOrder = (currentOrder > 2 ? 2 :
				(currentOrder < 1 ? 1 : currentOrder));

			_order = currentOrder;

			delete _fitX;
			delete _fitY;

			_fitX = new polyNfit(1, 4, 1, 2 + _order);
			_fitY = new polyNfit(1, 4, 1, 2 + _order);

			_hasSuccess=false;
			evalPoly();
		}

		if (vPinInWorld->PinIsChanged || vPinInProjection->PinIsChanged)
		{
			_hasSuccess=false;
			evalPoly();
		}

		vPinOutSuccess->SetValue(0, (_hasSuccess ? 1 : 0));
	}

	void PadeValue::evalPoly()
	{
		_debugMessage = "";
		_hasSuccess=false;

		// --------------
		// FILL VECTORS
		// --------------

		_nDataPoints = min(vPinInWorld->SliceCount, vPinInProjection->SliceCount);

		if (_nDataPoints==0)
		{
			_debugMessage = gcnew String("No data points");
			_hasSuccess = false;
			return;
		}

		vector<vector<double> > vecDataInX, vecDataOutX;
		vector<vector<double> > vecDataInY, vecDataOutY;
		vector<double> vecDummy;


		vecDataInX.clear();
		vecDataOutX.clear();
		
		vecDataInY.clear();
		vecDataOutY.clear();

		/////////////////////////////
		// Build up the blank vectors
		/////////////////////////////
		vecDummy.resize(4);
		for (int iDataPoint = 0; iDataPoint<_nDataPoints; iDataPoint++)
		{
			vecDataInX.push_back(vecDummy);
			vecDataInY.push_back(vecDummy);
		}
		//
		vecDummy.resize(1);
		for (int iDataPoint = 0; iDataPoint<_nDataPoints; iDataPoint++)
		{
			vecDataOutX.push_back(vecDummy);
			vecDataOutY.push_back(vecDummy);
		}
		/////////////////////////////

		for (int iDataPoint = 0; iDataPoint<_nDataPoints; iDataPoint++)
		{
			////////////////////////////
			// make the 4-vector inputs
			//
			//xyz
			for (int iInDim=0; iInDim<3; iInDim++)
			{
				vPinInWorld->GetValue(iDataPoint*3 +
									iInDim,
									vecDataInX[iDataPoint][iInDim]);
				vecDataInY[iDataPoint][iInDim] = vecDataInX[iDataPoint][iInDim];
			}

			//x'
			vPinInProjection->GetValue(iDataPoint*2,
										vecDataInX[iDataPoint][3]);
			vPinInProjection->GetValue(iDataPoint*2+1,
										vecDataInY[iDataPoint][3]);
			//
			////////////////////////////

			
			////////////////////////////
			// make the 1-vector outputs
			//
			double x_dash, y_dash;

			vPinInProjection->GetValue(iDataPoint*2, x_dash);
			vPinInProjection->GetValue(iDataPoint*2+1, y_dash);
			
			vecDataOutX[iDataPoint][0] = -x_dash;
			vecDataOutY[iDataPoint][0] = -y_dash;
			////////////////////////////

		}

		// ----------------------------
		// RUN FIT
		// ----------------------------
		
		int nCoefficients = (_order==1 ? 7 : 11);
		try {
			if (_nDataPoints>0)
			{
				_fitX->init(vecDataInX, vecDataOutX,  _nDataPoints);
				_fitY->init(vecDataInY, vecDataOutY, _nDataPoints);
				_hasSuccess = true;
			} else
				throw("No data points");

			if (_nDataPoints< nCoefficients)
				throw("Insufficient data points for full fit");
		}

		catch(char * message)
		{
			_debugMessage = gcnew String(message);
			_hasSuccess = false;
		}
		



		// ----------------------------
		// SOLVE
		// ----------------------------

		if (!_hasSuccess)
		{
			vPinOutOutput->SliceCount=0;
			return;
		} else
			vPinOutOutput->SliceCount=(nCoefficients+1)*2;

		double *coefficientsX = new double[nCoefficients];
		double *coefficientsY = new double[nCoefficients];

		int nBases; // dummy value

		_fitX->solve(0, coefficientsX, nBases);
		_fitY->solve(0, coefficientsY, nBases);

		// ----------------------------
		// OUTPUT
		// ----------------------------
		if (_order==1)
		{
			double Output[18];

			Output[0] = -coefficientsX[3];
			Output[1] = -coefficientsX[4];
			Output[2] = -coefficientsX[5];
			Output[3] = -coefficientsX[6];

			Output[4] = coefficientsX[0];
			Output[5] = coefficientsX[1];
			Output[6] = coefficientsX[2];
			Output[7] = 1;

			Output[8] = -coefficientsY[3];
			Output[9] = -coefficientsY[4];
			Output[10] = -coefficientsY[5];
			Output[11] = -coefficientsY[6];

			Output[12] = coefficientsY[0];
			Output[13] = coefficientsY[1];
			Output[14] = coefficientsY[2];
			Output[15] = 1;



			for (int iCoeff=0; iCoeff<16; iCoeff++)
				vPinOutOutput->SetValue(iCoeff, Output[iCoeff]);
		} else if (_order==2) {
			double Output[26];

			double *out = Output;
			double *coefficients;
			for (int iDim=0; iDim<2 ; iDim++)
			{
				coefficients = (iDim==0 ? coefficientsX : coefficientsY);

				//1st order
				*out++ = -coefficients[3];
				*out++ = -coefficients[4];
				*out++ = -coefficients[5];

				//2nd order
				*out++ = -coefficients[6];
				*out++ = -coefficients[7];
				*out++ = -coefficients[8];
				*out++ = -coefficients[9];

				//0th order
				*out++ = -coefficientsX[10];
				
				//denominator
				*out++ = coefficientsX[0];
				*out++ = coefficientsX[1];
				*out++ = coefficientsX[2];
				*out++ = 1;
			}

			for (int iCoeff=0; iCoeff<26; iCoeff++)
				vPinOutOutput->SetValue(iCoeff, Output[iCoeff]);
		}

		_hasSuccess = true;

	}

}