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
#ifndef __OBJECT_RENDERER_H__
#define __OBJECT_RENDERER_H__

#include "DesignObj_Orbiter.h"
#include "CacheImageManager.h"

#define FLOORPLAN_OBJ_NAME	"floorplan object"
#define HOLLOW_OBJ_NAME		"hollow"
#define DATAGRID_THUMB_NAME "datagrid-thumb"
#define DYNAMIC_OBJ_NAME	"dynamic object"

class ObjectRenderer
{
	friend class DesignObj_Orbiter;

protected:


	DesignObj_Orbiter *m_pObj_Owner;

	CacheImageManager *GetCacheImageManager();
	std::string GetLocalDirectory();

public:

	ObjectRenderer(DesignObj_Orbiter *pOwner);
	virtual ~ObjectRenderer(void);

	virtual void Init() {};
	virtual void RenderGraphic(PlutoRectangle rectTotal, bool bDisableAspectRatio, PlutoPoint point = PlutoPoint(0, 0));
	virtual void RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point = PlutoPoint(0, 0));

	 int GetAlphaLevel();	

protected:

	virtual bool PreRenderActions(DesignObj_Orbiter *pObj_Screen, PlutoPoint point = PlutoPoint(0, 0));
	virtual bool PostRenderActions(DesignObj_Orbiter *pObj_Screen, PlutoPoint point = PlutoPoint(0, 0));

	virtual void HideObject() {};
};

#endif //__OBJECT_RENDERER_H__
