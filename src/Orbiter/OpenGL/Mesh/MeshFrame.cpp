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
#include "MeshFrame.h"
#include "MeshPainter.h"
#include "DCE/Logger.h"

#ifdef DEBUG
	#define DEBUG_MESH_FRAMES
#endif

MeshFrame::MeshFrame(string Name, MeshContainer* Mesh) 
	: 
	m_pParent(NULL),
	m_bVisible(true),
	m_bVolatile(false),
	Transform(),
	TextureTransform()
{
	m_pMeshContainer = Mesh;
	m_sName = Name;

#if defined(DEBUG_MESH_FRAMES)
	DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "MeshFrame constructor %p/%s", this, m_sName.c_str());
#endif

#if defined(DETECT_LEAKS)
	if(m_sName.find("clone") != string::npos)
		LeaksDetector::Instance().NewClone(m_sName);
	else
		LeaksDetector::Instance().NewMeshFrame(m_sName);
#endif
}

MeshFrame::~MeshFrame(void)
{
	if(NULL != m_pMeshContainer)
		m_pMeshContainer->DisposeTextures(IsVolatile());

	delete m_pMeshContainer;
	m_pMeshContainer = NULL;

	TextureManager::Instance()->InvalidateItem(this);

#if defined(DEBUG_MESH_FRAMES)
	DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "MeshFrame destructor %p/%s, volatile %d", this, m_sName.c_str(), m_bVolatile);
#endif

#if defined(DETECT_LEAKS)
	if(m_sName.find("clone") != string::npos)
		LeaksDetector::Instance().DestroyedClone(m_sName);
	else
		LeaksDetector::Instance().DestroyedMeshFrame(m_sName);
#endif
}

void MeshFrame::MarkAsVolatile() 
{ 
	m_bVolatile = true; 
}

void MeshFrame::MarkAsVolatileRecursively() 
{ 
	m_bVolatile = true; 

	vector<MeshFrame*>::iterator Child;
	for(Child = Children.begin(); Child!=Children.end();++Child)
	{
		MeshFrame *pMeshFrame = *Child;
		pMeshFrame->MarkAsVolatileRecursively();
	}
}

/*virtual*/ void MeshFrame::CleanUp(bool VolatilesOnly/* = false*/)
{
#ifdef DEBUG_MESH_FRAMES
	DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "MeshFrame::CleanUp: %p/%s", this, m_sName.c_str());	
#endif

	vector<MeshFrame*>::iterator Child;
	for(Child = Children.begin(); Child!=Children.end();++Child)
	{
		MeshFrame *pMeshFrame = *Child;
		pMeshFrame->CleanUp(VolatilesOnly);

		if(!VolatilesOnly || pMeshFrame->IsVolatile())
		{
			delete pMeshFrame;
			pMeshFrame = NULL;
		}
		else
		{
			pMeshFrame->ResetParent();
		}
	}	

	Children.clear();
}

void MeshFrame::SetMeshContainer(MeshContainer* Mesh)
{
	m_pMeshContainer = Mesh;
}

void MeshFrame::AddChild(MeshFrame* Frame)
{
	if(Frame == NULL)
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "MeshFrame::AddChild: Frame is NULL!!!");	
		return;
	}

	if(NULL != Frame->m_pParent)
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "MeshFrame::AddChild: Frame %p/%s already has a parent %p!",
			Frame, Frame->m_sName.c_str(), Frame->m_pParent);	
	}

#ifdef DEBUG_MESH_FRAMES
	DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "MeshFrame::AddChild: Added %p/%s to %p/%s", Frame, Frame->m_sName.c_str(),
		this, m_sName.c_str());	
#endif

	Children.push_back(Frame);
	Frame->m_pParent = this;

	CheckIntegrity(Frame);
}

void MeshFrame::RemoveChild(MeshFrame* Frame, bool bDetachOnly)
{
	if(Frame == NULL)
		return;

	if(NULL != Frame->m_pParent)
	{
		vector<MeshFrame*>::iterator Child = 
			find(Frame->m_pParent->Children.begin(), Frame->m_pParent->Children.end(), Frame);
		if(Child == Frame->m_pParent->Children.end())
		{
			DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "MeshFrame::RemoveChild: Got a parent, but doesn't have us as child!");
		}
		else
		{
			Frame->m_pParent->Children.erase(Child);

#ifdef DEBUG_MESH_FRAMES
			DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "MeshFrame::RemoveChild %p/%s from parent %p/%s", 
				Frame, Frame->m_sName.c_str(), Frame->m_pParent, Frame->m_pParent->m_sName.c_str());
#endif

			Frame->m_pParent = NULL;

			if(!bDetachOnly && Frame->IsVolatile())
			{
				Frame->CleanUp(true);
				delete Frame;
				Frame = NULL;
			}
		}
	}
	else
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "MeshFrame::RemoveChild: Got no parent! :(");
	}
}

MeshFrame* MeshFrame::ReplaceChild(MeshFrame* OldFrame, MeshFrame* NewFrame)
{
	CheckIntegrity(OldFrame);

	if(OldFrame == NULL)
		return NULL;

	if(NULL != OldFrame->m_pParent)
	{
		vector<MeshFrame*>::iterator Child = find(OldFrame->m_pParent->Children.begin(), 
			OldFrame->m_pParent->Children.end(), OldFrame);

		if(Child == OldFrame->m_pParent->Children.end())
		{
			DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "MeshFrame::ReplaceChild: Got a parent, but doesn't have us as child!");
		}
		else
		{
			NewFrame->m_pParent = OldFrame->m_pParent;
			*Child = NewFrame;

#ifdef DEBUG_MESH_FRAMES
			DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "MeshFrame::ReplaceChild %p/%s from parent %p/%s with %p/%s", 
				OldFrame, OldFrame->m_sName.c_str(), OldFrame->m_pParent, OldFrame->m_pParent->m_sName.c_str(),
				NewFrame, NewFrame->m_sName.c_str());
#endif

			bool bRemoveAllReplacedFrame = TextureManager::Instance()->MemoryManagementEnabled();

			if(
				NewFrame != OldFrame && NULL != OldFrame && 
				(OldFrame->IsVolatile() || bRemoveAllReplacedFrame)
			)
			{
				OldFrame->CleanUp(true);

				if(OldFrame->IsVolatile())
				{
					delete OldFrame;
					OldFrame = NULL;
				}
			}

			CheckIntegrity(NewFrame);

			return NewFrame;
		}
	}
	else
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "MeshFrame::ReplaceChild: Got no parent! :(");
	}

	return OldFrame;
}

void MeshFrame::Paint(MeshTransform ChildTransform)
{
	if(!m_bVisible)
		return; 
	
	MeshTransform Transform;
	Transform.ApplyTransform(this->Transform);
	Transform.ApplyTransform(ChildTransform);
	MeshPainter* Painter = MeshPainter::Instance();

	if(MeshIsValid())
	{
		if(m_pMeshContainer != NULL)
			Painter->PaintContainer(*m_pMeshContainer, Transform, TextureTransform);

		vector<MeshFrame *>::iterator Child, EndChild;
		for(Child = Children.begin(), EndChild = Children.end(); Child != EndChild; ++Child)
		{  
			MeshFrame *pMeshFrame = *Child;
			pMeshFrame->Paint(Transform);
		}
	}
}

/*virtual*/ void MeshFrame::SetTransform(MeshTransform& Transform)
{
	this->Transform.CopyFrom(Transform);
}

/*virtual*/ void MeshFrame::ApplyTransform(MeshTransform& Transform)
{
	this->Transform.ApplyTransform(Transform);
}

/*virtual*/ void MeshFrame::SetVisible(bool Visible)
{
	m_bVisible = Visible;
}

/*virtual*/ void MeshFrame::Paint()
{
    MeshTransform Transform;
	Paint(Transform);
}

/*virtual*/ void MeshFrame::SetTextureTransform(MeshTransform& TextureTransform)
{
	this->TextureTransform = TextureTransform;
}

MeshContainer* MeshFrame::GetMeshContainer()
{
	return m_pMeshContainer;
}

/*virtual*/ void MeshFrame::SetAlpha(float Alpha, string ExcludePattern /* = "" */)
{
	//we don't want to setup alpha for Frames that have their name starting with exclude pattern
	if(!ExcludePattern.empty())
	{
		vector<string> vectPatterns;
		StringUtils::Tokenize(ExcludePattern, "|", vectPatterns);

		for(vector<string>::iterator it = vectPatterns.begin(); it != vectPatterns.end(); ++it)
		{
			string sPattern = *it;
			if(m_sName.find(sPattern) == 0)
				return;
		}
	}

	if(m_pMeshContainer)
		m_pMeshContainer->SetAlpha(Alpha);

	vector<MeshFrame *>::iterator Child, EndChild;
	for(Child = Children.begin(), EndChild = Children.end(); Child != EndChild; ++Child)
	{  
		MeshFrame *pMeshFrame = *Child;
		pMeshFrame->SetAlpha(Alpha, ExcludePattern);
	}
}

/*virtual*/ void MeshFrame::SetColor(Point3D& Color)
{
	if(m_pMeshContainer)
		m_pMeshContainer->SetColor(Color);
	vector<MeshFrame *>::iterator Child, EndChild;
	for(Child = Children.begin(), EndChild = Children.end(); Child != EndChild; ++Child)
	{  
		MeshFrame *pMeshFrame = *Child;
		pMeshFrame->SetColor(Color);
	}
}

MeshFrame *MeshFrame::Clone()
{
	MeshFrame *Result = new MeshFrame(m_sName + " clone");

	Result->MarkAsVolatile();
	Result->m_bVisible = m_bVisible;

	if(NULL != m_pMeshContainer)
	{
		Result->m_pMeshContainer = m_pMeshContainer;
		m_pMeshContainer = NULL;
	}

	Result->Transform = Transform;
	Result->TextureTransform = TextureTransform;

	for(vector<MeshFrame*>::iterator it = Children.begin(), end = Children.end(); it != end; ++it)
	{
		Result->AddChild((*it)->Clone());
	}
	return Result;
}

void MeshFrame::Print(string sIndent)
{
	//if(m_sName.find("rectangle") != string::npos)
	//	return;

	//if(m_sName.find("text") != string::npos)
	//	return;

	DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "%s%s '%s' %s %p", sIndent.c_str(), 
		Children.size() ? "+ " : "- ", m_sName.c_str(), m_bVolatile ? "(v)" : "", this);

	for(vector<MeshFrame*>::iterator it = Children.begin(), end = Children.end(); it != end; ++it)
	{
		(*it)->Print(sIndent + "`-");
	}
}

MeshFrame *MeshFrame::FindChild(string Name)
{
	MeshFrame *ChildMesh = NULL;

	if(m_sName == Name)
		ChildMesh = this;
	else
	{
		//skip datagrid's children
		if(m_sName.find("datagrid ") != 0)
		{
			for(vector<MeshFrame*>::iterator it = Children.begin(), end = Children.end(); it != end; ++it)
			{
				ChildMesh = (*it)->FindChild(Name);

				if(NULL != ChildMesh)
					return ChildMesh;
			}
		}
	}

	return ChildMesh;
}

void MeshFrame::Stealth()
{	
	m_sName = m_sName + " stealth";

	for(vector<MeshFrame*>::iterator it = Children.begin(), end = Children.end(); it != end; ++it)
		(*it)->Stealth();
}

bool MeshFrame::CheckIntegrity(MeshFrame *Frame)
{
	bool Result = false;

	if(NULL != Frame && NULL != Frame->m_pParent)
	{
		vector<MeshFrame*>::iterator Child = find(Frame->m_pParent->Children.begin(), 
			Frame->m_pParent->Children.end(), Frame);

        Result = Child != Frame->m_pParent->Children.end();	
	}

	if(!Result)
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "MeshFrame::CheckIntegrity failed for %p/%s",
			Frame, NULL != Frame ? Frame->Name().c_str() : "null frame");
	}

	return Result;
}

bool MeshFrame::MeshIsValid()
{
	if(NULL != m_pMeshContainer && NULL != m_pMeshContainer->Triangles && m_pMeshContainer->NoTriangles > 0)
		for(int i = 0; i < m_pMeshContainer->NoTriangles; ++i)
			if(NULL != m_pMeshContainer->Triangles[i].Texture && m_pMeshContainer->Triangles[i].Texture->Texture == 0)
			{
				DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "MeshFrame is invalid %s", m_sName.c_str());
				return false;
			}

	return true;
}
