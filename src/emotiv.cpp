#include "emotiv.h"



emotiv::emotiv(){
	init();
}

void emotiv::init(){

	EE_DataChannel_t targetChannelList[] = {
		ED_COUNTER,
		ED_AF3, ED_F7, ED_F3, ED_FC5, ED_T7, 
		ED_P7, ED_O1, ED_O2, ED_P8, ED_T8, 
		ED_FC6, ED_F4, ED_F8, ED_AF4, ED_GYROX, ED_GYROY, ED_TIMESTAMP, 
		ED_FUNC_ID, ED_FUNC_VALUE, ED_MARKER, ED_SYNC_SIGNAL
	};

const char header[] = "COUNTER,AF3,F7,F3, FC5, T7, P7, O1, O2,P8" 
                      ", T8, FC6, F4,F8, AF4,GYROX, GYROY, TIMESTAMP, "   
                      "FUNC_ID, FUNC_VALUE, MARKER, SYNC_SIGNAL,";
const char affectivSuitesName[] = "Engagement,Frustration,Meditation,Excitement,";

	EmoEngineEventHandle eEvent			= EE_EmoEngineEventCreate();
	EmoStateHandle eState				= EE_EmoStateCreate();
	unsigned int userID					= 0;
	const unsigned short composerPort	= 1726;
	float secs							= 1;
	unsigned int datarate				= 0;
	bool readytocollect					= false;
	int option							= 0;
	int state							= 0;


	
	if (EE_EngineConnect() != EDK_OK) {
		throw std::exception("Emotiv Engine start up failed.");
	}
				
		
		std::cout << "Start receiving EEG Data and affectiv data! Press any key to stop logging...\n" << std::endl;
    	std::ofstream ofs("../bin/EEG_Data.csv",std::ios::trunc);
		ofs << header << std::endl;
		std::ofstream ofs2("../bin/Affectiv_Data.csv",std::ios::trunc);
		ofs2 << affectivSuitesName << std::endl;
		
		DataHandle hData = EE_DataCreate();
		EE_DataSetBufferSizeInSec(secs);

		std::cout << "Buffer size in secs:" << secs << std::endl;

		while (!_kbhit()) {

			state = EE_EngineGetNextEvent(eEvent);
			EE_Event_t eventType;

			if (state == EDK_OK) {

				eventType = EE_EmoEngineEventGetType(eEvent);
				EE_EmoEngineEventGetUserId(eEvent, &userID);
				EE_EmoEngineEventGetEmoState(eEvent, eState);

				// Log the EmoState if it has been updated
				if (eventType == EE_UserAdded) {
					std::cout << "User added";
					EE_DataAcquisitionEnable(userID,true);
					readytocollect = true;
				}
			

			if (readytocollect && (eventType == EE_EmoStateUpdated)) {
												
						EE_DataUpdateHandle(0, hData);

						unsigned int nSamplesTaken=0;
						EE_DataGetNumberOfSample(hData,&nSamplesTaken);
		
						std::cout << "Updated " << nSamplesTaken << std::endl;
						
						if (nSamplesTaken != 0  ) {

							double* data = new double[nSamplesTaken];
							for (int sampleIdx=0 ; sampleIdx<(int)nSamplesTaken ; ++ sampleIdx) {
								for (int i = 0 ; i<sizeof(targetChannelList)/sizeof(EE_DataChannel_t) ; i++) {

									EE_DataGet(hData, targetChannelList[i], data, nSamplesTaken);
									ofs << data[sampleIdx] << ",";
								}	
								ofs << std::endl;
							}
							delete[] data;							
						}
						
						float affEngegement = ES_AffectivGetEngagementBoredomScore(eState);
						float affFrus = ES_AffectivGetFrustrationScore(eState);
						float affMed = ES_AffectivGetMeditationScore(eState);
						float affExcitement = ES_AffectivGetExcitementShortTermScore(eState);
						printf("Engagement: %f, Frustration: %f, ...\n",affEngegement,affFrus);
						ofs2 <<affEngegement<<","<<	affFrus<<","<<affMed<<","<<affExcitement<<","<<std::endl;

			}
			}
			Sleep(100);
		}

		ofs.close();
		ofs2.close();
		EE_DataFree(hData);

}

//--------------------------------------------------------------
void emotiv::update(){

}

//--------------------------------------------------------------
void emotiv::draw(){

}

void emotiv::logEmoState(std::ostream& os, unsigned int userID, EmoStateHandle eState, bool withHeader) {

	ofNotifyEvent(elEventoEmo, eState);

	// Create the top header
	if (withHeader) {
		os << "Time,";
		os << "UserID,";
		os << "Wireless Signal Status,";
		os << "Blink,";
		os << "Wink Left,";
		os << "Wink Right,";
		os << "Look Left,";
		os << "Look Right,";
		os << "Eyebrow,";
		os << "Furrow,";
		os << "Smile,";
		os << "Clench,";
		os << "Smirk Left,";
		os << "Smirk Right,";
		os << "Laugh,";
		os << "Short Term Excitement,";
		os << "Long Term Excitement,";
		os << "Engagement/Boredom,";
		os << "Cognitiv Action,";
		os << "Cognitiv Power,";
		os << std::endl;
	}

	// Log the time stamp and user ID
	os << ES_GetTimeFromStart(eState) << ",";
	os << userID << ",";
	os << static_cast<int>(ES_GetWirelessSignalStatus(eState)) << ",";

	// Expressiv Suite results
	os << ES_ExpressivIsBlink(eState) << ",";
	os << ES_ExpressivIsLeftWink(eState) << ",";
	os << ES_ExpressivIsRightWink(eState) << ",";

	os << ES_ExpressivIsLookingLeft(eState) << ",";
	os << ES_ExpressivIsLookingRight(eState) << ",";

	std::map<EE_ExpressivAlgo_t, float> expressivStates;

	EE_ExpressivAlgo_t upperFaceAction = ES_ExpressivGetUpperFaceAction(eState);
	float			   upperFacePower  = ES_ExpressivGetUpperFaceActionPower(eState);

	EE_ExpressivAlgo_t lowerFaceAction = ES_ExpressivGetLowerFaceAction(eState);
	float			   lowerFacePower  = ES_ExpressivGetLowerFaceActionPower(eState);

	expressivStates[ upperFaceAction ] = upperFacePower;
	expressivStates[ lowerFaceAction ] = lowerFacePower;
	
	os << expressivStates[ EXP_EYEBROW     ] << ","; // eyebrow
	os << expressivStates[ EXP_FURROW      ] << ","; // furrow
	os << expressivStates[ EXP_SMILE       ] << ","; // smile
	os << expressivStates[ EXP_CLENCH      ] << ","; // clench
	os << expressivStates[ EXP_SMIRK_LEFT  ] << ","; // smirk left
	os << expressivStates[ EXP_SMIRK_RIGHT ] << ","; // smirk right
	os << expressivStates[ EXP_LAUGH       ] << ","; // laugh

	// Affectiv Suite results
	os << ES_AffectivGetExcitementShortTermScore(eState) << ",";
	os << ES_AffectivGetExcitementLongTermScore(eState) << ",";

	os << ES_AffectivGetEngagementBoredomScore(eState) << ",";

	// Cognitiv Suite results
	os << static_cast<int>(ES_CognitivGetCurrentAction(eState)) << ",";
	os << ES_CognitivGetCurrentActionPower(eState);

	os << std::endl;
};

