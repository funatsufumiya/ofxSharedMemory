#pragma once

#include "ofMain.h"

#include "ofxSharedMemory.h"

class ofApp : public ofBaseApp{

	public:
		std::shared_ptr<ofxSharedMemory::Reader> reader;
		std::shared_ptr<ofxSharedMemory::Writer> writer;

		std::string transferredData;

		float startedTimef = 0.0f;
		float elapsedTimef = 0.0f;

		int counter = 0;

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
};
