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
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <time.h>

#include "EventHandler.h"
#include "EventInstance.h"
#include "CriteriaParm.h"
#include "Criteria.h"
#include "EventInfo.h"
#include "DeviceData_Router.h"
#include "Message.h"
#include "DCERouter.h"

#include "Climate_Plugin/Climate_Plugin.h"

#include "pluto_main/Define_CriteriaParmList.h"
#include "pluto_main/Define_ParameterType.h"
#include "pluto_main/Table_EventParameter.h"

#define OPERATOR_EQUALS			1
#define OPERATOR_NOTEQUALS		2
#define OPERATOR_GREATERTHAN	3
#define OPERATOR_LESSTHAN		4
#define OPERATOR_CONTAINS		5

bool EvaluateTimeOfDay(CriteriaParm* pCriteriaParm,void *pExtraInfo, struct tm newtime);
bool EvaluateInt(int oper, int val1, int val2);

bool Criteria::EvaluateExpression(class CriteriaParm *pCriteriaParm,class EventInfo *pEventInfo,void *pExtraInfo)
{
	if ( pEventInfo->m_bTimedEvent && 
	     (pCriteriaParm->m_iPK_CriteriaParmList == CRITERIAPARMLIST_Time_of_day_CONST ||
	      pCriteriaParm->m_iPK_CriteriaParmList == CRITERIAPARMLIST_Day_Of_Week_CONST ||
          pCriteriaParm->m_iPK_CriteriaParmList == CRITERIAPARMLIST_Day_Of_Month_CONST ||
          pCriteriaParm->m_iPK_CriteriaParmList == CRITERIAPARMLIST_Specific_Date_CONST))
	{
		// if this is a timed event, these criteria are handled by the timer itself
		return true;
	}

	string::size_type pos=0;
	string temp;
	
	time_t long_time;
    time( &long_time );
	struct tm newtime;
	localtime_r( &long_time, &newtime );

	if( pCriteriaParm->m_sValue.size()==0 )
		return true;  // Was not specified
	int iRValue=0;
	string sRValue="";

	int PK_ParameterType= pCriteriaParm->m_iPK_ParameterType;
	string *sLValue=NULL;
	unsigned long *iLValue=NULL;

	// Available for temporary use
	unsigned long  iTmp;
	string sTmp;

	switch(pCriteriaParm->m_iPK_CriteriaParmList)
	{
	case CRITERIAPARMLIST_Month_CONST:
		iTmp = newtime.tm_mon+1;  // Month is 0 based
		iLValue = &iTmp;
		break;
	case CRITERIAPARMLIST_PK_Device_CONST:
	        if( atoi(pCriteriaParm->m_sValue.c_str())==0 )
			return true;
		iLValue = (unsigned long *)&pEventInfo->m_pDevice->m_dwPK_Device;
		break;
	case CRITERIAPARMLIST_PK_DeviceTemplate_CONST:
		iLValue = (unsigned long *)&pEventInfo->m_pDevice->m_dwPK_DeviceTemplate;
		break;
	case CRITERIAPARMLIST_Time_of_day_CONST:
	        return EvaluateTimeOfDay(pCriteriaParm,pExtraInfo, newtime);
	case CRITERIAPARMLIST_PK_DeviceCategory_CONST:
		iLValue = &pEventInfo->m_pDevice->m_dwPK_DeviceCategory;
		break;
	case CRITERIAPARMLIST_PK_Room_CONST:
		if( !pEventInfo->m_pDevice->m_pRoom )
			return false;
		iLValue = &pEventInfo->m_pDevice->m_pRoom->m_dwPK_Room;
		break;
//	case CRITERIAPARMLIST_PK_DeviceGroup_CONST:
//		return pEventInfo->m_pDevice->m_mapDeviceGroups[atoi(pCriteriaParm->m_sValue.c_str())];
	case CRITERIAPARMLIST_House_Mode_CONST:
		iLValue = (unsigned long *) (&pEventInfo->m_PK_HouseMode);
		break;
//	case CRITERIAPARMLIST_Room_Mode_CONST:
//		iLValue = &pEventInfo->PK_C_Mode_Room;
//		break;
	case CRITERIAPARMLIST_Day_Of_Week_CONST:
		iLValue = (unsigned long *) (&newtime.tm_wday);
		break;
	case CRITERIAPARMLIST_Day_Of_Month_CONST:
		iLValue = (unsigned long *) (&newtime.tm_mday);
		break;
	case CRITERIAPARMLIST_Specific_Date_CONST:
		break;
//	case CRITERIAPARMLIST_PK_RoomType_CONST:
//		iLValue = &pEventInfo->m_pDevice->m_pRoom->m_im_iPK_RoomType;
//		break;
	case CRITERIAPARMLIST_PK_EventList_CONST:
		iLValue = (unsigned long *) (&pEventInfo->m_pMessage->m_dwID);
		break;
	case CRITERIAPARMLIST_PK_EventHandler_CONST:
		iLValue = &pEventInfo->m_pEventHandler->m_PK_EventHander;
		break;
	case CRITERIAPARMLIST_PK_EventParameter_CONST:
		{
			int PK_EventParameter = atoi( pCriteriaParm->m_sParm.c_str() );
			temp = pEventInfo->m_pMessage->m_mapParameters[PK_EventParameter];
			sLValue = &temp;
			// Look up parameter type and override criteria type
			Router *pRouter = (Router *) pExtraInfo;
			if (pRouter)
			{
			        Row_EventParameter* eventParameter = pRouter->GetDatabase()->EventParameter_get()->GetRow(PK_EventParameter);
				PK_ParameterType = eventParameter->FK_ParameterType_get();
				LoggerWrapper::GetInstance()->Write(LV_EVENT,"Criteria::EvaluateExpression parameterType %d", PK_ParameterType);
				if (PK_ParameterType == PARAMETERTYPE_int_CONST || PK_ParameterType == PARAMETERTYPE_bool_CONST)
				{
				        iTmp = (unsigned long)atoi(sLValue->c_str());
					iLValue = &iTmp;
				}
			}
		}
		break;
	case CRITERIAPARMLIST_State_CONST:
	        {
			Router *pRouter = (Router *) pExtraInfo;
			int PK_Device = 0, valueType = 0;
			string::size_type pos = pCriteriaParm->m_sParm.find('.');
			if (pos != string::npos)
			{
				PK_Device = atoi( pCriteriaParm->m_sParm.substr(0,pos).c_str() );
				valueType = atoi( pCriteriaParm->m_sParm.substr(pos+1).c_str() );
			} else {
				PK_Device = atoi( pCriteriaParm->m_sParm.c_str() );
			}
			LoggerWrapper::GetInstance()->Write(LV_EVENT,"Criteria::EvaluateExpression Device State: PK_Device = %d, valueType = %d", PK_Device, valueType);
			
			if (PK_Device > 0) 
			{
				PK_ParameterType = PARAMETERTYPE_string_CONST;
				DeviceData_Router* pDevice = pRouter->m_mapDeviceData_Router_Find(PK_Device);
				if (pDevice)
				{
					if (pDevice->m_dwPK_DeviceCategory == DEVICECATEGORY_Climate_Device_CONST)
					{
						string sOn;	string sMode; string sFan; string sSetPoint; string sTemp;
						Climate_Plugin::GetStateVar(pDevice, sOn, sMode, sFan, sSetPoint, sTemp);
						LoggerWrapper::GetInstance()->Write(LV_EVENT,"Criteria::EvaluateExpression Climate device: sOn = %s, sMode = %s, sFan = %s, sSetPoint = %s, sTemp = %s", sOn.c_str(), sMode.c_str(), sFan.c_str(), sSetPoint.c_str(), sTemp.c_str());
						switch (valueType)
						{
						case 1:
							sTmp = sOn;
							sLValue = &sTmp;
							break;
						case 2:
							sTmp = sOn;
							sLValue = &sTmp;
							break;
						case 3:
							sTmp = sOn;
							sLValue = &sFan;
							break;
						case 4:
							PK_ParameterType = PARAMETERTYPE_int_CONST;
							iTmp = atoi(sSetPoint.c_str());
							iLValue = &iTmp;
							break;
						case 5:
						default:
							PK_ParameterType = PARAMETERTYPE_int_CONST;
							iTmp = atoi(sTemp.c_str());
							iLValue = &iTmp;
							break;
						}
					} else if (pDevice->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Brightness_sensor_CONST)
					{
						PK_ParameterType = PARAMETERTYPE_int_CONST;
						iTmp = atoi(pDevice->m_sState_get().c_str());
						iLValue = &iTmp;
					} else {
						// Here we can add special handling for more types of devices, just make sure to
						// also make changes to the web admin (utils.inc.php::displayCriteria)
						sTmp = pDevice->m_sState_get();
						sLValue = &sTmp;
					}
				}
			}
		}
		break;
	default:
	  LoggerWrapper::GetInstance()->Write(LV_WARNING,"Criteria::EvaluateExpression Unknown Criteria Parm List %d", pCriteriaParm->m_iPK_CriteriaParmList);
	        throw exception();
	}

	bool bCriteriaList = PK_ParameterType==PARAMETERTYPE_int_CONST && pCriteriaParm->m_sValue.find(",") != string::npos;
	if( !bCriteriaList && (PK_ParameterType==PARAMETERTYPE_int_CONST || PK_ParameterType==PARAMETERTYPE_bool_CONST))
		iRValue = atoi(pCriteriaParm->m_sValue.c_str());
	else
		sRValue = pCriteriaParm->m_sValue;

	if( PK_ParameterType==PARAMETERTYPE_string_CONST && sLValue==NULL )
		throw(string("comparing string, string is null"));
	else if( (PK_ParameterType==PARAMETERTYPE_int_CONST || PK_ParameterType==PARAMETERTYPE_bool_CONST) && iLValue==NULL )
		throw(string("comparing int, int is null"));

	if( PK_ParameterType==PARAMETERTYPE_int_CONST || PK_ParameterType==PARAMETERTYPE_bool_CONST )
	{
		pCriteriaParm->m_sComparedValue = StringUtils::itos(*iLValue);
	}
	else
	{
		pCriteriaParm->m_sComparedValue = *sLValue;
	}

	if (PK_ParameterType==PARAMETERTYPE_int_CONST || PK_ParameterType==PARAMETERTYPE_bool_CONST ) {
		if (bCriteriaList) {
			LoggerWrapper::GetInstance()->Write(LV_EVENT,"Criteria::EvaluateExpression criteria is a list of values: %s", sRValue.c_str());
			vector<string> vecValues;
			StringUtils::Tokenize(sRValue, ",", vecValues);
			if (pCriteriaParm->m_Operator == OPERATOR_EQUALS) {
				int i = 0;
				bool bValueFound = false;
				while (i < vecValues.size() && !bValueFound) {
					bValueFound = *iLValue == atoi(vecValues[i].c_str());
					i++;
				}
				return bValueFound;
			} else if (pCriteriaParm->m_Operator == OPERATOR_NOTEQUALS) {
				for (int i = 0; i < vecValues.size(); i++) {
					if (*iLValue == atoi(vecValues[i].c_str())) {
						return false;
					}
				}
				return true;
			}
		} else {
			return EvaluateInt(pCriteriaParm->m_Operator, *iLValue, iRValue);
		}
	} else if( PK_ParameterType==PARAMETERTYPE_string_CONST ) {
		if( pCriteriaParm->m_Operator==OPERATOR_EQUALS )
			return *sLValue==sRValue;
		if( pCriteriaParm->m_Operator==OPERATOR_NOTEQUALS )
			return *sLValue!=sRValue;
		if( pCriteriaParm->m_Operator==OPERATOR_CONTAINS )
			return StringUtils::ToUpper(*sLValue).find(StringUtils::ToUpper(sRValue))!=string::npos;
		if( pCriteriaParm->m_Operator==OPERATOR_GREATERTHAN )
       			return *sLValue>sRValue;
		if( pCriteriaParm->m_Operator==OPERATOR_LESSTHAN )
			return *sLValue<sRValue;
	}

	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Criteria::EvaluateExpression unhandled criteria operator %d",pCriteriaParm->m_Operator);
	return false;
}

#include "../General_Info_Plugin/General_Info_Plugin.h"
#include "DCERouter.h"

bool EvaluateTimeOfDay(CriteriaParm* pCriteriaParm,void *pExtraInfo, struct tm newtime)
{
        string Expression = StringUtils::ToUpper(pCriteriaParm->m_sValue);
	General_Info_Plugin *pGeneral_Info_Plugin = NULL;
	Router *pRouter = (Router *) pExtraInfo;
	if( pRouter )
	{
		ListDeviceData_Router *pList = pRouter->m_mapDeviceByTemplate_Find(DEVICETEMPLATE_General_Info_Plugin_CONST);
		if( pList && pList->size() )
		{
			DeviceData_Router *pDevice = *(pList->begin());
			pGeneral_Info_Plugin = (General_Info_Plugin *) pRouter->m_mapPlugIn_Find(pDevice->m_dwPK_Device);
		}
	}

	if( Expression=="DAY" || Expression=="NIGHT")
	{
		if( !pGeneral_Info_Plugin )
			return false;

		bool bIsDaytime;
		pGeneral_Info_Plugin->CMD_Is_Daytime(&bIsDaytime);
		return (Expression=="DAY" && bIsDaytime) || (Expression=="NIGHT" && !bIsDaytime);
	}

	if (Expression.find(":")!=string::npos) 
	{
	        if (Expression.find(":", Expression.find(":")+1)==string::npos)
		{
		  // Only one :, add 00 seconds
		  Expression += ":00";
		}
		int iTimeNow = newtime.tm_hour * 3600 + newtime.tm_min * 60 + newtime.tm_sec;
		int iTimeExp = StringUtils::TimeAsSeconds(Expression);
		return EvaluateInt(pCriteriaParm->m_Operator, iTimeNow, iTimeExp);
	}
	return true;
}

bool EvaluateInt(int oper, int val1, int val2)
{
        if (oper == OPERATOR_EQUALS)
	{
	        return val1 == val2;
	}
        if (oper == OPERATOR_NOTEQUALS)
	{
	        return val1 != val2;
	}
        if (oper == OPERATOR_GREATERTHAN)
	{
	        return val1 > val2;
	}
        if (oper == OPERATOR_LESSTHAN)
	{
	        return val1 < val2;
	}
	return false;
}
