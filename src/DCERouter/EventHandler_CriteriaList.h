/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com


 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the GNU Public License, available at:
 http://www.fsf.org/licensing/licenses/gpl.html

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Public License for more details.

 */

 /** @file EventHandler_CriteriaList.h
For ???
 */
#ifndef EVENTHANDLER_CRITERIALIST_H
#define EVENTHANDLER_CRITERIALIST_H

#include "CriteriaList.h"


/** @class EventHandler_CriteriaList
For ???
*/
class EventHandler_CriteriaList : public CriteriaList
{
public:
	EventHandler_CriteriaList(int PK_CriteriaList,int PK_Criteria,class CriteriaNesting *ptrCriteriaNesting) :
		CriteriaList(PK_CriteriaList,PK_Criteria,ptrCriteriaNesting) {}
	virtual ~EventHandler_CriteriaList()
	{}

	virtual bool EvaluateExpression(class CriteriaParm *CriteriaParm,class EventInfo *pEventInfo);
};

#endif

