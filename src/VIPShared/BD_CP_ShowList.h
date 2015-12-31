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
#ifndef BD_CP_ShowList_h
#define BD_CP_ShowList_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

class BD_CP_ShowList : public BDCommand
{
	unsigned long m_x, m_y, m_Width, m_Height;
	bool m_bSendSelectedOnMove, m_bTurnOn;
	unsigned long m_SelectedIndex;

#ifndef SYMBIAN
	#ifdef SMARTPHONE 
		vector<string> m_vectDataGrid;
	#else
		list<string> m_DataGridList;
	#endif
#else
	RPointerArray<string> m_DataGridList;
#endif //SYMBIAN

public:
	BD_CP_ShowList()  {}

#ifndef SYMBIAN
	BD_CP_ShowList(
		unsigned long x, 
		unsigned long y, 
		unsigned long Width, 
		unsigned long Height,
		unsigned long SelectedIndex,
		list<string>  &DataGridList,
		bool		  bSendSelectedOnMove,
		bool		  bTurnOn
	);
#endif //SYMBIAN

	virtual ~BD_CP_ShowList();

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size, const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_CP_SHOW_LIST; }
	virtual const char *Description() { return "Show List"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};
#endif
