#include <iostream>
#include <conio.h>
#include <map>
#include <sstream>
#include <cassert>

#include "EmoStateDLL.h"
#include "edk.h"
#include "edkErrorCode.h"
#include "Socket.h"
#include "CognitivControl.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "lib/edk.lib")

void sendCognitivAnimation(SocketClient& sock, EmoStateHandle eState);
void handleCognitivEvent(std::ostream& os, EmoEngineEventHandle cognitivEvent);
bool handleUserInput();
void promptUser();


int main(int argc, char** argv) {

	// location of the machine running the 3D motion cube
	std::string receiverHost = "localhost";
	
	if (argc > 2) {
		std::cout << "Usage: " << argv[0] << " <hostname>" << std::endl;
		std::cout << "The arguments specify the host of the motion cube (Default: localhost)" << std::endl;
		return 1;
	}

	if (argc > 1) {
		receiverHost = std::string(argv[1]);
	}

	EmoEngineEventHandle eEvent	= EE_EmoEngineEventCreate();
	EmoStateHandle eState		= EE_EmoStateCreate();
	unsigned int userID			= 0;
	
	try {

//		if (EE_EngineConnect() != EDK_OK) {
		if (EE_EngineRemoteConnect("127.0.0.1", 3008) != EDK_OK) {
			throw std::exception("Emotiv Engine start up failed.");
		}
		else {
			std::cout << "Emotiv Engine started!" << std::endl;
		}

		int startSendPort = 20000;
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
                        std::cout << std::endl << "New user " << userID << " added, sending Cognitiv animation to ";
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
					
					// Send the Cognitiv animation if EmoState has been updated
					case EE_EmoStateUpdated:
					{
						//std::cout << "New EmoState from user " << userID << "..." << std::endl;
						EE_EmoEngineEventGetEmoState(eEvent, eState);

						std::map<unsigned int, SocketClient>::iterator iter;
						iter = socketMap.find(userID);
						if (iter != socketMap.end()) {
							sendCognitivAnimation(iter->second, eState);
						}
						break;
					}

					// Handle Cognitiv training related event
					case EE_CognitivEvent:
					{
						handleCognitivEvent(std::cout, eEvent);
						break;
					}

					default:
						break;
				}
			}
			else if (state != EDK_NO_EVENT) {
				std::cout << "Internal error in Emotiv Engine!" << std::endl;
				break;
			}

			Sleep(1);
		}
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::cout << "Press any keys to exit..." << std::endl;
		getchar();
	}

	EE_EngineDisconnect();
	EE_EmoStateFree(eState);
	EE_EmoEngineEventFree(eEvent);

	return 0;
}


void sendCognitivAnimation(SocketClient& sock, EmoStateHandle eState) {

	std::ostringstream os;

	EE_CognitivAction_t actionType	= ES_CognitivGetCurrentAction(eState);
	float				actionPower = ES_CognitivGetCurrentActionPower(eState);

	os << static_cast<int>(actionType) << "," << static_cast<int>(actionPower*100.0f);

	sock.SendBytes(os.str());
}


void handleCognitivEvent(std::ostream& os, EmoEngineEventHandle cognitivEvent) {

	unsigned int userID = 0;
	EE_EmoEngineEventGetUserId(cognitivEvent, &userID);
	EE_CognitivEvent_t eventType = EE_CognitivEventGetType(cognitivEvent);


	switch (eventType) {

		case EE_CognitivTrainingStarted:
		{
			os << std::endl << "Cognitiv training for user " << userID << " STARTED!" << std::endl;
			break;
		}

		case EE_CognitivTrainingSucceeded:
		{
			os << std::endl << "Cognitiv training for user " << userID << " SUCCEEDED!" << std::endl;
			break;
		}

		case EE_CognitivTrainingFailed:
		{
			os << std::endl << "Cognitiv training for user " << userID << " FAILED!" << std::endl;
			break;
		}

		case EE_CognitivTrainingCompleted:
		{
			os << std::endl << "Cognitiv training for user " << userID << " COMPLETED!" << std::endl;
			break;
		}

		case EE_CognitivTrainingDataErased:
		{
			os << std::endl << "Cognitiv training data for user " << userID << " ERASED!" << std::endl;
			break;
		}

		case EE_CognitivTrainingRejected:
		{
			os << std::endl << "Cognitiv training for user " << userID << " REJECTED!" << std::endl;
			break;
		}

		case EE_CognitivTrainingReset:
		{
			os << std::endl << "Cognitiv training for user " << userID << " RESET!" << std::endl;
			break;
		}

		case EE_CognitivAutoSamplingNeutralCompleted:
		{
			os << std::endl << "Cognitiv auto sampling neutral for user " << userID << " COMPLETED!" << std::endl;
			break;
		}

		case EE_CognitivSignatureUpdated:
		{
			os << std::endl << "Cognitiv signature for user " << userID << " UPDATED!" << std::endl;
			break;
		}

		case EE_CognitivNoEvent:
			break;

		default:
			//@@ unhandled case
			assert(0);
			break;
	}
    promptUser();
}


bool handleUserInput() {

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

void promptUser()
{
	std::cout << "CognitivDemo> ";
}
