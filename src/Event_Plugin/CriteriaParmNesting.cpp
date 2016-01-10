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
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include "CriteriaParmNesting.h"
#include "Criteria.h"
#include "CriteriaParm.h"
#include "EventInfo.h"
#include "EventHandler.h"

CriteriaParmNesting::~CriteriaParmNesting()
{
	for(vector<CriteriaParm *>::iterator itc = m_vectCriteriaParm.begin(), endc = m_vectCriteriaParm.end(); itc != endc; ++itc)
		delete *itc;
	
	m_vectCriteriaParm.clear();

	for(vector<CriteriaParmNesting *>::iterator itcn = m_vectCriteriaParmNesting.begin(), endcn = m_vectCriteriaParmNesting.end(); itcn != endcn; ++itcn)
		delete *itcn;

	m_vectCriteriaParmNesting.clear();
}

bool CriteriaParmNesting::Evaluate(class Criteria *pCriteria,class EventInfo *pEventInfo,void *pExtraInfo)
{
	if( m_vectCriteriaParm.size()==0 && m_vectCriteriaParmNesting.size()==0 )
		return true;

	for(size_t i=0;i<m_vectCriteriaParm.size();++i)
	{
		CriteriaParm *pCriteriaParm = m_vectCriteriaParm[i];
//		pEventInfo->pEventHandler->m_vectCriteriaParm.push_back(pCriteriaParm);

		pCriteriaParm->m_bResult = pCriteria->EvaluateExpression(pCriteriaParm,pEventInfo,pExtraInfo);
		if( m_bAnd==true && pCriteriaParm->m_bResult==false )
			return !(!m_bNot ^ false) ;
		if( m_bAnd==false && pCriteriaParm->m_bResult==true )
			return !(!m_bNot ^ true);
	}

	for(size_t i=0;i<m_vectCriteriaParmNesting.size();++i)
	{
		bool bResult = m_vectCriteriaParmNesting[i]->Evaluate(pCriteria,pEventInfo,pExtraInfo);
		if( m_bAnd==true && bResult==false )
			return !(!m_bNot ^ false);
		if( m_bAnd==false && bResult==true )
			return !(!m_bNot ^ true);
	}
	// If this is an 'and' we're true because we got through everything without a false,
	// If this is an 'or' we're false because we never got a true value
	return !(!m_bNot ^ m_bAnd);  
}

