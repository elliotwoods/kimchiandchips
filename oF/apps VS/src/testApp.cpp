#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){	
	ofSetFrameRate(60);

	imgTest.loadImage("t-shirt.png");
	w = imgTest.getWidth();
	h = imgTest.getHeight();
	memShare.init("texSend", w*h*3);
}
 

//--------------------------------------------------------------
void testApp::update(){
	memShare.setData(this->imgTest.getPixels(), w*h*3);
}

//--------------------------------------------------------------
void testApp::draw(){

	imgTest.draw(100,100);

	ofPushStyle();
	ofSetColor(0,0,0);
	ofDrawBitmapString(ofToString(ofGetFrameRate(),0),10,10);
	ofDrawBitmapString(ofToString(w,0) + "," + ofToString(h,0),10,30);
	ofPopStyle();


}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){ 
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

	unsigned char blue = float(x)/ofGetWidth() * 255.0f;

	unsigned char * imgPixels = this->imgTest.getPixels();
	for (int i=0; i<w*h; i++)
		imgPixels[i*3+2] = blue;

	imgTest.update();
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

