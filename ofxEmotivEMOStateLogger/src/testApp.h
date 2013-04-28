#pragma once

#include "ofMain.h"
#include <iostream>
#include <fstream>
#include <conio.h>
#include <sstream>
#include <windows.h>
#include <map>

#include "edk.h"
#include "edkErrorCode.h"
#include "EmoStateDLL.h" 
 
#pragma comment(lib, "lib/edk.lib")


class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void logEmoState(std::ostream& os, unsigned int userID, EmoStateHandle eState, bool withHeader = false);

		bool oneTime ;
		bool outOfBound;
		float currX ,currY;
		double maxRadius;
		float xmax , ymax ;
		float preX , preY ;
		int incOrDec ;
		float oldXVal, oldYVal ;
		unsigned long pass ,globalElapsed ;
		int count ;
		float x;
		
};
