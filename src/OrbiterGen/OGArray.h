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
#ifndef CGArray_h
#define CGArray_h

#include "OGText.h"

class ArrayValue
{
public:
	string m_ID;
	string m_sDescription;
	class Row_Icon * m_pRow_Icon;
	int m_iPK_CommandGroup;
	int m_iPK_Variable;
	bool m_bCanHide;
	bool m_bHideByDefault;
	bool m_bSamePosition;
	int m_PK_Criteria_Orbiter;
	int m_PK_DesignObjID_Substitute;

	ArrayValue(string ID,string Description,class Row_Icon * drIcon,int PK_Criteria_Orbiter,int PK_DesignObjID_Substitute,int PK_CommandGroup,int PK_Variable,bool bCanHide,bool bHideByDefault,bool bSamePosition)
	{ 
		m_ID=ID; m_sDescription=Description; m_pRow_Icon=drIcon; m_PK_Criteria_Orbiter=PK_Criteria_Orbiter;
		m_iPK_CommandGroup=PK_CommandGroup; m_iPK_Variable=PK_Variable; m_bCanHide=bCanHide; m_bSamePosition=bSamePosition; m_bHideByDefault=bHideByDefault;
		m_PK_DesignObjID_Substitute=PK_DesignObjID_Substitute;
	}
};

class CGArray
{
public:
	bool m_bContainsMore,m_bExpandHorizontal,m_bExpandVertical;
	int m_iLastVisibleArrayEntry,iScreenNum;
	vector<class DesignObj_Generator *> m_alChildDesignObjs_OfArray;
	vector<class ArrayValue *> *m_alValues;
	int m_iStartingOffset,m_iPage,m_PK_Array;
	class DesignObj_Generator *m_DesignObj_Generator_Parent,*m_ocBack,*m_ocFwd;
	class PlutoRectangle m_rBounds;
	class Database_pluto_main *m_mds;
	map<int,string> *m_VariableMap;
	class Row_DesignObjVariation *m_drDesignObjVariation,*m_drStandardVariation;
	
	vector<class Row_DesignObjVariation *> m_alDesignObjVariations;
	class Row_DesignObj *m_pRow_DesignObj_MoreFwd,*m_pRow_DesignObj_MoreBack;
	class Row_DesignObjVariation_DesignObj_Skin_Language * m_pRow_DesignObjVariation_DesignObj_Skin_Language;
	class PlutoPoint m_ptNextPosition;

	CGArray(class DesignObj_Generator *DesignObj_Generator_Parent,class Row_DesignObjVariation_DesignObj_Skin_Language *pRow_DesignObjVariation_DesignObj_Skin_Language,vector<class ArrayValue *> *alValues,class PlutoRectangle rBounds,int StartingOffset,int Page,int PK_Array);
	void CheckLastEntry();
};

#endif
