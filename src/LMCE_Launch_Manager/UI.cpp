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
	string timestamp = "Today";//QDateTime::currentDateTime().toString();
	s = "+"+s;
	m_vLog.push_back(s);
	if (m_vLog.size() > 10) {
		m_vLog.erase(m_vLog.begin());
	}
	draw();
}
void UI::appendLog(string s)
{
	s = "+"+s;
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
	cout << "+-GENERAL INFO------------+--MYSQL INFO--------------+-STATUS------------------+" << endl;
	cout << "Core Ip:                     MySQL Host:               Remote Assistance [ ]    " << endl;
	cout << "MD Status:                   MySQL User:               Autostart Core    [ ]    " << endl;
	cout << "Video Driver:                MySQL Pass:               Autostart MD      [ ]    " << endl;
        cout << "Resolution:                                                                     " << endl;
	cout << "Audio Connector:  	   							 " << endl;  
	cout << "IP Address:                                                                     " << endl;
	cout << "-ACTIONS------------------+--------------------------+-------------------------+" << endl;	
	cout << "[1]Reload Router             [5]Toggle Remote Asst.                             " << endl;
	cout << "[2]Regen Orbiter             [6]Reboot                                          " << endl;
	cout << "[3]Regen All Orbiters                                                           " << endl;
	cout << "[4]Change Resolutions                                                           " << endl;
	cout << "+-LOG--------------------------------------------------------------------------+" << endl;
	for(int i=0;i<m_vLog.size();i++) {
		cout << m_vLog[i] << endl;
	}  
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
string padString(string s, int len)
{

}
string wrapTo(string s, int width)
{
	int len = s.length();
	while (len>width) {
		s=s;
	}
	return s;
	
}
