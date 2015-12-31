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
#ifndef TEXT_WRAPPER
#define TEXT_WRAPPER

#include "Orbiter/TextStyle.h"
#include "TextSplitter.h"

#include "pluto_main/Define_HorizAlignment.h"
#include "pluto_main/Define_VertAlignment.h"
//-------------------------------------------------------------------------------------------------------
//forward declarations
struct SDL_Surface;
class RendererImage;
//-------------------------------------------------------------------------------------------------------
void WrapAndRenderText(SDL_Surface * Surface, string text, int X, int Y, int W, int H,
					   string FontPath, TextStyle *pTextStyle,int PK_HorizAlignment,int PK_VertAlignment,
                       MapTextStyle *pmapTextStyle = NULL);
//-------------------------------------------------------------------------------------------------------
class LineAttr
{
	public:
		int HAlign;
		int VAlign;
		int Height, Width;

		LineAttr() : HAlign(HORIZALIGNMENT_Left_CONST), VAlign(VERTALIGNMENT_Top_CONST), Height(0), Width(0) {}
};
//-------------------------------------------------------------------------------------------------------
typedef list<RendererImage *> ImageRow;
//-------------------------------------------------------------------------------------------------------
class TextLineWrap
{
	protected:
		int lastX, Width, Height, origX, origY;
		TextLineExtract T;
		string m_FontPath, m_FontFile;
		int m_FontSize;

        MapTextStyle *m_pmapTextStyle;
		
		pair<int, int> WordWidth(string word, RendererImage * & RI, TextStyle * pTextStyle, bool NewSurface = true);
		void AddRow(Row line, ImageRow ImageLine, LineAttr LA);
		void AddImageWord(ImageRow & line, RendererImage * word);

	public:
		list<Row> lines;
		list<ImageRow> ImageLines;
		list<LineAttr> LineAttrData;

        TextLineWrap(MapTextStyle *pmapTextStyle = NULL);
		~TextLineWrap();
		list<Row> & Wrap(string text, int atX, int atY, int W, int H, string FontPath, TextStyle * pTextStyle,int PK_HorizAlignment,int PK_VertAlignment);
		void Clear();
		void RenderToSurface(SDL_Surface * Surface);
};
//-------------------------------------------------------------------------------------------------------
#endif
