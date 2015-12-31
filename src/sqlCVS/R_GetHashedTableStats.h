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
 * @file R_GetHashedTableStats.h
 * @brief header file for the R_GetHashedTableStats class
 * @author
 *
 */

#ifndef R_GetHashedTableStats_H
#define R_GetHashedTableStats_H

#include "RA/RA_Request.h"
#include "sqlCVSrequests.h"

/**
 * @brief  class modelling a GetAll_psc_id request
 * @todo complete documentation
 */

class R_GetHashedTableStats : public RA_Request
{
public:
	/** @brief Request Variables */
	string m_sTable;
	int m_iRange_Begin;
	int m_iRange_End;
	int m_iInterval_Length;

	vector<int> m_vectRestrictions; /** The restrictions we're interested in */

	/** @brief Response Variables */
	vector<pair<int,int> > m_hashedCount;  //
	vector<string> m_hashedCheckSum;  //

	/** @brief constructor */
	R_GetHashedTableStats(string sTable, vector<int> *p_vectRestrictions, int range_begin, int range_end, int interval_length);

	/** @brief constructor */
	R_GetHashedTableStats( ) {};

	/**
	 * @brief Returns the id of @todo ask
	 */

	virtual uint32_t ID( ) { return R_GET_HASHED_TABLE_STATS; }

	/**
	 * @brief Sets up the serialization request
	 */

	virtual void SetupSerialization_Request( )
	{
		RA_Request::SetupSerialization_Request( );
		StartSerializeList( ) + m_sTable + m_vectRestrictions + m_iRange_Begin + m_iRange_End + m_iInterval_Length;
	}

	/**
	 * @brief Sets up the serialization response
	 */

	virtual void SetupSerialization_Response( )
	{
		RA_Request::SetupSerialization_Response( );
		StartSerializeList( ) + m_hashedCount + m_hashedCheckSum;
	}

	/**
	 * @brief This will say whether the request was processed successfully or not
	 */

	virtual bool ProcessRequest( class RA_Processor *pRA_Processor );
};


#endif

