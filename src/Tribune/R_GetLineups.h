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
/**
 *
 * @file R_GetConditional_psc_id.h
 * @brief header file for the R_GetConditional_psc_id class
 * @author
 *
 */

#ifndef R_GETLINEUPS__
#define R_GETLINEUPS__

#include "RA/RA_Request.h"
#include "Tribunerequests.h"

#include <string>
#include <vector>

//using namespace std;


/**
 *in: table to use and the zip code
 *out: all the lineups coresponding to that zip code
 */


class R_GetLineups : public RA_Request
{
public:
	/** @brief Request Variables */
	string m_sTable;
	int m_zipcode;

	/** @brief Response Variables */
	map<string,string> m_mapPrimaryKey_LineupName;  // psc_id + batch

	/** @brief constructor */
	R_GetLineups(string sTable, int zipcode);

	/** @brief constructor */
	R_GetLineups( ) {};

	/**
	 * @brief Returns the id of @todo ask
	 */

	virtual unsigned long ID( ) { return R_GET_LINEUPS; }

	/**
	 * @brief Sets up the serialization request
	 */

	virtual void SetupSerialization_Request( )
	{
		RA_Request::SetupSerialization_Request( );
		StartSerializeList( ) + m_sTable + m_zipcode;
	}

	/**
	 * @brief Sets up the serialization response
	 */

	virtual void SetupSerialization_Response( )
	{
		RA_Request::SetupSerialization_Response( );
		StartSerializeList( ) + m_mapPrimaryKey_LineupName;
	}

	/**
	 * @brief This will say whether the request was processed successfully or not
	 */

	virtual bool ProcessRequest( class RA_Processor *pRA_Processor );
};


#endif

