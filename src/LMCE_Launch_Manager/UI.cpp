#include "LMCE_Launch_Manager.h"
#include "LM.h"
#include "List.h"
#include "DB.h"
#include "UI.h"
#include "DCE/DCEConfig.h"
#include "DCE/Logger.h"
#include "DCE/ClientSocket.h"
#include "pluto_main/Define_DeviceData.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "PlutoUtils/ProcessUtils.h"
#include <iostream>
#include <fstream>


using namespace std; //DCE

#include "Gen_Devices/AllCommandsRequests.h"

//UI class method impmlementations
UI::UI()
{
	m_sTitle = "Generic CLI app";
	m_bCenterTitle = false;
}
UI::~UI()
{

}
void UI::initialize(string sTitle, bool bCenterTitle)
{
	m_sTitle = sTitle;
	m_bCenterTitle = bCenterTitle;
}
void UI::writeLog(string s)
{
	m_vLog.push_back(s);
	if (m_vLog.size() > 10) {
		m_vLog.erase(m_vLog.begin());
	}
	draw();
}
void UI::appendLog(string s)
{
	m_vLog.pop_back();
	m_vLog.push_back(s);
	draw();

}
void UI::flushLog()
{
	DCE::LoggerWrapper::GetInstance()->Flush();
}
void UI::draw()
{
	system("clear");  //yes, I know this is not the best way to do it...
	cout << " LinuxMCE Launch Manager, V1.0                                                  " << endl;
	cout << "+-GENERAL INFO------------+-MYSQL INFO---------------+-STATUS------------------+" << endl;
	cout << "Core Ip:"+padString(m_sCoreIP," ",21)+"MySQL Host:"+padString(m_sMySQLHost," ",15)+"Remote Assistance "+checkBox(m_bRemoteAssistanceRunning) << endl;
	cout << "Video Driver:"+padString(m_sVideoDriver," ",16)+"MySQL User:"+padString(m_sMySQLUser," ",15)+"Autostart Core    "+checkBox(m_sAutostartCore) << endl;
	cout << "Resolution:"+padString(m_sVideoResolution," ",18)+"MySQL Pass:"+padString(m_sMySQLPass," ",15)+"Autostart MD      "+checkBox(m_sAutostartMedia) << endl;
	string sAC3 = "";
	if(m_bAC3Pass)
		sAC3 = " (AC3 Passthrough enabled)";
	cout << "Audio Connector:"+padString(m_sSoundSetting+sAC3," ",64) << endl;  
	//cout << "IP Address:"+padString(""," ",69) << endl;
	cout << "-ACTIONS------------------+--------------------------+-------------------------+" << endl;	
	cout << "[1]Reload Router            [5]Toggle Remote Asst.                              " << endl;
	cout << "[2]Regen Orbiter            [6]Toggle AutostartCore                             " << endl;
	cout << "[3]Regen All Orbiters       [7]Toggle AutostartMedia                            " << endl;
	cout << "[4]Change Resolutions       [8]Reboot                                           " << endl;
	cout << "+-LOG--------------------------------------------------------------------------+" << endl;
	for(int i=0;i<m_vLog.size();i++) {
		cout << m_vLog[i] << endl;
	}  
}
string UI::checkBox(string s)
{
	string sRet;

	if(s=="1") {
		sRet = "[X]";
	} else {
		sRet = "[ ]";
	}
	return sRet;
}
string UI::checkBox(bool b)
{
	string sRet;
	
	if(b) {
		sRet = "[X]";
	} else {
		sRet = "[ ]";
	}
	return sRet;
}
void UI::setCoreIP(string thisIP)
{
	m_sCoreIP = thisIP;
}
void UI::setVideoDriver(string thisVideoDriver)
{
	m_sVideoDriver = thisVideoDriver;
}
void UI::setVideoResolution(string thisVideoResolution)
{
	m_sVideoResolution = thisVideoResolution;
}
void UI::setMySQLInfo(string mysqlHost, string mysqlUser, string mysqlPass)
{
	m_sMySQLHost = mysqlHost;
	m_sMySQLUser = mysqlUser;
	m_sMySQLPass = mysqlPass;
}
void UI::setAudioInfo(string thisSetting, bool thisAC3Pass)
{
	m_bAC3Pass = thisAC3Pass;
	m_sSoundSetting = thisSetting;
}
void UI::setStatus(string sAutostartCore, string sAutostartMedia, bool bRemoteAssistance)
{
	m_sAutostartCore = sAutostartCore;
	m_sAutostartMedia = sAutostartMedia;
	m_bRemoteAssistanceRunning = bRemoteAssistance;
}
string UI::padString(std::string s, std::string sPad, int len)
{
	string sRet;

	if(s.length()>len) {
		//chop off the rest
		return s.substr(0,len);
		
	}
	//Pad the string
	for(int i=0;i<len-s.length();i++) {
		sRet=sRet+sPad;
	}
	return s+sRet;
}
string UI::wrapTo(string s, int width)
{
	int len = s.length();
	while (len>width) {
		s=s;
	}
	return s;
	
}

//CB (checkbox) class method implementations
CB::CB()
{

}
CB::~CB()
{

}
void CB::initialize(string sLabel,bool startingValue, string sCheckChar)
{
	m_sLabel = sLabel;
	m_bValue = startingValue;
	m_sCheckChar = sCheckChar;
}
bool CB::checked()
{
	return m_bValue;
}
bool CB::value()
{
	return checked();
}
void CB::set(bool bValue){
	m_bValue = bValue;
}

//Other functions

