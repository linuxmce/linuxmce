/*
 TextStyle
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */
 
#ifndef ORBITER
	#include "PlutoUtils/CommonIncludes.h"	
	#include "pluto_main/Table_Style.h"
	#include "pluto_main/Table_StyleVariation.h"
#endif

#include "DCE/Logger.h"
#include "TextStyle.h"

#if !defined(POCKETFROG)
#include <SDL_ttf.h>
#endif

TextStyle::TextStyle() : m_iPK_Style(0), m_iPK_StyleVariation(0),
	m_iPK_Style_Selected(0), m_iPK_Style_Highlighed(0), m_iPK_Style_Alt(0),
	m_ForeColor(PlutoColor()), m_BackColor(PlutoColor()), m_ShadowColor(PlutoColor()),
	m_iPixelHeight(0), m_bBold(false), m_bItalic(false), m_bUnderline(false),
	m_iShadowX(0), m_iShadowY(0), m_iBorderStyle(0), 
	m_iPK_HorizAlignment(0), m_iPK_VertAlignment(0), m_pTTF_Font(NULL)
{ 
} 

TextStyle::TextStyle(class Row_StyleVariation *pRow_StyleVariation)
{
#ifndef ORBITER
	m_iPK_Style = pRow_StyleVariation->FK_Style_get();
	m_iPK_StyleVariation = pRow_StyleVariation->PK_StyleVariation_get();
	m_iPK_Style_Selected = pRow_StyleVariation->FK_Style_getrow()->FK_Style_Selected_get();
	m_iPK_Style_Highlighed = pRow_StyleVariation->FK_Style_getrow()->FK_Style_Highlighted_get();
	m_iPK_Style_Alt = pRow_StyleVariation->FK_Style_getrow()->FK_Style_Alt_get();
	m_sFont = pRow_StyleVariation->Font_get();
	m_ForeColor = pRow_StyleVariation->ForeColor_get();
	m_BackColor = pRow_StyleVariation->BackColor_get();
	m_ShadowColor = pRow_StyleVariation->ShadowColor_get();
	m_iPixelHeight = pRow_StyleVariation->PixelHeight_get();
	m_bBold = pRow_StyleVariation->Bold_get()==1;
	m_bItalic = pRow_StyleVariation->Italic_get()==1;
	m_bUnderline = pRow_StyleVariation->Underline_get()==1;
	m_iShadowX = pRow_StyleVariation->ShadowX_get();
	m_iShadowY = pRow_StyleVariation->ShadowY_get();
	m_iBorderStyle = pRow_StyleVariation->BorderStyle_get();
	m_iPK_HorizAlignment=pRow_StyleVariation->FK_HorizAlignment_get();
	m_iPK_VertAlignment=pRow_StyleVariation->FK_VertAlignment_get();
#endif //ORBITER

	m_pTTF_Font = NULL;
}

TextStyle::~TextStyle()
{
#if !defined(POCKETFROG)
	if(NULL != m_pTTF_Font)  
		TTF_CloseFont(m_pTTF_Font);
#endif
}

bool TextStyle::Serialize( bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData )
{
	// Save the starting points
	m_pcDataBlock=pcDataBlock; m_dwAllocatedSize=dwAllocatedSize; m_pcCurrentPosition=pcCurrentPosition;

	if( bWriting )
	{
		Write_long(m_iPK_Style);
		Write_long(m_iPK_StyleVariation);
		Write_long(m_iPK_Style_Selected);
		Write_long(m_iPK_Style_Highlighed);
		Write_long(m_iPK_Style_Alt);
		Write_long(m_iVersion);
		Write_long(m_iPixelHeight);
		Write_long(m_iShadowX);
		Write_long(m_iShadowY);
		Write_long(m_iBorderStyle);
		Write_long(m_iRotate);
		Write_long(m_iPK_HorizAlignment);
		Write_long(m_iPK_VertAlignment);
		Write_char(m_bBold);
		Write_char(m_bItalic);
		Write_char(m_bUnderline);
		Write_string(m_sFont);
		Write_long(m_ForeColor.m_Value);
		Write_long(m_BackColor.m_Value);
		Write_long(m_ShadowColor.m_Value);
	}
	else
	{
		m_iPK_Style=Read_long();
		m_iPK_StyleVariation=Read_long();
		m_iPK_Style_Selected=Read_long();
		m_iPK_Style_Highlighed=Read_long();
		m_iPK_Style_Alt=Read_long();
		m_iVersion=Read_long();
		m_iPixelHeight=Read_long();
		m_iShadowX=Read_long();
		m_iShadowY=Read_long();
		m_iBorderStyle=Read_long();
		m_iRotate=Read_long();
		m_iPK_HorizAlignment=Read_long();
		m_iPK_VertAlignment=Read_long();
		m_bBold=Read_char()==1;
		m_bItalic=Read_char()==1;
		m_bUnderline=Read_char()==1;
		Read_string(m_sFont);
		m_ForeColor.m_Value=Read_long();
		m_BackColor.m_Value=Read_long();
		m_ShadowColor.m_Value=Read_long();
	}
	// We may have re-allocated the memory block and size, and the position will have changed
	pcDataBlock=m_pcDataBlock; dwAllocatedSize=m_dwAllocatedSize; pcCurrentPosition=m_pcCurrentPosition;
	return true;
}

