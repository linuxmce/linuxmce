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
#ifndef Critera_h
#define Critera_h

class Row_CriteriaParmNesting;
class Row_CriteriaParm;

namespace DefaultScenarios
{
	class CriteriaParm;
	typedef enum
	{
		operatorEquals=1,
		operatorNotEquals=2,
		operatorGreaterThan=3,
		operatorLessThan=4
	}
	enumOperator;

	// This holds a Critera, which is a database-driven way to evaluate expressions, using a concept of
	// nesting and criteria within the nesting.  Such as:

	//	Nesting 1 -- AND  (CriteriaParmNesting)
	//	house mode = 1  (CriteriaParm)
	//	nighttime = 1  (CriteriaParm)
	//		Nesting 2 -- OR  (CriteriaParmNesting)
	//		Device = 2  (CriteriaParm)
	//		Device = 5  (CriteriaParm)
	//			Nesting 3 -- AND  (CriteriaParmNesting)
	//			User Status = 1  (CriteriaParm)
	//			User = 5  (CriteriaParm)

	// translation:
	// house_mode==1 && nighttime==1 && (Device==2 || Device==5 || (UserStatus==1 && User==5))

class CriteriaParmNesting
{
public:
	size_t iNumCriteriaAdded;
	vector<Row_CriteriaParmNesting *> m_vectRow_CriteriaParmNesting;
	vector<Row_CriteriaParm *> m_vectRow_CriteriaParm;
	deque<CriteriaParmNesting *> m_dequeCriteriaParmNesting;
	deque<CriteriaParm *> m_dequeCriteriaParm;
	Row_CriteriaParmNesting *m_pRow_CriteriaParmNesting;
	bool m_bIsAnd;
	Row_CriteriaParmNesting *m_vectRow_CriteriaParmNesting_get(size_t iPosition);  // Return the given position, creating the record if it doesn't exist

	CriteriaParmNesting(bool bIsAnd,Row_CriteriaParmNesting *pRow_CriteriaParmNesting);

	bool UserMadeChanges();
	bool Commit(CriteriaParmNesting *pCriteriaParmNesting_Parent);
	CriteriaParm *new_CriteriaParm(int FK_CriteriaParmList,string sParm,
		enumOperator Operator,string sValue,int FK_CannedEvents_CriteriaParmList=0);
};

class CriteriaParm
{
public:
	Row_CriteriaParm *m_pRow_CriteriaParm;

	int m_iFK_CriteriaParmList,m_iFK_CannedEvents_CriteriaParmList;
	string m_sParm,m_sValue;
	enumOperator m_Operator;

	CriteriaParm(Row_CriteriaParm *pRow_CriteriaParm, int FK_CriteriaParmList,string sParm,
		enumOperator Operator,string sValue,int FK_CannedEvents_CriteriaParmList=0)
	{ 
		m_pRow_CriteriaParm=pRow_CriteriaParm;
		m_iFK_CriteriaParmList=FK_CriteriaParmList;
		m_iFK_CannedEvents_CriteriaParmList=FK_CannedEvents_CriteriaParmList;
		m_sParm=sParm;
		m_sValue=sValue;
		m_Operator=Operator;
	}

	bool Commit(CriteriaParmNesting *pCriteriaParmNesting_Parent);
};

};

#endif
