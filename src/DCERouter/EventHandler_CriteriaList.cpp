/*
 EventHandler_CriteriaList
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the GNU Public License, available at: 
 http://www.fsf.org/licensing/licenses/gpl.html 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Public License for more details.
 
 */
 
#include <time.h>

#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "Message.h"
#include "Event_Impl.h"
#include "EventHandler.h"
#include "EventHandler_CriteriaList.h"
#include "EventInstance.h"
#include "CriteriaParm.h"
#include "EventInfo.h"
#include "DeviceData_Router.h"

#include "../pluto_main/Table_CriteriaParmList.h"
#include "../pluto_main/Table_DesignObjParameter.h"
#include "../pluto_main/Table_ParameterType.h"

#define OPERATOR_EQUALS			1
#define OPERATOR_NOTEQUALS		2
#define OPERATOR_GREATERTHAN	3
#define OPERATOR_LESSTHAN		4

bool EventHandler_CriteriaList::EvaluateExpression(class CriteriaParm *CriteriaParm,class EventInfo *pEventInfo)
{
	/*
	int PK_EventParameter;
	string::size_type pos=0;
	string temp;
	
	time_t long_time;
    time( &long_time );
	struct tm *newtime = localtime( &long_time );

	int iRValue=0;
	string sRValue="";

	if( CriteriaParm->m_iPK_ParameterType==C_PARAMETERTYPE_INT_CONST || CriteriaParm->m_iPK_ParameterType==C_PARAMETERTYPE_BOOL_CONST)
		iRValue = atoi(CriteriaParm->m_sValue.c_str());
	else
		sRValue = CriteriaParm->m_sValue;

	string *sLValue=NULL;
	int *iLValue=NULL;

	// Available for temporary use
	int iTmp;
	string sTmp;

	switch(CriteriaParm->m_iPK_CriteriaParmList)
	{
	case CRITERIAPARMLIST_MONTH_CONST:
		iTmp = newtime->tm_mon+1;  // Month is 0 based
		iLValue = &iTmp;
		break;
	case CRITERIAPARMLIST_PK_DEVICE_CONST:
		iLValue = &pEventInfo->pDevice->m_dwPK_Device;
		break;
	case CRITERIAPARMLIST_PK_DEVICETEMPLATE_CONST:
		iLValue = &pEventInfo->pDevice->m_dwPK_DeviceTemplate;
		break;
	case CRITERIAPARMLIST_TIME_OF_DAY_CONST:
		break;
	case CRITERIAPARMLIST_PK_DEVICECATEGORY_CONST:
		iLValue = &pEventInfo->pDevice->m_dwPK_DeviceCategory;
		break;
	case CRITERIAPARMLIST_PK_ROOM_CONST:
		iLValue = &pEventInfo->pDevice->m_dwPK_Room;
		break;
	case CRITERIAPARMLIST_PK_DEVICEGROUP_CONST:
		return pEventInfo->pDevice->m_mapDeviceGroups[atoi(CriteriaParm->m_sValue.c_str())];
	case CRITERIAPARMLIST_C_MODE_HOUSE_CONST:
		iLValue = &pEventInfo->PK_C_Mode_House;
		break;
	case CRITERIAPARMLIST_C_MODE_ROOM_CONST:
		iLValue = &pEventInfo->PK_C_Mode_Room;
		break;
	case CRITERIAPARMLIST_DAY_OF_WEEK_CONST:
		iLValue = &newtime->tm_wday;
		break;
	case CRITERIAPARMLIST_DAY_OF_MONTH_CONST:
		iLValue = &newtime->tm_mday;
		break;
	case CRITERIAPARMLIST_SPECIFIC_DATE_CONST:
		break;
	case CRITERIAPARMLIST_PK_C_ROOMTYPE_CONST:
		iLValue = &pEventInfo->pDevice->m_dwPK_RoomType;
		break;
	case CRITERIAPARMLIST_PK_EVENT_CONST:
		{
			int ID = pEventInfo->pMessage->m_dwID;
			iLValue = &ID;
		}
		break;
	case CRITERIAPARMLIST_PK_EVENT_CONST:
		iLValue = &pEventInfo->pEventHandler->m_PK_EventHander;
		break;
	case CRITERIAPARMLIST_EVENT_PARM_CONST:
		PK_EventParameter = atoi( StringUtils::Tokenize(sRValue, "~", pos).c_str());;
		sRValue = StringUtils::Tokenize(sRValue, "~", pos);
		temp = pEventInfo->pMessage->m_mapParameters[PK_EventParameter];
		sLValue = &temp;
		break;
	default:
		throw(string("Unknown Criteria Parm List" + StringUtils::itos(CriteriaParm->m_iPK_CriteriaParmList)));
	}

	if( CriteriaParm->m_iPK_ParameterType==C_PARAMETERTYPE_INT_CONST || CriteriaParm->m_iPK_ParameterType==C_PARAMETERTYPE_BOOL_CONST )
	{
		CriteriaParm->m_sComparedValue = StringUtils::itos(*iLValue);
	}
	else
	{
		CriteriaParm->m_sComparedValue = *sLValue;
	}

	if( CriteriaParm->m_iPK_ParameterType==C_PARAMETERTYPE_STRING_CONST && sLValue==NULL )
		throw(string("comparing string, string is null"));
	else if( (CriteriaParm->m_iPK_ParameterType==C_PARAMETERTYPE_INT_CONST || CriteriaParm->m_iPK_ParameterType==C_PARAMETERTYPE_BOOL_CONST) && iLValue==NULL )
		throw(string("comparing int, int is null"));

	if( CriteriaParm->m_Operator==OPERATOR_EQUALS )
	{
		if( CriteriaParm->m_iPK_ParameterType==C_PARAMETERTYPE_STRING_CONST )
			return *sLValue==sRValue;
		else if( CriteriaParm->m_iPK_ParameterType==C_PARAMETERTYPE_INT_CONST || CriteriaParm->m_iPK_ParameterType==C_PARAMETERTYPE_BOOL_CONST)
			return *iLValue==iRValue;
	}
	if( CriteriaParm->m_Operator==OPERATOR_NOTEQUALS )
	{
		if( CriteriaParm->m_iPK_ParameterType==C_PARAMETERTYPE_STRING_CONST )
			return *sLValue!=sRValue;
		else if( CriteriaParm->m_iPK_ParameterType==C_PARAMETERTYPE_INT_CONST || CriteriaParm->m_iPK_ParameterType==C_PARAMETERTYPE_BOOL_CONST)
			return *iLValue!=iRValue;
	}
	if( CriteriaParm->m_Operator==OPERATOR_GREATERTHAN )
	{
		if( CriteriaParm->m_iPK_ParameterType==C_PARAMETERTYPE_STRING_CONST )
			return *sLValue>sRValue;
		else if( CriteriaParm->m_iPK_ParameterType==C_PARAMETERTYPE_INT_CONST || CriteriaParm->m_iPK_ParameterType==C_PARAMETERTYPE_BOOL_CONST)
			return *iLValue>iRValue;
	}
	if( CriteriaParm->m_Operator==OPERATOR_LESSTHAN )
	{
		if( CriteriaParm->m_iPK_ParameterType==C_PARAMETERTYPE_STRING_CONST )
			return *sLValue<sRValue;
		else if( CriteriaParm->m_iPK_ParameterType==C_PARAMETERTYPE_INT_CONST || CriteriaParm->m_iPK_ParameterType==C_PARAMETERTYPE_BOOL_CONST)
			return *iLValue<iRValue;
	}
	throw(string("Unhandled criteria"));
*/
return false;
}
