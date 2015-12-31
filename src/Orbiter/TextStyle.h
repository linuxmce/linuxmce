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
#ifndef TextStyle_h
#define TextStyle_h

#include "SerializeClass/SerializeClass.h"
#include "SerializeClass/ShapesColors.h"

struct _TTF_Font;
typedef struct _TTF_Font TTF_Font;

// Encapsulate a text style
class TextStyle : public SerializeClass
{
public:
	string m_sFont;
	int m_iPK_Style,m_iPK_StyleVariation,m_iPK_Style_Selected,m_iPK_Style_Highlighed,m_iPK_Style_Alt;
	int m_iVersion; // This is for the selected, alt, or highlighted versions
	PlutoColor m_ForeColor,m_BackColor,m_ShadowColor;
	int m_iPixelHeight;
	bool m_bBold, m_bItalic, m_bUnderline;
	int m_iShadowX, m_iShadowY;  // Pixels to offset a drop shadow
	int m_iBorderStyle;
	int m_iRotate;
	int m_iPK_HorizAlignment,m_iPK_VertAlignment;  // To hold the values so we can store them with the text

	TTF_Font *m_pTTF_Font; 

	TextStyle(class Row_StyleVariation *pRow_StyleVariation);
	TextStyle();

	virtual ~TextStyle();
	virtual bool Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData=NULL);
};

typedef map<int,TextStyle *> MapTextStyle;

#endif
