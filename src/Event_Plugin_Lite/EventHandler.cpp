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
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "Logger.h"
#include "EventHandler.h"
#include "Event_Plugin.h"
#include "pluto_main/Define_CriteriaParmList.h"
#include "Criteria.h"
#include "CriteriaParm.h"
#include "CriteriaParmNesting.h"

EventHandler::EventHandler(unsigned long PK_EventHander,Event_Plugin *pEvent_Plugin,struct json_object *json_obj)
{
	m_pEvent_Plugin=pEvent_Plugin;
	m_tLastFired=0;
	m_PK_EventHander = PK_EventHander;
	m_bDisabled=false;
	m_OncePerSeconds=0;
	m_PK_CannedEvents = m_PK_Event = 0;

	struct json_object_iter iter_sceneparams;
	json_object_object_foreachC(json_obj, iter_sceneparams)
	{
		string sKey = iter_sceneparams.key;

		if(iter_sceneparams.val->o_type == json_type_int || iter_sceneparams.val->o_type == json_type_string)
		{
			string sValue = json_object_get_string(iter_sceneparams.val);

			if(sKey == "Description")
			{
				m_sDescription = sValue;
			}
			else if(sKey == "PK_CannedEvents" || sKey == "FK_CannedEvents")
			{
				m_PK_CannedEvents = atoi(sValue.c_str());
			}
			else if(sKey == "PK_Event")
			{
				m_PK_Event = atoi(sValue.c_str());
			}
			else if(sKey == "Enabled")
			{
				m_bDisabled = (sValue=="0");
			}
		}
		else if(sKey == "commands" && iter_sceneparams.val->o_type == json_type_object)
		{
			m_pEvent_Plugin->ParseCommands(m_mapCommands, iter_sceneparams.val);
		}
		else if(sKey == "Criteria" && iter_sceneparams.val->o_type == json_type_object)
		{
			ParseCriteria(iter_sceneparams.val);
		}
	}

	CannedEvent *pCannedEvent = m_pEvent_Plugin->m_mapCannedEvents_Find(m_PK_CannedEvents);
	if( pCannedEvent!=NULL )
	{
		if( m_PK_Event==0 )
			m_PK_Event = pCannedEvent->m_FK_Event;
		
		m_pCriteria->m_pCriteriaParmNesting->m_bAnd = pCannedEvent->m_bIsAnd;
		m_pCriteria->m_pCriteriaParmNesting->m_bNot = pCannedEvent->m_bIsNot;
	}
}

void EventHandler::ParseCriteria(struct json_object *json_obj)
{
	CriteriaParmNesting *pCriteriaParmNesting = new CriteriaParmNesting(false,true);
	m_pCriteria = new Criteria(1,pCriteriaParmNesting);

	struct json_object_iter iter_criteriaparams;
	json_object_object_foreachC(json_obj, iter_criteriaparams)
	{
		string sKey = iter_criteriaparams.key;

		if(StringUtils::StartsWith(sKey,"param_") && iter_criteriaparams.val->o_type == json_type_object)
		{
			struct json_object_iter iter_criteriaparams_2;

			int PK_CriteriaParmList=0,Operator=0,PK_ParameterType=0;
			string sParm,sValue;

			json_object_object_foreachC(iter_criteriaparams.val, iter_criteriaparams_2)
			{
				string sKey_2 = iter_criteriaparams_2.key;

				if(sKey_2=="PK_CriteriaParmList" && (iter_criteriaparams_2.val->o_type == json_type_int || iter_criteriaparams_2.val->o_type == json_type_string) )
					PK_CriteriaParmList =json_object_get_int(iter_criteriaparams_2.val);
				else if( sKey_2=="Operator" && (iter_criteriaparams_2.val->o_type == json_type_int || iter_criteriaparams_2.val->o_type == json_type_string) )
					Operator = json_object_get_int(iter_criteriaparams_2.val);
				else if( sKey_2=="PK_ParameterType" && (iter_criteriaparams_2.val->o_type == json_type_int || iter_criteriaparams_2.val->o_type == json_type_string) )
					PK_ParameterType = json_object_get_int(iter_criteriaparams_2.val );
				else if( sKey_2=="Parm" && (iter_criteriaparams_2.val->o_type == json_type_int || iter_criteriaparams_2.val->o_type == json_type_string) )
					sParm = json_object_get_string(iter_criteriaparams_2.val);
				else if( sKey_2=="Value" && (iter_criteriaparams_2.val->o_type == json_type_int || iter_criteriaparams_2.val->o_type == json_type_string) )
					sValue = json_object_get_string(iter_criteriaparams_2.val);
			}

			CriteriaParm *pCriteriaParm = new CriteriaParm(
					1 /*pRow_CriteriaParm->PK_CriteriaParm_get() */,PK_CriteriaParmList,
					PK_ParameterType==0 ? m_pEvent_Plugin->m_mapCriteriaParmList_ParameterType_Find(PK_CriteriaParmList) : PK_ParameterType,Operator,
					sValue,sParm);

			pCriteriaParmNesting->m_vectCriteriaParm.push_back( pCriteriaParm );
		}
	}
}
