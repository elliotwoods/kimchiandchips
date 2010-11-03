
//
//
//		P A D E (T r a n s f o r m)
//
//

//		vvvv plugin

#include "Stdafx.h"

#include "PadeTransform.h"

using namespace System;
using namespace VVVV::PluginInterfaces::V1;
using namespace std;

namespace PolyFitND 
{

	PadeTransform::PadeTransform()
	{
		_hasSuccess=false;
		_debugMessage = gcnew String("");

		_fitX = new polyNfit(1, 4, 1, 3);
		_fitY = new polyNfit(1, 4, 1, 3);
	}

	PadeTransform::~PadeTransform()
	{
		delete _fitX, _fitY;
	}


	void PadeTransform::SetPluginHost(IPluginHost ^ Host) 
	{
		this->FHost = Host;

		array<String ^> ^ arr = gcnew array<String ^>(1);

		FHost->CreateValueInput("World",3,arr,TSliceMode::Dynamic,TPinVisibility::True,this->vPinInWorld);
		FHost->CreateValueInput("Projection",2,arr,TSliceMode::Dynamic,TPinVisibility::True,this->vPinInProjection);



		FHost->CreateTransformOutput("Transform",TSliceMode::Dynamic,TPinVisibility::True,this->vPinOutTransform);

		FHost->CreateValueOutput("Sucess",1,arr,TSliceMode::Single,TPinVisibility::True,this->vPinOutSuccess);
		vPinOutSuccess->SetSubType(0,1,1,0,false,false,true);
	}

	void PadeTransform::Configurate(IPluginConfig^ Input) 
	{	}

	void PadeTransform::Evaluate(int SpreadMax) 
	{
		if (vPinInWorld->PinIsChanged || vPinInProjection->PinIsChanged)
		{
			_hasSuccess=false;
			evalPoly();
		}

		vPinOutSuccess->SetValue(0, (_hasSuccess ? 1 : 0));
	}

	void PadeTransform::evalPoly()
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
		// BUILD TRANSFORM MATRIX
		// ----------------------------

		if (!_hasSuccess)
		{
			vPinOutTransform->SliceCount=0;
			return;
		} else
			vPinOutTransform->SliceCount=1;

		double *coefficientsX = new double[7];
		double *coefficientsY = new double[7];

		int nBases; // dummy value

		_fitX->solve(0, coefficientsX, nBases);
		_fitY->solve(0, coefficientsY, nBases);

		//build w
		double wCoefficients[4];
		//wCoefficients[0] = (coefficientsX[0] + coefficientsY[0]) / 2;
		//wCoefficients[1] = (coefficientsX[1] + coefficientsY[1]) / 2;
		//wCoefficients[2] = (coefficientsX[2] + coefficientsY[2]) / 2;
		wCoefficients[0] = coefficientsX[0];
		wCoefficients[1] = coefficientsX[1];
		wCoefficients[2] = coefficientsX[2];
		wCoefficients[3] = 1;


		Matrix4x4 Output;

		Output.m11 = -coefficientsX[3];
		Output.m21 = -coefficientsX[4];
		Output.m31 = -coefficientsX[5];
		Output.m41 = -coefficientsX[6];

		Output.m12 = -coefficientsY[3];
		Output.m22 = -coefficientsY[4];
		Output.m32 = -coefficientsY[5];
		Output.m42 = -coefficientsY[6];

		Output.m14 = wCoefficients[0];
		Output.m24 = wCoefficients[1];
		Output.m34 = wCoefficients[2];
		Output.m44 = wCoefficients[3];
		// END BUILD TRANSFORM MATRIX
		// ----------------------------

		// ----------------------------
		// BUILD Z
		// ----------------------------
		//
		// map w onto z, with a scale/translate factor
		// that brings z into range -1 -> 1 for all points
		double zCoefficients[4];

		// the resulting z after matrix multiply, is divided by w
		// so if we presume z could approximately be w....
		Output.m13 = 0;
		Output.m23 = 0;
		Output.m33 = 0;
		Output.m43 = -0.2;

		/*
		// then evaluate range offset
		double zRange[2];
		Vector4D currentPoint;
		for (int iPoint=0; iPoint<_nDataPoints; iPoint++)
		{
			vPinInWorld->GetValue3D(iPoint,
				currentPoint.x,
				currentPoint.y,
				currentPoint.z);

			currentPoint.w = 1;
			currentPoint = Output * currentPoint;

			if (iPoint==0 || currentPoint.z < zRange[0])
				zRange[0] = currentPoint.z;
			if (iPoint==0 || currentPoint.z > zRange[1])
				zRange[1] = currentPoint.z;
		}

		double factor = 1 / (zRange[1] - zRange[0]);
		double offset = - zRange[0] - 1;

		factor *= -1;
		offset = -1;

		//Output.m13 *= factor;
		//Output.m23 *= factor;
		//Output.m33 *= factor;
		//Output.m43 = offset;
		*/

		// END BUILD Z
		// ----------------------------

		vPinOutTransform->SetMatrix(0, Output);
	}

}