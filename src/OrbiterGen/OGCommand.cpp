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
#include "pluto_main/Table_DesignObjVariation_DesignObjParameter.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_Device_Orbiter.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Command.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_CommandGroup_Command.h"
#include "pluto_main/Table_CommandGroup_Command_CommandParameter.h"
#include "pluto_main/Table_CommandGroup_D.h"
#include "pluto_main/Table_CommandGroup_D_Command.h"
#include "pluto_main/Table_CommandGroup_D_Command_CommandParameter.h"
#include "pluto_main/Table_Device_Orbiter.h"
#include "pluto_main/Table_Orbiter.h"
#include "pluto_main/Table_Text_LS_AltVersions.h"
#include "pluto_main/Table_DesignObjParameter.h"
#include "pluto_main/Table_CommandParameter.h"
#include "pluto_main/Table_Command.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_Variable.h"
#include "pluto_main/Table_DeviceTemplate.h"

CGCommand::CGCommand(int PK_Command,DesignObj_Generator *Parent)
{
	m_PK_Device=0;
	m_PK_DeviceGroup=0;
	m_PK_Command=PK_Command;
	BroadcastLevel=0;
	m_PK_DeviceTemplate=0;
	m_PK_DeviceCategory=0;
	m_bRelativeToSender=false;
	m_drCommand = Parent->m_mds->Command_get()->GetRow(m_PK_Command);
// todo 2.0	m_bHandleLocally=m_drCommand->HandledByOrbiter_get()==1;
}

CGCommand::CGCommand(Row_CommandGroup_Command * drAGA,DesignObj_Generator *Parent)
{
	m_PK_Device=0;
	m_PK_DeviceTemplate=0;
	m_PK_DeviceGroup=0;
	BroadcastLevel=0;
	m_PK_DeviceCategory=0;
	m_bRelativeToSender=false;
	m_drCommandGroup_D_Command = NULL;
	m_drCommandGroup_Command = drAGA;
	m_PK_Command = m_drCommandGroup_Command->FK_Command_get();
	m_drCommand = m_drCommandGroup_Command->FK_Command_getrow();
	m_bDeliveryConfirmation = 0 != m_drCommandGroup_Command->DeliveryConfirmation_get();
	if( !m_drCommandGroup_Command->FK_Device_isNull() )
		m_PK_Device = m_drCommandGroup_Command->FK_Device_get();
	if( !m_drCommandGroup_Command->FK_DeviceGroup_isNull() )
		m_PK_DeviceGroup = m_drCommandGroup_Command->FK_DeviceGroup_get();


	vector<Row_CommandGroup_Command_CommandParameter *> ragas;
	m_drCommandGroup_Command->CommandGroup_Command_CommandParameter_FK_CommandGroup_Command_getrows(&ragas);
	for(size_t s=0;s<ragas.size();++s)
	{
		Row_CommandGroup_Command_CommandParameter * drAGACP = ragas[s];
		AddParm(drAGACP,Parent);
		/*
		CGCommandParm *oap = new CGCommandParm(drAGACP,Parent);
		m_alParms.push_back(oap); */
	}
}

CGCommand::CGCommand(Row_CommandGroup_D_Command * drAGA,DesignObj_Generator *Parent)
{
	m_PK_Device=0;
	m_PK_DeviceTemplate=0;
	m_PK_DeviceGroup=0;
	BroadcastLevel=0;
	m_PK_DeviceCategory=0;
	m_bRelativeToSender=false;
	m_drCommandGroup_Command = NULL;
	m_drCommandGroup_D_Command = drAGA;
	m_PK_Command = m_drCommandGroup_D_Command->FK_Command_get();
	m_drCommand = m_drCommandGroup_D_Command->FK_Command_getrow();
	m_bDeliveryConfirmation = 0 != m_drCommandGroup_D_Command->DeliveryConfirmation_get();
	if( !m_drCommandGroup_D_Command->FK_DeviceTemplate_isNull() )
		m_PK_DeviceTemplate = m_drCommandGroup_D_Command->FK_DeviceTemplate_get();
	if( !m_drCommandGroup_D_Command->FK_DeviceCategory_isNull() )
		m_PK_DeviceCategory = m_drCommandGroup_D_Command->FK_DeviceCategory_get();

	BroadcastLevel=m_drCommandGroup_D_Command->BroadcastLevel_get();
	m_bRelativeToSender=m_drCommandGroup_D_Command->RelativeToSender_get()==1;

	vector<Row_CommandGroup_D_Command_CommandParameter *> ragas;
	m_drCommandGroup_D_Command->CommandGroup_D_Command_CommandParameter_FK_CommandGroup_D_Command_getrows(&ragas);
	for(size_t s=0;s<ragas.size();++s)
	{
		Row_CommandGroup_D_Command_CommandParameter * drAGACP = ragas[s];
		AddParm(drAGACP,Parent);
		/*
		CGCommandParm *oap = new CGCommandParm(drAGACP,Parent);
		m_alParms.push_back(oap);
		*/
	}
}
void CGCommand::SharedConstructor(DesignObj_Generator *Parent)
{
// todo 2.0	m_bHandleLocally=m_drCommand->HandledByOrbiter_get()==1;
	m_PK_Device=0;
	m_PK_DeviceGroup=0;

	// Trap for virtual devices that can be resolved at buildtime
	if( m_PK_Device==-16 )	// DEVICEID_SIPPHONE
	{
		vector<Row_Device_Orbiter *> vectdc;
		Parent->m_pOrbiterGenerator->m_pRow_Orbiter->Device_Orbiter_FK_Orbiter_getrows(&vectdc);
		for(size_t s = 0; s<vectdc.size();++s)
		{
			Row_Device_Orbiter *drD_C = vectdc[s];
/* todo 2.0
			if( drD_C->FK_Device_getrow()->FK_DeviceTemplate_getrow()->FK_DeviceCategory_get()==DEVICECATEGORY_SIP_EXTENSION_CONST )
			{
				m_PK_Device = drD_C->FK_Device_get();
				break;
			}
*/
		}
		if( m_PK_Device<0 )
			throw "No Sip Phone associated with controller " + StringUtils::itos(Parent->m_pOrbiterGenerator->m_pRow_Orbiter->PK_Orbiter_get()) + " as requested in object: " + StringUtils::itos(Parent->m_pRow_DesignObj->PK_DesignObj_get());
	}
	else if( m_PK_Device==-17 )	// DEVICEID_VIDEOPHONE
	{
		vector<Row_Device_Orbiter *> vectrdc;
		Parent->m_pOrbiterGenerator->m_pRow_Orbiter->Device_Orbiter_FK_Orbiter_getrows(&vectrdc);
		for(size_t t=0;t<vectrdc.size();++t)
		{
			Row_Device_Orbiter *drD_C = vectrdc[t];
/* todo 2.0
			if( drD_C->FK_Device_getrow()->FK_DeviceTemplate_get()==DeviceTemplate_OHPHONE_CONST )
			{
				m_PK_Device = drD_C->FK_Device_get();
				break;
			}
*/
		}
		if( m_PK_Device<0 )
			throw "No video phone associated with controller " + StringUtils::itos(Parent->m_pOrbiterGenerator->m_pRow_Orbiter->PK_Orbiter_get()) + " as requested in object: " + StringUtils::itos(Parent->m_pRow_DesignObj->PK_DesignObj_get());
	}
}


void CGCommand::AddParm(Row_CommandGroup_Command_CommandParameter * drAGACP,DesignObj_Generator *Parent)
{
	bool bContainsRunTimeVariables;
	m_ParameterList[drAGACP->FK_CommandParameter_get()] = Parent->SubstituteVariables(drAGACP->IK_CommandParameter_get(),&bContainsRunTimeVariables);
}

void CGCommand::AddParm(Row_CommandGroup_D_Command_CommandParameter * drAGACP,DesignObj_Generator *Parent)
{
	bool bContainsRunTimeVariables;
	m_ParameterList[drAGACP->FK_CommandParameter_get()] = Parent->SubstituteVariables(drAGACP->IK_CommandParameter_get(),&bContainsRunTimeVariables);
}


CGZone::CGZone(Row_CommandGroup * drCommandGroup,DesignObj_Generator *Parent)
{
	m_drCommandGroup_D=NULL;
	m_drCommandGroup = drCommandGroup;
	if( m_drCommandGroup!=NULL )
	{
		vector<class Row_CommandGroup_Command *> vectraga;
		m_drCommandGroup->Table_CommandGroup_get()->Database_pluto_main_get()->CommandGroup_Command_get()->GetRows("WHERE FK_CommandGroup=" + StringUtils::itos(drCommandGroup->PK_CommandGroup_get()) + " ORDER BY OrderNum",&vectraga);
		for(size_t s=0;s<vectraga.size();++s)
		{
			class Row_CommandGroup_Command * drAGA = vectraga[s];
			CGCommand *oca = new CGCommand(drAGA,Parent);
			m_Commands.push_front((DesignObjCommand *) oca);
		}
	}
}

CGZone::CGZone(Row_CommandGroup_D * drCommandGroup_D,DesignObj_Generator *Parent)
{
	m_drCommandGroup=NULL;
	m_drCommandGroup_D = drCommandGroup_D;
	if( drCommandGroup_D!=NULL )
	{
		vector<class Row_CommandGroup_D_Command *> vectraga;
		drCommandGroup_D->Table_CommandGroup_D_get()->Database_pluto_main_get()->CommandGroup_D_Command_get()->GetRows(
			"WHERE FK_CommandGroup_D=" + StringUtils::itos(drCommandGroup_D->PK_CommandGroup_D_get()) + " ORDER BY OrderNum",&vectraga);
//		m_drCommandGroup_D->CommandGroup_D_Command_FK_CommandGroup_D_getrows(&vectraga);
		for(size_t s=0;s<vectraga.size();++s)
		{
			class Row_CommandGroup_D_Command * drAGA = vectraga[s];
			CGCommand *oca = new CGCommand(drAGA,Parent);
			m_Commands.push_front((DesignObjCommand *) oca);
		}
	}
}
