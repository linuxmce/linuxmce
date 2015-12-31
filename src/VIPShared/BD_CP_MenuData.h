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
#ifndef BD_CP_MenuData_h
#define BD_CP_MenuData_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

#if defined(SMARTPHONE) || defined(BLUETOOTH_DONGLE)
	#include "MenuData.h"
#else
	class MenuData { /*not implemented!*/ };
#endif


/*
 *
 *	BD_CP_MenuData command - sends menu items and drawing info for local rendered menus
 *
 */
class BD_CP_MenuData : public BDCommand
{
protected:
	MenuData m_Data;
public:
	BD_CP_MenuData()  {}

	BD_CP_MenuData( MenuData& data );

	virtual ~BD_CP_MenuData();

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size, const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_CP_MENU_DATA; }
	virtual const char *Description() { return "Menu Data"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};
#endif
