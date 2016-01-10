/*
 ChangedRow
 
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
 * @file ChangedRow.cpp
 * @brief source file for the Dependency, DependetRow and ChangedRow classes
 *
 */
 
 
#include "PlutoUtils/CommonIncludes.h"	
#include "ChangedRow.h"	
#include "Table.h"
#include "Field.h"
#include <sstream>

using namespace sqlCVS;

string ChangedRow::GetWhereClause()
{
	std::ostringstream sWhere;
	/**
	 * If it's not a new row, it will have m_psc_id assigned
	 */
	 
	cout << "where clause for psc_id: " << m_psc_id << " has keys:" << (int) m_vectPrimaryKey.size() << endl;

	if( m_psc_id )
		sWhere << " WHERE psc_id=" << m_psc_id;
	else
	{
		long Count=0;
		for(ListField::iterator it=m_pTable->m_listField_PrimaryKey.begin();it!=m_pTable->m_listField_PrimaryKey.end();++it)
		{
			Field *pField = *it;
			if( Count )
				sWhere << " AND ";
			else
				sWhere << " WHERE ";

			sWhere << " `" << pField->Name_get() << "`=";

			// The ChangedRows are built at the beginning of a check-in.  During the check-in the auto-increment
			// values may have changed, and this primary key may include a foreign key to one of those fields.
			// If so, be sure to get the new value, because the PropagateUpdates will have already altered this one.
			if( pField->m_pField_IReferTo_Directly && 
				pField->m_pField_IReferTo_Directly->m_pTable->m_pField_AutoIncrement &&
				pField->m_pField_IReferTo_Directly->m_pTable->m_mapUncommittedAutoIncrChanges.find( atoi(m_vectPrimaryKey[Count].c_str()) ) != pField->m_pField_IReferTo_Directly->m_pTable->m_mapUncommittedAutoIncrChanges.end() )
			{
cout << "Changed row translating w/ m_mapUncommittedAutoIncrChanges original: " << m_vectPrimaryKey[Count] << " now: " << pField->m_pField_IReferTo_Directly->m_pTable->m_mapUncommittedAutoIncrChanges[ atoi(m_vectPrimaryKey[Count].c_str())] << endl;
				sWhere << pField->m_pField_IReferTo_Directly->m_pTable->m_mapUncommittedAutoIncrChanges[ atoi(m_vectPrimaryKey[Count].c_str())];
				Count++;
			}
			else
				sWhere << "'" << StringUtils::SQLEscape(m_vectPrimaryKey[Count++]) << "'";
		}
	}
	return sWhere.str();
}