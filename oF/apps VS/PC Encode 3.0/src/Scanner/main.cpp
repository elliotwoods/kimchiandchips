#include "ofMain.h"
#include "ofVideoGrabber.h"
#include "ofAppGlutWindow.h"
#include "PCApp.h"
#include "PCconstants.h"

#include "GUIConstants.h"

//========================================================================
int main( ){

    ofAppGlutWindow window;
	ofSetupOpenGL(&window,
				  PC_SCREENS_X * PC_SCREEN_RESOLUTION_X,
				  PC_SCREENS_Y * PC_SCREEN_RESOLUTION_Y + (10 * PC_STATUS_ROWS),
				  OF_WINDOW);			// <-------- setup the GL context
	
	bool test=true;
	cout << test++ << test++ << test++ << "\n";
	
	ofRunApp(new PCApp());

}

