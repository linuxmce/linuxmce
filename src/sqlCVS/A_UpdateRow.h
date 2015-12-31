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
 * @file A_UpdatedRow.h
 * @brief header file for the A_UpdatedRow class
 * @author
 *
 */

#ifndef A_UpdateRow_H
#define A_UpdateRow_H

#include "RA/RA_Action.h"
#include "sqlCVSrequests.h"

/**
 * @brief class modelling the update row action
 */
 
class A_UpdateRow : public RA_Action
{
public:
	/** Request Variables */
	int m_psc_id;
	int m_psc_batch;
	int m_psc_user;
	vector<string> m_vectValues;

	/** The server will call this constructor, then ProcessRequest */
	A_UpdateRow(int psc_id, int psc_batch, int psc_user) { m_psc_id=psc_id; m_psc_batch=psc_batch; m_psc_user=psc_user;};
	
	/**
	 * @brief constructor
	 */
	A_UpdateRow() {};
	
	/**
	 * @brief returns the id of the updated row
	 */

	virtual uint32_t ID() { return A_UPDATE_ROW; }
	
	/**
	 * @brief sets up the serialization
	 */

	virtual void SetupSerialization(int iSC_Version)
	{
		RA_Action::SetupSerialization(iSC_Version);
		StartSerializeList() + m_psc_id + m_psc_batch + m_psc_user +
			m_vectValues;
	}

	/**
	 * @brief This will process the update row action
	 */
	 	
	virtual void ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor);
};


#endif

