#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	 oneTime = true;
	outOfBound = false;
	currX = 0;
	currY = 0;
	maxRadius = 10000;
	xmax = 0, ymax = 0;
	preX = 0, preY = 0;
	incOrDec = 10;
	oldXVal =0, oldYVal = 0;
	pass = 0;
	globalElapsed = 0;
	count = 0;

	ofSetFrameRate(30);

	 EmoEngineEventHandle hEvent = EE_EmoEngineEventCreate();
   EmoStateHandle eState = EE_EmoStateCreate();
   unsigned int userID = -1;
   EE_EngineConnect();

    if(oneTime)
   {
      printf("Start after 2 seconds\n");
	  Sleep(4000);
	  oneTime = false;
   }

	
}

//--------------------------------------------------------------
void testApp::update(){
	//printf("Sta\n");
   int gyroX = 0,gyroY = 0;
   EE_HeadsetGetGyroDelta(0,&gyroX,&gyroY);
   xmax += gyroX;
   ymax += gyroY;

   if( outOfBound )
   {
	   if( preX != gyroX && preY != gyroY )
	   {
		   xmax = currX;
		   ymax = currY;
	   }
   }

   double val = sqrt((float)(xmax*xmax + ymax*ymax));

   if( val >= maxRadius )
   {
	  // changeXY(1);	
	   outOfBound = true;
	   preX = gyroX;
	   preY = gyroY;
   }
   else
   {		
	   outOfBound = false;
		if(oldXVal == gyroX && oldYVal == gyroY)
		{
			++count;
			if( count > 10 )
			{									
				//changeXY(0);
			}
		}
		else
		{
			count = 0;
			currX = xmax;
			currY = ymax;
			oldXVal = gyroX;
			oldYVal = gyroY;			
		}
   }
   Sleep(15);
}

//--------------------------------------------------------------
void testApp::draw(){
	ofDrawBitmapString("xmax : "+ofToString(xmax), 100,100);
	ofDrawBitmapString("ymax : "+ofToString(ymax), 100,150);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

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

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}