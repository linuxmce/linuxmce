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

 /** @file CriteriaParm.h
For ???
 */
#ifndef CRITERIAPARM_H
#define CRITERIAPARM_H


/** @class CriteriaParm
For ???
*/
class CriteriaParm
{
public:
	string m_sComparedValue;
	bool m_bResult;
	int m_iPK_CriteriaParm;
	int m_iPK_CriteriaParmList;
	int m_iPK_ParameterType;
	int m_Operator;
	string m_sValue;

	CriteriaParm(int PK_CriteriaParm,int PK_CriteriaParmList,int PK_ParameterType,int Operator,string sValue) :
		m_iPK_CriteriaParm(PK_CriteriaParm),m_iPK_CriteriaParmList(PK_CriteriaParmList),
		m_iPK_ParameterType(PK_ParameterType), m_Operator(Operator), m_sValue(sValue)
	{}


};

#endif

