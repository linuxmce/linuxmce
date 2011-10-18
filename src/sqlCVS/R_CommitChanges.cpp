/*
 R_CommitChanges
 
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
 * @file R_CommitChanges.cpp
 * @brief source file for the R_CommitChanges class
 *
 */
 
#include "R_CommitChanges.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "sqlCVSprocessor.h"
#include "Database.h"
#include "Repository.h"
#include "CommonFunctions.h"

#include <iostream>
#include <sstream>
#include <stdlib.h>

using namespace sqlCVS;

R_CommitChanges::R_CommitChanges( string sRepository, string sDefaultUser, string sComments, int iSchemaVersion )
{
	m_sRepository=sRepository;
	m_sDefaultUser=sDefaultUser;
	m_sComments=sComments;
	m_iSchemaVersion=iSchemaVersion;
}

bool R_CommitChanges::ProcessRequest( class RA_Processor *pRA_Processor )
{
	int psc_user=0;
	(( sqlCVSprocessor * ) pRA_Processor)->LogActivityTime();

	cout << "Received Commit changes for repository: " << m_sRepository << endl;
	sqlCVSprocessor *psqlCVSprocessor = ( sqlCVSprocessor * ) pRA_Processor;
	psqlCVSprocessor->m_sComments = m_sComments;
	Repository *pRepository = g_GlobalConfig.m_pDatabase->m_mapRepository_Find( m_sRepository );
	if( !pRepository )
	{
		cerr << "Cannot find repository2: " << m_sRepository;
		m_cProcessOutcome=INTERNAL_ERROR;
	}
	else
	{
		cout << "Ready to validate users.  m_map: " << &m_mapUsersPasswords << endl;
		if( !g_GlobalConfig.ValidateUsers(psqlCVSprocessor->m_bSupervisor,true,&m_mapUsersPasswords) )
		{
			m_cProcessOutcome=LOGIN_FAILED;
			return true;
		}

		// See if any user supplied a password, or if this is a totally anonymous checkin
		psqlCVSprocessor->m_bAllAnonymous = true;
		for(map<int,ValidatedUser *>::iterator it=g_GlobalConfig.m_mapValidatedUsers.begin();it!=g_GlobalConfig.m_mapValidatedUsers.end();++it)
		{
			ValidatedUser *pValidatedUser = (*it).second;
			psc_user = pValidatedUser->m_psc_user;
			if( !pValidatedUser->m_bWithoutPassword )
			{
				psqlCVSprocessor->m_bAllAnonymous = false;
				break;
			}
		}
		
		// Figure out who is the default user that will receive ownership of any new records that
		// don't have an owner specified
		bool bNoPassword,bIsSupervisor; // We're not going to use them.  Only needed to pass them in
		psqlCVSprocessor->m_ipsc_user_default = ValidateUser(m_sDefaultUser,"nopass",bNoPassword,bIsSupervisor);
		if( !psqlCVSprocessor->m_ipsc_user_default && g_GlobalConfig.m_mapValidatedUsers.size() )
			psqlCVSprocessor->m_ipsc_user_default = (*g_GlobalConfig.m_mapValidatedUsers.begin()).first;

		psqlCVSprocessor->m_pRepository = pRepository;
		m_psc_batch = psqlCVSprocessor->m_psc_bathdr_orig = psqlCVSprocessor->m_i_psc_batch = pRepository->CreateBatch( psqlCVSprocessor, &g_GlobalConfig.m_mapValidatedUsers );

		if( m_iSchemaVersion!=pRepository->GetSchemaVersion() )
			pRepository->UpdateClientSchema(this,m_iSchemaVersion);

		if( !psqlCVSprocessor->m_i_psc_batch )
		{
			m_cProcessOutcome=INTERNAL_ERROR;
		}
		else
		{
			// execute post-commit hook if any
			char buff[1024];
			std::string path;

			// create postcommit script path variable. 
			// Name = sqlCVS bin name + -postcommit.sh. 
			// Example: "sqlCVS-postcommit.sh"
			ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
			if (len != -1) {
				buff[len] = '\0';
				path = std::string(buff);
			}
			path.append("-postcommit.sh");
			
			// test if postcommit script exists
			fstream fin;
			fin.open(path.c_str(),ios::in);
			if( fin.is_open() )
			{
				// postcommit script exists, so execute it
				cout << "post-commit hook: Executing " << path << endl;
				
				// $1 = batch ID
				path.append(" ").append(StringUtils::itos(psqlCVSprocessor->m_i_psc_batch));
				// $2 = repo
				path.append(" ").append(psqlCVSprocessor->m_pRepository->Name_get());
				// $3 = user
				if(psqlCVSprocessor->m_bAllAnonymous == true)
					path.append(" ").append("Anonymous");
				else
					{
					MySqlHelper mySqlHelper(g_GlobalConfig.dceConfig.m_sDBHost,g_GlobalConfig.dceConfig.m_sDBUser,g_GlobalConfig.dceConfig.m_sDBPassword,"MasterUsers");
					MYSQL_ROW row=NULL;
					row = mysql_fetch_row(mySqlHelper.mysql_query_result("SELECT Username FROM MasterUsers WHERE PK_MasterUsers = "+StringUtils::itos(psc_user)));
					path.append(" ").append(row[0]);
					}
				// $4 = comment
				path.append(" '").append(StringUtils::URLEncode(psqlCVSprocessor->m_sComments)).append("'");
				
				cout << path << endl;
				system(path.c_str());
			}
			else
			{
				cout << "post-commit hook: No " << path << " found" << endl;
			}
			fin.close();

			m_cProcessOutcome=SUCCESSFULLY_PROCESSED; /** @todo -- process it */
		}
	}
	return true;
}

