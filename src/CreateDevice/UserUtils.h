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
#ifndef UserUtils_h
#define UserUtils_h

#include "PlutoUtils/DBHelper.h"

namespace DCE
{
	class Command_Impl;
};

class UserUtils
{

#define FIRST_USER_EXTENSION	301
#define LAST_USER_EXTENSION		399

	DBHelper *m_pDBHelper;
	int m_PK_Installation;

public:
	UserUtils(DBHelper *pDBHelper, int PK_Installation);
	bool AlreadyHasMasterUsers();
	string GetGoodExtension(map<int,bool> &mapUsedExtensions) ;
	void CheckExtensions();
	int AddUser(string sUsername,DCE::Command_Impl *pCommand_Impl=NULL);
	void RemoveUser(int PK_Users);
	void ConfirmMasterUserExists(); // Confirm there's always at least one master user
};

#endif
