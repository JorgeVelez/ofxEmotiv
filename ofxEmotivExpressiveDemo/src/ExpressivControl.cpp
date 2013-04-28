#include <map>
#include <cassert>
#include "edk.h"
#include "edkErrorCode.h"
#include "ExpressivControl.h"

class _Expressiv_ {
	std::map<EE_ExpressivAlgo_t, std::string> _expMap;
public:
	_Expressiv_() {
		_expMap[EXP_NEUTRAL		] = "neutral";
		_expMap[EXP_BLINK		] = "blink";
		_expMap[EXP_WINK_LEFT	] = "wink_left";
		_expMap[EXP_WINK_RIGHT	] = "winkright";
		_expMap[EXP_HORIEYE		] = "horieye"; 
		_expMap[EXP_EYEBROW		] = "eyebrow_raise";
		_expMap[EXP_FURROW		] = "eyebrow_furrow";
		_expMap[EXP_SMILE		] = "smile";
		_expMap[EXP_CLENCH		] = "clench";
		_expMap[EXP_LAUGH		] = "laugh";
		_expMap[EXP_SMIRK_LEFT	] = "smirk_left";
		_expMap[EXP_SMIRK_RIGHT	] = "smirk_right";
	}
	const std::map<EE_ExpressivAlgo_t, std::string>& getMap() const {
		return _expMap;
	}
};

static _Expressiv_ _exp_;


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

bool stringToExpression(const std::string& expStr, EE_ExpressivAlgo_t* exp) {
	assert(exp);
	std::map<EE_ExpressivAlgo_t, std::string>::const_iterator it;
	for (it = _exp_.getMap().begin(); it != _exp_.getMap().end(); it++) {
		if (it->second == expStr) {
			*exp = it->first;
			return true;
		}
	}
	return false;
}

std::string expressionToString(EE_ExpressivAlgo_t expType)
{
	const std::map<EE_ExpressivAlgo_t, std::string> actionMap = _exp_.getMap();
    std::map<EE_ExpressivAlgo_t, std::string>::const_iterator it = actionMap.find(expType);
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
            std::string actionsHelpText = "\t\t\t\t\t [expressions: \"neutral\",\"smile\",\"clench\",\"laugh\",\"smirk_left\",\"smirk_right\",\"eyebrow_raise\",\"eyebrow_furrow\"]";
			os << "Available commands:" << std::endl;

			os << "trained_sig [userID] \t\t\t query if the user has trained sufficient actions to activate a trained signature" << std::endl;

			os << "set_sig [userID] [signatureType]\t configure to use the built-in, universal signature or a personal, trained signature" << std::endl;
			os << "\t\t\t\t\t [signatureType: 0 for universal or 1 for trained signature]" << std::endl;
			
			os << "training_exp [userID] [expression] \t set Expressiv training expression" << std::endl;
			os << actionsHelpText << std::endl;
			
			os << "training_start [userID] \t\t start Expressiv training" << std::endl;
			os << "training_accept [userID] \t\t accept Expressiv training" << std::endl;
			os << "training_reject [userID] \t\t reject Expressiv training" << std::endl;
            os << "training_erase [userID] \t\t erase Expressiv training data for the current expression" << std::endl;
			os << "exit \t\t\t\t\t exit this program";

			wrongArgument = false;
		}

		// Query for activating a trained signature
		else if (commands.at(0) == "trained_sig") {

			if (commands.size() == 2) {
				unsigned int userID;
				int signatureAvailable = 0;
				if (convert(commands.at(1), userID)) {

                    os << "Querying availability of a trained Expressiv signature for user " << userID << "..." << std::endl;
					
					wrongArgument = (EE_ExpressivGetTrainedSignatureAvailable(userID, &signatureAvailable) != EDK_OK);

					if (!wrongArgument) {
						os << "A trained Expressiv signature is " << ((!signatureAvailable) ? "not" : "")
							<< " available for user " << userID;
					}
				}
			}
		}

		// Switch between the universal signature or a trained signature
		else if (commands.at(0) == "set_sig") {

			if (commands.size() == 3) {
				unsigned int userID, signatureType;
                int result;
				if (convert(commands.at(1), userID) && convert(commands.at(2), signatureType)) {

					if (inRange(signatureType, (unsigned int)EXP_SIG_UNIVERSAL, (unsigned int)EXP_SIG_TRAINED)) {

						os << "Switching to " << ((signatureType) ? "a trained" : "the universal")
                            << " Expressiv signature for user " << userID << "..." << std::endl;
						
						result = EE_ExpressivSetSignatureType(userID,(EE_ExpressivSignature_t)signatureType);
                        wrongArgument = (result != EDK_OK && result != EDK_EXP_NO_SIG_AVAILABLE);
                        if ( result == EDK_EXP_NO_SIG_AVAILABLE ) {
                            os << "A trained Expressiv signature is not available for user " << userID << std::endl;
                        }
					}
				}
			}
		}

		// Change Expressiv training action
		else if (commands.at(0) == "training_exp") {

			if (commands.size() == 3) {
				unsigned int userID;
				EE_ExpressivAlgo_t expType;
				if (convert(commands.at(1), userID)) {

					const std::string& expStr = commands.at(2);

					if (stringToExpression(expStr, &expType)) {
						os << "Setting Expressiv training expression for user " << userID;
						os << " to " << expStr << "...";
						
						wrongArgument = (EE_ExpressivSetTrainingAction(userID, expType) != EDK_OK);
					}
					else {
						os << "Expression [" << expStr << "] cannot be trained.";
					}
				}
			}
		}

		// Start Expressiv training
		else if (commands.at(0) == "training_start") {

			if (commands.size() == 2) {
				unsigned int userID;
				if (convert(commands.at(1), userID)) {

					os << "Start Expressiv training for user " << userID << "...";
					
					wrongArgument = (EE_ExpressivSetTrainingControl(userID, EXP_START) != EDK_OK);
				}
			}
		}

		// Accept Expressiv training
		else if (commands.at(0) == "training_accept") {

			if (commands.size() == 2) {
				unsigned int userID;
				if (convert(commands.at(1), userID)) {

					os << "Accepting Expressiv training for user " << userID << "...";
					
					wrongArgument = (EE_ExpressivSetTrainingControl(userID, EXP_ACCEPT) != EDK_OK);
				}
			}
		}

		// Reject Expressiv training
		else if (commands.at(0) == "training_reject") {

			if (commands.size() == 2) {
				unsigned int userID;
				if (convert(commands.at(1), userID)) {

					os << "Rejecting Expressiv training for user " << userID << "...";
					
					wrongArgument = (EE_ExpressivSetTrainingControl(userID, EXP_REJECT) != EDK_OK);
				}
			}
		}

		// Erase training data
		else if (commands.at(0) == "training_erase") {

			if (commands.size() == 2) {
				unsigned int userID;
				if (convert(commands.at(1), userID)) {
                    EE_ExpressivAlgo_t expType;
                    if (EE_ExpressivGetTrainingAction(userID, &expType) == EDK_OK) {
                        os << "Erasing Expressiv training for action \"" << expressionToString(expType) << "\" for user " << userID << "...";
                        
                        wrongArgument = (EE_ExpressivSetTrainingControl(userID, EXP_ERASE) != EDK_OK);
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
