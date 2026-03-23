#include "ofApp.h"

using namespace lsm;

#define USE_WRITER 1
#define USE_READER 1

#ifdef USE_WRITER
inline static bool writer_enabled = true;
#else
inline static bool writer_enabled = false;
#endif // USE_WRITER

#ifdef USE_READER
inline static bool reader_enabled = true;
#else
inline static bool reader_enabled = false;
#endif // USE_READER

//--------------------------------------------------------------
void ofApp::setup(){
	ofLogToConsole();

	startedTimef = ofGetElapsedTimef();

	std::string dataToTransfer = "Hello World!";
    
	if(writer_enabled){
		writer = std::make_shared<SharedMemoryWriteStream>("strPipe", 65535, true);
		writer->write(dataToTransfer);
		ofLogNotice() << "Data wrote: " << dataToTransfer;
	}

	if(reader_enabled){

		if(checkReader()){
			std::string data = reader->readString();
			ofLogNotice() << "Data read: " << data;
			transferredData = data;
		}
	}else{
		transferredData = dataToTransfer;
	}
}

//--------------------------------------------------------------
bool ofApp::checkReader(){
	if(!reader){
		try{
			reader = std::make_shared<SharedMemoryReadStream>("strPipe", 65535, true);
			return true;
		}catch(std::exception e){
			return false;
		}

		// WORKAROUND
		return false;
	}else{
		return true;
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	elapsedTimef = ofGetElapsedTimef() - startedTimef;

	if (elapsedTimef >= 1.0){
		counter++;

		std::string dataToTransfer = "Hello World! " + ofToString(counter);

		if(writer_enabled){
			writer->write(dataToTransfer);
			ofLogNotice() << "Data wrote: " << "Hello World! " + ofToString(counter);
		}

		if(reader_enabled){
			if(checkReader()){
				std::string data = reader->readString();
				ofLogNotice() << "Data read: " << data;
				transferredData = data;
			}
		}else{
			transferredData = dataToTransfer;
		}

		

		startedTimef = ofGetElapsedTimef();
		elapsedTimef = 0.0;
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofDrawBitmapStringHighlight("Transferred data: " + transferredData, 20, 20);
	ofDrawBitmapStringHighlight("writer: " + ofToString(writer_enabled? "on" : "off"), 20, 40);
	ofDrawBitmapStringHighlight("reader: " + ofToString(reader_enabled? "on" : "off"), 20, 60);
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
