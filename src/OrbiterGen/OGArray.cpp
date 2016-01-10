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
#include "PlutoUtils/CommonIncludes.h"	
#include "OGArray.h"
#include "DesignObj_Generator.h"
#include "OGCommand.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "OrbiterGen.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Text_LS.h"
#include "pluto_main/Table_DesignObj.h"
#include "pluto_main/Table_DesignObjVariation.h"
#include "pluto_main/Table_DesignObjVariation_Text_Skin_Language.h"
#include "pluto_main/Table_DesignObjVariation_Text.h"
#include "pluto_main/Table_DesignObjVariation_DesignObj.h"
#include "pluto_main/Table_DesignObjVariation_DesignObj_Skin_Language.h"
#include "pluto_main/Table_DesignObjVariation_DesignObjParameter.h"
#include "pluto_main/Table_Orbiter.h"
#include "pluto_main/Table_Text_LS_AltVersions.h"
#include "pluto_main/Table_DesignObjParameter.h"
#include "pluto_main/Table_CommandParameter.h"
#include "pluto_main/Table_Command.h"
#include "pluto_main/Table_Variable.h"
#include "pluto_main/Define_Button.h"

CGArray::CGArray(class DesignObj_Generator *DesignObj_Generator_Parent,class Row_DesignObjVariation_DesignObj_Skin_Language *pRow_DesignObjVariation_DesignObj_Skin_Language,vector<class ArrayValue *> *alValues,class PlutoRectangle rBounds,int StartingOffset,int Page,int PK_Array)
{
	m_PK_Array=PK_Array;
	m_bContainsMore=false;
	m_iLastVisibleArrayEntry=0;
	iScreenNum=0;
	m_ocBack=NULL;
	m_ocFwd=NULL;
	m_drDesignObjVariation=NULL;
	m_drStandardVariation=NULL;

	m_pRow_DesignObjVariation_DesignObj_Skin_Language = pRow_DesignObjVariation_DesignObj_Skin_Language;
	m_DesignObj_Generator_Parent=DesignObj_Generator_Parent;
	m_mds = m_DesignObj_Generator_Parent->m_mds;
	m_alValues=alValues;
	m_iStartingOffset=StartingOffset;
	m_iPage=Page;
	m_rBounds=rBounds;
	m_bExpandHorizontal=(m_rBounds.Width==-1);
	m_bExpandVertical=(m_rBounds.Height==-1);

	m_VariableMap=&m_DesignObj_Generator_Parent->m_pOrbiterGenerator->m_mapVariable;

	DesignObj_Generator::PickVariation(m_DesignObj_Generator_Parent->m_pOrbiterGenerator,pRow_DesignObjVariation_DesignObj_Skin_Language->FK_DesignObjVariation_DesignObj_getrow()->FK_DesignObj_Child_getrow(),&m_drDesignObjVariation,&m_drStandardVariation,&m_alDesignObjVariations);
	if( !m_drDesignObjVariation )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Aborting creation of array.  No variation for object: %d",
			pRow_DesignObjVariation_DesignObj_Skin_Language->FK_DesignObjVariation_DesignObj_getrow()->FK_DesignObj_Child_getrow()->PK_DesignObj_get());
		return;
	}

	Row_DesignObjVariation_DesignObjParameter *pRow_DesignObjVariation_DesignObjParameter = m_mds->DesignObjVariation_DesignObjParameter_get()->GetRow(m_drDesignObjVariation->PK_DesignObjVariation_get(),DESIGNOBJPARAMETER_More_button_fwd_CONST);
	if( pRow_DesignObjVariation_DesignObjParameter )
		m_pRow_DesignObj_MoreFwd = m_mds->DesignObj_get()->GetRow( atoi(pRow_DesignObjVariation_DesignObjParameter->Value_get().c_str()) );
	else
		m_pRow_DesignObj_MoreFwd = NULL;

	pRow_DesignObjVariation_DesignObjParameter = m_mds->DesignObjVariation_DesignObjParameter_get()->GetRow(m_drDesignObjVariation->PK_DesignObjVariation_get(),DESIGNOBJPARAMETER_More_button_back_CONST);
	if( pRow_DesignObjVariation_DesignObjParameter )
		m_pRow_DesignObj_MoreBack = m_mds->DesignObj_get()->GetRow( atoi(pRow_DesignObjVariation_DesignObjParameter->Value_get().c_str()) );
	else
		m_pRow_DesignObj_MoreBack = NULL;

	int RowSpacing=0,ColumnSpacing=0,MaxNumRows=0,MaxNumColumns=0,FixedRowHeight=0,FixedColumnWidth=0;
	bool bDownAcross=false;

	FixedRowHeight = atoi(m_DesignObj_Generator_Parent->GetParm(DESIGNOBJPARAMETER_Fixed_Row_Height_CONST,m_drDesignObjVariation).c_str());  
	
	FixedColumnWidth = atoi(m_DesignObj_Generator_Parent->GetParm(DESIGNOBJPARAMETER_Fixed_Column_Width_CONST,m_drDesignObjVariation).c_str());  
	
	RowSpacing = atoi(m_DesignObj_Generator_Parent->GetParm(DESIGNOBJPARAMETER_Row_Spacing_CONST,m_drDesignObjVariation).c_str());  
	
	ColumnSpacing = atoi(m_DesignObj_Generator_Parent->GetParm(DESIGNOBJPARAMETER_Column_Spacing_CONST,m_drDesignObjVariation).c_str());  
	
	MaxNumRows = atoi(m_DesignObj_Generator_Parent->GetParm(DESIGNOBJPARAMETER_Num_of_Rows_CONST,m_drDesignObjVariation).c_str());  
	
	MaxNumColumns = atoi(m_DesignObj_Generator_Parent->GetParm(DESIGNOBJPARAMETER_Num_of_Columns_CONST,m_drDesignObjVariation).c_str());  
	
	bDownAcross = m_DesignObj_Generator_Parent->GetParm(DESIGNOBJPARAMETER_Down_then_Across_CONST,m_drDesignObjVariation)=="1";  
	

	int iCurrentNumRows=0;
	int iCurrentNumColumns=0;

	m_ptNextPosition = rBounds.Location();
if( DesignObj_Generator_Parent->m_pRow_DesignObj->PK_DesignObj_get()==1270 || pRow_DesignObjVariation_DesignObj_Skin_Language->FK_DesignObjVariation_DesignObj_getrow()->FK_DesignObj_Child_get()==1264 )
int k=2;
	if( StartingOffset!=0 && m_pRow_DesignObj_MoreBack )
	{
		m_ocBack = new DesignObj_Generator(m_DesignObj_Generator_Parent->m_pOrbiterGenerator,
			m_pRow_DesignObj_MoreBack,
			PlutoRectangle(m_ptNextPosition.X,m_ptNextPosition.Y,0,0),m_DesignObj_Generator_Parent,false,false);

		m_ocBack->m_bChildrenBeforeText = pRow_DesignObjVariation_DesignObj_Skin_Language->DisplayChildrenBeforeText_get()==1;
		m_ocBack->m_bChildrenBehind = pRow_DesignObjVariation_DesignObj_Skin_Language->DisplayChildrenBehindBackground_get()==1;
		m_ocBack->m_bDontMergeBackground = pRow_DesignObjVariation_DesignObj_Skin_Language->DontMergeBackground_get()==1;
		m_ocBack->m_bTabStop = pRow_DesignObjVariation_DesignObj_Skin_Language->IsTabStop_get()==1;

		if( m_ocBack->m_pRow_DesignObjVariation )
		{
			m_ocBack->m_sDesignObjGoto = m_ocBack->m_pOrbiterGenerator->m_iPK_DesignObj_Screen + "." + StringUtils::itos(m_ocBack->m_iVersion) + "." + StringUtils::itos(m_iPage-1);
			m_alChildDesignObjs_OfArray.push_back(m_ocBack);
		}
		else
			delete m_ocBack;
	}

	for(m_iLastVisibleArrayEntry=StartingOffset;m_iLastVisibleArrayEntry<(int) m_alValues->size();++m_iLastVisibleArrayEntry)
	{
		// We're ready to place the next object, make a copy first
		ArrayValue *av = (*m_alValues)[m_iLastVisibleArrayEntry];

		if( av->m_PK_Criteria_Orbiter!=0 )
			m_DesignObj_Generator_Parent->m_pOrbiterGenerator->m_mapUsedOrbiterCriteria[av->m_PK_Criteria_Orbiter]=true;

		if( m_alChildDesignObjs_OfArray.size()>0 && !av->m_bSamePosition )  // Get the next position unless this is the first object, or this goes ontop of the prior
		{
			// We've already got some objects on this row/column.  Figure out the placement of the next object
			DesignObj_Generator *ocLastDesignObj = m_alChildDesignObjs_OfArray[m_alChildDesignObjs_OfArray.size()-1];

			if( bDownAcross )
			{
				iCurrentNumRows++;
				// Get the placement of the next object
				if( FixedRowHeight>0 )
					m_ptNextPosition.Y += FixedRowHeight;
				else
					m_ptNextPosition.Y = ocLastDesignObj->m_rPosition.Bottom() + RowSpacing;

				// See if another one of those objects won't fit and we need a new column
				if( (MaxNumRows>0 && iCurrentNumRows>=MaxNumRows) || 
					(m_ptNextPosition.Y + ocLastDesignObj->m_rPosition.Height > rBounds.Bottom() && !m_bExpandVertical) ||
					m_ptNextPosition.Y + ocLastDesignObj->m_rPosition.Height > 1600 )  // 2844x1600 is the maximum size
				{
					if( MaxNumColumns>0 && ++iCurrentNumColumns>=MaxNumColumns )
					{ CheckLastEntry(); return; }

					iCurrentNumRows=0;
					m_ptNextPosition.Y = rBounds.Top();

					if( FixedColumnWidth>0 )
						m_ptNextPosition.X += FixedColumnWidth;
					else
						m_ptNextPosition.X = ocLastDesignObj->m_rPosition.Right() + ColumnSpacing;
				
					if( m_ptNextPosition.X + ocLastDesignObj->m_rPosition.Width > rBounds.Right() )
					{ CheckLastEntry(); return; }
				}
			}
			else
			{
				iCurrentNumColumns++;
				// Get the placement of the next object
				if( FixedColumnWidth>0 )
					m_ptNextPosition.X += FixedColumnWidth;
				else
					m_ptNextPosition.X = ocLastDesignObj->m_rPosition.Right() + ColumnSpacing;

				// See if another one of those objects won't fit and we need a new column
				if( (MaxNumColumns>0 && iCurrentNumColumns>=MaxNumColumns) || 
					(m_ptNextPosition.X + ocLastDesignObj->m_rPosition.Width > rBounds.Right() && !m_bExpandHorizontal) ||
					m_ptNextPosition.X + ocLastDesignObj->m_rPosition.Width > 2844 )  // 2844x1600 is the maximum size
				{
					if( MaxNumRows>0 && ++iCurrentNumRows>=MaxNumRows )
					{ CheckLastEntry(); return; }

					iCurrentNumColumns=0;
					m_ptNextPosition.X = rBounds.Left();

					if( FixedRowHeight>0 )
						m_ptNextPosition.Y += FixedRowHeight;
					else
						m_ptNextPosition.Y = ocLastDesignObj->m_rPosition.Bottom() + RowSpacing;
				
					if( m_ptNextPosition.Y + ocLastDesignObj->m_rPosition.Height > rBounds.Bottom() )
					{ CheckLastEntry(); return; }
				}
			}
		}

		m_DesignObj_Generator_Parent->m_pOrbiterGenerator->m_iID_Array=av->m_ID;
		m_DesignObj_Generator_Parent->m_pOrbiterGenerator->m_sDesc_Array=av->m_sDescription;
		m_DesignObj_Generator_Parent->m_pOrbiterGenerator->m_pRow_Icon=av->m_pRow_Icon;
		m_DesignObj_Generator_Parent->m_pOrbiterGenerator->m_iPK_CommandGroup = av->m_iPK_CommandGroup;
	
		DesignObj_Generator *ocNextDesignObj;

		Row_DesignObj *pRow_DesignObj_Substitute;
		if( av->m_PK_DesignObjID_Substitute==0 || (pRow_DesignObj_Substitute=m_mds->DesignObj_get()->GetRow(av->m_PK_DesignObjID_Substitute))==NULL )
		{
			vector<Row_DesignObjVariation_DesignObj *> vectOVRO;
			m_drDesignObjVariation->DesignObjVariation_DesignObj_FK_DesignObjVariation_Parent_getrows(&vectOVRO);
			if( vectOVRO.size()!=1 )
				throw "Array: " + m_drDesignObjVariation->FK_DesignObj_getrow()->Description_get() + " does not have 1 child";

			Row_DesignObjVariation_DesignObj * drChild = vectOVRO[0];
			ocNextDesignObj = new DesignObj_Generator(m_DesignObj_Generator_Parent->m_pOrbiterGenerator,drChild->FK_DesignObj_Child_getrow(),PlutoRectangle(m_ptNextPosition,PlutoSize(0,0)),m_DesignObj_Generator_Parent,false,false);
		}
		else
		{
			ocNextDesignObj = new DesignObj_Generator(m_DesignObj_Generator_Parent->m_pOrbiterGenerator,pRow_DesignObj_Substitute,PlutoRectangle(m_ptNextPosition,PlutoSize(0,0)),m_DesignObj_Generator_Parent,false,false);
		}

		if( !ocNextDesignObj->m_pRow_DesignObjVariation )
		{
			delete ocNextDesignObj;
			continue;
		}

		if( m_bExpandHorizontal && m_rBounds.Width < ocNextDesignObj->m_rPosition.Right() )
			m_rBounds.Right( ocNextDesignObj->m_rPosition.Right() );
		if( m_bExpandVertical && m_rBounds.Height < ocNextDesignObj->m_rPosition.Bottom() )
			m_rBounds.Bottom( ocNextDesignObj->m_rPosition.Bottom() );

		// In an array we want to increment the buttons
		if( ocNextDesignObj->m_iPK_Button )
		{
			if( m_DesignObj_Generator_Parent->m_pOrbiterGenerator->m_bIsMobilePhone )
			{
				if( ocNextDesignObj->m_iPK_Button==BUTTON_1_CONST && m_alChildDesignObjs_OfArray.size()==9 )
					ocNextDesignObj->m_iPK_Button = BUTTON_Asterisk_CONST;
				else if( ocNextDesignObj->m_iPK_Button==BUTTON_1_CONST && m_alChildDesignObjs_OfArray.size()==10 )
					ocNextDesignObj->m_iPK_Button = BUTTON_0_CONST;
				else if( ocNextDesignObj->m_iPK_Button==BUTTON_1_CONST && m_alChildDesignObjs_OfArray.size()==11 )
					ocNextDesignObj->m_iPK_Button = BUTTON_Pound_CONST;
				else
					ocNextDesignObj->m_iPK_Button += int(m_alChildDesignObjs_OfArray.size());
			}
			else
				ocNextDesignObj->m_iPK_Button += int(m_alChildDesignObjs_OfArray.size());
		}
		ocNextDesignObj->m_bCanBeHidden = av->m_bCanHide;
		ocNextDesignObj->m_bHideByDefault = av->m_bHideByDefault;
		ocNextDesignObj->m_bChildrenBeforeText = pRow_DesignObjVariation_DesignObj_Skin_Language->DisplayChildrenBeforeText_get()==1;
		ocNextDesignObj->m_bChildrenBehind = pRow_DesignObjVariation_DesignObj_Skin_Language->DisplayChildrenBehindBackground_get()==1;
		ocNextDesignObj->m_bDontMergeBackground = pRow_DesignObjVariation_DesignObj_Skin_Language->DontMergeBackground_get()==1;
		ocNextDesignObj->m_bTabStop = pRow_DesignObjVariation_DesignObj_Skin_Language->IsTabStop_get()==1;

		// Insert a zone in the beginning to add the variables
		CGZone *oz = new CGZone();
		
		CGCommand *oa = new CGCommand(COMMAND_Set_Variable_CONST,m_DesignObj_Generator_Parent);
		oa->m_ParameterList[COMMANDPARAMETER_PK_Variable_CONST]=StringUtils::itos(VARIABLE_Array_ID_CONST);;
		oa->m_ParameterList[COMMANDPARAMETER_Value_To_Assign_CONST]=av->m_ID;
		oa->m_PK_Device = DEVICEID_HANDLED_INTERNALLY;
		oz->m_Commands.push_back(oa);

		oa = new CGCommand(COMMAND_Set_Variable_CONST,m_DesignObj_Generator_Parent);
		oa->m_ParameterList[COMMANDPARAMETER_PK_Variable_CONST]=StringUtils::itos(VARIABLE_Array_Desc_CONST);
		oa->m_ParameterList[COMMANDPARAMETER_Value_To_Assign_CONST]=av->m_sDescription;
		oa->m_PK_Device = DEVICEID_HANDLED_INTERNALLY;
		oz->m_Commands.push_back(oa);

		oa = new CGCommand(COMMAND_Set_Variable_CONST,m_DesignObj_Generator_Parent);
		oa->m_ParameterList[COMMANDPARAMETER_PK_Variable_CONST]=StringUtils::itos(VARIABLE_PK_Array_CONST);
		oa->m_ParameterList[COMMANDPARAMETER_Value_To_Assign_CONST]=StringUtils::itos(m_PK_Array);
		oa->m_PK_Device = DEVICEID_HANDLED_INTERNALLY;
		oz->m_Commands.push_back(oa);

		if( av->m_iPK_Variable>0 )
		{
			oa = new CGCommand(COMMAND_Set_Variable_CONST,m_DesignObj_Generator_Parent);
			oa->m_ParameterList[COMMANDPARAMETER_PK_Variable_CONST]=StringUtils::itos(av->m_iPK_Variable).c_str();
			oa->m_ParameterList[COMMANDPARAMETER_Value_To_Assign_CONST]=av->m_ID;
			oa->m_PK_Device = DEVICEID_HANDLED_INTERNALLY;
			oz->m_Commands.push_back(oa);
		}
		ocNextDesignObj->m_ZoneList.insert(ocNextDesignObj->m_ZoneList.begin(),oz);

		m_alChildDesignObjs_OfArray.push_back(ocNextDesignObj);

		m_DesignObj_Generator_Parent->m_pOrbiterGenerator->m_iID_Array="0";
		m_DesignObj_Generator_Parent->m_pOrbiterGenerator->m_pRow_Icon=NULL;
	}
}

void CGArray::CheckLastEntry()
{
	if( m_iLastVisibleArrayEntry<(int) m_alValues->size() && m_pRow_DesignObj_MoreFwd )
	{
		DesignObj_Generator *obPrevious = m_alChildDesignObjs_OfArray[m_alChildDesignObjs_OfArray.size()-1];
		m_ocFwd = new DesignObj_Generator(m_DesignObj_Generator_Parent->m_pOrbiterGenerator,
			m_pRow_DesignObj_MoreFwd,
			PlutoRectangle(obPrevious->m_rPosition.X,obPrevious->m_rPosition.Y,0,0),
			m_DesignObj_Generator_Parent,false,false);

		if( !m_ocFwd->m_pRow_DesignObjVariation )
		{
			delete m_ocFwd;
			return;
		}

		m_ocFwd->m_bChildrenBeforeText = m_pRow_DesignObjVariation_DesignObj_Skin_Language->DisplayChildrenBeforeText_get()==1;
		m_ocFwd->m_bChildrenBehind = m_pRow_DesignObjVariation_DesignObj_Skin_Language->DisplayChildrenBehindBackground_get()==1;
		m_ocFwd->m_bDontMergeBackground = m_pRow_DesignObjVariation_DesignObj_Skin_Language->DontMergeBackground_get()==1;
		m_ocFwd->m_bTabStop = m_pRow_DesignObjVariation_DesignObj_Skin_Language->IsTabStop_get()==1;

		// See if maybe we're just supposed to change screens rather than paging through all the options
		if( m_ocFwd->m_PK_DesignObj_Goto!=0 )
			m_bContainsMore = false;
		else if( !m_pRow_DesignObj_MoreBack )  // if there's no back button, we can't do anything
			return;
		else
		{
			m_ocFwd->m_sDesignObjGoto = m_ocFwd->m_pOrbiterGenerator->m_iPK_DesignObj_Screen + "." + StringUtils::itos(m_ocFwd->m_iVersion) + "." + StringUtils::itos(m_iPage+1);
			m_bContainsMore=true; 
		}
		m_alChildDesignObjs_OfArray[m_alChildDesignObjs_OfArray.size()-1] = m_ocFwd;
		--m_iLastVisibleArrayEntry;
	}
}
