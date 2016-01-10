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
#include "GalleryScenario.h"

#include "GL/gl.h"

#include "Builder.h"
#include "Painter.h"
#include "../DCE/Logger.h"

using namespace DCE;

GalleryScenario::GalleryScenario(int Width, int Height, int FaddingTime, 
	int ZoomTime, string SearchImageFolder, bool bUseAnimation,int nMaxSize)
: AfterPicture(NULL), BeforePicture(NULL), StateMachine(NULL), 
	nLastTimeUpdated(0), Browser(NULL), Fades(NULL)
{
	StateMachine = new GaleryStateMachine();
	this->FaddingTime = FaddingTime;
	this->ZoomTime = ZoomTime;

	Browser = new FileBrowser(SearchImageFolder);
	this->Width = Width;
	this->Height = Height;
	m_bUseAnimation = bUseAnimation;
	m_nMaxSize = nMaxSize;

	ZoomFactory::Instance()->Setup(Width, Height);

	AfterPicture = new AnimatedPicture(Width, Height, m_nMaxSize);
	BeforePicture = new AnimatedPicture(Width, Height, m_nMaxSize);
}

GalleryScenario::~GalleryScenario(void)
{
	delete StateMachine;
	delete Browser;

	delete AfterPicture;
	delete BeforePicture;

	delete Fades;

	ZoomFactory::Destroy();
	FadeFactory::Destroy();
	TextureManager::Destroy();
}

void GalleryScenario::Reset()
{
	delete AfterPicture;
	delete BeforePicture;
	delete StateMachine;

	AfterPicture = new AnimatedPicture(Width, Height, m_nMaxSize);
	BeforePicture = new AnimatedPicture(Width, Height, m_nMaxSize);
	StateMachine = new GaleryStateMachine();
}

bool GalleryScenario::Update(bool bForceRefresh)
{
	glEnable(GL_BLEND);
	int Status = StateMachine->GetStatus();
	int CurrentTime = SDL_GetTicks();

	if(!m_bUseAnimation)
	{
		if(Status == STATUS_STARTUP)
		{
			LoggerWrapper::GetInstance()->Write(LV_ALARM, "Painting the image...");
			BeforePicture->LoadFromFile(Browser->NextFile());
			BeforePicture->SetZoomKind(CurrentTime, ZoomTime);
			BeforePicture->Update(CurrentTime);
			BeforePicture->Paint();
			StateMachine->AboutToPaintStaticImage();
			return true;
		}

		if(int(SDL_GetTicks()) - nLastTimeUpdated > ZoomTime)
		{
			LoggerWrapper::GetInstance()->Write(LV_ALARM, "Painting the image...");
			BeforePicture->LoadFromFile(Browser->NextFile());
			BeforePicture->SetZoomKind(CurrentTime, ZoomTime);
			BeforePicture->Update(CurrentTime);
			BeforePicture->Paint();
			nLastTimeUpdated = SDL_GetTicks();
			return true;
		}

		if(bForceRefresh)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Painting the image... (force refresh) ");
			BeforePicture->Paint();
			return true;
		}

		return false;
	}

	bool Result;
	switch(Status) {
	case STATUS_STARTUP:
		BeforePicture->LoadFromFile(Browser->NextFile());
		AfterPicture->LoadFromFile(Browser->NextFile());


		BeforePicture->SetZoomKind(CurrentTime, ZoomTime);
		AfterPicture->SetZoomKind(CurrentTime, ZoomTime);

		NewImageStartTime = CurrentTime;

		StateMachine->FadeImages();
		Fades = FadeFactory::Instance()->CreateFadeMode(0, 
			AfterPicture->GetFrame(), BeforePicture->GetFrame(), 
			SDL_GetTicks(),
			FaddingTime);

		break;
	case STATUS_FADE_IMAGES:
		Result = Fades->Update(CurrentTime);

		AfterPicture->Update(CurrentTime);
		BeforePicture->Update(CurrentTime);

		AfterPicture->Paint();
		BeforePicture->Paint();

		if (Result)
		{
			LoggerWrapper::GetInstance()->Write(LV_ALARM, "Start zoom one image");
			StateMachine->AnimateCurrentImage();
		}

		break;
	case STATUS_ANIMATE_CURRENT_IMAGE:
		Result = AfterPicture->Update(CurrentTime);
		AfterPicture->Paint();
		if (Result)
		{
			LoggerWrapper::GetInstance()->Write(LV_ALARM, "Start fade images");
			AnimatedPicture* Temp = AfterPicture;
			AfterPicture = BeforePicture;
			BeforePicture = Temp;
			AfterPicture->LoadFromFile(Browser->NextFile());
			StateMachine->FadeImages();

			AfterPicture->SetZoomKind(CurrentTime, ZoomTime);
			StateMachine->FadeImages();

			delete Fades;
			Fades = FadeFactory::Instance()->CreateFadeMode(0, 
				AfterPicture->GetFrame(), BeforePicture->GetFrame(), 
				SDL_GetTicks(),
				FaddingTime);
		}
		break;
	default:;
	}

	return true;
}

void GalleryScenario::RefreshFileList(string sFileList)
{
	if(NULL != Browser)
		Browser->RefreshFileList(sFileList);
}
