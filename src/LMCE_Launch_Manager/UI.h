#ifndef UI_h
#define UI_h
#include "LMCE_Launch_Manager.h"
#include "List.h"
#include "DB.h"
#include "Gen_Devices/LMCE_Launch_ManagerBase.h"
#include <vector>

class UI
{
private:
	string m_sTitle;
	bool m_bCenterTitle;
	vector<string> m_vLog;
	string m_sCoreIP;
	string m_sVideoDriver;
	string m_sVideoResolution;
	string m_sMySQLHost;
	string m_sMySQLUser;
	string m_sMySQLPass;
	bool m_bAC3Pass;
	string m_sSoundSetting;
	string m_sAutostartCore;
	string m_sAutostartMedia;
	bool m_bRemoteAssistanceRunning;
public:
	UI();
	~UI();
	void initialize(string sTitle, bool bCenterTitle=false);
	void writeLog(string s);
	void appendLog(string s); 
	string padString(std::string s, std::string sPad, int len);
	string wrapTo(std::string s, int width);
	void flushLog();
	void draw();
	string checkBox(string s);
	string checkBox(bool b);
	//accessor methods
	void setCoreIP(string thisIP);
	void setVideoDriver(string thisVideoDriver);
	void setVideoResolution(string thisVideoResolution);
	void setMySQLInfo(string mysqlHost, string mysqlUser, string mysqlPass);
	void setAudioInfo(string thisSetting, bool thisAC3Pass);
	void setStatus(string sAutostartCore, string sAutostartMedia, bool bRemoteAssistance);

};

class CB
{
private:
	string m_sLabel;
	bool m_bValue;
	string *m_pVar; //variable we are tracking 
	//UI m_uiParent;
	string m_sCheckChar;
public:
	CB();
	~CB();
	void initialize(string sLabel,bool startingValue, string sCheckChar);
	bool checked();
	bool value();  //same as checked()
	void set(bool bValue);
	

};


#endif
