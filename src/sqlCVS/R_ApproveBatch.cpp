/*
 R_ApproveBatch
 
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
 * @file R_ApproveBatch.cpp
 * @brief source file for the R_ApproveBatch class
 *
 */
 
#include "R_ApproveBatch.h"
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

using namespace sqlCVS;

R_ApproveBatch::R_ApproveBatch( string sRepository, int psc_batch, bool bReject )
{
	m_sRepository=sRepository;
	m_psc_batch=psc_batch;
	m_bReject=bReject;
}

bool R_ApproveBatch::ProcessRequest( class RA_Processor *pRA_Processor )
{
	(( sqlCVSprocessor * ) pRA_Processor)->LogActivityTime();

	cout << "Received approve batch: " << m_psc_batch << " for repository: " << m_sRepository << endl;
	sqlCVSprocessor *psqlCVSprocessor = ( sqlCVSprocessor * ) pRA_Processor;
	Repository *pRepository = g_GlobalConfig.m_pDatabase->m_mapRepository_Find( m_sRepository );
	if( !pRepository )
	{
		cerr << "Cannot find repository1: " << m_sRepository;
		m_cProcessOutcome=INTERNAL_ERROR;
	}
	else
	{
		if( !g_GlobalConfig.ValidateUsers(psqlCVSprocessor->m_bSupervisor,true,&m_mapUsersPasswords) )
		{
			m_cProcessOutcome=LOGIN_FAILED;
			return true;
		}

		ValidatedUser *pValidatedUser_FirstOneWithPassword=NULL;
		for(map<int,ValidatedUser *>::iterator it=g_GlobalConfig.m_mapValidatedUsers.begin();
			it!=g_GlobalConfig.m_mapValidatedUsers.end();++it)
		{
			ValidatedUser *pValidatedUser = (*it).second;
			if( !pValidatedUser->m_bWithoutPassword )
			{
				pValidatedUser_FirstOneWithPassword = pValidatedUser;
				break;
			}
                
		}

		if( !pValidatedUser_FirstOneWithPassword )
		{
			cout << "Cannot approve batch since nobody logged in" << endl;
			m_cProcessOutcome=LOGIN_FAILED;
			return true;
		}
		
		psqlCVSprocessor->m_pRepository = pRepository;
		psqlCVSprocessor->m_i_psc_batch = pRepository->CreateBatch( psqlCVSprocessor, &g_GlobalConfig.m_mapValidatedUsers );
		psqlCVSprocessor->m_psc_bathdr_unauth = m_psc_batch;

		if( !psqlCVSprocessor->m_i_psc_batch || !pRepository->ApproveBatch(this,psqlCVSprocessor) )
			m_cProcessOutcome=INTERNAL_ERROR;
		else
		{
			psqlCVSprocessor->st.Commit();
			m_cProcessOutcome=SUCCESSFULLY_PROCESSED; /** @todo -- process it */
		}
	}
	return true;
}

