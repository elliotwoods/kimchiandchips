
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



		FHost->CreateValueOutput("Output",4,arr,TSliceMode::Dynamic,TPinVisibility::True,this->vPinOutOutput);
		FHost->CreateValueOutput("Sucess",1,arr,TSliceMode::Single,TPinVisibility::True,this->vPinOutSuccess);
		vPinOutSuccess->SetSubType(0,1,1,0,false,false,true);
	}

	void PadeValue::Configurate(IPluginConfig^ Input) 
	{	}

	void PadeValue::Evaluate(int SpreadMax) 
	{
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
		_hasSuccess=true;

		// --------------
		// FILL VECTORS
		// --------------

		_nDataPoints = min(vPinInWorld->SliceCount, vPinInProjection->SliceCount);

		vector<vector<double> > vecDataInX, vecDataOutX;
		vector<vector<double> > vecDataInY, vecDataOutY;
		vector<double> vecDummy;


		vecDataInX.clear();
		vecDataOutX.clear();
		
		vecDataInY.clear();
		vecDataOutY.clear();

		vecDummy.resize(4);
		for (int iDataPoint = 0; iDataPoint<_nDataPoints; iDataPoint++)
		{
			vecDataInX.push_back(vecDummy);
			vecDataInY.push_back(vecDummy);
		}

		vecDummy.resize(1);
		for (int iDataPoint = 0; iDataPoint<_nDataPoints; iDataPoint++)
		{
			vecDataOutX.push_back(vecDummy);
			vecDataOutY.push_back(vecDummy);
		}

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
		
		try {
			if (_nDataPoints>0)
			{
				_fitX->init(vecDataInX, vecDataOutX,  _nDataPoints);
				_fitY->init(vecDataInY, vecDataOutY, _nDataPoints);
			} else
				throw("No data points");
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
			vPinOutOutput->SliceCount=4;

		double *coefficientsX = new double[7];
		double *coefficientsY = new double[7];

		int nBases; // dummy value

		_fitX->solve(0, coefficientsX, nBases);
		_fitY->solve(0, coefficientsY, nBases);

		// ----------------------------
		// OUTPUT
		// ----------------------------
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

	}

}