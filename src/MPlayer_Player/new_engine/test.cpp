#include <iostream>

#include "MPlayerEngine.h"
#include "Log.h"

using namespace std;

int main() {
	try {
		MPlayerEngine *pEngine = new MPlayerEngine();
		Log("Created MPlayerEngine object");
		
		sleep(5);
		Log("Playing mp3...");
		pEngine->ExecuteCommand("loadfile /home/fenikso/Music/SinnerMan.mp3");
		sleep(3);
		string sAudioCodec;
		Log("Detecting audio codec...");
		if ( pEngine->ExecuteCommand("get_audio_codec", "AUDIO_CODEC", sAudioCodec) ) {
			Log("Detected audio codec: "+sAudioCodec);
		}
		else {
			Log("Failed to detect audio codec");
		}
		
		sleep(10);
		
		Log("Destroying engine");
		delete pEngine;
	}
	catch (...) {
		Log("Engine failed to start");
	}
}

