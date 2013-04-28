#pragma once

#include "ofMain.h"
#include<windows.h>
#include <stdlib.h>
#include<iostream>
#include<conio.h>
#include<math.h>

#include "edk.h"
#include "edkErrorCode.h"
#include "EmoStateDLL.h"

#pragma comment(lib, "lib/edk.lib")
#pragma comment(lib, "lib/glut32.lib")

#define PI 3.1418

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
