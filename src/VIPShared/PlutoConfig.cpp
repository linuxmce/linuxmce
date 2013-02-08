/*
 PlutoConfig
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include "VIPShared/PlutoConfig.h"
#ifndef SYMBIAN
#include "PlutoUtils/StringUtils.h"
#include "VIPShared/VIPIncludes.h"

PlutoConfig::PlutoConfig() 
#ifdef MULTI_THREADED 
: m_MyErrorLogMutex("error"),m_MyUsageLogMutex("usage")
#endif
#ifdef USE_MYSQL
,m_MySqlMutex("MYSQL")
#endif
{ 
#ifdef MULTI_THREADED 
	m_MyErrorLogMutex.Init(NULL);
	m_MyUsageLogMutex.Init(NULL);
#ifdef USE_MYSQL
	m_MySqlMutex.Init(NULL);
#endif
#endif

	m_pBDCommandProcessor = NULL;
	m_bSendKeyStrokes = false;
	m_iPollingInterval = 500000000;  // half a second

#ifdef VIPESTABLISHMENT
	m_pVIPUIRequestProcessor = NULL;
	m_iEstablishmentID=0;
	m_iSoftwareVersion=0;
	m_iSendToEmulator=0;
	m_pVIPUISendRequestToPhone_Callback=NULL;
#endif

	m_pRequestProcessor=NULL; 
	m_iTransNumber=0;
	string tab; // temporary

// AppServer won't be using a config file
#ifndef APPSERVER
	FILE *file = fopen("PlutoConfig.conf","rb");
	if( !file )
		file = fopen("../PlutoConfig.conf","rb");
	if( !file )
	{
		file = fopen("PutConfigHere.conf","wb");
		fwrite("here",4,1,file);
		fclose(file);
		printf("Cannot open config\n");
#ifdef WIN32
		MessageBox(NULL,"Cannot open config","Error",MB_OK);
#endif
		exit(2);
	}

	fseek(file, 0L, SEEK_END);
	int Size = ftell(file);
	fseek(file, 0L, SEEK_SET);

	char *buffer = new char[Size+1];
	buffer[Size]=0;
	fread(buffer,Size,1,file);
	fclose(file);

	m_sRole = GetSetting("Role:",buffer,"VIPESTABLISHMENT");
	m_sErrorLog = GetSetting("ErrorLog:",buffer,"/home/");
	m_sUsageLog = GetSetting("UsageLog:",buffer,"/home/");
	m_sServerAddress = GetSetting("AddressToSendRequestsTo:",buffer,"localhost");
	m_sMenuPath = GetSetting("PathToMenus:",buffer,"/home/");
	m_sDatabaseInfo = GetSetting("DatabaseInfo:",buffer,"/home/");
	m_iServerPort = atoi(GetSetting("PortToListenOn:",buffer,"3460").c_str());
	m_sPicturePath = GetSetting("PathToPictures:",buffer,"/home/");
	m_sEmulatorAddress = GetSetting("AddressOfEmulator:",buffer, "localhost");
	m_iEmulatorPort = atoi(GetSetting("PortForEmulator:",buffer,"3461").c_str());

	delete[] buffer;
	buffer = NULL;

	time_t t = time(NULL);
	struct tm *tm = localtime(&t);
	char LogFile[200];
	sprintf(LogFile,"%s_%d_%d_%d %d-%d-%d.log",m_sErrorLog.c_str(),tm->tm_mon,tm->tm_mday,tm->tm_year,
		tm->tm_hour,tm->tm_min,tm->tm_sec);
	m_ofErrorLog = new ofstream(LogFile);
	sprintf(LogFile,"%s_%d_%d_%d %d-%d-%d.log",m_sUsageLog.c_str(),tm->tm_mon,tm->tm_mday,tm->tm_year,
		tm->tm_hour,tm->tm_min,tm->tm_sec);
	m_ofUsageLog = new ofstream(LogFile);
	// Servers use MySql
	if( m_sRole.find("_SERVER")!=string::npos )
	{
#ifdef USE_MYSQL
		string::size_type pos=0;
		m_sMysqlHost=StringUtils::Tokenize(m_sDatabaseInfo,",",pos);
		m_sMysqlUser=StringUtils::Tokenize(m_sDatabaseInfo,",",pos);
		m_sMysqlPass=StringUtils::Tokenize(m_sDatabaseInfo,",",pos);
		m_sMysqlDatabase=StringUtils::Tokenize(m_sDatabaseInfo,",",pos);
		MysqlConnect(false);
#endif

	}
#endif
}

string PlutoConfig::GetSetting(const char *Token,char *Buffer,char *Default)
{
	if( !Buffer )
		return Default;

	char *ptr = strstr(Buffer,Token);
	if( !ptr )
		return Default;

	ptr += strlen(Token);
	while( *ptr==' ' || *ptr=='\t' )
		ptr++;

	if( !*ptr )
		return Default;

	char *pStart = ptr,*pEnd = ptr;
	while( *pEnd!='\n' && *pEnd!='\r' && *pEnd )
		pEnd++;

	char cBefore = *pEnd;
	*pEnd=0;
	string s = pStart;
	*pEnd=cBefore;
	return s;
}

#ifdef USE_MYSQL
bool PlutoConfig::MysqlConnect(bool bReset)
{
	my_bool reconnect = true;
	PLUTO_SAFETY_LOCK(ms,m_MySqlMutex);

	if( bReset )
		mysql_close(&m_MySQL);

	mysql_init(&m_MySQL);
	mysql_options(&m_MySQL, MYSQL_OPT_RECONNECT, &reconnect);
	if(! mysql_real_connect(&m_MySQL, m_sMysqlHost.c_str(), m_sMysqlUser.c_str(), m_sMysqlPass.c_str(), m_sMysqlDatabase.c_str(), 0, NULL,0))
	{
		ErrorLog << "Failed to connect to MySQL (2):" << mysql_error(&m_MySQL); }
		return false;
	} 
	return true;
}

MYSQL_RES *PlutoConfig::mysql_query_result(string query)
{
	PLUTO_SAFETY_LOCK(sl,m_MySqlMutex);
	long iresult;
	if( (iresult=mysql_query(&m_MySQL,query.c_str()))!=0 )
	{
		sl.Release();
		ErrorLog << "Query failed" << mysql_error(&m_MySQL) << query.c_str() << iresult; }
		cout << "Query failed" << mysql_error(&m_MySQL) << query.c_str() << iresult << "\n";
		MysqlConnect(true);
		return NULL;
	}
	return mysql_store_result(&m_MySQL);
}

long PlutoConfig::threaded_mysql_query(string query,bool bIgnoreErrors)
{
	PLUTO_SAFETY_LOCK(sl,m_MySqlMutex);
	long iresult;
	if( (iresult=mysql_query(&m_MySQL,query.c_str()))!=0 )
	{
		sl.Release();
		if( bIgnoreErrors )
			return -1;
		ErrorLog << "Query failed " << mysql_error(&m_MySQL) << query << iresult; }
		cout << "Query failed " << mysql_error(&m_MySQL) << query << iresult << "\n";
		MysqlConnect(true);
	}

	return iresult;
}

long PlutoConfig::threaded_mysql_query_withID(string query)
{
	PLUTO_SAFETY_LOCK(sl,m_MySqlMutex);
	long iresult;
	if( (iresult=mysql_query(&m_MySQL,query.c_str()))!=0 )
	{
		sl.Release();
		ErrorLog << "Query failed " << mysql_error(&m_MySQL) << query << iresult; }
		MysqlConnect(true);
		return 0;
	}

	return (long) mysql_insert_id(&m_MySQL);
}

#endif

	
PrettyLog::PrettyLog(ofstream *pStream)
{
	m_pStream=pStream;
	(*m_pStream) << "date";
}

PrettyLog::~PrettyLog()
{
	(*m_pStream) << "\n";
	m_pStream->flush();
}

#endif //SYMBIAN
