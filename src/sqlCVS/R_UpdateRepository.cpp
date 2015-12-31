/*
 R_UpdateRepository
 
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
 * @file R_UpdateRepository.cpp
 * @brief source file for the R_UpdateRepository class
 *
 */
 
  
#include "R_UpdateRepository.h"
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

R_UpdateRepository::R_UpdateRepository( string sRepository, int iSchemaVersion )
{
	m_sRepository=sRepository;
	m_iSchemaVersion=iSchemaVersion;
}

bool R_UpdateRepository::ProcessRequest( class RA_Processor *pRA_Processor )
{
	(( sqlCVSprocessor * ) pRA_Processor)->LogActivityTime();

	cout << "Update Repository: " << m_sRepository << endl;
	sqlCVSprocessor *psqlCVSprocessor = ( sqlCVSprocessor * ) pRA_Processor;
	Repository *pRepository = g_GlobalConfig.m_pDatabase->m_mapRepository_Find( m_sRepository );
	if( !pRepository )
	{
		cerr << "Cannot find repository3: " << m_sRepository;
		m_cProcessOutcome=INTERNAL_ERROR;
	}
	else
	{
#pragma warning("validate users, and store in request processor");
		psqlCVSprocessor->m_pRepository = pRepository;

		if( m_iSchemaVersion!=pRepository->GetSchemaVersion() )
			pRepository->UpdateClientSchema(this,m_iSchemaVersion);

		m_cProcessOutcome=SUCCESSFULLY_PROCESSED; /** @todo -- process it */
	}
	return true;   /** The request was processed successfully */
}
