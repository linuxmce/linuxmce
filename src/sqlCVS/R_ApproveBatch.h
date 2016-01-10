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
 * @file R_ApproveBatch.h
 * @brief header file for the R_ApproveBatch class
 * @author
 *
 */
 
#ifndef R_ApproveBatch_H
#define R_ApproveBatch_H

#include "RA/RA_Request.h"
#include "sqlCVSrequests.h"

/**
 * @brief class modelling commit changes requests
 */
 
class R_ApproveBatch : public RA_Request
{
public:
	/** @brief Request Variables */
	map<string, string> m_mapUsersPasswords; /**< All the users who are checking in this session */
	int m_psc_batch;
	string m_sRepository;
	bool m_bReject;

	/** @brief Response Variables */
	string m_sMessage;

	/** @brief constructors */
	R_ApproveBatch( string sRepository, int psc_batch, bool bReject );
	R_ApproveBatch( ) {};

	/**
	 * @brief Returns the id of the request @todo ask
	 */
	virtual uint32_t ID( ) { return R_APPROVE_BATCH; }

	/**
	 * @brief Sets up the serialization request
	 */
	virtual void SetupSerialization_Request( )
	{
		RA_Request::SetupSerialization_Request( );
		StartSerializeList( ) + m_sRepository + m_psc_batch + m_mapUsersPasswords + m_bReject;
	}

	virtual void SetupSerialization_Response( )
	{
		RA_Request::SetupSerialization_Response( );
		StartSerializeList( ) + m_sMessage ;
	}
	
	/**
	 * @brief  This will say whether the request was processed successfully or not
	 */
	virtual bool ProcessRequest( class RA_Processor *pRA_Processor );
};


#endif

