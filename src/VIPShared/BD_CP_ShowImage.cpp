/*
 BD_CP_ShowImage
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#ifdef VIPPHONE
#ifdef VIPDESIGN
#include "stdafx.h"
#include "VIPDesign/VIPDesignDoc.h"
#include "VIPShared/PlutoConfig.h"
#endif
#else
#ifndef SYMBIAN
#include <iostream>
#include <fstream>
#endif
#endif

#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/PlutoDefs.h"
#include "BD/BDCommandProcessor.h"
#else
#include "VIPShared/VIPIncludes.h"
#include "PlutoMOAppUi.h"
#include "Logger.h"
#endif

#ifdef SMARTPHONE
#include "OrbiterApp.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "BD_CP_ShowImage.h"
#include "BD_PC_GetSignalStrength.h"

BD_CP_ShowImage::BD_CP_ShowImage(unsigned char ImageType,unsigned long ImageSize,const char *pImage,
                                 unsigned long KeysListSize, const char* pRepeatedKeysList, bool bSignalStrengthScreen,
                                 bool bImageQualityScreen, unsigned long nImageQuality) 
	
{
	m_iImageType=ImageType;
	
    m_ImageSize=ImageSize;
	m_pImage = new char[m_ImageSize];
	memcpy(m_pImage, pImage, m_ImageSize);

    m_KeysListSize = KeysListSize;
    m_pRepeatedKeysList = new char[m_KeysListSize];
    memcpy(m_pRepeatedKeysList, pRepeatedKeysList, m_KeysListSize);

    m_bSignalStrengthScreen = bSignalStrengthScreen;
    m_bImageQualityScreen = bImageQualityScreen;
    m_nImageQuality = nImageQuality;
}

BD_CP_ShowImage::~BD_CP_ShowImage()
{
#ifndef SYMBIAN
	PLUTO_SAFE_DELETE_ARRAY(m_pImage);
    PLUTO_SAFE_DELETE_ARRAY(m_pRepeatedKeysList);
#endif
}

void BD_CP_ShowImage::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_unsigned_char(m_iImageType);
	Write_long(m_ImageSize);
	Write_block(m_pImage, m_ImageSize);
    Write_long(m_KeysListSize);
    Write_block(m_pRepeatedKeysList, m_KeysListSize);
    Write_unsigned_char(m_bSignalStrengthScreen);
    Write_unsigned_char(m_bImageQualityScreen);
    Write_long(m_nImageQuality);
}

void BD_CP_ShowImage::ParseCommand(unsigned long size,const char *data)
{
#ifdef SYMBIAN
    LOG("#	BD_CP_ShowImage::ParseCommand  #\n");
#endif

	BDCommand::ParseCommand(size, data);


	m_iImageType = Read_unsigned_char();
	m_ImageSize = Read_long();
	m_pImage = Read_block(m_ImageSize);
    m_KeysListSize = Read_long();
    m_pRepeatedKeysList = Read_block(m_KeysListSize);
    m_bSignalStrengthScreen = !!Read_unsigned_char();
    m_bImageQualityScreen = !!Read_unsigned_char();
    m_nImageQuality = Read_long();
}

bool BD_CP_ShowImage::ProcessCommand(BDCommandProcessor *pProcessor)
{
#ifdef VIPPHONE
#ifdef SYMBIAN
	 LOG("#	Received 'ShowImage' command  #\n");

     LOG("Signal strength on/off \n");
     LOG(m_bSignalStrengthScreen);
     ((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_bSignalStrengthScreen = m_bSignalStrengthScreen;
     ((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_bImageQualityScreen = m_bImageQualityScreen;
     ((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_nImageQuality = m_nImageQuality;

	 LOG("Open image\n");
	 ((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->OpenImage(m_iImageType, m_ImageSize, m_pImage);
	 ((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->Show();

	 LOG("Intercept repeated keys\n");
     ((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->InterceptRepeatedKeys(m_KeysListSize, m_pRepeatedKeysList);

	//request signal strength
	BDCommandProcessor_Symbian_Base* pBDCommandProcessor = 
		((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->m_pBDCommandProcessor;

	if(pBDCommandProcessor && m_bSignalStrengthScreen)
	{
		BDCommand *pCommand = new BD_PC_GetSignalStrength();
		pBDCommandProcessor->AddCommand(pCommand);
	}
#endif //SYMBIAN

#ifdef SMARTPHONE
	 OrbiterApp::GetInstance()->SetSignalStrengthScreen(m_bSignalStrengthScreen);
     OrbiterApp::GetInstance()->SetImageQualityScreen(m_bImageQualityScreen);
	 OrbiterApp::GetInstance()->ShowImage(m_iImageType, m_ImageSize, m_pImage);
     OrbiterApp::GetInstance()->SetImageQuality(m_nImageQuality);
	 OrbiterApp::GetInstance()->InterceptRepeatedKeys(m_KeysListSize, m_pRepeatedKeysList);
#endif

#ifdef VIPDESIGN
	g_pPlutoConfig->m_pDoc->m_pImageStatic_Type=m_iImageType;
	g_pPlutoConfig->m_pDoc->m_pImageStatic_Size=m_ImageSize;
	g_pPlutoConfig->m_pDoc->m_pImageStatic_Data=m_pImage;
    //TODO: intercept repetead keys?
	g_pPlutoConfig->m_pDoc->InvalidateAllViews();
#endif //VIPDESIGN

#endif //VIPPHONE

	return true;
}
