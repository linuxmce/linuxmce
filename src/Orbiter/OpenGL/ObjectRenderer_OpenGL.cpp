/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
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
#include "ObjectRenderer_OpenGL.h"

#include "../Orbiter.h"
#include "../OrbiterRenderer.h"
#include "OrbiterRenderer_OpenGL.h"
#include "Texture/TextureManager.h"
#include "../../pluto_main/Define_DesignObjParameter.h"
#include "../../Gen_Devices/AllCommandsRequests.h"

using namespace DCE;

ObjectRenderer_OpenGL::ObjectRenderer_OpenGL(DesignObj_Orbiter *pOwner) : ObjectRenderer(pOwner)
{
	
}

/*virtual*/ ObjectRenderer_OpenGL::~ObjectRenderer_OpenGL(void)
{

}

/*virtual*/ void ObjectRenderer_OpenGL::RenderGraphic(PlutoRectangle rectTotal, bool bDisableAspectRatio, PlutoPoint point)
{
	string sParentObjectID = "";
	DesignObj_Orbiter *pParentObj = dynamic_cast<DesignObj_Orbiter *>(m_pObj_Owner->m_pParentObject);
	if(NULL != pParentObj)
	{
		sParentObjectID = pParentObj->GenerateObjectHash(point, false);
	}

	if(TextureManager::Instance()->CacheEnabled())
	{
		MeshFrame* Frame = TextureManager::Instance()->GetCacheItem(m_pObj_Owner->GenerateObjectHash(point));
		if(NULL != Frame)
		{
			TextureManager::Instance()->AttachToScene(sParentObjectID, Frame);
			return;
		}
	}

	PlutoGraphic *pPlutoGraphic = NULL;
	pPlutoGraphic = 
		m_pObj_Owner->m_pvectCurrentGraphic != NULL && m_pObj_Owner->m_pvectCurrentGraphic->size() > 0 ?
		(m_pObj_Owner->m_pvectCurrentGraphic->operator [](0)) : 
		m_pObj_Owner->m_vectGraphic.size() > 0 ? 
			m_pObj_Owner->m_vectGraphic[0] :
			NULL;

	//we have nothing to render
	if(NULL == pPlutoGraphic)
		return;

	LoadPicture(pPlutoGraphic);

	if(!pPlutoGraphic->IsEmpty())
	{
		OrbiterRenderer_OpenGL *pOrbiterRenderer_OpenGL = 
			dynamic_cast<OrbiterRenderer_OpenGL *>(m_pObj_Owner->m_pOrbiter->Renderer());

		int nAlphaChannel = GetAlphaLevel();

		pOrbiterRenderer_OpenGL->RenderGraphic(pPlutoGraphic, rectTotal, 
			bDisableAspectRatio, point, nAlphaChannel, sParentObjectID, 
			m_pObj_Owner->GenerateObjectHash(point, false),
			m_pObj_Owner->GenerateObjectHash(point));
	}
#ifdef DEBUG
	else
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "No graphic to render for object %s", m_pObj_Owner->m_ObjectID.c_str());
#endif
}


void ObjectRenderer_OpenGL::LoadPicture(PlutoGraphic* pPlutoGraphic, bool bIgnoreTexture/* = false*/)
{
	bool bNeedsLoading = pPlutoGraphic->IsEmpty() || bIgnoreTexture;

	string sFileName = "";
	if(
		bNeedsLoading && NULL != GetCacheImageManager() && pPlutoGraphic->m_Filename.length() &&
		GetCacheImageManager()->IsImageInCache(pPlutoGraphic->m_Filename, m_pObj_Owner->m_Priority)
		)
	{
		//if we have the file in cache
		sFileName = GetCacheImageManager()->GetCacheImageFileName(pPlutoGraphic->m_Filename);
	}
	else if(bNeedsLoading && GetLocalDirectory().length() > 0 && pPlutoGraphic->m_Filename.length() )
	{
		//the file is in our localdrive
		sFileName = GetLocalDirectory() + pPlutoGraphic->m_Filename;
	}

	//if we don't have the file in cache or on our localdrive
	if(bNeedsLoading && sFileName.empty() && !pPlutoGraphic->m_Filename.empty())
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
			if(pPlutoGraphic->m_Filename.find(DYNAMIC_OBJ_NAME) == string::npos)
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Unable to get file from server %s", pPlutoGraphic->m_Filename.c_str());

			return;
		}

		//save the image in cache
		if(NULL != GetCacheImageManager()) //cache manager is enabled ?
		{
			GetCacheImageManager()->CacheImage(pGraphicFile, iSizeGraphicFile, pPlutoGraphic->m_Filename, m_pObj_Owner->m_Priority);
			
			sFileName = GetCacheImageManager()->GetCacheImageFileName(pPlutoGraphic->m_Filename);
		}

		if( (sFileName.empty() || iSizeGraphicFile) && !pPlutoGraphic->LoadGraphic(pGraphicFile, iSizeGraphicFile))
		{
			delete pGraphicFile;
			pGraphicFile = NULL;
			return;
		}

		delete pGraphicFile;
		pGraphicFile = NULL;
	}

	if(bNeedsLoading && !sFileName.empty() && sFileName.find(DYNAMIC_OBJ_NAME) == string::npos)
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

				if(!size)
				{
					return;
				}

				if(!pPlutoGraphic->LoadGraphic(pData, size))
				{
					return;
				}

				delete [] pData;
			}
			break;

		default:
			;
		}
	}
}

void ObjectRenderer_OpenGL::HideObject()
{
	OrbiterRenderer_OpenGL *pOrbiterRenderer = dynamic_cast<OrbiterRenderer_OpenGL *>(m_pObj_Owner->m_pOrbiter->Renderer());
	
	if(NULL != pOrbiterRenderer)
		pOrbiterRenderer->RemoveGraphic(m_pObj_Owner->GenerateObjectHash(m_pObj_Owner->m_pPopupPoint, false));
}

bool ObjectRenderer_OpenGL::PreRenderActions(DesignObj_Orbiter *pObj_Screen, PlutoPoint point /*= PlutoPoint(0, 0)*/)
{
	if(m_pObj_Owner->m_ObjectType == DESIGNOBJTYPE_Floorplan_CONST)
		return true;

	return ObjectRenderer::PreRenderActions(pObj_Screen, point);
}
