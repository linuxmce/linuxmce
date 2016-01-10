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
#ifndef __LightMouseHandler_H__
#define __LightMouseHandler_H__
//-----------------------------------------------------------------------------------------------------
#include <list>
#include <string>
#include <map>
using namespace std;

#include "DesignObj_Orbiter.h"
#include "Orbiter.h"
#include "PlutoUtils/ProcessUtils.h"
#include "MouseBehavior.h"
using namespace DCE;

namespace DCE
{
	//-----------------------------------------------------------------------------------------------------
	/**
	* @brief Handles special mouse behavior for lighting control
	*/
	//-----------------------------------------------------------------------------------------------------
	class LightMouseHandler : public MouseHandler
	{
	public:
		int m_iLastGoodPosition;

		LightMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior) : MouseHandler(pObj,sOptions,pMouseBehavior) {}
		virtual EMouseHandler TypeOfMouseHandler() { return mh_Light; }

		void Start();
		void Stop();

		bool ButtonDown(int PK_Button);
		bool ButtonUp(int PK_Button);
		void Move(int X,int Y,int PK_Direction);
		bool SlowDrift(int &X,int &Y);  // We're about to call a move after the user has been slowly drifting.  The handler can alter the position, and/or return true to ignore the move

		void DoIteration(int Parm);
		void CustomRender();
	};

}
//-----------------------------------------------------------------------------------------------------
#endif
