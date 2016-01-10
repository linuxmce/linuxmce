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
 * @file R_GetRow.h
 * @brief header file for the R_GetRow class
 * @author
 *
 */
 
#ifndef R_GetRow_H
#define R_GetRow_H

#include "RA/RA_Request.h"
#include "sqlCVSrequests.h"
#include "CommonFunctions.h"

#define SERIALIZE_DATA_TYPE_CHANGED_ROW		1001

namespace sqlCVS
{
	class ChangedRow;
}

/**
 * @brief class modelling  commit row requests
 * @todo complete documentation
 */
 
class R_GetRow : public RA_Request
{
public:
	/** @brief Request Variables */
	int m_psc_id;
	string m_sTable;

	/** @brief Response Variables */
	MapStringString m_mapCurrentValues;
	string m_sResponseMessage; /** @brief In the event of a failure, this will contain descriptive information */

	/** @brief The server will call this constructor, then ProcessRequest */
	R_GetRow( string sTable, int psc_id );
	
	/** @brief constructor */
	R_GetRow( ) { m_psc_id=0; };

	/**
	 * @brief  Returns the id of the request
	 */
	virtual uint32_t ID( ) { return R_GET_ROW; }

	/**
	 * @brief Sets up the serialization request
	 */	
	virtual void SetupSerialization_Request( )
	{
		RA_Request::SetupSerialization_Request( );
		StartSerializeList( ) + m_psc_id + m_sTable;
	}

	/**
	 * @brief Sets up the serialization response
	 */	
	virtual void SetupSerialization_Response( )
	{
		RA_Request::SetupSerialization_Response( );
		StartSerializeList( ) + m_mapCurrentValues + m_sResponseMessage;
	}

	/**
	 * @brief This will return true if the request is processed properly
	 */	
	virtual bool ProcessRequest( class RA_Processor *pRA_Processor );
};


#endif

