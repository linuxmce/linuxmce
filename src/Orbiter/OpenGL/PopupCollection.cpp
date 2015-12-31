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
Orbiter

Copyright (C) 2006 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

@author: "Ciprian Mustiata" ciprian.m at plutohome dot com, "Cristian Miron" chris.m at plutohome dot com 

*/
#include "PopupCollection.h"

#include "Orbiter.h"

#include "Mesh/MeshBuilder.h"
#include "Mesh/MeshFrame.h"
#include "Texture/TextureManager.h"
#include "OpenGL3DEngine.h"

PopupCollection::PopupCollection(OpenGL3DEngine* Engine)
: Scene(NULL)
{
	Current = "";
	this->Engine = Engine;
}

PopupCollection::~PopupCollection(void)
{
	Reset();
}

void PopupCollection::Reset()
{
	Current = "";
	map<string, PopupDescription* >::iterator Item = Popups.begin(),
		End = Popups.end();
	for(; Item != End; ++Item)
	{
		delete (Item->second);
	}
	Popups.clear();
}


void PopupCollection::HidePopup(string ID, string ObjectHash, bool bMarkAsVolation)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PopupCollection::HidePopup %s", ObjectHash.c_str());
#endif
	PopupDescription* Item;
	Item = Popups[ObjectHash];
	if(Item)
	{
		if(bMarkAsVolation && Item->Frame())
		{
			Item->Frame()->MarkAsVolatileRecursively();

			map<string, PopupDescription* >::iterator it = Popups.find(ObjectHash);
			Popups.erase(it);
			delete Item;
		}
		else
		{
			Item->Hide();
		}

		if(ObjectHash == Current)
			Current = "";
	}
}

void PopupCollection::PaintPopup(string ID, string ObjectHash, PlutoPopup *Popup, int EffectID)
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "PopupCollection::PaintPopup %s", ObjectHash.c_str());
#endif
	PopupDescription* Item;
	if(Current == ObjectHash)
		return;

	Current = ObjectHash;
	if (Exists(ObjectHash))
	{
		Item = Popups[ObjectHash];
		if(Item)
			Item->Show();
	}
	else
	{
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Added in map popup %s, now size is %d", ID.c_str(), Popups.size());
#endif
		
		Engine->StartFrameDrawing(ObjectHash);

		Popup->m_pObj->RenderObject(Popup->m_pObj, Popup->m_Position); 
		Item = new PopupDescription(Engine, ID, ObjectHash, NULL);
		if(EffectID)
		{
			
			PlutoRectangle HighlightRectangle(
				Popup->m_pObj->m_rPosition.X + Popup->m_Position.X,
				Popup->m_pObj->m_rPosition.Y + Popup->m_Position.Y,
				Popup->m_pObj->m_rPosition.Width, 
				Popup->m_pObj->m_rPosition.Height);

			//Engine->ShowHighlightRectangle(Popup->m_pObj->m_rPosition);
		}
		Popups[ObjectHash] = Item;
	}
	
}

bool PopupCollection::Exists(string ID)
{
	bool Result = Popups.find(ID) != Popups.end();
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Searching in map (size = %d): %s and the result is: %d ", 
		Popups.size(),
		ID.c_str(), 
		Result);
#endif
	return Result; 
}

void PopupCollection::ResetObjectInPopup(std::string PopupID, DesignObj_Orbiter *pObj)
{
	std::map <std::string, PopupDescription* >::iterator it = Popups.find(PopupID);

	if(it != Popups.end())
	{
		if( it->second==NULL )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"PopupCollection::ResetObjectInPopup frame is null");
			return;
		}
		MeshFrame *pPopupFrame = it->second->Frame();
		MeshFrame* pOldFrame = pPopupFrame->FindChild(pObj->GenerateObjectHash(pObj->m_pPopupPoint, false));
		pObj->m_GraphicToDisplay_set("ogl2",pObj->m_GraphicBeforeHighlight);
		MeshFrame* pNewFrame = TextureManager::Instance()->GetCacheItem(pObj->GenerateObjectHash(pObj->m_pPopupPoint));
		
		if(NULL != pPopupFrame && NULL != pOldFrame && NULL != pNewFrame)
		{
			pPopupFrame->ReplaceChild(pOldFrame, pNewFrame);
		}
	}
}
