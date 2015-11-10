/*
 DesignObj_Data
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */
 
#include "PlutoUtils/CommonIncludes.h"	
#include "DesignObj_Orbiter.h"
#include "DesignObj_DataGrid.h"
#include "pluto_main/Define_DesignObjType.h"

#ifdef OrbiterGen
#include "OrbiterGen/DesignObj_Generator.h"
#endif

DesignObj_Data::~DesignObj_Data()
{
	m_vectAltGraphicFilename.clear();
	m_mapObjParms.clear();
	m_Action_LoadList.clear();
	m_Action_UnloadList.clear();
	m_Action_HighlightList.clear();
	m_Action_UnhighlightList.clear();
	m_Action_TimeoutList.clear();
	m_Action_StartupList.clear();
	m_ZoneList.clear();
	m_ChildObjects.clear();	
	m_vectDesignObjText.clear();
}

bool DesignObjText::Serialize( bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData )	
{
	// Save the starting points
	m_pcDataBlock=pcDataBlock; m_dwAllocatedSize=dwAllocatedSize; m_pcCurrentPosition=pcCurrentPosition;

	if( bWriting )
	{
		Write_char(m_bPreRender);
		Write_long(m_PK_Text);
		Write_long(m_iPK_VertAlignment);
		Write_long(m_iPK_HorizAlignment);
		Write_long(m_rPosition.X);
		Write_long(m_rPosition.Y);
		Write_long(m_rPosition.Width);
		Write_long(m_rPosition.Height);
		Write_string(m_sText);

		{
			Write_long(long(m_mapAltVersions.size()));
			for(map<int,string>::iterator it=m_mapAltVersions.begin();it!=m_mapAltVersions.end();++it)
			{
				Write_long(it->first);
				Write_string(it->second);
			}
		}

		{
			Write_long(long(m_mapTextStyle.size()));
			for(MapTextStyle::iterator it=m_mapTextStyle.begin();it!=m_mapTextStyle.end();++it)
			{
				Write_long(it->first);
				it->second->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
			}
		}
	}
	else
	{
		m_bPreRender=Read_char()==1;
		m_PK_Text=Read_long();
		m_iPK_VertAlignment=Read_long();
		m_iPK_HorizAlignment=Read_long();
		m_rPosition.X=Read_long();
		m_rPosition.Y=Read_long();
		m_rPosition.Width=Read_long();
		m_rPosition.Height=Read_long();
		Read_string(m_sText);

		{
			size_t count = size_t(Read_long());
			for(size_t s=0;s<count;++s)
			{
				long l = Read_long();
				string str;
				Read_string(str);
				m_mapAltVersions[l]=str;
			}
		}

		{
			size_t count = size_t(Read_long());
			for(size_t s=0;s<count;++s)
			{
				long l=Read_long();
				TextStyle *pTextStyle = new TextStyle();
				pTextStyle->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
				m_mapTextStyle[l]=pTextStyle;
			}
		}
	}

	// We may have re-allocated the memory block and size, and the position will have changed
	pcDataBlock=m_pcDataBlock; dwAllocatedSize=m_dwAllocatedSize; pcCurrentPosition=m_pcCurrentPosition;
	return true;
}

bool DesignObjZone::Serialize( bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData )
{
	// Save the starting points
	m_pcDataBlock=pcDataBlock; m_dwAllocatedSize=dwAllocatedSize; m_pcCurrentPosition=pcCurrentPosition;

	if( bWriting )
	{
		Write_long(m_Rect.X);
		Write_long(m_Rect.Y);
		Write_long(m_Rect.Width);
		Write_long(m_Rect.Height);

		{
			Write_long(long(m_Commands.size()));
			for(DesignObjCommandList::iterator it=m_Commands.begin();it!=m_Commands.end();++it)
				(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
		}
	}
	else
	{
		m_Rect.X=Read_long();
		m_Rect.Y=Read_long();
		m_Rect.Width=Read_long();
		m_Rect.Height=Read_long();

		{
			size_t count = size_t(Read_long());
			for(size_t s=0;s<count;++s)
			{
				DesignObjCommand *pDesignObjCommand = new DesignObjCommand();
				pDesignObjCommand->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
				m_Commands.push_back(pDesignObjCommand);
			}
		}
	}

	// We may have re-allocated the memory block and size, and the position will have changed
	pcDataBlock=m_pcDataBlock; dwAllocatedSize=m_dwAllocatedSize; pcCurrentPosition=m_pcCurrentPosition;
	return true;
}

bool DesignObjCommand::Serialize( bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData )
{
	// Save the starting points
	m_pcDataBlock=pcDataBlock; m_dwAllocatedSize=dwAllocatedSize; m_pcCurrentPosition=pcCurrentPosition;

	if( bWriting )
	{
		Write_long(m_PK_Command);
		Write_long(m_PK_Device);
		Write_long(m_PK_DeviceGroup);
		Write_long(BroadcastLevel);
		Write_long(m_PK_DeviceTemplate);
		Write_long(m_PK_DeviceCategory);
		Write_char(m_bRelativeToSender);
		Write_char(m_bDeliveryConfirmation);
		{
			Write_long(long(m_ParameterList.size()));
			for(map<int,string>::iterator it=m_ParameterList.begin();it!=m_ParameterList.end();++it)
			{
				Write_long(it->first);
				Write_string(it->second);
			}
		}
	}
	else
	{
		m_PK_Command=Read_long();
		m_PK_Device=Read_long();
		m_PK_DeviceGroup=Read_long();
		BroadcastLevel=Read_long();
		m_PK_DeviceTemplate=Read_long();
		m_PK_DeviceCategory=Read_long();
		m_bRelativeToSender=Read_char()==1;
		m_bDeliveryConfirmation=Read_char()==1;
		{
			size_t count = size_t(Read_long());
			for(size_t s=0;s<count;++s)
			{
				long l = Read_long();
				string str;
				Read_string(str);
				m_ParameterList[l]=str;
			}
		}
	}
	// We may have re-allocated the memory block and size, and the position will have changed
	pcDataBlock=m_pcDataBlock; dwAllocatedSize=m_dwAllocatedSize; pcCurrentPosition=m_pcCurrentPosition;
	return true;
}

bool DesignObj_Data::Serialize( bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData )	
{
	// Save the starting points
	m_pcDataBlock=pcDataBlock; m_dwAllocatedSize=dwAllocatedSize; m_pcCurrentPosition=pcCurrentPosition; m_pExtraSerializationData=pExtraSerializationData;

	if( bWriting )
	{
		Write_char(m_bChild);
		Write_char(m_bDontResetState);
		Write_char(m_bCantGoBack);
		Write_char(m_bCanGoBackToSameScreen);
		Write_char(m_bChildrenBeforeText);
		Write_char(m_bProcessActionsAtServer);
		Write_char(m_bAnimate);
		Write_char(m_bHideByDefault);
		Write_char(m_bKeepGraphicInCache);
		Write_char(m_cScreenType);

		Write_char(m_bTabStop);
		Write_char(m_bRepeatParm);

		Write_long(m_iPK_Button);
		Write_long(m_dwTimeoutSeconds);
		Write_long(m_Priority);
		Write_long(m_iPK_Criteria);

		Write_long(m_iBaseObjectID);
		Write_long(m_iVersion);
		Write_long(m_iPage);

		Write_long(m_ObjectType);
		Write_long(m_PK_Effect_On_Screen);
		Write_long(m_PK_Effect_Off_Screen);
		Write_char(m_bCustomRender);

		Write_long(m_rPosition.X);
		Write_long(m_rPosition.Y);
		Write_long(m_rPosition.Width);
		Write_long(m_rPosition.Height);

		Write_long(m_rBackgroundPosition.X);
		Write_long(m_rBackgroundPosition.Y);
		Write_long(m_rBackgroundPosition.Width);
		Write_long(m_rBackgroundPosition.Height);

		Write_long(m_sOriginalSize.Width);
		Write_long(m_sOriginalSize.Height);

		Write_long(m_PK_DesignObj_Up);
		Write_long(m_PK_DesignObj_Down);
		Write_long(m_PK_DesignObj_Left);
		Write_long(m_PK_DesignObj_Right);

		Write_long(m_iRepeatIntervalInMS);
		Write_long(m_dwIDim);
		Write_long(m_iRegenInterval);

		Write_string(m_sBackgroundFile);
		Write_string(m_sSelectedFile);
		Write_string(m_sHighlightGraphicFilename);

		Write_string(m_ObjectID);
		Write_string(m_sPK_DesignObj_TiedTo);
		Write_string(m_sVisibleState);
		Write_block(m_dbHitTest.m_pBlock,m_dbHitTest.m_dwSize);
		

		{
			Write_long(long(m_vectAltGraphicFilename.size()));
			for(size_t s=0;s<m_vectAltGraphicFilename.size();++s)
				Write_string(m_vectAltGraphicFilename[s]);
		}

		{
			Write_long(long(m_mapObjParms.size()));
			for(map<int,string>::iterator it=m_mapObjParms.begin();it!=m_mapObjParms.end();++it)
			{
				Write_long(it->first);
				Write_string(it->second);
			}
		}
		{
			Write_long(long(m_Action_LoadList.size()));
			for(DesignObjCommandList::iterator it=m_Action_LoadList.begin();it!=m_Action_LoadList.end();++it)
				(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
		}

		{
			Write_long(long(m_Action_UnloadList.size()));
			for(DesignObjCommandList::iterator it=m_Action_UnloadList.begin();it!=m_Action_UnloadList.end();++it)
				(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
		}
		{
			Write_long(long(m_Action_HighlightList.size()));
			for(DesignObjCommandList::iterator it=m_Action_HighlightList.begin();it!=m_Action_HighlightList.end();++it)
				(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
		}
		{
			Write_long(long(m_Action_UnhighlightList.size()));
			for(DesignObjCommandList::iterator it=m_Action_UnhighlightList.begin();it!=m_Action_UnhighlightList.end();++it)
				(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
		}
		{
			Write_long(long(m_Action_TimeoutList.size()));
			for(DesignObjCommandList::iterator it=m_Action_TimeoutList.begin();it!=m_Action_TimeoutList.end();++it)
				(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
		}
		{
			Write_long(long(m_Action_StartupList.size()));
			for(DesignObjCommandList::iterator it=m_Action_StartupList.begin();it!=m_Action_StartupList.end();++it)
				(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
		}
		{
			Write_long(long(m_ZoneList.size()));
			for(DesignObjZoneList::iterator it=m_ZoneList.begin();it!=m_ZoneList.end();++it)
				(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
		}

#ifdef OrbiterGen
		if( m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
		{
			DesignObj_Generator *pObj = (DesignObj_Generator *) this;
			Write_long(long(pObj->m_mapChildDgObjects.size()));
			for(map< pair<int,int>, DesignObj_Generator *>::iterator it=pObj->m_mapChildDgObjects.begin();it!=pObj->m_mapChildDgObjects.end();++it)
			{
				Write_long(long(it->first.first));
				Write_long(long(it->first.second));
				Write_string(it->second->m_ObjectID);
			}
		}
#endif

		{
			Write_long(long(m_ChildObjects.size()));
			for(DesignObj_DataList::iterator it=m_ChildObjects.begin();it!=m_ChildObjects.end();++it)
			{
				Write_unsigned_long( (*it)->m_ObjectType );
				(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
			}
		}
		{
			Write_long(long(m_vectDesignObjText.size()));
			for(VectorDesignObjText::iterator it=m_vectDesignObjText.begin();it!=m_vectDesignObjText.end();++it)
				(*it)->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
		}
		{
			//effects
			Write_long(m_FK_Effect_Selected_WithChange);
			Write_long(m_FK_Effect_Selected_NoChange);
			Write_long(m_FK_Effect_Highlighted);
		}
	}
	else
	{
		m_bChild=Read_char()==1;
		m_bDontResetState=Read_char()==1;
		m_bCantGoBack=Read_char()==1;
		m_bCanGoBackToSameScreen=Read_char()==1;
		m_bChildrenBeforeText=Read_char()==1;
		m_bProcessActionsAtServer=Read_char()==1;
		m_bAnimate=Read_char()==1;
		m_bHideByDefault=Read_char()==1;
		m_bKeepGraphicInCache=Read_char()==1;
		m_cScreenType=Read_char();

		m_bTabStop=Read_char()==1;
		m_bRepeatParm=Read_char()==1;

		m_iPK_Button=Read_long();
		m_dwTimeoutSeconds=Read_long();
		m_Priority=Read_long();
		m_iPK_Criteria=Read_long();

		m_iBaseObjectID=Read_long();
		m_iVersion=Read_long();
		m_iPage=Read_long();

		m_ObjectType=Read_long();
		m_PK_Effect_On_Screen=Read_long();
		m_PK_Effect_Off_Screen=Read_long();
		m_bCustomRender=Read_char()==1;

		m_rPosition.X=Read_long();
		m_rPosition.Y=Read_long();
		m_rPosition.Width=Read_long();
		m_rPosition.Height=Read_long();

		m_rBackgroundPosition.X=Read_long();
		m_rBackgroundPosition.Y=Read_long();
		m_rBackgroundPosition.Width=Read_long();
		m_rBackgroundPosition.Height=Read_long();

		m_sOriginalSize.Width=Read_long();
		m_sOriginalSize.Height=Read_long();

		m_PK_DesignObj_Up=Read_long();
		m_PK_DesignObj_Down=Read_long();
		m_PK_DesignObj_Left=Read_long();
		m_PK_DesignObj_Right=Read_long();

		m_iRepeatIntervalInMS=Read_long();
		m_dwIDim=Read_long();
		m_iRegenInterval=Read_long();

		Read_string(m_sBackgroundFile);
		Read_string(m_sSelectedFile);
		Read_string(m_sHighlightGraphicFilename);

		Read_string(m_ObjectID);
		Read_string(m_sPK_DesignObj_TiedTo);
		Read_string(m_sVisibleState);
		m_dbHitTest.m_pBlock=Read_block(m_dbHitTest.m_dwSize);

		{
			size_t count = size_t(Read_long());
			for(size_t s=0;s<count;++s)
			{
				string str;
				Read_string(str);
				m_vectAltGraphicFilename.push_back(str);
			}
		}

		{
			size_t count = size_t(Read_long());
			for(size_t s=0;s<count;++s)
			{
				long l = Read_long();
				string str;
				Read_string(str);
				m_mapObjParms[l]=str;
			}
		}

		{
			size_t count = size_t(Read_long());
			for(size_t s=0;s<count;++s)
			{
				DesignObjCommand *pDesignObjCommand = new DesignObjCommand();
				pDesignObjCommand->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
				m_Action_LoadList.push_back(pDesignObjCommand);
			}
		}

		{
			size_t count = size_t(Read_long());
			for(size_t s=0;s<count;++s)
			{
				DesignObjCommand *pDesignObjCommand = new DesignObjCommand();
				pDesignObjCommand->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
				m_Action_UnloadList.push_back(pDesignObjCommand);
			}
		}

		{
			size_t count = size_t(Read_long());
			for(size_t s=0;s<count;++s)
			{
				DesignObjCommand *pDesignObjCommand = new DesignObjCommand();
				pDesignObjCommand->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
				m_Action_HighlightList.push_back(pDesignObjCommand);
			}
		}

		{
			size_t count = size_t(Read_long());
			for(size_t s=0;s<count;++s)
			{
				DesignObjCommand *pDesignObjCommand = new DesignObjCommand();
				pDesignObjCommand->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
				m_Action_UnhighlightList.push_back(pDesignObjCommand);
			}
		}
	
		{
			size_t count = size_t(Read_long());
			for(size_t s=0;s<count;++s)
			{
				DesignObjCommand *pDesignObjCommand = new DesignObjCommand();
				pDesignObjCommand->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
				m_Action_TimeoutList.push_back(pDesignObjCommand);
			}
		}

		{
			size_t count = size_t(Read_long());
			for(size_t s=0;s<count;++s)
			{
				DesignObjCommand *pDesignObjCommand = new DesignObjCommand();
				pDesignObjCommand->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
				m_Action_StartupList.push_back(pDesignObjCommand);
			}
		}
		{
			size_t count = size_t(Read_long());
			for(size_t s=0;s<count;++s)
			{
				DesignObjZone *pDesignObjZone = new DesignObjZone();
				pDesignObjZone->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
				m_ZoneList.push_back(pDesignObjZone);
			}
		}

		map<string, pair<int,int> > mapDgObjects;  // This is only serialized for datagrids
		if( m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
		{
			size_t count = size_t(Read_long());
			for(size_t s=0;s<count;++s)
			{
				long Col = Read_long();
				long Row = Read_long();
				string sObjectID;
				Read_string(sObjectID);
#ifdef WIN32
				mapDgObjects[sObjectID] = make_pair (Col, Row);
#else
				mapDgObjects[sObjectID] = make_pair<int,int> (Col,Row);
#endif
			}
		}

		{
			size_t count = size_t(Read_long());
			for(size_t s=0;s<count;++s)
			{
				unsigned long Type = Read_unsigned_long();
	#ifdef ORBITER
				DesignObj_Orbiter *pDesignObj_Data=NULL;
				if( Type==DESIGNOBJTYPE_Datagrid_CONST )
					pDesignObj_Data = new DesignObj_DataGrid((class Orbiter *) m_pExtraSerializationData);
				else
					pDesignObj_Data = new DesignObj_Orbiter((class Orbiter *) m_pExtraSerializationData);
	#elif defined(OrbiterGen)
				DesignObj_Generator *pDesignObj_Data = new DesignObj_Generator();
	#else
				DesignObj_Data *pDesignObj_Data = new DesignObj_Data();
	#endif

				pDesignObj_Data->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
#ifdef ORBITER
				// If this is a datagrid and it has child objects, match them to the rows/columns
				if( m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
				{
					map<string, pair<int,int> >::iterator it=mapDgObjects.find( pDesignObj_Data->m_ObjectID );
					if( it!=mapDgObjects.end() )
					{
						pDesignObj_Data->m_pDesignObj_DataGrid = ( (DesignObj_DataGrid *)this );
						pDesignObj_Data->m_iGridCol=it->second.first;
						pDesignObj_Data->m_iGridRow=it->second.second;
#ifdef WIN32
						pDesignObj_Data->m_pDesignObj_DataGrid->m_mapChildDgObjects[make_pair (it->second.first, it->second.second)] = pDesignObj_Data;
#else
						pDesignObj_Data->m_pDesignObj_DataGrid->m_mapChildDgObjects[ make_pair<int,int> (it->second.first,it->second.second) ] = pDesignObj_Data;
#endif
					}
				}
#elif defined(OrbiterGen)
				// If this is a datagrid and it has child objects, match them to the rows/columns
				if( m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
				{
					map<string, pair<int,int> >::iterator it=mapDgObjects.find( pDesignObj_Data->m_ObjectID );
					if( it!=mapDgObjects.end() )
					{
						((DesignObj_Generator *) this)->m_mapChildDgObjects[ make_pair<int,int> (it->second.first,it->second.second) ] = pDesignObj_Data;
					}
				}
#endif

				pDesignObj_Data->m_pParentObject=this;
				m_ChildObjects.push_back(pDesignObj_Data);
			}
		}

		{
			size_t count = size_t(Read_long());
			for(size_t s=0;s<count;++s)
			{
				DesignObjText *pDesignObjText = new DesignObjText();
				pDesignObjText->Serialize(bWriting,m_pcDataBlock,m_dwAllocatedSize,m_pcCurrentPosition,pExtraSerializationData);
				m_vectDesignObjText.push_back(pDesignObjText);
			}
		}
		{
			m_FK_Effect_Selected_WithChange = Read_long();
			m_FK_Effect_Selected_NoChange = Read_long();
			m_FK_Effect_Highlighted = Read_long();
		}
	}
	// We may have re-allocated the memory block and size, and the position will have changed
	pcDataBlock=m_pcDataBlock; dwAllocatedSize=m_dwAllocatedSize; pcCurrentPosition=m_pcCurrentPosition;
	return true;
}

