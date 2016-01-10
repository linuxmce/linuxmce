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
 * @file sqlCVSrequests.h
 * @brief header file  @todo ask
 * @author
 *
 */

#ifndef sqlCVSrequests_h
#define sqlCVSrequests_h

/**
 * @brief Implement the mandatory BuildRequestFromData for use with PlutoVIP
 */

#define R_COMMIT_CHANGES		1
#define R_COMMIT_TABLE			2
#define R_COMMIT_ROW			3
#define R_GET_ALL_PSC_ID		4
#define R_UPDATE_REPOSITORY		5
#define R_UPDATE_TABLE			6
#define R_CLOSE_TRANSACTION		7
#define R_APPROVE_BATCH			8
#define R_GET_ROW				9

#define R_GET_HASHED_TABLE_STATS		10
#define R_GET_CONDITIONAL_PSC_ID		11


#define A_UPDATE_ROW			1
#define A_UPDATE_SCHEMA			2

#endif
