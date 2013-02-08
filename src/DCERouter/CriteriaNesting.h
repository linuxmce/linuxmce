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

/** @file CriteriaNesting.h
For ???
 */
#ifndef CRITERIANESTING_H
#define CRITERIANESTING_H

#include "PlutoUtils/CommonIncludes.h"
#include "CriteriaParm.h"


/** @class CriteriaNesting
For ???
*/
class CriteriaNesting
{
public:
	bool m_bNot,m_bAnd;
	vector<class CriteriaParm *> m_vectCriteriaParm;
	vector<class CriteriaNesting *> m_vectCriteriaNesting;

	CriteriaNesting(bool bNot,bool bAnd) : m_bNot(bNot), m_bAnd(bAnd) {}

	bool Evaluate(class CriteriaList *ptrCriteriaList,class EventInfo *pEventInfo);
/*
	{
		for(int i=0;i<m_vectCriteriaParm.size();++i)
		{
			bool bResult = ptrCriteriaList->EvaluateExpression(m_vectCriteriaParm[i]);
			if( bAnd==true && bResult==false )
				return !(!bNot ^ false) ;
			if( bAnd==false && bResult==true )
				return !(!bNot ^ true);
		}

		for(int i=0;i<m_vectCriteriaNesting.size();++i)
		{
			bool bResult = m_vectCriteriaNesting[i].Evaluate(ptrCriteriaList);
			if( bAnd==true && bResult==false )
				return !(!bNot ^ false);
			if( bAnd==false && bResult==true )
				return !(!bNot ^ true);
		}
		// If this is an 'and' we're true because we got through everything without a false,
		// If this is an 'or' we're false because we never got a true value
		return !(!bNot ^ bAnd);
	}
	*/
};

#endif

