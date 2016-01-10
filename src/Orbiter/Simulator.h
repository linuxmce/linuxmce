/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__
//------------------------------------------------------------------------------------------------------
#include "pthread.h"
#include <string>
#include <map>
using namespace std;
//------------------------------------------------------------------------------------------------------
namespace DCE
{

class Simulator
{
	static Simulator* m_pInstance;
	static bool m_bInstanceCreated;

	pthread_t m_GeneratorThreadID;

	Simulator(); //private constructor

	map<string,string> m_mapParameters;
	string m_mapParameters_Find(string Token) {	map<string,string>::iterator it = m_mapParameters.find( Token ); return it == m_mapParameters.end() ? "" : (*it).second; }
	bool m_mapParameters_Exists(string Token) {	map<string,string>::iterator it = m_mapParameters.find( Token ); return it != m_mapParameters.end(); }

	inline int ReadInteger(string sToken, int iDefaultValue = 0);
	inline string ReadString(string sToken, string sDefaultValue = "");

public: //data

	class Orbiter *m_pOrbiter;

	bool m_bEnableGenerator;
	long m_dwDelayMin;
	long m_dwDelayMax;
	long m_dwTimeout;
	int m_iNumberOfButtonsPerClick;
	bool m_bGenerateMouseClicks;
	int m_iMilisecondsTransition;
	int m_iMilisecondsHighLight;
	bool m_bGenerateKeyboardEvents;
	int m_iKeysSetToGenerate;
	string m_sHomeScreen;

	bool m_bTryToDetermineAutomatically;
	string m_sDeviceID;
	string m_sRouterIP;

	bool m_bStopGeneratorThread;
	long m_dwStartGeneratorThreadDelay;

	bool m_bFullScreen;
	bool m_bLogToServer;

	bool m_bIsRunning;

	string m_sConfigurationFile;

public: //methods

	static Simulator *GetInstance();
	static void Cleanup();
	~Simulator();

	void LoadConfigurationFile(string sConfigurationFile);
	void SaveConfigurationFile(string sConfigurationFile = "");

	void StartRandomEventGenerator();
	void StopRandomEventGenerator();
	static bool IsRunning() { return NULL != m_pInstance && m_pInstance->m_bIsRunning; }

	static void SimulateActionDelay(long delay);

	int GetPreseededInstallValue(string Token);
	bool PreseededInstallValueDefined(string Token);

};
}
//------------------------------------------------------------------------------------------------------
#endif
