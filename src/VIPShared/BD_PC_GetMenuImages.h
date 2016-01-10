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
#ifndef BD_PC_GetMenuImages_h
#define BD_PC_GetMenuImages_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"
#include "../SerializeClass/SerializeClass.h"
#include "MenuData.h"
#include "MenuItemInfo.h"

/*
 *
 *	BD_PC_GetMenuImages command - sends current phone configuration
 *
 */
class BD_PC_GetMenuImages : public BDCommand 
{
protected:	
	vector<MenuItemInfo> m_vItems;
	MenuItemInfo::ItemType m_nItemsType;

	void ClearItems();

public:
	BD_PC_GetMenuImages()  {}
	BD_PC_GetMenuImages( MenuItemInfo::ItemType nItemsType, vector<MenuItemInfo>& vItems );
	~BD_PC_GetMenuImages();

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_PC_GETMENUIMAGES; }
	virtual const char *Description() { return "GetMenuImages"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};

#endif
