#include <map>
#include <cassert>
#include "edk.h"
#include "edkErrorCode.h"
#include "CognitivControl.h"

class _Cognitiv_ {
	std::map<EE_CognitivAction_t, std::string> _actionMap;
public:
	_Cognitiv_() {
		_actionMap[COG_NEUTRAL					] = "neutral";
		_actionMap[COG_PUSH						] = "push";
		_actionMap[COG_PULL						] = "pull";
		_actionMap[COG_LIFT						] = "lift";
		_actionMap[COG_DROP						] = "drop"; 
		_actionMap[COG_LEFT						] = "left";
		_actionMap[COG_RIGHT					] = "right";
		_actionMap[COG_ROTATE_LEFT				] = "rotate_left";
		_actionMap[COG_ROTATE_RIGHT				] = "rotate_right";
		_actionMap[COG_ROTATE_CLOCKWISE			] = "rotate_clockwise";
		_actionMap[COG_ROTATE_COUNTER_CLOCKWISE ] = "rotate_counter_clockwise";
		_actionMap[COG_ROTATE_FORWARDS			] = "rotate_forwards";
		_actionMap[COG_ROTATE_REVERSE			] = "rotate_reverse";
		_actionMap[COG_DISAPPEAR				] = "disappear";
	}
	const std::map<EE_CognitivAction_t, std::string>& getMap() const {
		return _actionMap;
	}
};

static const _Cognitiv_ _cog_;


void split(const std::string& input, std::vector<std::string>& tokens) {

	tokens.clear();
	std::stringstream ss(input);
	std::string oneToken;
	
	while (ss >> oneToken) {
		try {
			tokens.push_back(oneToken);
		}
		catch (const std::exception&) {}
	}
}

template <typename T>
bool inRange(const T& value, const T& minValue, const T& maxValue) {
	return (value >= minValue && value <= maxValue);
}

template <typename T>
bool convert(const std::string& str, T& value) {
	std::istringstream iss(str);
	return (iss >> value) ? true : false;
}

bool stringToAction(const std::string& actionStr, EE_CognitivAction_t* action) {
	assert(action);
	std::map<EE_CognitivAction_t, std::string>::const_iterator it;
	for (it = _cog_.getMap().begin(); it != _cog_.getMap().end(); it++) {
		if (it->second == actionStr) {
			*action = it->first;
			return true;
		}
	}
	return false;
}

std::string actionToString(EE_CognitivAction_t actionType)
{
	const std::map<EE_CognitivAction_t, std::string>& actionMap = _cog_.getMap();
    std::map<EE_CognitivAction_t, std::string>::const_iterator it = actionMap.find(actionType);
    if ( it != actionMap.end() ) return it->second;
    else return "<unknown action>";
}

bool parseCommand(const std::string& input, std::ostream& output) {

	bool result = true;
	std::ostringstream os;

	if (input.length()) {

		bool wrongArgument = true;
		std::vector<std::string> commands;
		split(input, commands);

		os << "==> ";

		// Quit command
		if (commands.at(0) == "exit") {
			os << "Bye!";
			result = false;
			wrongArgument = false;
		}

		// Print available commands
		else if (commands.at(0) == "help") {

			const std::string activeActionList = "\"push\",\"pull\",\"lift\",\"drop\",\"left\",\"right\",\"rotate_left\",\"rotate_right\",\"rotate_clockwise\",\"rotate_counter_clockwise\",\"rotate_forwards\",\"rotate_reverse\",\"disappear\"";
			
			os << "Available commands:" << std::endl;
			
			os << "set_actions [userID] [action list]" << std::endl;
			os << "\t\t\t\t\t set the Cognitiv active actions" << std::endl;
			os << "\t\t\t\t\t [action list: " << activeActionList  << "]"<< std::endl;

			os << "training_action [userID] [action] \t set Cognitiv training action" << std::endl;
			os << "\t\t\t\t\t [action: \"neutral\"," << activeActionList  << "]"<< std::endl;
			
			os << "training_start [userID] \t\t start Cognitiv training" << std::endl;
			os << "training_accept [userID] \t\t accept previous Cognitiv training" << std::endl;
			os << "training_reject [userID] \t\t reject previous Cognitiv training";
			os << "training_erase [userID] \t\t erase Cognitiv training data for the current action" << std::endl;
			os << "exit \t\t\t\t\t exit this program";

			wrongArgument = false;
		}

		// Set Cognitiv Active actions
		else if (commands.at(0) == "set_actions") {

			if (commands.size() <= 6) { //@@ set_actions userID a1 a2 a3 a4
				
				unsigned int  userID;
				unsigned long activeActions = 0;
				EE_CognitivAction_t actionType;
				bool actionCheckOK = true;

				if (convert(commands.at(1), userID)) {

					for (size_t i=2; i < commands.size(); i++) {

						const std::string& actionStr = commands.at(i);
						
						if (stringToAction(actionStr, &actionType)) {
							activeActions |= actionType;
						}
						else {
							os << "Action [" << actionStr << "] cannot be set to active.";
							actionCheckOK = false;
							break;
						}
					}

					if (actionCheckOK) {
						os << "Setting Cognitiv active actions for user " << userID << "...";
						
						wrongArgument = (EE_CognitivSetActiveActions(userID, activeActions) != EDK_OK);
					}
				}
			}
		}

		// Change Cognitiv training action
		else if (commands.at(0) == "training_action") {

			if (commands.size() == 3) {

				unsigned int userID;
				EE_CognitivAction_t actionType;

				if (convert(commands.at(1), userID)) {

					const std::string& actionStr = commands.at(2);

					if (stringToAction(actionStr, &actionType)) {
						os << "Setting Cognitiv training action for user " << userID;
						os << " to " << actionStr << "...";
						
						wrongArgument = (EE_CognitivSetTrainingAction(userID, actionType) != EDK_OK);
					}
					else {
						os << "Action [" << actionStr << "] cannot be trained.";
					}
				}
			}
		}

		// Start Cognitiv training
		else if (commands.at(0) == "training_start") {

			if (commands.size() == 2) {
				unsigned int userID;
				if (convert(commands.at(1), userID)) {

					os << "Start Cognitiv training for user " << userID << "...";
					
					wrongArgument = (EE_CognitivSetTrainingControl(userID, COG_START) != EDK_OK);
				}
			}
		}

		// Accept Cognitiv training
		else if (commands.at(0) == "training_accept") {

			if (commands.size() == 2) {
				unsigned int userID;
				if (convert(commands.at(1), userID)) {

					os << "Accepting Cognitiv training for user " << userID << "...";
					
					wrongArgument = (EE_CognitivSetTrainingControl(userID, COG_ACCEPT) != EDK_OK);
				}
			}
		}

		// Reject Cognitiv training
		else if (commands.at(0) == "training_reject") {

			if (commands.size() == 2) {
				unsigned int userID;
				if (convert(commands.at(1), userID)) {

					os << "Rejecting Cognitiv training for user " << userID << "...";
					
					wrongArgument = (EE_CognitivSetTrainingControl(userID, COG_REJECT) != EDK_OK);
				}
			}
		}

		// Erase Cognitiv training data
		else if (commands.at(0) == "training_erase") {

			if (commands.size() == 2) {
				unsigned int userID;
				if (convert(commands.at(1), userID)) {
                    EE_CognitivAction_t actionType;
                    if (EE_CognitivGetTrainingAction(userID, &actionType) == EDK_OK) {
                        os << "Erasing Cognitiv training for action \"" << actionToString(actionType) << "\" for user " << userID << "...";
                        
                        wrongArgument = (EE_CognitivSetTrainingControl(userID, COG_ERASE) != EDK_OK);
                    }
				}
			}
		}
		
		// Unknown command
		else {
			os << "Unknown command [" << commands.at(0) << "]. Type \"help\" to list available commands.";
			wrongArgument = false;
		}

		if (wrongArgument) {
			os << "Wrong argument(s) for command [" << commands.at(0) << "]";
		}
	}

	const std::string& outString = os.str();
	if (outString.length()) {
		output << outString << std::endl << std::endl;
	}

	return result;
}
