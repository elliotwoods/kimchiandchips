
//
//
//		P O L Y A L
//
//

#pragma once

using namespace System;
using namespace VVVV::PluginInterfaces::V1;
using namespace std;

namespace PolyFitND {

#include <math.h>

	public ref class PolyValND :IPlugin
	{
		private:
			IPluginHost^ FHost;
			
			/////////////////////////////////////
			// PINS
			//
			
			IValueIn^			vPinInInput;

			IValueIn^			vPinInCoefficients;
			IValueIn^			vPinInBasisIndicies;
			
			IValueIn^			vPinDimensionsIn;
			IValueIn^			vPinDimensionsOut;

			IValueOut^			vPinOutOutput;
			
			/////////////////////////////////////

			/////////////////////////////////////
			// VARIABLES
			//
			bool			_boolConfigChanged;
			int				dimensionsIn, dimensionsOut;
			/////////////////////////////////////
			
			/////////////////////////////////////
			// CLASSES
			/////////////////////////////////////


			/////////////////////////////////////
			// FUNCTIONS
			void			changeDimensions();

			void			evaluatePoly();
			/////////////////////////////////////

		public:
			PolyValND();
			~PolyValND();

			static property IPluginInfo^ PluginInfo 
			{
				IPluginInfo^ get() 
				{
					//IPluginInfo^ Info;
					IPluginInfo^ Info = gcnew VVVV::PluginInterfaces::V1::PluginInfo();
					Info->Name = "PolyValND";
					Info->Category = "Value";
					Info->Version = "";
					Info->Help = "Companion to PolyFitND";
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
