#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofLogToConsole();

	startedTimef = ofGetElapsedTimef();

	std::string dataToTransfer = "Hello World!";
    
	writer = std::make_shared<ofxSharedMemory::Writer>("strPipe", 65535, false);
	writer->write(dataToTransfer);
	ofLogNotice() << "Data wrote: " << dataToTransfer;

	reader = std::make_shared<ofxSharedMemory::Reader>("strPipe", 65535, false);
	std::string data = reader->readString();
	ofLogNotice() << "Data read: " << data;

	transferredData = data;
}

//--------------------------------------------------------------
void ofApp::update(){
	elapsedTimef = ofGetElapsedTimef() - startedTimef;

	if (elapsedTimef >= 1.0){
		counter++;
		writer->write("Hello World! " + ofToString(counter));
		ofLogNotice() << "Data wrote: " << "Hello World! " + ofToString(counter);

		std::string data = reader->readString();
		ofLogNotice() << "Data read: " << data;

		transferredData = data;

		startedTimef = ofGetElapsedTimef();
		elapsedTimef = 0.0;
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofDrawBitmapStringHighlight("Transferred data: " + transferredData, 20, 20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
