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
public:
	UI();
	~UI();
	void initialize(string sTitle, bool bCenterTitle=false);
	void writeLog(string s);
	void appendLog(string s); 
	void flushLog();
	void draw();

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

string padString(string s, string sPad, int len);
string wrapTo(string s, int width);
#endif
