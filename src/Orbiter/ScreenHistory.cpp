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
#include "ScreenHistory.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCE/Message.h"
#include "Orbiter.h"

using namespace DCE;
//-----------------------------------------------------------------------------------------------------
ScreenHistory::ScreenHistory(int nPK_Screen, class ScreenHistory *pScreenHistory_Prior,Message *pMessage,Orbiter *pOrbiter)
{
	m_bPutObjectsOffScreen=false;
	m_pOrbiter=pOrbiter;
	m_nPK_Screen = nPK_Screen;
	m_tTime = time(NULL);
	m_pObj = NULL;
	m_bCantGoBack = false; 

	if( pMessage )
		for(map<long, string>::iterator it = pMessage->m_mapParameters.begin(); it != pMessage->m_mapParameters.end(); it++)
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"ScreenHistory::ScreenHistory %d with %d=%s",m_nPK_Screen,it->first,it->second.c_str());
#endif
			m_mapParameters[it->first]=it->second;
		}

}
//-----------------------------------------------------------------------------------------------------
ScreenHistory::~ScreenHistory()
{
	m_listObjs.clear();
	m_mapVariable.clear();
	m_mapVisibilityContext.clear();
}
//-----------------------------------------------------------------------------------------------------
DesignObj_Orbiter *ScreenHistory::GetObj() 
{ 
	return m_pObj; 
}
//-----------------------------------------------------------------------------------------------------
void ScreenHistory::SetObj(DesignObj_Orbiter *pObj) 
{ 
	m_pObj = pObj;
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "ScreenHistory - replaced in screen %d obj with %d", 
		m_nPK_Screen, pObj->m_iBaseObjectID);
#endif
}
//-----------------------------------------------------------------------------------------------------
void ScreenHistory::AddToHistory()
{
	PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
	m_listObjs.push_front(m_pObj);

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Added %d obj to stack m_listObjs (size %d) - screen %d this: %p", 
		m_pObj->m_iBaseObjectID, m_listObjs.size(), m_nPK_Screen, this);
#endif
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHistory::HistoryEmpty()
{
	PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
	return m_listObjs.size() == 0;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHistory::PurgeHistory() 
{ 
	PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "ScreenHistory::PurgeHistory m_listObjs %d obj (size %d) - screen %d this: %p", 
		m_pObj->m_iBaseObjectID, m_listObjs.size(), m_nPK_Screen, this);
#endif
	m_listObjs.clear(); 
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHistory::CantGoBack()
{
	return m_bCantGoBack;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHistory::CantGoBack(bool bCantGoBack)
{
	m_bCantGoBack = bCantGoBack;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHistory::GoBack()
{
	PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
	if( m_bCantGoBack )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "m_listObjs obj %d cannot go back (size left %d) - screen %d, front left: %d this: %p", 
			m_pObj->m_iBaseObjectID, m_listObjs.size(), m_nPK_Screen, 
			m_listObjs.size() ? m_listObjs.front()->m_iBaseObjectID : 0, this);
#endif
		return false;
	}

	if(m_listObjs.size())
	{
		m_pObj = m_listObjs.front();
		m_listObjs.pop_front();

#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Removing  m_listObjs %d from stack (size left %d) - screen %d, front left: %d this: %p", 
			m_pObj->m_iBaseObjectID, m_listObjs.size(), m_nPK_Screen, 
			m_listObjs.size() ? m_listObjs.front()->m_iBaseObjectID : 0, this);
#endif
		return true;
	}

#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "m_listObjs Nothing to remove from queue screen %d", m_nPK_Screen);
#endif

	return false;
}
//-----------------------------------------------------------------------------------------------------
string ScreenHistory::ToString()
{
	PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
	string sOutput;
	sOutput += "ID:" + StringUtils::ltos(m_nPK_Screen) + (m_bCantGoBack ? " *no back*" : "") +  "(\"" + m_sID + "\" - ";

	list<DesignObj_Orbiter *>::iterator it;
	for(it = m_listObjs.begin(); it != m_listObjs.end(); it++)
	{
		if(it != m_listObjs.begin())
			sOutput += ",";

		DesignObj_Orbiter *pObj = *it;
		sOutput += StringUtils::ltos(pObj->m_iBaseObjectID);
	}

	sOutput += ")";

	//sOutput += " vars[";

	//for(map<int, string>::const_iterator itm = m_mapVariable.begin(); itm != m_mapVariable.end(); ++itm)
	//{
	//	if(itm->second != "")
	//		sOutput += StringUtils::ltos(itm->first) + "-" + itm->second + " ";
	//}

	//sOutput += "]";

	return sOutput;
}
//-----------------------------------------------------------------------------------------------------
time_t ScreenHistory::TimeCreated() const
{
	return m_tTime;
}
//-----------------------------------------------------------------------------------------------------
string ScreenHistory::ScreenID()
{
	return m_sID;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHistory::ScreenID(string sID)
{
	m_sID = sID;
}
//-----------------------------------------------------------------------------------------------------
int ScreenHistory::PK_Screen()
{
	return m_nPK_Screen;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHistory::PK_Screen(int nPK_Screen)
{
	m_nPK_Screen = nPK_Screen;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHistory::SaveContext(const map<int, string>& mapVariable,
								const map<DesignObj_Orbiter *, bool>& mapVisibilityContext)
{
	PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Saving context for screen %d / %p, object %s (%d variables, %d visibility status) ...", 
		m_nPK_Screen, this, m_pObj->m_ObjectID.c_str(), mapVariable.size(), mapVisibilityContext.size());

	m_mapVariable.clear();
	for(map<int, string>::const_iterator it = mapVariable.begin(); it != mapVariable.end(); ++it)
	{
#ifdef DEBUG
		if(it->second != "")
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Saving var (screen %d) %d = %s", m_nPK_Screen, it->first, it->second.c_str());
#endif

		m_mapVariable[it->first] = it->second;
	}

	m_mapVisibilityContext.clear();
	for(map<DesignObj_Orbiter *, bool>::const_iterator itv = mapVisibilityContext.begin(); itv != mapVisibilityContext.end(); ++itv)
	{
#ifdef DEBUG
		DesignObj_Orbiter *pObj = itv->first;
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Saving state (screen %d) for obj %s hidden %d", m_nPK_Screen, pObj->m_ObjectID.c_str(), itv->second);
#endif
		m_mapVisibilityContext[itv->first] = itv->second;
	}
}
//-----------------------------------------------------------------------------------------------------
void ScreenHistory::RestoreContext(map<int, string>& mapVariable,
								   map<DesignObj_Orbiter *, bool>& mapVisibilityContext)
{
	PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Restoring context for screen %d / %p, object %s (%d variables, %d visibility status) ...", 
		m_nPK_Screen, this, m_pObj->m_ObjectID.c_str(), mapVariable.size(), mapVisibilityContext.size());

	for(map<int, string>::iterator it = m_mapVariable.begin(); it != m_mapVariable.end(); ++it)
	{
#ifdef DEBUG	
		if(it->second != "")
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Restoring var (screen %d) %d = %s", m_nPK_Screen, it->first, it->second.c_str());
#endif
		mapVariable[it->first] = it->second;
	}

	mapVisibilityContext.clear();
	for(map<DesignObj_Orbiter *, bool>::iterator itv = m_mapVisibilityContext.begin(); itv != m_mapVisibilityContext.end(); ++itv)
	{
#ifdef DEBUG
		DesignObj_Orbiter *pObj = itv->first;
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Restoring state (screen %d) for %s hidden %d", m_nPK_Screen, pObj->m_ObjectID.c_str(), itv->second);
#endif
		mapVisibilityContext[itv->first] = itv->second;
	}
}
//-----------------------------------------------------------------------------------------------------
