/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/
/**
@file DCEMySqlConfig.h
Header file for the DCEMySqlConfig class

@todo notcommented
*/
#ifndef __DCEMYSQLCONFIG_H__
#define __DCEMYSQLCONFIG_H__
//------------------------------------------------------------------------------------------------------
#include "DCEConfig.h"
#include "PlutoUtils/MultiThreadIncludes.h"
//------------------------------------------------------------------------------------------------------
#include "db_wrapper/db_wrapper.h"
//------------------------------------------------------------------------------------------------------

/**
@class DCEMySqlConfig
This derived class is for ???
*/
class DCEMySqlConfig : public DCEConfig
{
protected:
	pluto_pthread_mutex_t m_MySqlMutex;
	DB_LINK m_MySQL;

public:
	int m_iTransNumber; //maybe we should derive  a class from DCEMySqlConfig with VIPServer specifics (?)

public:
	DCEMySqlConfig(string sFilename="/etc/pluto.conf");
	virtual ~DCEMySqlConfig();

	bool MysqlConnect(bool bReset);
	DB_RES *MySqlQueryResult(string sQuery);
	long ThreadedMysqlQuery(string sQuery, bool bIgnoreErrors = false);
	long ThreadedMysqlQueryWithID(string sQuery);
};
//------------------------------------------------------------------------------------------------------
#endif //__DCEMYSQLCONFIG_H__
