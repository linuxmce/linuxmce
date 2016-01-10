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
#ifndef __MediaBrowserMouseHandler_H__
#define __MediaBrowserMouseHandler_H__
//-----------------------------------------------------------------------------------------------------
#include <list>
#include <string>
#include <map>
using namespace std;

#include "DesignObj_Orbiter.h"
#include "Orbiter.h"
#include "PlutoUtils/ProcessUtils.h"
#include "MouseBehavior.h"
#include "DatagridMouseHandlerHelper.h"
#include "HorizMenuMouseHandler.h"
using namespace DCE;

#define MAX_SPEEDS		17

namespace DCE
{
	//-----------------------------------------------------------------------------------------------------
	/**
	* @brief Handles special mouse behavior for the media browser
	*/
	//-----------------------------------------------------------------------------------------------------
	class MediaBrowserMouseHandler : public MouseHandler
	{
	public://temptest
		DatagridMouseHandlerHelper *m_pDatagridMouseHandlerHelper;
		HorizMenuMouseHandler *m_pHorizMenuMouseHandler;  // This also has a horiz menu, so we'll pass commands on here when needed
		friend class ScreenHandler;
		int m_RelativeVirtualY;
		DesignObj_DataGrid *m_pObj_ListGrid,*m_pObj_PicGrid;
		DesignObj_Orbiter *m_pObj_CoverArtPopup,*m_pObj_Sort,*m_pObj_More;
		int m_LastRow;
		PlutoRectangle m_rSortFilterMenu;

	public:
		MediaBrowserMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior);
		virtual ~MediaBrowserMouseHandler();
		virtual EMouseHandler TypeOfMouseHandler() { return mh_MediaBrowser; }

		void Start();
		void Stop();

		bool ButtonDown(int PK_Button);
		bool ButtonUp(int PK_Button);
		void Move(int X,int Y,int PK_Direction);
		void ShowCoverArtPopup();
    };

}
//-----------------------------------------------------------------------------------------------------
#endif
