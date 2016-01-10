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
 * @file R_UpdateTable.h
 * @brief header file for the R_UpdateTable class
 * @author
 *
 */
 
#ifndef R_UpdateTable_H
#define R_UpdateTable_H

#include "RA/RA_Request.h"
#include "sqlCVSrequests.h"

/**
 * @brief class modelling the update table request
 */
 
class R_UpdateTable : public RA_Request
{
public:
	/** @brief Request Variables */
	string m_sTableName;
	vector<string> *m_pvectFields;
	vector<int> m_vect_psc_batch;
	int m_psc_batch_last_sync;
	int m_psc_id_last_sync;
	vector<int> m_vectRestrictions; /** The restrictions we're interested in */

	/** @brief Response Variables */

	bool m_bDeleteVectFields; /**< In case we create m_pvectFields ourselves */

	/** @brief The call will call this constructor, then ConvertRequestToBinary */
	
	R_UpdateTable( string sTableName, int psc_batch_last_sync, int psc_id_last_sync, vector<string> *pvectFields, vector<int> *p_vectRestrictions );

	/** @brief The server will call this constructor, then ProcessRequest */
	
	R_UpdateTable( );

	/** @brief destructor */
	
	~R_UpdateTable( );

	/**
	 * @brief returns the id of the updated table @todo ask
	 */
	 
	virtual uint32_t ID( ) { return R_UPDATE_TABLE; }

	/**
	 * @brief Sets up the serialization request
	 */
	 
	void SetupSerialization_Request( )
	{
		RA_Request::SetupSerialization_Request( );
		StartSerializeList( ) + m_sTableName + *m_pvectFields + m_psc_batch_last_sync + m_psc_id_last_sync + m_vect_psc_batch + m_vectRestrictions;
	}

	/**
	 * @brief  Sets up the serialization response
	 */
	 
	void SetupSerialization_Response( )
	{
		RA_Request::SetupSerialization_Response( );
		StartSerializeList( ) + m_psc_batch_last_sync + m_psc_id_last_sync;
	}

	/**
	 * @brief This will say whether the request was processed successfully or not
	 */
	 
	virtual bool ProcessRequest( class RA_Processor *pRA_Processor );
};


#endif

