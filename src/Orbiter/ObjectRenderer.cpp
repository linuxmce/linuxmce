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
#include "ObjectRenderer.h"
#include "pluto_main/Define_Variable.h"
#include "Orbiter.h"
#include "CallBackData.h"
#include "ScreenHandler.h"
#include "RendererMNG.h"
#include "OrbiterRenderer.h"
#include "pluto_main/Define_DesignObjParameter.h"
#include "Gen_Devices/AllCommandsRequests.h"

ObjectRenderer::ObjectRenderer(DesignObj_Orbiter *pOwner) : m_pObj_Owner(pOwner)
{
	// For now call this from the constructor, but should be 
	// moved outside the constructor at some point.
}

/*virtual*/ ObjectRenderer::~ObjectRenderer()
{
}

/*virtual*/ void ObjectRenderer::RenderGraphic(PlutoRectangle rectTotal, bool bDisableAspectRatio, PlutoPoint point)
{
	PLUTO_SAFETY_LOCK( cm, m_pObj_Owner->m_pOrbiter->m_ScreenMutex );
	vector<PlutoGraphic*> *pVectorPlutoGraphic = m_pObj_Owner->m_pvectCurrentGraphic;

	//we have nothing to render
	if(pVectorPlutoGraphic->size() == 0)
	return;

	//just in case
	if(int(pVectorPlutoGraphic->size()) <= m_pObj_Owner->m_iCurrentFrame)
	m_pObj_Owner->m_iCurrentFrame = 0;

	int iCurrentFrame = m_pObj_Owner->m_iCurrentFrame;
	PlutoGraphic *pPlutoGraphic = (*pVectorPlutoGraphic)[iCurrentFrame];
	bool bIsMNG = pPlutoGraphic->m_GraphicFormat == GR_MNG;

	//if a button doesn't have a mng as selected state (see bDisableEffects), then the png
	//used in normal state will be rendered for selected state + a blue rectangle to show the selection
	//
	// TODO: The blue button is rendered here!!!
	// 
	if(!bIsMNG && m_pObj_Owner->m_GraphicToDisplay == GRAPHIC_SELECTED)
	{
		if(m_pObj_Owner->m_vectSelectedGraphic.size() != 0)
			pVectorPlutoGraphic = &m_pObj_Owner->m_vectSelectedGraphic;
		else
			pVectorPlutoGraphic = &m_pObj_Owner->m_vectGraphic; //normal state

		//we have nothing to render
		if(pVectorPlutoGraphic->size() == 0)
			return;
		if(int(pVectorPlutoGraphic->size()) <= m_pObj_Owner->m_iCurrentFrame)
			m_pObj_Owner->m_iCurrentFrame = 0;

		iCurrentFrame = m_pObj_Owner->m_iCurrentFrame;
		pPlutoGraphic = (*pVectorPlutoGraphic)[iCurrentFrame];
		bIsMNG = pPlutoGraphic->m_GraphicFormat == GR_MNG;
	}

	string sFileName = "";
	if(
		pPlutoGraphic->IsEmpty() && NULL != m_pObj_Owner->m_pOrbiter->m_pCacheImageManager && pPlutoGraphic->m_Filename.length() &&
		m_pObj_Owner->m_pOrbiter->m_pCacheImageManager->IsImageInCache(pPlutoGraphic->m_Filename, m_pObj_Owner->m_Priority)
	)
	{
		//if we have the file in cache
		sFileName = m_pObj_Owner->m_pOrbiter->m_pCacheImageManager->GetCacheImageFileName(pPlutoGraphic->m_Filename);
	}
	else if(pPlutoGraphic->IsEmpty() && m_pObj_Owner->m_pOrbiter->m_sLocalDirectory.length() > 0 && pPlutoGraphic->m_Filename.length() )
	{
		//the file is in our localdrive
		sFileName = m_pObj_Owner->m_pOrbiter->m_sLocalDirectory + pPlutoGraphic->m_Filename;
	}

	//if we don't have the file in cache or on our localdrive
	if(pPlutoGraphic->IsEmpty() && sFileName.empty())
	{
		// Request our config info
		char *pGraphicFile=NULL;
		int iSizeGraphicFile=0;

		DCE::CMD_Request_File CMD_Request_File(
			m_pObj_Owner->m_pOrbiter->m_dwPK_Device,m_pObj_Owner->m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn,
			"orbiter/C" + StringUtils::itos(m_pObj_Owner->m_pOrbiter->m_dwPK_Device) + "/" + pPlutoGraphic->m_Filename,
			&pGraphicFile,&iSizeGraphicFile);
		m_pObj_Owner->m_pOrbiter->SendCommand(CMD_Request_File);

		if (!iSizeGraphicFile)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unable to get file from server %s", pPlutoGraphic->m_Filename.c_str());
			return;
		}

		//save the image in cache
		if(NULL != m_pObj_Owner->m_pOrbiter->m_pCacheImageManager) //cache manager is enabled ?
		{
			m_pObj_Owner->m_pOrbiter->m_pCacheImageManager->CacheImage(pGraphicFile, iSizeGraphicFile, pPlutoGraphic->m_Filename, m_pObj_Owner->m_Priority);
			sFileName = m_pObj_Owner->m_pOrbiter->m_pCacheImageManager->GetCacheImageFileName(pPlutoGraphic->m_Filename);
		}

		//TODO: same logic for in-memory data
		if( (sFileName.empty() || iSizeGraphicFile) && !pPlutoGraphic->LoadGraphic(pGraphicFile, iSizeGraphicFile))
		{
			delete pGraphicFile;
			pGraphicFile = NULL;
			return;
		}

		delete pGraphicFile;
		pGraphicFile = NULL;
	}

	if(pPlutoGraphic->IsEmpty() && !sFileName.empty() && sFileName.find(DYNAMIC_OBJ_NAME) == string::npos)
	{
		if(!FileUtils::FileExists(sFileName))
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unable to read file %s", (sFileName).c_str());
			return;
		}

		switch(pPlutoGraphic->m_GraphicFormat)
		{
		case GR_JPG:
		case GR_GIF:
		case GR_TIF:
		case GR_PNG:
		case GR_BMP:
		case GR_OCG:
			{
				size_t size = 0;
				char *pData = FileUtils::ReadFileIntoBuffer(sFileName.c_str(), size);

#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Read file to buffer %s, size=%d\n", sFileName.c_str(), size);
#endif

				if(!size)
					return;

				if(!pPlutoGraphic->LoadGraphic(pData, size))
					return;

				delete [] pData;
			}
			break;

		case GR_MNG:
			{
				//eGraphicFormat eGF = pPlutoGraphic->m_GraphicFormat;
				eGraphicManagement eGM = pPlutoGraphic->m_GraphicManagement;
				string sMNGFileName = pPlutoGraphic->m_Filename;

				vector<PlutoGraphic *>::iterator itPlutoGraphic;
				for(itPlutoGraphic = pVectorPlutoGraphic->begin(); itPlutoGraphic != pVectorPlutoGraphic->end(); ++itPlutoGraphic)
					RELEASE_GRAPHIC(*itPlutoGraphic);
				pVectorPlutoGraphic->clear();

				InMemoryMNG *pInMemoryMNG = InMemoryMNG::CreateInMemoryMNGFromFile(sFileName, rectTotal.Size());
				size_t framesCount = pInMemoryMNG->m_vectMNGframes.size();
				for(size_t i = 0; i < framesCount; i++)
				{
					size_t iFrameSize = 0;
					char *pFrameData = NULL;

					iFrameSize = pInMemoryMNG->GetFrame(int(i), pFrameData);

					if(iFrameSize)
					{
						PlutoGraphic *pGraphic = m_pObj_Owner->m_pOrbiter->Renderer()->CreateGraphic();
						pGraphic->m_GraphicManagement = eGM;
						pGraphic->m_Filename = sMNGFileName;
						pGraphic->m_GraphicFormat = GR_PNG; //this is an mng with multiple png frames
						pGraphic->LoadGraphic(pFrameData, iFrameSize);
						pGraphic->m_GraphicFormat = GR_MNG;
						(*pVectorPlutoGraphic).push_back(pGraphic);
					}

					delete [] pFrameData;
				}

				delete pInMemoryMNG;
				pInMemoryMNG = NULL;
			}
			break;

		default:;

		}
	}

	if(bIsMNG && m_pObj_Owner->m_pvectCurrentPlayingGraphic == NULL)
	{
		pPlutoGraphic = (*pVectorPlutoGraphic)[0];

		int iTime = 0; //hardcoding warning! don't know from where to get the framerate yet (ask Radu, libMNG)
	#ifndef WINCE
		iTime = 15;
	#endif

		bool bLoop = false; //hardcoding warning!  (ask Radu, libMNG)

		switch(m_pObj_Owner->m_GraphicToDisplay)
		{
		case GRAPHIC_HIGHLIGHTED:
			m_pObj_Owner->m_iTime_Highlighted = iTime;
			m_pObj_Owner->m_bLoop_Highlighted = bLoop;
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Display GRAPHIC_HIGHLIGHTED ...\n");
#endif

			break;
		case GRAPHIC_SELECTED:
			m_pObj_Owner->m_iTime_Selected = iTime;
			m_pObj_Owner->m_bLoop_Selected = bLoop;
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Display GRAPHIC_SELECTED ...\n");
#endif
		
			break;
		case GRAPHIC_NORMAL:
			m_pObj_Owner->m_iTime_Background = iTime;
			m_pObj_Owner->m_bLoop_Background = bLoop;
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Display GRAPHIC_NORMAL ...\n");
#endif
		
			break;
			//todo alternate graphics ?
		}

		m_pObj_Owner->m_iCurrentFrame = 1;

		//schedule next frame for animation
		m_pObj_Owner->m_pvectCurrentPlayingGraphic = m_pObj_Owner->m_pvectCurrentGraphic;
		m_pObj_Owner->m_GraphicToPlay = m_pObj_Owner->m_GraphicToDisplay;
		m_pObj_Owner->m_pOrbiter->CallMaintenanceInMiliseconds( iTime, (OrbiterCallBack) &Orbiter::PlayMNG_CallBack, this , pe_NO );
	}

	if(!pPlutoGraphic->IsEmpty())
	m_pObj_Owner->m_pOrbiter->Renderer()->RenderGraphic(pPlutoGraphic, rectTotal, bDisableAspectRatio, point);
	#ifdef DEBUG
	else
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "No graphic to render for object %s", m_pObj_Owner->m_ObjectID.c_str());
	#endif

	if(!bIsMNG && m_pObj_Owner->m_GraphicToDisplay == GRAPHIC_SELECTED)
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "m_pOrbiter->SelectObject ...\n");
#endif
	
		m_pObj_Owner->m_pOrbiter->SelectObject(m_pObj_Owner, point);
	}	
}

/*virtual*/ void ObjectRenderer::RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point)
{
	if(!PreRenderActions(pObj_Screen, point))
		return;

	switch(m_pObj_Owner->m_ObjectType)
	{
	case DESIGNOBJTYPE_Floorplan_CONST:
		m_pObj_Owner->m_pOrbiter->RenderFloorplan(m_pObj_Owner,pObj_Screen, point);
		break; // Render the child objects with their text

	case DESIGNOBJTYPE_Web_Browser_CONST:
		//      LocalHandleWebWindow( pObj,  rectTotal );
		break;

	case DESIGNOBJTYPE_App_Desktop_CONST:
		if(m_pObj_Owner->m_rPosition.Width == 0 && m_pObj_Owner->m_rPosition.Height == 0 && m_pObj_Owner->m_rPosition.X == 0 && m_pObj_Owner->m_rPosition.Y == 0)
			m_pObj_Owner->m_pOrbiter->RenderDesktop( m_pObj_Owner, PlutoRectangle( 0, 0, -1, -1 ), point );  // Full screen
		else
			m_pObj_Owner->m_pOrbiter->RenderDesktop( m_pObj_Owner, m_pObj_Owner->m_rPosition, point );
		break;

#ifdef PRONTO
	case DESIGNOBJTYPE_Pronto_File_CONST:
		if ( m_pObj_Owner->m_pCCF == NULL )
		{
			if ( !m_pObj_Owner->m_bInitialPaint )
			{
				string ccf = m_pObj_Owner->GetParameterValue( C_PARAMETER_PRONTO_FILE_CONST );
				if( ccf.length(  ) == 0 && m_pObj_Owner->m_bDeleteThis || ccf.find( '~' )==string::npos ) ccf = "3365~I49/ccf/1.ccf";
				string::size_type pos = 0;
				int IRDevice = atoi( StringUtils::Tokenize( ccf,  "~",  pos ).c_str(  ) );
				string fn = ccf.substr( pos );

				Message *pMessage=m_pObj_Owner->m_pcRequestSocket->SendReceiveMessage( new Message( m_dwPK_Device,  DEVICEID_DCEROUTER,  PRIORITY_NORMAL,  MESSAGETYPE_FILEREQUEST,  0,  1,  1,  fn.c_str(  ) ) );
				string sResult="";
				if ( pMessage )
				{
					if ( pMessage->m_dwID==FILERESULT_SUCCESS )
					{

						m_pCCF = new ProntoCCF( this,  pMessage->m_mapData_Parameters[1],  pMessage->m_mapData_Lengths[1],  pObj,  IRDevice );
						// Clear the data parameters so the Message's destructor doesn't delete the data.
						pMessage->m_mapData_Parameters.clear(  );
					}
					delete pMessage;
				}
			}
			else
			{
				m_pObj_Owner->m_AutoInvalidateTime = clock();
			}
		}
		if ( m_pObj_Owner->m_pCCF )
		{
			m_pObj_Owner->m_pCCF->RenderPronto( XOffset,  YOffset, point );
		}
		break;
#endif

	case DESIGNOBJTYPE_Broadcast_Video_CONST:
		if ( m_pObj_Owner->m_bOnScreen && !m_pObj_Owner->m_pOrbiter->m_bAlreadyQueuedVideo )
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Scheduling object @%p: %s", this, m_pObj_Owner->m_ObjectID.c_str());
#endif

			// If we've got no current graphic, such as during the first redraw since ObjectOnScreen
			// deletes it, then request a video frame right away, otherwise we're redrawing
			// and should wait the correct interval

			m_pObj_Owner->m_pOrbiter->CallMaintenanceInMiliseconds( m_pObj_Owner->m_vectGraphic.size()==0 ? 0 : m_pObj_Owner->m_pOrbiter->m_iVideoFrameInterval, &Orbiter::GetVideoFrame, NULL, pe_ALL );
			m_pObj_Owner->m_pOrbiter->m_bAlreadyQueuedVideo=true;  // Only schedule once -- that will redraw all video frames
		}
		else
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Not Scheduling object @%p: %s because is not on screen.", this, m_pObj_Owner->m_ObjectID.c_str());
#endif
		}

		break;
	}

	PostRenderActions(pObj_Screen, point);
}

/*virtual*/ bool ObjectRenderer::PreRenderActions(DesignObj_Orbiter *pObj_Screen, PlutoPoint point)
{
	if(m_pObj_Owner->m_pDesignObj_Orbiter_TiedTo)
	{
		m_pObj_Owner->m_bHidden = m_pObj_Owner->m_pDesignObj_Orbiter_TiedTo->IsHidden();
		if(
			(m_pObj_Owner->m_iRowTiedTo == -1 && m_pObj_Owner->m_iColumnTiedTo == -1) || 
			m_pObj_Owner->m_pDesignObj_Orbiter_TiedTo->m_ObjectType != DESIGNOBJTYPE_Datagrid_CONST
			)
		{
			if(m_pObj_Owner->m_pDesignObj_Orbiter_TiedTo == m_pObj_Owner->m_pOrbiter->m_pObj_Highlighted)
				m_pObj_Owner->m_GraphicToDisplay_set("or1",GRAPHIC_HIGHLIGHTED);
			else
				m_pObj_Owner->m_GraphicToDisplay_set("or2",m_pObj_Owner->m_pDesignObj_Orbiter_TiedTo->m_GraphicToDisplay);
		}
		else
		{
			DesignObj_DataGrid *pObjGrid = (DesignObj_DataGrid *) m_pObj_Owner->m_pDesignObj_Orbiter_TiedTo;

			// We must be pointing to a datagrid,  and we need to bind to the highlighted row
			if(
				(m_pObj_Owner->m_iRowTiedTo == -1 || m_pObj_Owner->m_iRowTiedTo == pObjGrid->m_iHighlightedRow) &&
				(m_pObj_Owner->m_iColumnTiedTo == -1 || m_pObj_Owner->m_iColumnTiedTo == pObjGrid->m_iHighlightedColumn)
				)
				m_pObj_Owner->m_GraphicToDisplay_set("or3",GRAPHIC_HIGHLIGHTED);
			else
				m_pObj_Owner->m_GraphicToDisplay_set("or4",GRAPHIC_NORMAL);
		}
	}

	// See if this object is only visible for a given state
	if(m_pObj_Owner->m_sVisibleState.size())
	{
		if(  
			(
			(m_pObj_Owner == m_pObj_Owner->m_pOrbiter->m_pObj_Highlighted || m_pObj_Owner->m_GraphicToDisplay==GRAPHIC_HIGHLIGHTED) && 
			m_pObj_Owner->m_sVisibleState.find("H")==string::npos) ||
			(m_pObj_Owner->m_GraphicToDisplay==GRAPHIC_SELECTED && m_pObj_Owner->m_sVisibleState.find("S")==string::npos) ||
			(m_pObj_Owner->m_GraphicToDisplay==GRAPHIC_NORMAL && m_pObj_Owner->m_sVisibleState.find("N")==string::npos) ||
			(m_pObj_Owner->m_GraphicToDisplay>0 && m_pObj_Owner->m_sVisibleState.find(StringUtils::itos(m_pObj_Owner->m_GraphicToDisplay)) == string::npos
			)
			)
			return false;
	}

	if ( m_pObj_Owner->m_bHidden || m_pObj_Owner->m_rPosition.Width==0 || m_pObj_Owner->m_rPosition.Height==0 )
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write( LV_STATUS, "object: %s  not visible: %d", m_pObj_Owner->m_ObjectID.c_str(), (int) m_pObj_Owner->m_bHidden );
#endif

		HideObject();

		if(m_pObj_Owner->m_pOrbiter->m_bShowShortcuts && m_pObj_Owner->m_iPK_Button)
			m_pObj_Owner->m_pOrbiter->Renderer()->RenderShortcut(m_pObj_Owner);

		return false;
	}

	PROFILE_START( ctObj )
		PlutoRectangle rectBackground = m_pObj_Owner->m_rBackgroundPosition;
	PlutoRectangle rectTotal = m_pObj_Owner->m_rPosition;
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"xzx Rendering %s with %d %p=%p size %d",m_pObj_Owner->m_ObjectID.c_str(),m_pObj_Owner->m_GraphicToDisplay,
					  m_pObj_Owner->m_pvectCurrentGraphic,&m_pObj_Owner->m_vectGraphic,(int) m_pObj_Owner->m_vectAltGraphics.size());
#endif
//if( m_pObj_Owner->m_iBaseObjectID==2355 )
//int k=2;
	if(
		(m_pObj_Owner == m_pObj_Owner->m_pOrbiter->m_pObj_Highlighted || 
		(m_pObj_Owner->ChildOf(m_pObj_Owner->m_pOrbiter->m_pObj_Highlighted) && (m_pObj_Owner->m_pOrbiter->m_pObj_Highlighted->m_ObjectType!=DESIGNOBJTYPE_Datagrid_CONST || ((DesignObj_DataGrid *) m_pObj_Owner->m_pOrbiter->m_pObj_Highlighted)->GridObjectIsHighlighted(m_pObj_Owner) ))
		|| m_pObj_Owner->m_GraphicToDisplay == GRAPHIC_HIGHLIGHTED ||
		(m_pObj_Owner->m_pOrbiter->m_pObj_Highlighted && m_pObj_Owner->m_bTabStop && m_pObj_Owner == m_pObj_Owner->m_pOrbiter->m_pObj_Highlighted->m_pParentObject)
		) && // If I'm also a tab stop, and my child is a highlighted object, leave me highlighted too
		m_pObj_Owner->m_vectHighlightedGraphic.size() 
		)
	{
		m_pObj_Owner->m_pvectCurrentGraphic = &(m_pObj_Owner->m_vectHighlightedGraphic);

		//make sure they are in sync
		m_pObj_Owner->m_GraphicToDisplay_set("or6",GRAPHIC_HIGHLIGHTED);

		//we'll need to do the highlighting here, since we need the normal surface for un-highlighting
		//we won't do it on RealRedraw for this kind of objects
		if(NULL != m_pObj_Owner->m_pOrbiter->m_pObj_Highlighted && m_pObj_Owner->m_pOrbiter->m_pObj_Highlighted != m_pObj_Owner->m_pOrbiter->m_pObj_Highlighted_Last)
		{
			m_pObj_Owner->m_pOrbiter->m_pObj_Highlighted_Last = m_pObj_Owner->m_pOrbiter->m_pObj_Highlighted;
			m_pObj_Owner->m_pOrbiter->Renderer()->DoHighlightObject();
		}
	}
	else if(m_pObj_Owner->m_GraphicToDisplay == GRAPHIC_SELECTED && m_pObj_Owner->m_vectSelectedGraphic.size())
		m_pObj_Owner->m_pvectCurrentGraphic = &(m_pObj_Owner->m_vectSelectedGraphic);
	else if(m_pObj_Owner->m_GraphicToDisplay >= 1 && m_pObj_Owner->m_GraphicToDisplay <= int(m_pObj_Owner->m_vectAltGraphics.size()))
		m_pObj_Owner->m_pvectCurrentGraphic = &(m_pObj_Owner->m_vectAltGraphics[m_pObj_Owner->m_GraphicToDisplay - 1]);
	else // Something that was invalid was chosen,  or just the normal version
		m_pObj_Owner->m_pvectCurrentGraphic = &(m_pObj_Owner->m_vectGraphic);

	if(!m_pObj_Owner->m_pvectCurrentGraphic && m_pObj_Owner->m_GraphicToDisplay != GRAPHIC_NORMAL  )
		m_pObj_Owner->m_pvectCurrentGraphic = &(m_pObj_Owner->m_vectGraphic);

	if( m_pObj_Owner->m_bCustomRender )
	{
		SETUP_SCREEN_HANDLER_CALLBACK(m_pObj_Owner->m_pOrbiter->m_pScreenHandler, cbOnCustomRender, RenderScreenCallBackData, 
			(m_pObj_Owner->m_pOrbiter->CurrentScreen(), m_pObj_Owner))
		m_pObj_Owner->m_pOrbiter->ExecuteScreenHandlerCallback(cbOnCustomRender);

		m_pObj_Owner->m_pOrbiter->Renderer()->ObjectRendered(m_pObj_Owner, point);
		return false;
	}

	// This is somewhat of a hack, but we don't have a clean method for setting the graphics on the user & location buttons to
	if( m_pObj_Owner->m_bIsBoundToUser )
	{
		vector<PlutoGraphic*> *pvectGraphic = m_pObj_Owner->m_pOrbiter->m_mapUserIcons[m_pObj_Owner->m_pOrbiter->m_dwPK_Users];

		// Put this here to debug why sometimes the user and room aren't selected when the screen first appears
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Orbiter::RenderObject obj %s bound to user %d, got ptr %p",
			m_pObj_Owner->m_ObjectID.c_str(),m_pObj_Owner->m_pOrbiter->m_dwPK_Users,pvectGraphic);

		if( pvectGraphic )
			m_pObj_Owner->m_pvectCurrentGraphic = pvectGraphic;
		if( m_pObj_Owner->m_pvectCurrentGraphic )
			RenderGraphic(rectTotal, m_pObj_Owner->m_bDisableAspectLock, point);
	}
	else if( m_pObj_Owner->m_bIsBoundToLocation )
	{
		if( m_pObj_Owner->m_vectDesignObjText.size()==1 )
		{
			DesignObjText *pText = *(m_pObj_Owner->m_vectDesignObjText.begin());
			pText->m_sText = m_pObj_Owner->m_pOrbiter->m_pLocationInfo ? m_pObj_Owner->m_pOrbiter->m_pLocationInfo->Description : "";
		}
		else
		{
			if( m_pObj_Owner->m_pOrbiter->m_pLocationInfo && m_pObj_Owner->m_pOrbiter->m_pLocationInfo->m_pvectGraphic )
				m_pObj_Owner->m_pvectCurrentGraphic = m_pObj_Owner->m_pOrbiter->m_pLocationInfo->m_pvectGraphic;
			if( m_pObj_Owner->m_pvectCurrentGraphic )
				RenderGraphic(rectTotal, m_pObj_Owner->m_bDisableAspectLock, point);
		}
	}
	// Maybe we're showing a non standard state
	else if(  m_pObj_Owner->m_pGraphicToUndoSelect && m_pObj_Owner->m_GraphicToDisplay==GRAPHIC_NORMAL  )
	{
		vector<PlutoGraphic*> *pvectGraphic_Hold = m_pObj_Owner->m_pvectCurrentGraphic;
		vector<PlutoGraphic*> vectGraphicToUndoSelect;
		vectGraphicToUndoSelect.push_back(m_pObj_Owner->m_pGraphicToUndoSelect);
		m_pObj_Owner->m_pvectCurrentGraphic = &vectGraphicToUndoSelect;
		RenderGraphic(rectTotal, m_pObj_Owner->m_bDisableAspectLock, point);
		m_pObj_Owner->m_pvectCurrentGraphic = pvectGraphic_Hold;

		vectGraphicToUndoSelect.clear();
		RELEASE_GRAPHIC(m_pObj_Owner->m_pGraphicToUndoSelect);
	}
	else if(  m_pObj_Owner->m_pvectCurrentGraphic  )
		RenderGraphic(rectTotal, m_pObj_Owner->m_bDisableAspectLock, point );

	return true; //can continue to render
}

/*virtual*/ bool ObjectRenderer::PostRenderActions(DesignObj_Orbiter *pObj_Screen, PlutoPoint point)
{
	m_pObj_Owner->m_pOrbiter->Renderer()->ObjectRendered(m_pObj_Owner, point);

	PROFILE_STOP( ctObj,  m_ObjectID.c_str(  ) )
		DesignObj_DataList::reverse_iterator iHao;


	//use this to prevent same object to be rendered twice
	DesignObj_DataList ChildObjectToRender;
	ChildObjectToRender.assign(m_pObj_Owner->m_ChildObjects.begin(), m_pObj_Owner->m_ChildObjects.end());

	for( iHao=m_pObj_Owner->m_ChildObjects.rbegin(  ); iHao != m_pObj_Owner->m_ChildObjects.rend(  ); ++iHao )
	{
		DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
		if(pDesignObj_Orbiter->m_bContainsDataGrid)
			continue;

		pDesignObj_Orbiter->RenderObject(pObj_Screen, point);
		ChildObjectToRender.remove(pDesignObj_Orbiter);
	}

	for( iHao=m_pObj_Owner->m_ChildObjects.rbegin(  ); iHao != m_pObj_Owner->m_ChildObjects.rend(  ); ++iHao )
	{
		// Hold back the tied to's and datagrids
		DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
		if(  pDesignObj_Orbiter->m_pDesignObj_Orbiter_TiedTo || pDesignObj_Orbiter->m_bContainsDataGrid  )
			continue;

		if(std::find(ChildObjectToRender.begin(), ChildObjectToRender.end(), pDesignObj_Orbiter) != ChildObjectToRender.end())
		{
			pDesignObj_Orbiter->RenderObject(pObj_Screen, point);
			ChildObjectToRender.remove(pDesignObj_Orbiter);
		}
	}
	for( iHao=m_pObj_Owner->m_ChildObjects.rbegin(  ); iHao != m_pObj_Owner->m_ChildObjects.rend(  ); ++iHao )
	{
		// Let the to's go through,  as long as there's no datagrid
		DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
		if(  !pDesignObj_Orbiter->m_pDesignObj_Orbiter_TiedTo || pDesignObj_Orbiter->m_bContainsDataGrid  )
			continue;

		if(std::find(ChildObjectToRender.begin(), ChildObjectToRender.end(), pDesignObj_Orbiter) != ChildObjectToRender.end())
		{
			pDesignObj_Orbiter->RenderObject(pObj_Screen, point);
			ChildObjectToRender.remove(pDesignObj_Orbiter);
		}
	}
//if( m_pObj_Owner->m_ObjectID.find("1784")!=string::npos || m_pObj_Owner->m_ObjectID.find("5137")!=string::npos || m_pObj_Owner->m_ObjectID.find("5138")!=string::npos || m_pObj_Owner->m_ObjectID.find("5193")!=string::npos || m_pObj_Owner->m_ObjectID.find("5189")!=string::npos)
//int k=2;
	for( iHao=m_pObj_Owner->m_ChildObjects.rbegin(  ); iHao != m_pObj_Owner->m_ChildObjects.rend(  ); ++iHao )
	{
		DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
		if(  !pDesignObj_Orbiter->m_bContainsDataGrid  )
			continue;
		
		if(std::find(ChildObjectToRender.begin(), ChildObjectToRender.end(), pDesignObj_Orbiter) != ChildObjectToRender.end())
		{
			pDesignObj_Orbiter->RenderObject(pObj_Screen, point);
			ChildObjectToRender.remove(pDesignObj_Orbiter);
		}
	}

	VectorDesignObjText::iterator iText;
	for( iText=m_pObj_Owner->m_vectDesignObjText.begin(  ); iText != m_pObj_Owner->m_vectDesignObjText.end(  ); ++iText )
	{
		DesignObjText *pText = *iText;

		if(  pText->m_bPreRender || pText->m_rPosition.Width<1 || pText->m_rPosition.Height<1 )
			continue;
		PROFILE_START( ctText );
		TextStyle *pTextStyle = pText->m_mapTextStyle_Find( 0 );
		string TextToDisplay = m_pObj_Owner->m_pOrbiter->SubstituteVariables(m_pObj_Owner->m_pOrbiter->SubstituteVariables(pText->m_sText, pText->m_pObject, 0, 0), pText->m_pObject, 0, 0);
		m_pObj_Owner->m_pOrbiter->Renderer()->RenderText(TextToDisplay, pText, pTextStyle, point);
		PROFILE_STOP( ctText,  "Text ( obj below )" );
	}
	if(m_pObj_Owner->m_pFloorplanObject)
	{
		if(m_pObj_Owner->m_pOrbiter->m_mapDevice_Selected.find(m_pObj_Owner->m_pFloorplanObject->PK_Device) != m_pObj_Owner->m_pOrbiter->m_mapDevice_Selected.end() )
		{
			int i = 0;
			for(i = 0; i < 4; ++i)
				m_pObj_Owner->m_pOrbiter->Renderer()->HollowRectangle(
					point.X + m_pObj_Owner->m_rBackgroundPosition.X-i, 
					point.Y + m_pObj_Owner->m_rBackgroundPosition.Y-i, 
					m_pObj_Owner->m_rBackgroundPosition.Width+i+i, 
					m_pObj_Owner->m_rBackgroundPosition.Height+i+i,
					(i==1 || i==2 ? PlutoColor::Black() : PlutoColor::White()), 
					"", HOLLOW_OBJ_NAME " - " + m_pObj_Owner->m_ObjectID
				);

			//force an update because the object boundaries are not respected
			PlutoRectangle rect(point.X + m_pObj_Owner->m_rBackgroundPosition.X-i, point.Y + m_pObj_Owner->m_rBackgroundPosition.Y-i, m_pObj_Owner->m_rBackgroundPosition.Width+i+i, m_pObj_Owner->m_rBackgroundPosition.Height+i+i);
			m_pObj_Owner->m_pOrbiter->Renderer()->UpdateRect(rect, m_pObj_Owner->m_pPopupPoint);
		}
		else
		{
			m_pObj_Owner->m_pOrbiter->Renderer()->UnSelectObject("", HOLLOW_OBJ_NAME " - " + m_pObj_Owner->m_ObjectID);
		}

	}

	if(m_pObj_Owner->m_pOrbiter->m_bShowShortcuts && m_pObj_Owner->m_iPK_Button)
		m_pObj_Owner->m_pOrbiter->Renderer()->RenderShortcut(m_pObj_Owner);

	return true;
}

//-----------------------------------------------------------------------------------------------------
string ObjectRenderer::GetLocalDirectory()
{
	return m_pObj_Owner->m_pOrbiter->m_sLocalDirectory;
}
//-----------------------------------------------------------------------------------------------------
CacheImageManager *ObjectRenderer::GetCacheImageManager()
{
	return m_pObj_Owner->m_pOrbiter->m_pCacheImageManager;
}
//-----------------------------------------------------------------------------------------------------
int ObjectRenderer::GetAlphaLevel()
{

	int nAlphaChannel;
	string sAlphaChannel = m_pObj_Owner->GetParameterValue(DESIGNOBJPARAMETER_Alpha_channel_CONST);

	if(sAlphaChannel == "")
		nAlphaChannel = 255;
	else
	{
		nAlphaChannel = atoi(sAlphaChannel.c_str());
		nAlphaChannel = nAlphaChannel * 255 / 100; //translate from procents to 0-255
	}

	return nAlphaChannel;
}
//-----------------------------------------------------------------------------------------------------
