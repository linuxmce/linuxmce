/*
 R_GetRow
 
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
 * @file R_GetRow.cpp
 * @brief source file for the R_GetRow class
 *
 */
 
  
#include "R_GetRow.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "ChangedRow.h"
#include "sqlCVSprocessor.h"
#include "Table.h"

#include <iostream>
#include <sstream>

R_GetRow::R_GetRow( string sTable, int psc_id )
{
	m_sTable=sTable;
	m_psc_id=psc_id;
}

bool R_GetRow::ProcessRequest( class RA_Processor *pRA_Processor )
{
	(( sqlCVSprocessor * ) pRA_Processor)->LogActivityTime();

	sqlCVSprocessor *psqlCVSprocessor = ( sqlCVSprocessor * ) pRA_Processor;
	Table *pTable = g_GlobalConfig.m_pDatabase->m_mapTable_Find( m_sTable );
	if( !pTable )
	{
		cerr << "Cannot get table " << m_sTable << " to get row contents" << endl;
		m_cProcessOutcome=INTERNAL_ERROR;
	}

	pTable->GetCurrentValues(m_psc_id,&m_mapCurrentValues);
	cout << "Getting row table: " << m_sTable << " psc_id: " << m_psc_id << endl;

	m_cProcessOutcome=SUCCESSFULLY_PROCESSED; /** @todo -- process it */
	return true;
}
