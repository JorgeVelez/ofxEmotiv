#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	// location of the machine running the "BlueGirl" head model
	std::string receiverHost = "localhost";
	
	/*if (argc > 2) {
		std::cout << "Usage: " << argv[0] << " <hostname>" << std::endl;
		std::cout << "The arguments specify the host of the head model (Default: localhost)" << std::endl;
		return 1;
	}

	if (argc > 1) {
		receiverHost = std::string(argv[1]);
	}*/

	EmoEngineEventHandle eEvent	= EE_EmoEngineEventCreate();
	EmoStateHandle eState		= EE_EmoStateCreate();
	unsigned int userID			= 0;
    const int CONTROL_PANEL_PORT = 3008;
	
	try {

		if (EE_EngineConnect() != EDK_OK) { // Connect directly to EmoEngine
        //if (EE_EngineRemoteConnect("127.0.0.1", CONTROL_PANEL_PORT)) { // Connect to Emotiv Control Panel in proxy mode
			throw std::exception("Emotiv Engine start up failed.");
		}
		else {
			std::cout << "Emotiv Engine started!" << std::endl;
		}

		int startSendPort = 30000;
		std::map<unsigned int, SocketClient> socketMap;

		std::cout << "Type \"exit\" to quit, \"help\" to list available commands..." << std::endl;
		promptUser();

		while (true) {
			
			// Handle the user input
			if (_kbhit()) {
				if (!handleUserInput()) {
					break;
				}
			}

			int state = EE_EngineGetNextEvent(eEvent);

			// New event needs to be handled
			if (state == EDK_OK) {

				EE_Event_t eventType = EE_EmoEngineEventGetType(eEvent);
				EE_EmoEngineEventGetUserId(eEvent, &userID);

				switch (eventType) {

					// New headset connected, create a new socket to send the animation
					case EE_UserAdded:
					{
						std::cout << std::endl << "New user " << userID << " added, sending Expressiv animation to ";
						std::cout << receiverHost << ":" << startSendPort << "..." << std::endl;
                        promptUser();

						socketMap.insert(std::pair<unsigned int, SocketClient>(
							userID, SocketClient(receiverHost, startSendPort, UDP)));
						
						startSendPort++;
						break;
					}
				
					// Headset disconnected, remove the existing socket
					case EE_UserRemoved:
					{
						std::cout << std::endl << "User " << userID << " has been removed." << std::endl;

                        promptUser();
						std::map<unsigned int, SocketClient>::iterator iter;
						iter = socketMap.find(userID);
						if (iter != socketMap.end()) {
							socketMap.erase(iter);
						}
						break;
					}
					
					// Send the Expressiv animation if EmoState has been updated
					case EE_EmoStateUpdated:
					{
						//std::cout << "New EmoState from user " << userID << "..." << std::endl;
						EE_EmoEngineEventGetEmoState(eEvent, eState);

						std::map<unsigned int, SocketClient>::iterator iter;
						iter = socketMap.find(userID);
						if (iter != socketMap.end()) {
							sendExpressivAnimation(iter->second, eState);
						}
						break;
					}

                    // Handle Expressiv training event
                    case EE_ExpressivEvent:
                    {
                        handleExpressivEvent(std::cout, eEvent);
                    }

					default:
						break;
				}
			}
			else if (state != EDK_NO_EVENT) {
				std::cout << std::endl << "Internal error in Emotiv Engine!" << std::endl;
				break;
			}

			Sleep(15);
		}
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::cout << "Press 'Enter' to exit..." << std::endl;
		getchar();
	}
	
}


string testApp:: numberToString(const int n) {
	char buf[80];
	_itoa(n, buf, 10);
	return string(buf);
}


void testApp::sendExpressivAnimation(SocketClient& sock, EmoStateHandle eState) {

	std::ostringstream output;

	EE_ExpressivAlgo_t upperFaceType = ES_ExpressivGetUpperFaceAction(eState);
	EE_ExpressivAlgo_t lowerFaceType = ES_ExpressivGetLowerFaceAction(eState);

	float upperFaceAmp = ES_ExpressivGetUpperFaceActionPower(eState);
	float lowerFaceAmp = ES_ExpressivGetLowerFaceActionPower(eState);

	if (ES_ExpressivIsBlink(eState)) {
		output << "B,";
	}

	if (ES_ExpressivIsLeftWink(eState)) {
		output << "l,";
	}

	if (ES_ExpressivIsRightWink(eState)) {
		output << "r,";
	}

	if (ES_ExpressivIsLookingRight(eState)) {
		output << "R,";
	}

	if (ES_ExpressivIsLookingLeft(eState)) {
		output << "L,";
	}

	if (upperFaceAmp > 0.0) {
		switch (upperFaceType) {
			case EXP_EYEBROW:	output << "b";	break;
			case EXP_FURROW:    output << "f";  break;
			default:			break;
		}
		output << numberToString(static_cast<int>(upperFaceAmp*100.0f)) << ",";
	}

	if (lowerFaceAmp > 0.0) {
		switch (lowerFaceType) {
			case EXP_CLENCH:	output << "G";	break;
			case EXP_SMILE:		output << "S";	break;
			case EXP_LAUGH:     output << "H";  break;
			case EXP_SMIRK_LEFT:  output << "sl"; break;
			case EXP_SMIRK_RIGHT: output << "sr"; break;
			default:			break;
		}
		output << numberToString(static_cast<int>(lowerFaceAmp*100.0f)) << ",";
	}

	std::string outString = output.str();

	// Remove the last comma
	if (outString.length()) {
		outString.resize(outString.length()-1);
	}

	if (!outString.length()) {
		outString = std::string("neutral");
	}

	sock.SendBytes(outString);
}


void testApp::handleExpressivEvent(std::ostream& os, EmoEngineEventHandle expressivEvent) {

	unsigned int userID = 0;
	EE_EmoEngineEventGetUserId(expressivEvent, &userID);
	EE_ExpressivEvent_t eventType = EE_ExpressivEventGetType(expressivEvent);


	switch (eventType) {

		case EE_ExpressivTrainingStarted:
		{
			os << std::endl << "Expressiv training for user " << userID << " STARTED!" << std::endl;
			break;
		}

		case EE_ExpressivTrainingSucceeded:
		{
			os << std::endl << "Expressiv training for user " << userID << " SUCCEEDED!" << std::endl;
			break;
		}

		case EE_ExpressivTrainingFailed:
		{
			os << std::endl << "Expressiv training for user " << userID << " FAILED!" << std::endl;
			break;
		}

		case EE_ExpressivTrainingCompleted:
		{
			os << std::endl << "Expressiv training for user " << userID << " COMPLETED!" << std::endl;
			break;
		}

		case EE_ExpressivTrainingDataErased:
		{
			os << std::endl << "Expressiv training data for user " << userID << " ERASED!" << std::endl;
			break;
		}

		case EE_ExpressivTrainingRejected:
		{
			os << std::endl << "Expressiv training for user " << userID << " REJECTED!" << std::endl;
			break;
		}

		case EE_ExpressivTrainingReset:
		{
			os << std::endl << "Expressiv training for user " << userID << " RESET!" << std::endl;
			break;
		}

		case EE_ExpressivNoEvent:
		default:
			//@@ unhandled case
			assert(0);
			return;
	}
    promptUser();
}


bool testApp::handleUserInput() {

	static std::string inputBuffer;

	char c = _getch();

	if (c == '\r') {
		std::cout << std::endl;
		std::string command;

		const size_t len = inputBuffer.length();
		command.reserve(len);

		// Convert the input to lower case first
		for (size_t i=0; i < len; i++) {
			command.append(1, tolower(inputBuffer.at(i)));
		}

		inputBuffer.clear();

		bool success = parseCommand(command, std::cout);
		promptUser();
		return success;
	}
	else {
		if (c == '\b') { // Backspace key
			if (inputBuffer.length()) {
				putchar(c);
				putchar(' ');
				putchar(c);
				inputBuffer.erase(inputBuffer.end()-1);
			}
		}
		else {
			inputBuffer.append(1,c);
			std::cout << c;
		}
	}	

	return true;
}

void testApp:: promptUser()
{
	std::cout << "ExpressivDemo> ";
}



//--------------------------------------------------------------
void testApp::update(){
	
}

//--------------------------------------------------------------
void testApp::draw(){
	
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