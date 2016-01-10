/*
 A_UpdatedRow
 
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
 
  
#include "A_UpdateRow.h"
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

void A_UpdateRow::ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor)
{
    {
	sqlCVSprocessor *pProcessor = dynamic_cast<sqlCVSprocessor*>(pRA_Processor);
	if (pProcessor)
	    pProcessor->LogActivityTime();
    }
	if( pRequest->ID()!=R_UPDATE_TABLE )
	{
		cerr << "Got an update row action, but it wasn't in response to an update table request" << endl;
		throw "Internal error";
	}
	R_UpdateTable *pR_UpdateTable = (R_UpdateTable *) pRequest;
	sqlCVSprocessor *psqlCVSprocessor = (sqlCVSprocessor *) pRA_Processor;

	Table *pTable = g_GlobalConfig.m_pDatabase->m_mapTable_Find(pR_UpdateTable->m_sTableName);

	cout << "A_UpdateRow - table: " << pTable->Name_get() << " psc id:" << m_psc_id << " batch: " << m_psc_batch << " user: " << m_psc_user << " fields: " << m_vectValues.size() << endl;

	pTable->UpdateRow(this,pR_UpdateTable,psqlCVSprocessor);
}
