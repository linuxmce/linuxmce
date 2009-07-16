/*
 CriteriaList
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the GNU Public License, available at: 
 http://www.fsf.org/licensing/licenses/gpl.html 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Public License for more details.
 
 */
 
#include "CriteriaList.h"
#include "CriteriaNesting.h"

bool CriteriaList::Evaluate(class EventInfo *pEventInfo)
{
	return m_ptrCriteriaNesting->Evaluate(this,pEventInfo);
}

