#ifndef _PCApp
#define _PCApp


#include "ofMain.h"
#include "PCconstants.h"
#include "PCAssets.h"
#include "PCConfig.h"
#include "PCEncode.h"
#include "PCDecode.h"
#include "PCLogger.h"
#include "GUI.h"
#include "PayloadGraycode.h"
#include "PayloadBinary.h"

class PCApp : public ofBaseApp, public PCConfig, public PCAssets {

	public:
		PCApp();
		~PCApp();
		void load();
		void initialise();
		void uninitialse();
	
		void setup();
		void update();
		void draw();
		void drawStatus();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button); 
		void mouseReleased(int x, int y, int button);

		void mouseDown(int x, int y); // new function. is called whenever mouse is down / moved when down
	
		void windowResized(int w, int h);
	
	private:
		bool				_isInitialised;
	
		//legacy stuff. maybe move this to screens?
		int					_MouseStartX, _MouseStartY;
		float				_MouseStartScreenFloatX, _MouseStartScreenFloatY;
		int					_MouseStartScreenPixX, _MouseStartScreenPixY;

		PCLogger			*_logger;
		PCEncode			*_encoder;
		PCDecode			*_decoder;
		GUI					*_screens;
		PayloadBase			*_payload;

	
		Camera				*_camera;

		int					_PC_State;

		int					_iScanFrame;
		int					_iCalibrationFrame;
		
		int					_lastCaptureTime;
	
		string				_strStatus;
		char				_strDebug[200];
		string				_strHistograms;
	

		//void				updateHistograms(float xX, float xY);
		
		bool				hasWaitedForCapture();
		void				captureFrame(int iFrame, int nFrames), sendFrame(int iFrame);

		int					iScanInterleaveFrame(int iScanFrame);
		int					iInterleave(int iScanFrame);

	
};

#endif
