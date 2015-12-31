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

#ifndef R_GetConditional_psc_id_H
#define R_GetConditional_psc_id_H

#include "RA/RA_Request.h"
#include "sqlCVSrequests.h"

/**
 * @brief  class modelling a GetAll_psc_id request
 * @todo complete documentation
 */

class R_GetConditional_psc_id : public RA_Request
{
public:
	/** @brief Request Variables */
	string m_sTable;
	int m_psc_id,m_psc_batch;
	vector<int> m_vectRestrictions; /** The restrictions we're interested in */
	string m_sExtraCondition;	/** extra conditions for filtering */

	/** @brief Response Variables */
	vector< pair<int,int> > m_vectAll_psc_id;  // psc_id + batch

	/** @brief constructor */
	R_GetConditional_psc_id(string sTable, vector<int> *p_vectRestrictions, string extraCondition="");

	/** @brief constructor */
	R_GetConditional_psc_id( ) {};

	/**
	 * @brief Returns the id of @todo ask
	 */

	virtual uint32_t ID( ) { return R_GET_CONDITIONAL_PSC_ID; }

	/**
	 * @brief Sets up the serialization request
	 */

	virtual void SetupSerialization_Request( )
	{
		RA_Request::SetupSerialization_Request( );
		StartSerializeList( ) + m_sTable + m_vectRestrictions + m_sExtraCondition;
	}

	/**
	 * @brief Sets up the serialization response
	 */

	virtual void SetupSerialization_Response( )
	{
		RA_Request::SetupSerialization_Response( );
		StartSerializeList( ) + m_vectAll_psc_id;
	}

	/**
	 * @brief This will say whether the request was processed successfully or not
	 */

	virtual bool ProcessRequest( class RA_Processor *pRA_Processor );
};


#endif

