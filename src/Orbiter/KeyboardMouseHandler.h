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
#ifndef __KeyboardMouseHandler_H__
#define __KeyboardMouseHandler_H__
//-----------------------------------------------------------------------------------------------------
#include <list>
#include <string>
#include <map>
#include <memory>
using namespace std;

#include "DesignObj_Orbiter.h"
#include "Orbiter.h"
#include "PlutoUtils/ProcessUtils.h"
#include "MouseBehavior.h"
#include "DatagridMouseHandlerHelper.h"

using namespace DCE;

namespace DCE
{
	//-----------------------------------------------------------------------------------------------------
	/**
	* @brief Handles special mouse behavior for lighting control
	*/
	//-----------------------------------------------------------------------------------------------------
	class KeyboardMouseHandler : public MouseHandler
	{
		bool m_b1Step; // If true, do not do continuous motion, only 1 step at a time
		char m_cDirection;
		int m_PK_Direction_Last;
		std::auto_ptr<DatagridMouseHandlerHelper> m_spDatagridMouseHandlerHelper;

	public:
		KeyboardMouseHandler(DesignObj_Orbiter *pObj,string sOptions,MouseBehavior *pMouseBehavior);
		~KeyboardMouseHandler() {}
		virtual EMouseHandler TypeOfMouseHandler() { return mh_Keyboard; }

		void Start();
		void Stop();

		bool ButtonDown(int PK_Button);
		bool ButtonUp(int PK_Button);
		void Move(int X,int Y,int PK_Direction);
		bool SlowDrift(int &X,int &Y); // We're about to call a move after the user has been slowly drifting.  The handler can alter the position, and/or return true to ignore the move
		void MoveExternalApp(int X,int Y);
		void MoveExternalApp1Step(int X,int Y);

		void DoIteration();
		void IterateObject();
		void IterateExternalApp();

friend class Orbiter; // Temporary hack
void TempHack_DrawAlphaSquare();
	};
}
//-----------------------------------------------------------------------------------------------------
#endif
