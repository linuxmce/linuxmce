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
#include "Logger.h"
#include "UpdateEntArea.h"
#include "Criteria.h"

#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_CommandGroup_Room.h"
#include "pluto_main/Table_CommandGroup_EntertainArea.h"
#include "pluto_main/Table_CommandGroup_Command.h"
#include "pluto_main/Table_CommandGroup_Command_CommandParameter.h"
#include "pluto_main/Table_CriteriaParmNesting.h"
#include "pluto_main/Table_CriteriaParm.h"

using namespace DefaultScenarios;

CriteriaParmNesting::CriteriaParmNesting(bool bIsAnd,Row_CriteriaParmNesting *pRow_CriteriaParmNesting)
{
	iNumCriteriaAdded=0;
	m_bIsAnd=bIsAnd;
	m_pRow_CriteriaParmNesting=pRow_CriteriaParmNesting; 
	m_pRow_CriteriaParmNesting->CriteriaParmNesting_FK_CriteriaParmNesting_Parent_getrows(&m_vectRow_CriteriaParmNesting);
	m_pRow_CriteriaParmNesting->CriteriaParm_FK_CriteriaParmNesting_getrows(&m_vectRow_CriteriaParm);
}

bool CriteriaParmNesting::Commit(CriteriaParmNesting *pCriteriaParmNesting_Parent)
{
	if( UserMadeChanges() )
		return false;

	m_pRow_CriteriaParmNesting->IsAnd_set(m_bIsAnd);
	if( pCriteriaParmNesting_Parent )
		m_pRow_CriteriaParmNesting->FK_CriteriaParmNesting_Parent_set(pCriteriaParmNesting_Parent->m_pRow_CriteriaParmNesting->PK_CriteriaParmNesting_get());
	else
		m_pRow_CriteriaParmNesting->FK_CriteriaParmNesting_Parent_setNull(true);

	m_pRow_CriteriaParmNesting->Table_CriteriaParmNesting_get()->Commit();
	m_pRow_CriteriaParmNesting->Table_CriteriaParmNesting_get()->Database_pluto_main_get()->threaded_db_wrapper_query(
		"UPDATE CriteriaParmNesting set psc_mod=0 WHERE PK_CriteriaParmNesting=" + 
		StringUtils::itos(m_pRow_CriteriaParmNesting->PK_CriteriaParmNesting_get()));

	for(size_t s=0;s<m_dequeCriteriaParm.size();++s)
		if( !m_dequeCriteriaParm[s]->Commit(this) )
			return false;

	for(size_t s=0;s<m_dequeCriteriaParmNesting.size();++s)
		if( !m_dequeCriteriaParmNesting[s]->Commit(this) )
			return false;

	return true;
}

bool CriteriaParmNesting::UserMadeChanges()
{
	for(size_t s=0;s<m_dequeCriteriaParmNesting.size();++s)
		if( m_dequeCriteriaParmNesting[s]->UserMadeChanges() )
			return true;
	
	return false;
}

CriteriaParm *CriteriaParmNesting::new_CriteriaParm(int FK_CriteriaParmList,string sParm,
	enumOperator Operator,string sValue,int FK_CannedEvents_CriteriaParmList)
{
	Row_CriteriaParm *pRow_CriteriaParm;
	if( iNumCriteriaAdded<m_vectRow_CriteriaParm.size() )
		pRow_CriteriaParm = m_vectRow_CriteriaParm[iNumCriteriaAdded++];
	else
		pRow_CriteriaParm = m_pRow_CriteriaParmNesting->Table_CriteriaParmNesting_get()->
			Database_pluto_main_get()->CriteriaParm_get()->AddRow();

	CriteriaParm *pCriteriaParm = new CriteriaParm(pRow_CriteriaParm,FK_CriteriaParmList,sParm,
		Operator,sValue,FK_CannedEvents_CriteriaParmList);
	m_dequeCriteriaParm.push_back(pCriteriaParm);
	return pCriteriaParm;
}

Row_CriteriaParmNesting *CriteriaParmNesting::m_vectRow_CriteriaParmNesting_get(size_t iPosition)
{
	if( iPosition<m_vectRow_CriteriaParmNesting.size() )
		return m_vectRow_CriteriaParmNesting[iPosition];
	return m_pRow_CriteriaParmNesting->Table_CriteriaParmNesting_get()->AddRow();
}

bool CriteriaParm::Commit(CriteriaParmNesting *pCriteriaParmNesting_Parent)
{
	m_pRow_CriteriaParm->FK_CriteriaParmNesting_set(pCriteriaParmNesting_Parent->m_pRow_CriteriaParmNesting->PK_CriteriaParmNesting_get());
	m_pRow_CriteriaParm->FK_CriteriaParmList_set(m_iFK_CriteriaParmList);
	m_pRow_CriteriaParm->FK_CannedEvents_CriteriaParmList_set(m_iFK_CannedEvents_CriteriaParmList);
	m_pRow_CriteriaParm->Parm_set(m_sParm);
	m_pRow_CriteriaParm->Value_set(m_sValue);
	m_pRow_CriteriaParm->Operator_set((int) m_Operator);
	m_pRow_CriteriaParm->Table_CriteriaParm_get()->Commit();
	m_pRow_CriteriaParm->Table_CriteriaParm_get()->Database_pluto_main_get()->threaded_db_wrapper_query(
		"UPDATE CriteriaParm set psc_mod=0 WHERE PK_CriteriaParm=" + 
		StringUtils::itos(m_pRow_CriteriaParm->PK_CriteriaParm_get()));

	return true;
}
