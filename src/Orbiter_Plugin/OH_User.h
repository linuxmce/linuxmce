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
#ifndef OH_User_h
#define OH_User_h


namespace DCE
{
	// Store an orbiter list within the handler
	class OH_User
	{
	public:
		int m_iPK_Users;
		bool m_bFollowMe_Lighting,m_bFollowMe_Media,m_bFollowMe_Climate,m_bFollowMe_Telecom,m_bFollowMe_Security;

		OH_User(int iPK_Users)
		{
			m_iPK_Users = iPK_Users;
			m_bFollowMe_Lighting=m_bFollowMe_Media=m_bFollowMe_Climate=m_bFollowMe_Telecom=m_bFollowMe_Security=false;
		}
	};
}

#endif
