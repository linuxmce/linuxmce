/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU Lesser General Public License for more details.

*/

/** @file CorpServerDefines.h
 The DCE Definitions.
 */
#ifndef __CORP_SERVER_DEFINES_H__0983475
#define __CORP_SERVER_DEFINES_H__0983475


#define CORPSERVER_COMMAND_UPDATE_EPG 		1
#define CORPSERVER_COMMAND_UPDATE_CONFIG 	2
#define CORPSERVER_COMMAND_UPDATE_SOFTWARE 	3
#define CORPSERVER_COMMAND_RESTART	 	4
#define CORPSERVER_COMMAND_SEND_LOG	 	5


#define CORPSERVER_EVENT_SHUTDOWN	 	6
#define CORPSERVER_EVENT_STARTUP 		7
#define CORPSERVER_EVENT_PACKAGEVERSION		8
#define CORPSERVER_EVENT_PING			9

#define CORPSERVER_PARAM_STATUS			1
#define CORPSERVER_PARAM_NAME			2
#define CORPSERVER_PARAM_VERSION		3
#define CORPSERVER_PARAM_IP			4

#define CORPSERVER_STATUS_OK			1
#define CORPSERVER_STATUS_ERROR			2
#define CORPSERVER_STATUS_RELOAD		3
#define CORPSERVER_STATUS_SOCKETFAILED		4
#define CORPSERVER_STATUS_RECONNECT		5
#define CORPSERVER_STATUS_UNKNOWN		6

#endif
