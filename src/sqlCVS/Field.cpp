/*
 Field
 
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
 * @file Field.cpp
 * @brief source file for the Field class
 *
 */
 

#include "PlutoUtils/CommonIncludes.h"	
#include "Field.h"

#include "CommonFunctions.h"

#include <iostream>
#include <sstream>

#include "Database.h"

using namespace sqlCVS;

Field::Field(class Table *pTable, MYSQL_FIELD *pMYSQL_FIELD)
	:FieldInfo(pMYSQL_FIELD)
{
	m_pTable=pTable;

	m_bIsAutoIncrement = (m_iFlags & PRI_KEY_FLAG) && (m_iFlags & AUTO_INCREMENT_FLAG);

	m_pField_IReferTo_Directly=m_pField_IndirectSpecifier=NULL;
}


