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
#include "MouseIterator.h"
#include "MouseBehavior.h"
#include "MouseGovernor.h"
#include "KeyboardMouseHandler.h"
#include "VolumeMouseHandler.h"
#include "LightMouseHandler.h"
#include "MediaBrowserMouseHandler.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/Other.h"
#include "pluto_main/Define_Button.h"

using namespace DCE;

void *IteratorThread(void *p)
{
	MouseIterator *pMouseIterator = (MouseIterator *) p;
	pMouseIterator->Run();
	return NULL;
}

//-----------------------------------------------------------------------------------------------------
MouseIterator::MouseIterator(MouseBehavior *pMouseBehavior) : m_IteratorMutex("Iterator")
{
	m_pMouseBehavior=pMouseBehavior;
	m_pOrbiter=m_pMouseBehavior->m_pOrbiter;

	pthread_cond_init(&m_IteratorCond, NULL);
	m_IteratorMutex.Init(&m_pOrbiter->m_MutexAttr,&m_IteratorCond);

	m_bQuit=false;
	m_bThreadRunning=true;
	m_EIteratorFunction=if_None;
	m_dwTime_Last_Iterated=0;
	m_pMouseHandler=NULL;

	pthread_t pthread_id; 
	if(pthread_create( &pthread_id, NULL, IteratorThread, (void*)this) )
	{
		m_bThreadRunning=false;
		LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot create Iterator thread" );
	}
	else
		pthread_detach(pthread_id);
}

MouseIterator::~MouseIterator()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MouseIterator::~MouseIterator");
	m_bQuit=true;
	while( m_bThreadRunning )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"MouseIterator::~MouseIterator broadcasting");
		pthread_cond_broadcast( &m_IteratorCond );
		Sleep(5);
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MouseIterator::~MouseIterator done");
}

void MouseIterator::Run()
{
	PLUTO_SAFETY_LOCK(m,m_IteratorMutex);
	while( !m_bQuit )
	{
		if( m_EIteratorFunction==if_None )
			m.CondWait(); // Nothing to do.  Just wait
		
		unsigned long dwTime = ProcessUtils::GetMsTime();
		if( int(dwTime) - m_dwTime_Last_Iterated >= m_dwFrequency_Ms )
		{
			// To prevent a deadlock we must always block the screen mutex before the iterator mutex
			m.Release();
			PLUTO_SAFETY_LOCK(sm,m_pOrbiter->m_ScreenMutex);
			m.Relock();
			DoIteration();
			m_dwTime_Last_Iterated=dwTime;
		}
		else
		{
			int TimeToWaitInMs = m_dwFrequency_Ms - ( dwTime-m_dwTime_Last_Iterated );
			int SecondsToWait = TimeToWaitInMs / 1000;
			m.TimedCondWait(SecondsToWait,(TimeToWaitInMs % 1000) * 1000000);
		}
	}
	m_bThreadRunning=false;
}

void MouseIterator::SetIterator(EIteratorFunction eIteratorFunction,int dwParm,string sParm,int dwFrequency_Ms,MouseHandler *pMouseHandler)
{
	PLUTO_SAFETY_LOCK(sm,m_pOrbiter->m_ScreenMutex);
	PLUTO_SAFETY_LOCK(m,m_IteratorMutex);
	m_pMouseHandler=pMouseHandler;
	m_EIteratorFunction=eIteratorFunction;
	m_dwParm=dwParm;
	m_sParm=sParm;
	m_dwFrequency_Ms=dwFrequency_Ms;
	if( m_dwFrequency_Ms<0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MouseIterator::SetIterator frequency <0");
		m_dwFrequency_Ms = 1000;  // shouldn't happen
	}
	switch(eIteratorFunction)
	{
		case if_Volume:
			break;
		default:
			break;
	};
	if( m_EIteratorFunction!=if_None )
		pthread_cond_broadcast( &m_IteratorCond );
}

void MouseIterator::DoIteration()
{
	switch(m_EIteratorFunction)
	{
	case if_Volume:
		{
			// Call back the volume handler so it can also update the total
			VolumeMouseHandler *pVolumeMouseHandler = (VolumeMouseHandler *) m_pMouseHandler;
			pVolumeMouseHandler->DoIteration(m_dwParm);
		}
		break;
	case if_Light:
		{
			// Call back the Light handler so it can also update the total
			LightMouseHandler *pLightMouseHandler = (LightMouseHandler *) m_pMouseHandler;
			pLightMouseHandler->DoIteration(m_dwParm);
		}
		break;
	case if_Keyboard:
		{
			// Since there's quite a bit of logic here, call back the keyboard handler
			// and we'll put the logic there
			KeyboardMouseHandler *pKeyboardMouseHandler = (KeyboardMouseHandler *) m_pMouseHandler;
			pKeyboardMouseHandler->DoIteration();
		}			
		break;
	case if_MediaGrid:
		{
			// Since there's quite a bit of logic here, call back the keyboard handler
			// and we'll put the logic there
			MediaBrowserMouseHandler *pMediaBrowserMouseHandler = (MediaBrowserMouseHandler *) m_pMouseHandler;
			if( !pMediaBrowserMouseHandler->m_pDatagridMouseHandlerHelper->DoIteration() )
				m_EIteratorFunction=if_None;
		}			
		break;
	case if_MediaTracks:
		{
			bool bResult=true;

			if( m_dwParm==-2 )
			{
				NeedToRender render( m_pOrbiter, "iterator grid" );  // Redraw anything that was changed by this command
				bResult = m_pOrbiter->Scroll_Grid("",m_sParm,DIRECTION_Up_CONST);
			}
			else if( m_dwParm==-1 )
				m_pOrbiter->CMD_Move_Up(0);
			else if( m_dwParm==1 )
				m_pOrbiter->CMD_Move_Down(0);
			else if( m_dwParm==2 )
			{
				NeedToRender render( m_pOrbiter, "iterator grid" );  // Redraw anything that was changed by this command
				bResult = m_pOrbiter->Scroll_Grid("",m_sParm,DIRECTION_Down_CONST);
			}

			if( !bResult )
				m_EIteratorFunction=if_None;
			/*
			if( !m_pMouseBehavior->m_pMouseHandler_Vertical || !m_pMouseBehavior->m_pMouseHandler_Vertical->m_pObj || m_pMouseBehavior->m_pMouseHandler_Vertical->m_pObj->m_ChildObjects.size()==0 )
				return; // Shouldn't happen, this should be the datagris control

			DesignObj_Orbiter *pObj_Child = (DesignObj_Orbiter *) *(m_pMouseBehavior->m_pMouseHandler_Vertical->m_pObj->m_ChildObjects.begin());
			PLUTO_SAFETY_LOCK( cm, m_pMouseBehavior->m_pOrbiter->m_ScreenMutex );  // Always lock this before datagrid to prevent a deadlock
			PLUTO_SAFETY_LOCK( dng, m_pMouseBehavior->m_pOrbiter->m_DatagridMutex );
			if( pObj_Child->m_ObjectType!=DESIGNOBJTYPE_Datagrid_CONST )
				return; // Also shouldn't happen
			DesignObj_DataGrid *pObj_Grid = (DesignObj_DataGrid *) pObj_Child;
			if( !pObj_Grid->DataGridTable_Get() )
				return; // Again shouldn't happen
				*/
		}
		break;
	default:
		break;
	};
}
