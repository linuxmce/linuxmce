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
/**
 *
 * @file CommonFunctions.h
 * @brief header file for the GlobalConfig, SerializeableStrings classes; it uses the Table, Repository and Database classes
 * @author
 *
 */

#ifndef CommonFunctions_h
#define CommonFunctions_h

#include <iostream>
#include <map>
#include <string>
#include "DCE/DCEConfig.h"

using namespace std;

#define NULL_TOKEN	"**( NULL )**"

typedef map<string,string> MapStringString;
typedef map<int,string> MapIntString;
typedef map<u_int64_t,string> MapUnsignedLLString;

namespace Tribune
{
	class Database;
	enum TypeOfChange { toc_New=0, toc_Delete=1, toc_Modify=2 };

	/**
	 * @brief  class modelling the global configuration
	 */

	class GlobalConfig
	{
	public:
		DCEConfig dceConfig;
		string m_sTableType;	/**< the table type */

		string m_sDBHost;	/**< MySQL information: the host */
		string m_sDBUser;	/**< MYSQL information: the user */
		string m_sDBPassword;	/**< MYSQL information: the password */
		string m_sDBName;	/**< MySQL information: the name */

		string m_sCommand;      /**< Command line */

		string m_sTribuneHost;	/**< The host name or IP for the Tribune listener */

		int m_iDBPort;		/**< The port of the database */
		int m_iTribunePort;	/**< The port of the Tribune */

		bool m_bImportTable;

		MapStringString mapPrimaryKey_Timestam_ProgramRecord;
		MapIntString mapPrimaryKey_Timestam_Station;
		MapUnsignedLLString mapPrimaryKey_Timestam_Schedule;
		MapStringString mapPrimaryKey_Actor;
		MapStringString mapPrimaryKey_Genre;
		MapStringString mapPrimaryKey_Role;
		MapStringString mapPrimaryKey_ActorRole;
		
		Database *m_pDatabase;		/**< points to the database */
		
		int m_iMaxConnectAttemptCount;
		int m_iServerProcessorTimeout;
		/**
		 * @ brief constructor
		 */

		GlobalConfig()
		{
			m_sTableType="InnoDB";
			m_sDBHost=dceConfig.ReadString("Tribune_MySqlHost","localhost");
			m_sDBUser=dceConfig.ReadString("Tribune_MySqlUser","root");
			m_sDBPassword=dceConfig.ReadString("Tribune_MySqlPassword","");
			m_sDBName=dceConfig.ReadString("Tribune_MySqlDatabase","pluto_tribune");
			m_iDBPort=dceConfig.ReadInteger("Tribune_MySqlPort",3306);
			m_iTribunePort=dceConfig.ReadInteger("Tribune_Port",9003);
			m_sTribuneHost=dceConfig.ReadString("Tribune_Host","localhost");
			m_pDatabase=NULL;
			
			m_bImportTable=false;
			m_iMaxConnectAttemptCount = -1;
			m_iServerProcessorTimeout = 900;
		}

	};
 
 	extern GlobalConfig g_GlobalConfig;

}


string int2string(int i); /**< casts ints to strings */


#endif
