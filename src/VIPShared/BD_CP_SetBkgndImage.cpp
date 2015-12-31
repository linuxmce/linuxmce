/*
 BD_CP_SetBkgndImage
 
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
#include "BD_CP_SetBkgndImage.h"
#include "BD_PC_GetSignalStrength.h"

BD_CP_SetBkgndImage::BD_CP_SetBkgndImage(unsigned char ImageType,unsigned long ImageSize,const char *pImage ) 
	
{
	m_iImageType = ImageType;

    m_ImageSize=ImageSize;
	m_pImage = new char[m_ImageSize];
	memcpy(m_pImage, pImage, m_ImageSize);
}

BD_CP_SetBkgndImage::~BD_CP_SetBkgndImage()
{
#ifndef SYMBIAN
	PLUTO_SAFE_DELETE_ARRAY(m_pImage);
#endif
}

void BD_CP_SetBkgndImage::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_unsigned_char(m_iImageType);
	Write_long(m_ImageSize);
	Write_block(m_pImage, m_ImageSize);
}

void BD_CP_SetBkgndImage::ParseCommand(unsigned long size,const char *data)
{
#ifdef SYMBIAN
    LOG("#	BD_CP_SetBkgndImage::ParseCommand  #\n");
#endif

	BDCommand::ParseCommand(size, data);

	m_iImageType = Read_unsigned_char();
	m_ImageSize = Read_long();
	m_pImage = Read_block(m_ImageSize);
}

bool BD_CP_SetBkgndImage::ProcessCommand(BDCommandProcessor *pProcessor)
{
#ifdef VIPPHONE
#ifdef SYMBIAN
	 LOG("#	Received 'SetBkgndImage' command  #\n");

	 LOG("Open image\n");
	 ((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->OpenImage( m_iImageType, m_ImageSize, m_pImage );
	 ((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->Show();
#endif //SYMBIAN

#ifdef SMARTPHONE
	 OrbiterApp::GetInstance()->SetBkgndImage( m_iImageType, m_ImageSize, m_pImage);
#endif

#ifdef VIPDESIGN
	g_pPlutoConfig->m_pDoc->m_pImageStatic_Type=m_iImageType;
	g_pPlutoConfig->m_pDoc->m_pImageStatic_Size=m_ImageSize;
	g_pPlutoConfig->m_pDoc->m_pImageStatic_Data=m_pImage;

	g_pPlutoConfig->m_pDoc->InvalidateAllViews();
#endif //VIPDESIGN

#endif //VIPPHONE

	return true;
}
