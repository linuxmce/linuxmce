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
#ifndef DESIGNOBJ_DATA_H
#define DESIGNOBJ_DATA_H

#include "SerializeClass/SerializeClass.h"
#include "SerializeClass/ShapesColors.h"
#include "TextStyle.h"

#include <map>
#include <list>
using namespace std;

// We've got some custom data types to serialize
#define SERIALIZE_DATA_TYPE_INT_DESIGNOBJACTIONLIST		1000
#define SERIALIZE_DATA_TYPE_INT_DESIGNOBJZONELIST		1001
#define SERIALIZE_DATA_TYPE_VECT_DESIGNOBJTEXT			1002
#define SERIALIZE_DATA_TYPE_INT_DESIGNOBJCHILDLIST		1003
#define SERIALIZE_DATA_TYPE_INT_STYLEMAP				1004


typedef list<class DesignObjCommandParameter *> DesignObjCommandParameterList;
typedef list<class DesignObjCommand *> DesignObjCommandList;
typedef list<class DesignObjZone *> DesignObjZoneList;

class DesignObjCommand : public SerializeClass
{
public:
	long m_PK_Command,m_PK_Device,m_PK_DeviceGroup,m_PK_DeviceTemplate,m_PK_DeviceCategory; //,PK_C_ExecTime;
	int BroadcastLevel;
	bool m_bRelativeToSender,m_bDeliveryConfirmation;

	map<int, string> m_ParameterList;
	class Orbiter_CriteriaList *ptrCriteria;

	DesignObjCommand() : ptrCriteria(NULL) {};
	DesignObjCommand(long PK_Command,long PK_Criteria,bool bHandleLocally,long PK_Device,long PK_DeviceGroup, bool bDeliveryConfirmation) 
		: ptrCriteria(NULL) 
	{
		m_PK_Command=PK_Command;
		m_PK_Device=PK_Device;
		m_PK_DeviceGroup=PK_DeviceGroup;
		m_bDeliveryConfirmation=bDeliveryConfirmation;
	};
	virtual ~DesignObjCommand() { m_ParameterList.clear(); } 

	virtual string SerializeClassClassName() { return "DesignObjCommand"; }

	virtual bool Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData=NULL);
}; 

class DesignObjZone : public SerializeClass
{
public:
	DesignObjZone(int X,int Y,int Width,int Height) 
	{
		m_Rect.X=X; m_Rect.Y=Y; m_Rect.Width=Width; m_Rect.Height=Height;
		
	};
	DesignObjZone() { }
	~DesignObjZone()
	{
		DesignObjCommandList::iterator iA;

		for(iA = m_Commands.begin(); iA != m_Commands.end(); ++iA)
			delete *iA;
		
		m_Commands.clear();
	}
	PlutoRectangle m_Rect;
	DesignObjCommandList m_Commands;

	virtual string SerializeClassClassName() { return "DesignObjZone"; }

	virtual bool Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData=NULL);
}; 

typedef list<class DesignObj_Data *> DesignObj_DataList;
typedef map<string, class DesignObj_Data*> DesignObj_DataMap;

class DesignObjText : public SerializeClass
{
public:
	map<int,string> m_mapAltVersions;
	PlutoRectangle m_rPosition;
	bool m_bPreRender;
	int m_PK_Text;
	int m_iLastXOffset,m_iLastYOffset;  // Used only within the Orbiter
	string m_sText;
	int m_iPK_VertAlignment,m_iPK_HorizAlignment;
	MapTextStyle m_mapTextStyle;
	TextStyle *m_mapTextStyle_Find(int Version)
	{
		MapTextStyle::iterator it = m_mapTextStyle.find(Version);
		return it==m_mapTextStyle.end() ? NULL : (*it).second;
	}

	class DesignObj_Orbiter *m_pObject;

	DesignObjText(class DesignObj_Orbiter *pObj) 
	{ 
		m_pObject=pObj; m_iLastXOffset=m_iLastYOffset=0; m_iPK_VertAlignment=m_iPK_HorizAlignment=0;
	}
	DesignObjText() { m_pObject = NULL; m_iPK_VertAlignment=m_iPK_HorizAlignment=0; }
	
	virtual ~DesignObjText() 
	{  
		m_pObject = NULL; //someone else should detele the object

		MapTextStyle::iterator it;

		for(it = m_mapTextStyle.begin(); it != m_mapTextStyle.end(); it++)
		{
			delete (*it).second;
			(*it).second = NULL;
		}
		m_mapTextStyle.clear();
	};

	virtual string SerializeClassClassName() { return "DesignObjText"; }
	virtual bool Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData=NULL);
};

typedef vector<DesignObjText *> VectorDesignObjText;

class DesignObj_Data : public SerializeClass
{
public:
	// Our data
	bool m_bChild,m_bDontResetState,m_bCantGoBack,m_bCanGoBackToSameScreen,m_bChildrenBeforeText,m_bProcessActionsAtServer,m_bAnimate,m_bHideByDefault,m_bKeepGraphicInCache;
	bool m_bTabStop,m_bCustomRender;
	int m_PK_DesignObj_Up,m_PK_DesignObj_Down,m_PK_DesignObj_Left,m_PK_DesignObj_Right,m_iRegenInterval;
	string m_sPK_DesignObj_TiedTo;
	string m_sVisibleState;
	int m_iPK_Button,m_dwTimeoutSeconds,m_Priority;
	int m_iPK_Criteria;
	char m_cScreenType;

	string m_sBackgroundFile, m_sSelectedFile, m_sHighlightGraphicFilename;
	vector<string> m_vectAltGraphicFilename;
	map<int,string> m_mapObjParms;
	string m_mapObjParms_Find(int PK_DesignObjParameter) { map<int,string>::iterator it = m_mapObjParms.find(PK_DesignObjParameter); return it==m_mapObjParms.end() ? "" : (*it).second; }

	string m_ObjectID;
	int m_iBaseObjectID,m_iVersion,m_iPage;  // Version & Page will only be used for top level objects (screens)
	int m_ObjectType,m_PK_Effect_On_Screen,m_PK_Effect_Off_Screen;
	PlutoRectangle m_rectDontDim, m_rPosition, m_rBackgroundPosition;
	PlutoDataBlock m_dbHitTest;
	class PlutoSize m_sOriginalSize;
	int m_dwIDim;

	// If non-zero, this is a repeating button, and as long as it's held down, the command will keep
	// repeating until the user lets go this many milliseconds.  A parameter 'Repeat' will be added if bRepeatParm is true
	int m_iRepeatIntervalInMS;
	bool m_bRepeatParm;

	DesignObjCommandList m_Action_LoadList,m_Action_UnloadList,m_Action_HighlightList,m_Action_UnhighlightList,m_Action_TimeoutList,m_Action_StartupList;
	int m_FK_Effect_Selected_WithChange, m_FK_Effect_Selected_NoChange, m_FK_Effect_Highlighted;
	DesignObjZoneList m_ZoneList;
	VectorDesignObjText m_vectDesignObjText;
	DesignObj_DataList m_ChildObjects;
	class DesignObj_Data *m_pParentObject;

	DesignObj_Data() 
	{
		m_iBaseObjectID = 0;
		m_PK_DesignObj_Up=m_PK_DesignObj_Down=m_PK_DesignObj_Left=m_PK_DesignObj_Right=0;
		m_sPK_DesignObj_TiedTo="";
		m_iRegenInterval=m_iRepeatIntervalInMS=m_iVersion=m_iPage=0;
		m_bTabStop = m_bRepeatParm=false;
		m_FK_Effect_Selected_WithChange = m_FK_Effect_Selected_NoChange = m_FK_Effect_Highlighted = 0;
		return;
	}

	virtual string SerializeClassClassName() { return "DesignObj_Data"; }
	virtual ~DesignObj_Data();

	class DesignObj_Data *TopMostObject() { return m_pParentObject ? m_pParentObject->TopMostObject() : this; }
	bool ChildOf( DesignObj_Data *pObj_Parent ) { if( pObj_Parent && m_pParentObject==pObj_Parent ) return true;  else if( !m_pParentObject ) return false; else return m_pParentObject->ChildOf(pObj_Parent); }

	virtual bool Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData=NULL);
};

#endif
