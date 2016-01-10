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
#ifndef __ZORDER_H__
#define __ZORDER_H__

#include "utilities/linux/window_manager/WMController/WindowLayer.h"
#include "pluto_main/Define_DesignObj.h"

//temporary : to be defined in the database
#define DESIGNOBJTYPE_Orbiter_Popup_CONST

class ZOrder
{
	static WindowLayer GetLayer(int nObjectType)
	{
		if(Simulator::GetInstance()->m_nUI == 4)
		{
			switch(nObjectType)
			{
				case DESIGNOBJTYPE_App_Desktop_CONST:
					return LayerBelow;				
				case DESIGNOBJTYPE_wxWidgets_Applet_CONST:
					return LayerNormal;
				case DESIGNOBJTYPE_Orbiter_Popup_CONST
					return LayerAbove;
					
				default:
					return LayerBelow;
			}
		}
		else
		{
			switch(nObjectType)
			{
				case DESIGNOBJTYPE_App_Desktop_CONST:
					return LayerNormal;
				case DESIGNOBJTYPE_wxWidgets_Applet_CONST:
					return LayerNormal;
				case DESIGNOBJTYPE_Orbiter_Popup_CONST
					return LayerAbove;
					
				default:
					return LayerBelow;
			}			
		}
	}
	
	return LayerUnknown;
};

#endif