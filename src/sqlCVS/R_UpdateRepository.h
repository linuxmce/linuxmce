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
 * @file R_UpdateRepository.h
 * @brief header file for the R_UpdateRepository class
 * @author
 *
 */
 
#ifndef R_UpdateRepository_H
#define R_UpdateRepository_H

#include "RA/RA_Request.h"
#include "sqlCVSrequests.h"

/**
 * @brief class modelling the update repository request
 */
 
class R_UpdateRepository : public RA_Request
{
public:
	/** @brief Request Variables */
	map<string, string> m_mapUsersPasswords; /**< All the users who are checking in this session */
	vector<string> m_vectTables; /**< The tables we will be checking in this session  */
	string m_sRepository;
	int m_iSchemaVersion;

	/** Response Variables */

	/** @brief constructor */
	
	R_UpdateRepository( string sRepository,int iSchemaVersion );
	
	/** @brief constructor */
	
	R_UpdateRepository( ) {};

	/**
	 * @brief  Returns the id of the updated repository
	 */
	 
	virtual uint32_t ID( ) { return R_UPDATE_REPOSITORY; }

	/**
	 * @brief Sets up the serialization request
	 */
	 	
	virtual void SetupSerialization_Request( )
	{
		RA_Request::SetupSerialization_Request( );
		StartSerializeList( ) + m_sRepository + m_iSchemaVersion;
	}

	/**
	 * @brief This will say whether the request was processed successfully or not
	 */
	 	
	virtual bool ProcessRequest( class RA_Processor *pRA_Processor );
};


#endif

