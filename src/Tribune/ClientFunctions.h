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
#ifndef ClientFunctions_h
#define ClientFunctions_h

#include "RA/RA_Processor.h"
#include "CommonFunctions.h"

#include <map>
#include <string>

/** Common client functions*/

using namespace Tribune;

class ClientFunctions
{
private:
	string update_list;
	string delete_list;
public:
	class RA_Processor *ra_Processor;
	DCE::Socket **m_pSocket;
	string Connection;
	
	ClientFunctions( DCE::Socket **pSocket )
	{ 
		ra_Processor = new RA_Processor( 1, 1, NULL, g_GlobalConfig.m_iMaxConnectAttemptCount );
		m_pSocket = pSocket;
		Connection = g_GlobalConfig.m_sTribuneHost + ":" + StringUtils::itos(g_GlobalConfig.m_iTribunePort);
	}

	ClientFunctions();

	/** get the lineups for a code provided*/
	bool GetLineups(int zipcode, map <int,pair<string,string> > &);

	/** get channels for a lineup excluding the blacklist channels */
	bool GetChannels( int key, string extra_cond, map <string,string> & );

	/** get blacklist channels  from the client database*/
	string GetBlackListChannels();

	/** delete client lineup*/
	void DeleteClientLineup();
	
	/** store the client lineup*/
	void SetClientLineup(string key, string lineup);

	string GetClientLineup();

	/** get the 'diffs' for the client*/
	string CalcDiffs(string lineup, string blacklist, map <string,string> &mapProgramRecord, map <int,string> &mapStation, map <u_int64_t,string> &mapSchedule, map <string,string> &mapActor, map <string,string> &mapGenre, map <string,string> &mapRole, map <string,string> &mapActorRole);

	/** execute mysql commands from the file provided by the server */
	bool ModifyClientDatabase(string path);

	string getUpdateList();
	string getDeleteList();

	virtual ~ClientFunctions( ) 
	{ 
		delete m_pSocket;
	}

};

#endif
