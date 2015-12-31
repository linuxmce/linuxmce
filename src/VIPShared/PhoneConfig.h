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
#ifndef PlutoConfig_H
#define PlutoConfig_H

/*
 *
 *	PhoneConfig - current phone configuration
 *
 */
class PhoneConfig {
public:
	typedef enum { mmImage=0, mmMenu } MenuMode;
protected:	
	MenuMode m_MenuMode;
public:	
	PhoneConfig() { m_MenuMode = mmImage; }
	PhoneConfig( PhoneConfig& cfg ) { (*this)=cfg; }
	PhoneConfig& operator=( PhoneConfig& cfg ) { 
		m_MenuMode = cfg.m_MenuMode;
		return *this;
	}

	//---- MenuMode : mmMenu = local rendered menus ; mmImage = menu images received from Bluetooth_Dongle
	MenuMode GetMenuMode( ) { return m_MenuMode; }
	void SetMenuMode( MenuMode val ) { m_MenuMode = val; }
	//----

	void Write( SerializeClass& sc ){
		sc.Write_long( m_MenuMode );
	}
	void Read( SerializeClass& sc )
	{
		m_MenuMode = (MenuMode)sc.Read_long();
	}

};

#endif 

