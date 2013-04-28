#pragma once

#include "ofMain.h"
#include <iostream>
#include <conio.h>
#include <map>
#include <sstream>
#include <cassert>

#include "EmoStateDLL.h"
#include "edk.h"
#include "edkErrorCode.h"
#include "Socket.h"
#include "ExpressivControl.h"

#pragma comment(lib, "Ws2_32.lib")
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

		string testApp::numberToString(const int n);
		void testApp:: sendExpressivAnimation(SocketClient& sock, EmoStateHandle eState);
		void testApp:: handleExpressivEvent(std::ostream& os, EmoEngineEventHandle expressivEvent);
		bool testApp:: handleUserInput();
		void testApp:: promptUser();
		
};
