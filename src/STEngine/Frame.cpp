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
#include "Frame.h"

#include "Painter.h"

#include "DCE/Logger.h"

#include <algorithm>
using namespace DCE;

Frame::Frame(string Name, Container* pMesh) 
	: Visible_(true),
	pTransform(),
	Parent(NULL)
{
	this->pMesh = pMesh;
	this->Name_ = Name;
	
}

Frame::~Frame(void)
{
	delete pMesh;
	pMesh = NULL;
}

/*virtual*/ void Frame::CleanUp()
{
	//DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "xxxxx Frame::CleanUp: %pMesh", this);	

	vector<Frame*>::iterator Child;
	for(Child = Children.begin(); Child!=Children.end(); Child++)
	{
		Frame *pFrame = *Child;
	
		pFrame->CleanUp();
		delete pFrame;
		pFrame = NULL;
	}	
	Children.clear();
}

void Frame::SetContainer(Container* pMesh)
{
	this->pMesh = pMesh;
}

void Frame::AddChild(Frame* Frame)
{
	if(Frame == NULL)
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Frame::AddChild: Frame is NULL!!!");	
		return;
	}

	if(NULL != Frame->Parent)
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Frame::AddChild: Frame %pMesh/%s already has a parent %pMesh!",
			Frame, Frame->Name_.c_str(), Frame->Parent);	

		//throw "Frame already has a parent";
	}

//	DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "Frame::AddChild: Added %pMesh/%s to %pMesh/%s", Frame, Frame->Name_.c_str(),
//		this, Name_.c_str());	
	Children.push_back(Frame);
	Frame->Parent = this;
}

void Frame::RemoveChild(Frame* pFrame)
{
	//CheckIntegrity(Frame);

	if(pFrame == NULL)
	{
		//DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Frame::RemoveChild: Frame is NULL!!!");
		return;
	}

	if(NULL != pFrame->Parent)
	{
		vector<Frame*>::iterator Child = 
			find(pFrame->Parent->Children.begin(), pFrame->Parent->Children.end(), pFrame);
		if(Child == pFrame->Parent->Children.end())
		{
			DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Frame::RemoveChild: Got a parent, but doesn't have us as child!");
			//throw "Got a parent, but doesn't have us as child";
		}
		else
		{
			pFrame->Parent->Children.erase(Child);

//			DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "Frame::RemoveChild %pMesh/%s from parent %pMesh/%s", 
//				Frame, Frame->Name_.c_str(), Frame->Parent, Frame->Parent->Name_.c_str());

			pFrame->Parent = NULL;
		}
	}
	else
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Frame::RemoveChild: Got no parent! :(");
		//throw "Got no parent! :( I'm all alone!";
	}
}

void Frame::Paint(Transform ChildTransform)
{
	if(!Visible_)
	{
		//DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "NOT Painting %pMesh. It's invisible.", this);	
		return; 
	}
	
	Transform pTransform;
	pTransform.ApplyTransform(this->pTransform);
	pTransform.ApplyTransform(ChildTransform);
	Painter* Painter = Painter::Instance();
	if(pMesh!= NULL)
		Painter->PaintContainer(*pMesh, pTransform);

	//DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "xxxxx Painting %pMesh with %d children: ", this, Children.size());

	vector<Frame *>::iterator Child, EndChild;
	for(Child = Children.begin(), EndChild = Children.end(); Child != EndChild; ++Child)
	{  
		Frame *pFrame = *Child;
		pFrame->Paint(pTransform);
	}

	//DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "xxxxx Painting %pMesh END ", this);
}

/*virtual*/ void Frame::SetTransform(Transform& pTransform)
{
	this->pTransform.CopyFrom(pTransform);
}

/*virtual*/ void Frame::ApplyTransform(Transform& pTransform)
{
	this->pTransform.ApplyTransform(pTransform);
}

/*virtual*/ void Frame::SetVisible(bool Visible)
{
	Visible_ = Visible;
}

/*virtual*/ void Frame::Paint()
{
    Transform pTransform;
	Paint(pTransform);
}

Container* Frame::GetContainer()
{
	return pMesh;
}

/*virtual*/ void Frame::SetAlpha(float Alpha, string ExcludePattern /* = "" */)
{
	//we don't want to setup alpha for Frames that have their name starting with exclude pattern
	if(pMesh)
		pMesh->SetAlpha(Alpha);

	vector<Frame *>::iterator Child, EndChild;
	for(Child = Children.begin(), EndChild = Children.end(); Child != EndChild; ++Child)
	{  
		Frame *pFrame = *Child;
		pFrame->SetAlpha(Alpha);
	}
}

/*virtual*/ void Frame::SetColor(Point3D& Color)
{
	if(pMesh)
		pMesh->SetColor(Color);
	vector<Frame *>::iterator Child, EndChild;
	for(Child = Children.begin(), EndChild = Children.end(); Child != EndChild; ++Child)
	{  
		Frame *pFrame = *Child;
		pFrame->SetColor(Color);
	}
}

Frame *Frame::FindChild(string Name)
{
	Frame *Child = NULL;

	if(Name_ == Name)
		Child = this;
	else
	{
		for(vector<Frame*>::iterator it = Children.begin(), end = Children.end(); it != end; ++it)
		{
			Child = (*it)->FindChild(Name);

			if(NULL != Child)
				return Child;
		}
	}

	return Child;
}
