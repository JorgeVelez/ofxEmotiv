#ifndef EMOTIV
#define EMOTIV

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


class emotiv  
{    
public:	
	emotiv();
	void emotiv::init();
	void emotiv::update();
	void emotiv::draw();
	void emotiv::logEmoState(std::ostream& os, unsigned int userID, EmoStateHandle eState, bool withHeader);

	 ofEvent<EmoStateHandle> elEventoEmo;
};
#endif


