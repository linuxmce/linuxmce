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
#ifndef PlutoConfig_H
#define PlutoConfig_H

#include "PlutoUtils/MyStl.h"

#ifdef MULTI_THREADED
#include "PlutoUtils/MultiThreadIncludes.h"
#endif //MULTI_THREADED

#ifndef SYMBIAN
#include <iostream>
#include <string>
#include <list>
using namespace std;

extern class PlutoConfig *g_pPlutoConfig;
#endif //SYMBIAN

/*
class BasketOption
{
public:
	string m_sID,m_sDescription;
	long m_iCost,m_iQty;

	BasketOption() {};
	BasketOption (string ID,string Description,long Qty, long Cost) { m_sID=ID; m_sDescription=Description; m_iQty=Qty; m_iCost=Cost; }
};
*/

/*
class BasketItem
{
public:
	string m_sID,m_sDescription;
	long m_iCost,m_iQty;

	MYSTL_CREATE_LIST(m_listBasketOptions,BasketOption);

	BasketItem() {};
	BasketItem (string ID,string Description,long Qty, long Cost) { m_sID=ID; m_sDescription=Description; m_iQty=Qty; m_iCost=Cost; }

	~BasketItem() {
		MYSTL_ITERATE_LIST(m_listBasketOptions,BasketOption,pBO,it)
		{
			delete pBO;
		}
	}
};
*/

// Todo - Remove me, use macros
#ifndef SYMBIAN
#include <fstream>

class PrettyLog
{
	ofstream *m_pStream;
public:
	PrettyLog(ofstream *pStream);
	~PrettyLog();
};

//#define ErrorLog { std::cout
#define ErrorLog if(g_pPlutoConfig && g_pPlutoConfig->m_ofErrorLog){ PLUTO_SAFETY_LOCK(slErrorLog,g_pPlutoConfig->m_MyErrorLogMutex); PrettyLog plog(g_pPlutoConfig->m_ofErrorLog); (*g_pPlutoConfig->m_ofErrorLog) 
//#define UsageLog { std::cout  
#define UsageLog if(g_pPlutoConfig && g_pPlutoConfig->m_ofUsageLog){ PLUTO_SAFETY_LOCK(slUsageLog,g_pPlutoConfig->m_MyUsageLogMutex); PrettyLog plog(g_pPlutoConfig->m_ofUsageLog); (*g_pPlutoConfig->m_ofUsageLog) 

#ifdef USE_MYSQL
	#include <mysql.h>

	class MySqlSafetyRes
	{
	public:
		MYSQL_RES *r;
		MySqlSafetyRes() : r(NULL) {};
		~MySqlSafetyRes() { 
			if( r )
				mysql_free_result(r);
		}
	};
#endif

#endif //SYMBIAN


class PlutoConfig
{
public:

#ifndef SYMBIAN
	ofstream *m_ofErrorLog,*m_ofUsageLog;
#endif

	// serialized values
	string m_sRole; // LINUX_SERVER,WINDOWS_SERVER,LINUX_ESTABLISHMENT,WINDOWS_ESTABLISHMENT,SYMBIAN_PHONE
	string m_sErrorLog,m_sUsageLog; // Fully qualified paths
	string m_sServerAddress,m_sEmulatorAddress; // Used by the establishment to contact the server
	string m_sMenuPath,m_sPicturePath;  // Used by the server to find the menus and pictures.  Picture Path may be used by the Establishment also
	string m_sDatabaseInfo; // Depends on the database being used
	long m_iServerPort,m_iEmulatorPort; // The port for communication with the server

	string GetSetting(const char *Token,char *Buffer,char *Default);

	class BDCommandProcessor *m_pBDCommandProcessor;
	bool m_bSendKeyStrokes;
	unsigned long m_iPollingInterval;  // In nano seconds

	// Internal for VIPServer
#ifdef USE_MYSQL
	string m_sMysqlHost,m_sMysqlUser,m_sMysqlPass,m_sMysqlDatabase;
	pluto_pthread_mutex_t m_MySqlMutex;
	MYSQL m_MySQL;
	
	bool MysqlConnect(bool bReset);
	MYSQL_RES *mysql_query_result(string query);
	long threaded_mysql_query(string query,bool bIgnoreErrors=false);
	long threaded_mysql_query_withID(string query);
#endif //USE_MYSQL

#ifdef MULTI_THREADED
	pluto_pthread_mutex_t m_MyErrorLogMutex,m_MyUsageLogMutex;
#endif //MULTI_THREADED

#ifdef VIPDESIGN
	class CMultiDocTemplate* m_pDocTemplate;
	class CMainFrame* m_pMainFrame;
	class CVIPDesignDoc *m_pDoc;
	bool m_bWaitingForTermination;
	unsigned char m_iResolutionValue;
	HWND m_hWnd_Emulator;
#endif //VIPDESIGN

#ifdef VIPESTABLISHMENT
	// Send to emulator: 0=no, 1=yes, 2=both emulator and phone
	class VIPUIRequestProcessor *m_pVIPUIRequestProcessor;
	class VIPUISendRequestToPhone_Callback *m_pVIPUISendRequestToPhone_Callback;
	unsigned long m_iEstablishmentID,m_iSoftwareVersion,m_iSendToEmulator;
#endif //VIPESTABLISHMENT

	long m_iTransNumber;
	// Temp hack
	class RequestProcessor *m_pRequestProcessor;

	PlutoConfig();
};
#endif 

