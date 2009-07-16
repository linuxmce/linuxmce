/*
 CriteriaNesting
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the GNU Public License, available at: 
 http://www.fsf.org/licensing/licenses/gpl.html 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Public License for more details.
 
 */
 
#include "CriteriaNesting.h"
#include "CriteriaList.h"
#include "CriteriaParm.h"
#include "EventInfo.h"
#include "EventHandler.h"

bool CriteriaNesting::Evaluate(class CriteriaList *ptrCriteriaList,class EventInfo *pEventInfo)
{
	if( m_vectCriteriaParm.size()==0 && m_vectCriteriaNesting.size()==0 )
		return true;

	for(size_t i=0;i<m_vectCriteriaParm.size();++i)
	{
		CriteriaParm *pCriteriaParm = m_vectCriteriaParm[i];
		pEventInfo->pEventHandler->m_vectCriteriaParm.push_back(pCriteriaParm);

		pCriteriaParm->m_bResult = ptrCriteriaList->EvaluateExpression(pCriteriaParm,pEventInfo);
		if( m_bAnd==true && pCriteriaParm->m_bResult==false )
			return !(!m_bNot ^ false) ;
		if( m_bAnd==false && pCriteriaParm->m_bResult==true )
			return !(!m_bNot ^ true);
	}

	for(size_t i=0;i<m_vectCriteriaNesting.size();++i)
	{
		bool bResult = m_vectCriteriaNesting[i]->Evaluate(ptrCriteriaList,pEventInfo);
		if( m_bAnd==true && bResult==false )
			return !(!m_bNot ^ false);
		if( m_bAnd==false && bResult==true )
			return !(!m_bNot ^ true);
	}
	// If this is an 'and' we're true because we got through everything without a false,
	// If this is an 'or' we're false because we never got a true value
	return !(!m_bNot ^ m_bAnd);  
}
