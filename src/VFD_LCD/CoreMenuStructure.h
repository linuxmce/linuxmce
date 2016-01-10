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
#ifndef CoreMenuStructure_h
#define CoreMenuStructure_h

#include "MenuStructure.h"
#include "MenuNode.h"

namespace DCE
{
	class CoreMenuStructure : public MenuStructure
	{
	public:
		CoreMenuStructure(class VFD_LCD_Base *pVFD_LCD_Base); 
	};

	class CPUTemperature : public MenuNode
	{
	public:
		CPUTemperature(MenuStructure *pMenuStructure,string sText) : MenuNode(pMenuStructure,sText) {}
		virtual string GetText() { return "CPU is 98 degrees"; }
	};

	class ActiveProcesses : public MenuNode
	{
	public:
		ActiveProcesses(MenuStructure *pMenuStructure,string sText) : MenuNode(pMenuStructure,sText) {}
		virtual void PrepareNodes();
		virtual bool CanGoRight() { return true; };
	};

	class ActiveProcess : public MenuNode
	{
		int m_PID;
	public:
		ActiveProcess(int PID,MenuStructure *pMenuStructure,string sText);
		virtual void Enter();
		virtual bool CanGoRight() { return true; };
	};
}
#endif
