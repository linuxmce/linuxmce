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
 * @file A_UpdateSchema.h
 * @brief header file for the A_UpdateSchema class
 * @author
 *
 */

#ifndef A_UpdateSchema_H
#define A_UpdateSchema_H

#include "RA/RA_Action.h"
#include "sqlCVSrequests.h"

/**
 * @brief class modelling the update row action
 */
 
class A_UpdateSchema : public RA_Action
{
public:
	/** Request Variables */
	int m_iSchemaVersion;
	string m_sUpdateString,m_sRepository;

	/** The server will call this constructor, then ProcessRequest */
	A_UpdateSchema(string sRepository, int SchemaVersion,string sUpdateString) { m_sRepository=sRepository; m_iSchemaVersion=SchemaVersion; m_sUpdateString=sUpdateString; };
	
	/**
	 * @brief constructor
	 */
	A_UpdateSchema() {};
	
	/**
	 * @brief returns the id of the updated row
	 */

	virtual uint32_t ID() { return A_UPDATE_SCHEMA; }
	
	/**
	 * @brief sets up the serialization
	 */

	virtual void SetupSerialization(int iSC_Version)
	{
		RA_Action::SetupSerialization(iSC_Version);
		StartSerializeList() + m_iSchemaVersion + m_sUpdateString + m_sRepository;
	}

	/**
	 * @brief This will process the update row action
	 */
	 	
	virtual void ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor);
};


#endif

