/*
 A_UpdateSchema
 
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
 * @file A_UpdatedRow.cpp
 * @brief source file for A_UpdatedRow class
 *
 */
 
  
#include "A_UpdateSchema.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "R_UpdateTable.h"
#include "sqlCVSprocessor.h"
#include "Table.h"
#include "Database.h"
#include "CommonFunctions.h"

#include <iostream>
#include <sstream>

using namespace sqlCVS;

void A_UpdateSchema::ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor)
{
	(( sqlCVSprocessor * ) pRA_Processor)->LogActivityTime();

	Repository *pRepository = g_GlobalConfig.m_pDatabase->m_mapRepository_Find(m_sRepository);
	if( pRequest->ID()!=R_COMMIT_CHANGES && pRequest->ID()!=R_UPDATE_REPOSITORY )
	{
		cerr << "Got an update schema action, but it wasn't in response to a commit/update repository request or repository invalid" << endl;
		throw "Internal error";
	}

	vector<string> vectCommands;
	string Tokens="\r\n";
	StringUtils::Tokenize(m_sUpdateString,Tokens,vectCommands);
	cout << "A_UpdateSchema::ProcessAction got " << (int) vectCommands.size() << " commands" << endl;
	for(size_t s=0;s<vectCommands.size();++s)
	{
		if( vectCommands[s].length()==0 )
			continue;

		if( !pRepository->ProcessSchemaUpdate(vectCommands[s]) )
		{
			cout << "SQL failed: " << vectCommands[s] << endl;
			cerr << "The database is now corrupted, and the schema is out of sync" << endl
				<< "because the schema update failed!" << endl;
			throw "Schema update failed";
		}
	}

	ostringstream sSql;
	sSql << "INSERT INTO `psc_" << m_sRepository << "_schema` (`PK_psc_" << m_sRepository << "_schema`,`Value`) "
		<< "VALUES (" << m_iSchemaVersion << ",'" << StringUtils::SQLEscape(m_sUpdateString) << "');";
	if( pRepository->m_pDatabase->threaded_mysql_query( sSql.str() )<0 )
	{
		cout << "SQL failed: " << sSql << endl;
		cerr << "The database is now corrupted, and the schema is out of sync" << endl
			<< "because the schema update failed!" << endl;
		throw "Schema update failed";
	}

	pRepository->SetSetting("schema",StringUtils::itos(m_iSchemaVersion));
}
