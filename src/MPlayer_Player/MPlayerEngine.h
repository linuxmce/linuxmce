#ifndef __MPLAYER_ENGINE_H__
#define __MPLAYER_ENGINE_H__

#include <string>
#include <map>

#define BLACK_MPEG_FILE "/usr/pluto/share/black.mpeg"

#include "PlutoUtils/MultiThreadIncludes.h"

using namespace std;

void* EngineOutputReader(void *pInstance);

class MPlayerEngine {
private:
	pluto_pthread_mutex_t m_engineStateMutex;
	pluto_pthread_mutex_t m_engineReadPipeMutex;
	pluto_pthread_mutex_t m_engineWritePipeMutex;
	
	bool m_bEngineIsRunning;
	bool m_bRunEngineOutputReader;
	pthread_t m_tEngineOutputReaderThread;

	int m_iInPipe[2], m_iOutPipe[2];
	bool m_bInPipe[2], m_bOutPipe[2];

	friend void* EngineOutputReader(void *pInstance);

	int m_iChildPID;
	map<string,string> m_mEngineAnswers;

	bool StartEngine();
	void StopEngine();

	void ClosePipes();

	string ReadLine();
	
	string m_sCurrentFile;
	string m_sCurrentScreenshot;
	void SetCurrentFile(string sName);
	
	void SetCurrentScreenshot(string sName);
	string GetCurrentScreenshot();
	
	vector<string> m_vCurrentPlaylist;
	
	void SetEngineAnswers(string key, string value);
	string GetEngineAnswers(string key);
	
	string GetLastFile();
	void ClearPlaylist();
	
	string m_sALSADevice;
	bool m_bALSAPassthrough;
	bool m_bOverrideAudioSettings;

	string sHwAccelSetting;
	string sDeInterlaceSetting;

public:
	enum SeekType {SEEK_RELATIVE_TIME=0, SEEK_ABSOLUTE_PERCENTAGE, SEEK_ABSOLUTE_TIME};
	enum EngineState {PLAYBACK_STARTED=1, PLAYBACK_FINISHED};
	
//	MPlayerEngine(bool bOverrideAudioSettings=false, string sALSADevice="default", bool bALSAPassthrough=false);
	MPlayerEngine(bool bOverrideAudioSettings=false, string sALSADevice="default", bool bALSAPassthrough=false, string sGpuDrv = "", string sGpuDeInt = "");
	~MPlayerEngine();

	// executes command with no response
	void ExecuteCommand(string sCommand);
	int GetChildPID();

	// executes command with response
	bool ExecuteCommand(string sCommand, string sResponseName, string &sResponseValue);
	
	bool StartPlaylist(vector<string> &vFiles);
	void StopPlayback();
	
	float GetCurrentPosition();
	float GetFileLength();
	
	string GetAudioCodec();
	string GetVideoCodec();
	
	void Seek(float fValue, SeekType eType);
	EngineState GetEngineState();
	
	string GetCurrentFile();
	void GetScreenshot(int iWidth, int iHeight, char *&pData, int &iDataSize, string &sFormat, string &sCMD_Result);

private:
	bool StartPlayback(string sMediaFile);
	EngineState m_eEngineState;
	void SetEngineState(const EngineState &eValue);
};

void Tokenize(const string& str,
	      vector<string>& tokens,
       const string& delimiters = " ");

#endif
