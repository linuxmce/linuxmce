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
#ifndef BD_CP_SetMenuImages_h
#define BD_CP_SetMenuImages_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"
#include "MenuData.h"
#include "MenuItemInfo.h"

/*
 *
 *	BD_CP_SetMenuImages command - sets new background on smartphone
 *
 */
class BD_CP_SetMenuImages : public BDCommand
{
protected:
	vector<MenuItemInfo*> m_vItems;
	MenuItemInfo::ItemType m_nItemsType;	

	void ClearItems();

public:
	BD_CP_SetMenuImages()  { m_nItemsType = MenuItemInfo::stNone; }
	BD_CP_SetMenuImages( MenuItemInfo::ItemType nItemsType, vector<MenuItemInfo>& vItems );

	~BD_CP_SetMenuImages();

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_CP_SETMENUIMAGES; }

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);

	virtual const char *Description() { return "Set menu images"; };
};
#endif
