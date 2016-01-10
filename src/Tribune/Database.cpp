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
#include "PlutoUtils/CommonIncludes.h"	
#include "Database.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "RA/RA_Processor.h"
#include "CommonFunctions.h"

#include <iomanip>
#include <sstream>

using namespace Tribune;

Database::Database( string db_host, string db_user, string db_pass, string db_name, int db_port )
	: DBHelper( db_host, db_user, db_pass, db_name, db_port )
{
	if( !m_bConnected )
	{
		m_sDBDBName = db_name; // Set it back and try again
		DBConnect();
		if( !m_bConnected )
			return;
	}
}

void Database::StartTransaction( ) 
{
	if( threaded_db_wrapper_query("START TRANSACTION")<0 )
	{
		cerr << "Could not start transaction" << endl;
		throw "Database error";
	}
}

void Database::Commit( )
{
	cout << "Doing a Commit" << endl;
	if( threaded_db_wrapper_query("COMMIT")<0 )
	{
		cerr << "Could not commit transaction" << endl;
		throw "Database error";
	}
}

void Database::Rollback( )
{
	cout << "Doing a rollback" << endl;
	if( threaded_db_wrapper_query("ROLLBACK")<0 )
	{
		cerr << "Could not rollback transaction" << endl;
		throw "Database error";
	}
}

