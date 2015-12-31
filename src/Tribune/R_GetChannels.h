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
#ifndef R_GETCHANNELS__
#define R_GETCHANNELS__

#include "RA/RA_Request.h"
#include "Tribunerequests.h"

#include <string>
#include <vector>

using namespace std;


/**
 *in: lineup key and the blak list channels 
 *out: the channels for that lineup excluding the black list channels
 */

class R_GetChannels : public RA_Request
{
public:
	/** @brief Request Variables */
	int m_pk_headend;
	string m_extra_condition;

	/** @brief Response Variables */
	map<string,string> m_mapPrimaryKey_ChannelName;

	/** @brief constructor */
	R_GetChannels(int pk_headend, string extra_condition);

	/** @brief constructor */
	R_GetChannels( ) {};

	/**
	 * @brief Returns the id of @todo ask
	 */

	virtual unsigned long ID( ) { return R_GET_CHANNELS; }

	/**
	 * @brief Sets up the serialization request
	 */

	virtual void SetupSerialization_Request( )
	{
		RA_Request::SetupSerialization_Request( );
		StartSerializeList( ) + m_pk_headend + m_extra_condition;
	}

	/**
	 * @brief Sets up the serialization response
	 */

	virtual void SetupSerialization_Response( )
	{
		RA_Request::SetupSerialization_Response( );
		StartSerializeList( ) + m_mapPrimaryKey_ChannelName;
	}

	/**
	 * @brief This will say whether the request was processed successfully or not
	 */

	virtual bool ProcessRequest( class RA_Processor *pRA_Processor );
};


#endif

