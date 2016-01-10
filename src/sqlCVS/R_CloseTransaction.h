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
 * @file R_CloseTransaction.h
 * @brief header file for the R_CloseTransaction class
 * @author
 *
 */
 
#ifndef R_CloseTransaction_H
#define R_CloseTransaction_H

#include "RA/RA_Request.h"
#include "sqlCVSrequests.h"


/**
 * @brief class modelling  commit row requests
 * @todo complete documentation
 */
 
class R_CloseTransaction : public RA_Request
{
public:
	/**
	 * @brief  Returns the id of the request
	 */
	virtual uint32_t ID( ) { return R_CLOSE_TRANSACTION; }

	/**
	 * @brief This will return true if the request is processed properly
	 */	
	virtual bool ProcessRequest( class RA_Processor *pRA_Processor );
};


#endif

