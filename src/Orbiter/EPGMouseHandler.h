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
#ifndef __EPGMouseHandler_H__
#define __EPGMouseHandler_H__
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

using namespace DCE;

#define MAX_SPEEDS		17

namespace DCE
{
	//-----------------------------------------------------------------------------------------------------
	/**
	* @brief Handles special mouse behavior for lighting control
	*/
	//-----------------------------------------------------------------------------------------------------
	class EPGMouseHandler : public MouseHandler
	{
		DatagridMouseHandlerHelper *m_pDatagridMouseHandlerHelper;
		DesignObj_DataGrid *m_pDesignObj_DataGrid_Active;
		DesignObj_DataGrid *m_pDesignObj_UpcomingChannels;
		DesignObj_Orbiter *m_pObj_QuickNav,*m_pObj_Bookmark_Channel,*m_pObj_Bookmark_Show,*m_pObj_Record_Once,*m_pObj_Record_All;
		DesignObjText *m_pDesignObjText_Synopsis;
		int m_iRow_Last;

	public:
		EPGMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior);
		virtual EMouseHandler TypeOfMouseHandler() { return mh_Media; }
		virtual ~EPGMouseHandler();

		void Start();
		void Stop();

		bool ButtonDown(int PK_Button);
		bool ButtonUp(int PK_Button);
		void Move(int X,int Y,int PK_Direction);
	};

}
//-----------------------------------------------------------------------------------------------------
#endif
