
//
//
//		P O L F I T
//
//

#pragma once

#include "polyNfit.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace VVVV::PluginInterfaces::V1;
using namespace std;

namespace PolyFitND {

	public ref class PolyFitND :IPlugin
	{
		private:
			IPluginHost^ FHost;
			
			/////////////////////////////////////
			// PINS
			//
			IValueIn^			vPinInX;
			IValueIn^			vPinInXDash;
		
			IValueIn^			vPinInOrder;
			IValueIn^			vPinDimensionsIn;
			IValueIn^			vPinDimensionsOut;

			IValueOut^			vPinOutCoefficients;
			IValueOut^			vPinOutBasisIndicies;
			IStringOut^			vPinOutMessage;
			/////////////////////////////////////

			/////////////////////////////////////
			// VARIABLES
			//
			bool			_boolFitCreated;
			bool			_boolConfigChanged;

			int				_nDataSets;
			int				_nDataPoints;
			int				order;
			int				dimensionsIn, dimensionsOut;
			/////////////////////////////////////
			
			/////////////////////////////////////
			// CLASSES
			polyNfit		*_fit;
			/////////////////////////////////////


			/////////////////////////////////////
			// FUNCTIONS
			void			changeDimensions();
			void			changeOrder();

			void			evalPoly();
			void			returnBasisIndicies();

			/////////////////////////////////////

		public:
			PolyFitND();
			~PolyFitND();

			static property IPluginInfo^ PluginInfo 
			{
				IPluginInfo^ get() 
				{
					//IPluginInfo^ Info;
					IPluginInfo^ Info = gcnew VVVV::PluginInterfaces::V1::PluginInfo();
					Info->Name = "PolyFitND";
					Info->Category = "Value";
					Info->Version = "";
					Info->Help = "N dimensional power series polynomial fit with triangular bases";
					Info->Bugs = "";
					Info->Credits = "";
					Info->Warnings = "";

					//leave below as is
					System::Diagnostics::StackTrace^ st = gcnew System::Diagnostics::StackTrace(true);
					System::Diagnostics::StackFrame^ sf = st->GetFrame(0);
					System::Reflection::MethodBase^ method = sf->GetMethod();
					Info->Namespace = method->DeclaringType->Namespace;
					Info->Class = method->DeclaringType->Name;
					return Info;
				}
			}
			virtual void SetPluginHost(IPluginHost^ Host);
			virtual void Configurate(IPluginConfig^ Input);
			virtual void Evaluate(int SpreadMax);
			
			virtual property bool AutoEvaluate {
				bool get() { return false; }
			}


	};
}
