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
 * @file R_CommitRow.h
 * @brief header file for the R_CommitRow class
 * @author
 *
 */
 
#ifndef R_CommitRow_H
#define R_CommitRow_H

#include "RA/RA_Request.h"
#include "sqlCVSrequests.h"

#define SERIALIZE_DATA_TYPE_CHANGED_ROW		1001

namespace sqlCVS
{
	class ChangedRow;
}

/**
 * @brief class modelling  commit row requests
 * @todo complete documentation
 */
 
class R_CommitRow : public RA_Request
{
public:
	/** @brief Request Variables */
	int m_psc_id;
	int m_psc_batch;
	int m_psc_user;
	int m_psc_restrict;
	int m_iBeforeTransmit_iAutoIncrID;
	int m_eTypeOfChange;
	vector<string> m_vectValues;

	/** @brief Response Variables */
	int m_iNewAutoIncrID;
	int m_psc_id_new;
	int m_psc_batch_new;
	int m_psc_user_new;
	/** @brief Will be true if the row was frozen */
	bool m_bFrozen;
	/** @brief Will be non-zero if this person is not authorized to change the row.  This will be the id of the row's owner */
	int m_psc_user_needs_to_authorize;

	string m_sResponseMessage; /** @brief In the event of a failure, this will contain descriptive information */

	/** @brief The server will call this constructor, then ProcessRequest */
	R_CommitRow( sqlCVS::ChangedRow *pChangedRow );
	
	/** @brief constructor */
	R_CommitRow( ) { m_bFrozen=false; m_iNewAutoIncrID=m_psc_user_needs_to_authorize=m_psc_batch_new=m_psc_id_new=m_psc_user_new=0; };

	/**
	 * @brief  Returns the id of the request
	 */
	virtual uint32_t ID( ) { return R_COMMIT_ROW; }

	/**
	 * @brief Sets up the serialization request
	 */	
	virtual void SetupSerialization_Request( )
	{
		RA_Request::SetupSerialization_Request( );
		StartSerializeList( ) + m_psc_id + m_psc_batch + m_psc_user + m_psc_restrict
			+ m_iBeforeTransmit_iAutoIncrID + m_vectValues + m_eTypeOfChange;
	}

	/**
	 * @brief Sets up the serialization response
	 */	
	virtual void SetupSerialization_Response( )
	{
		RA_Request::SetupSerialization_Response( );
		StartSerializeList( ) + m_iNewAutoIncrID + m_psc_id_new + m_psc_batch_new + m_bFrozen + m_psc_user_needs_to_authorize + m_sResponseMessage + m_psc_user_new;
	}

	/**
	 * @brief This will return true if the request is processed properly
	 */	
	virtual bool ProcessRequest( class RA_Processor *pRA_Processor );
};


#endif

