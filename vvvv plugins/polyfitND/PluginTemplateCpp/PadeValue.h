
//
//
//		P A D E ( V a l u e )
//
//

//		vvvv plugin

#pragma once

using namespace System;
using namespace VVVV::PluginInterfaces::V1;
using namespace std;
using namespace VVVV::Utils::VMath;

#include <math.h>
#include "polyNfit.h"

namespace PolyFitND {


	public ref class PadeValue :IPlugin
	{
		private:
			IPluginHost^ FHost;
			
			/////////////////////////////////////
			// PINS
			//
			
			IValueIn^			vPinInWorld;
			IValueIn^			vPinInProjection;

			IValueIn^			vPinInOrder;

			IValueOut^			vPinOutOutput;
			IValueOut^			vPinOutSuccess;
			
			/////////////////////////////////////

			/////////////////////////////////////
			// VARIABLES
			//
			bool			_hasSuccess;

			int				_nDataPoints;
			int				_order;

			String^			_debugMessage;
			/////////////////////////////////////
			
			/////////////////////////////////////
			// CLASSES
			polyNfit		*_fitX, *_fitY;
			/////////////////////////////////////


			/////////////////////////////////////
			// FUNCTIONS
			void			evalPoly();
			/////////////////////////////////////

		public:
			PadeValue();
			~PadeValue();

			static property IPluginInfo^ PluginInfo 
			{
				IPluginInfo^ get() 
				{
					//IPluginInfo^ Info;
					IPluginInfo^ Info = gcnew VVVV::PluginInterfaces::V1::PluginInfo();
					Info->Name = "Pade";
					Info->Category = "Value";
					Info->Version = "";
					Info->Help = "Pade approximate projection matrix generator";
					Info->Bugs = "";
					Info->Credits = "Elliot Woods, Daniel Tang";
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
