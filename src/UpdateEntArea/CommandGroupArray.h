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
#ifndef CommandGroupArray_h
#define CommandGroupArray_h

#include "pluto_main/Table_EntertainArea.h"
#include "pluto_main/Table_Room.h"

class Row_CommandGroup;
class Row_EntertainArea;
class Row_Room;

namespace DefaultScenarios
{

struct TemplateParm
{
	int m_PK_Template,m_Parm1,m_Parm2;
	TemplateParm(int PK_Template,int Parm1,int Parm2) { m_PK_Template=PK_Template; m_Parm1=Parm1; m_Parm2=Parm2; }
};

class Command
{
public:
	class CommandGroup *m_pCommandGroup;
	int m_PK_Device,m_PK_Command,m_iOrder;
	map<int,string> m_mapParameters;

	Command(class CommandGroup *pCommandGroup, int PK_Device,int PK_Command,int iOrder) { m_pCommandGroup=pCommandGroup; m_PK_Device=PK_Device; m_PK_Command=PK_Command; m_iOrder=iOrder; }
	void Commit(Row_CommandGroup_Command *pRow_CommandGroup_Command);
};

class CommandGroup
{
public:
	Database_pluto_main *m_pDatabase_pluto_main;
	class CommandGroupArray *m_pCommandGroupArray;
	class Row_CommandGroup *m_pRow_CommandGroup;
	list<Command *> m_listCommand;
	bool m_bIsModified;

	CommandGroup(CommandGroupArray *pCommandGroupArray,Row_CommandGroup *pRow_CommandGroup);
	~CommandGroup();
	void Flush();  // Called just before it's deleted so we can flush changes, then call reset_psc_mod, then delete
	void AddCommand(int PK_Device,int PK_Command,int iOrder,int NumParms,...);
	void Commit();
};


// This holds an array (lighting/climate/media/etc.) of command groups.  If it is called
// with 'true' for the 'auto-delete extras', then when it goes out of scope if will
// not only commit any new CommandGroups added, but also delete any ones that have this
// array type in this room or Ent Area that were not re-added.  In other words, if there
// is a VCR commandgroup in EA=2 for Type=5 (Media) and this class is created for type 5
// with auto-delete=true, then if the command group for vcr isn't re-added, it will be
// deleted when this falls out of scope
class CommandGroupArray 
{

public:
	Row_EntertainArea *m_pRow_EntertainArea;
	Row_Room *m_pRow_Room;
    Database_pluto_main *m_pDatabase_pluto_main;
	bool m_bAutoDelete;
	int m_PK_Array,m_iPK_Installation;
	map< pair<int, pair<int,int> >,CommandGroup *> m_mapCommandGroup;
	CommandGroup *m_mapCommandGroup_Find(int PK_Template,int Parm1, int Parm2) { pair<int,int> p12 = make_pair<int,int> (Parm1,Parm2); map< pair<int, pair<int,int> >,CommandGroup *>::iterator it = m_mapCommandGroup.find( make_pair<int, pair<int,int> > ( PK_Template, p12 ) ); return it==m_mapCommandGroup.end() ? NULL : (*it).second; }

	CommandGroupArray(Row_EntertainArea *pRow_EntertainArea,int PK_Array,bool bAutoDelete=true)
	{
		m_pDatabase_pluto_main=pRow_EntertainArea->Table_EntertainArea_get()->Database_pluto_main_get();
		m_pRow_EntertainArea=pRow_EntertainArea;
		m_pRow_Room=NULL;
		m_PK_Array=PK_Array;
		m_bAutoDelete=bAutoDelete;
		m_iPK_Installation=pRow_EntertainArea->FK_Room_getrow()->FK_Installation_get();
	}

	CommandGroupArray(Row_Room *pRow_Room,int PK_Array,bool bAutoDelete=true)
	{
		m_pDatabase_pluto_main=pRow_Room->Table_Room_get()->Database_pluto_main_get();
		m_pRow_Room=pRow_Room;
		m_pRow_EntertainArea=NULL;
		m_PK_Array=PK_Array;
		m_bAutoDelete=bAutoDelete;
		m_iPK_Installation=m_pRow_Room->FK_Installation_get();
	}

	CommandGroupArray(Database_pluto_main *pDatabase_pluto_main,int PK_Installation,int PK_Array,bool bAutoDelete=true)
	{
		m_pDatabase_pluto_main=pDatabase_pluto_main;
		m_pRow_Room=NULL;
		m_pRow_EntertainArea=NULL;
		m_PK_Array=PK_Array;
		m_bAutoDelete=bAutoDelete;
		m_iPK_Installation=PK_Installation;
	}

	~CommandGroupArray();

	// If PK_CommandGroup is not NULL, that means the caller is only interested in the ID of an existing
	// command group.  In this case, if it exists, the value will be filled in, if it does not, then
	// this will exit returning NULL, and leave PK_CommandGroup=0
	CommandGroup *FindCommandGroupByTemplate(int PK_Template,string sDescription,int PK_Icon,int TemplateParm1,int TemplateParm2,int *PK_CommandGroup=NULL,int Sort=0,int PK_Text=0);
	CommandGroup *FindCommandGroupByTemplate(Row_Room *pRow_Room,int PK_Template,string sDescription,int PK_Icon,int TemplateParm1,int TemplateParm2,int *PK_CommandGroup,int Sort=0,int PK_Text=0);
	CommandGroup *FindCommandGroupByTemplate(Row_EntertainArea *pRow_EntertainArea,int PK_Template,string sDescription,int PK_Icon,int TemplateParm1,int TemplateParm2,int *PK_CommandGroup,int Sort=0,int PK_Text=0);
	CommandGroup *FindCommandGroupByTemplate_NoRoom(int PK_Template,string sDescription,int PK_Icon,int TemplateParm1,int TemplateParm2,int *PK_CommandGroup,int PK_Text=0);

	// The following versions just do a lookup.  No CommandGroup class is created, and AutoDelete is unaffected
	static Row_CommandGroup *FindCommandGroupByTemplate(Row_Room *pRow_Room,int PK_Array,int PK_Template,int TemplateParm1,int TemplateParm2);

	void DeleteCommandGroup(Row_CommandGroup *pRow_CommandGroup);
	void DeleteUnusedCommandGroups();
	void DeleteUnusedCommandGroups(Row_EntertainArea *pRow_EntertainArea);
	void DeleteUnusedCommandGroups(Row_Room *pRow_Room);
	void Reset_psc_mod(CommandGroup *pCommandGroup);
	bool CommandGroupIsModified(Row_CommandGroup *pRow_CommandGroup);

	void DeleteCommandGroup_Command(Row_CommandGroup_Command *pRow_CommandGroup_Command);

	int GetUnusedSortNo(int preferedOrder);
};

};

#endif
