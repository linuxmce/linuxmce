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
#include "TextWrapper.h"

#include "SDL_Helpers/SDL_Defs.h"
#include "SDL_Helpers/SDL_RenderText.h"
#include "SDL_Helpers/SDL_CompositeAlphaChannel.h"
#include "SDL_Helpers/SDL_RenderUtils.h"

#include "Orbiter/TextStyle.h"
#include "DCE/Logger.h"
using namespace DCE;

// Radu: I hope I'll get around to optimizing all this some time.
// Radu: From my point of view, this looks like its going to eat a lot of memory.
// Radu: and it also looks like it's slow

TextLineWrap::TextLineWrap(MapTextStyle *pmapTextStyle/* = NULL*/)
{
    m_pmapTextStyle = pmapTextStyle;
}

TextLineWrap::~TextLineWrap()
{
	Clear();
}

void TextLineWrap::Clear()
{
	T.Clear();
	lines.clear();

	// clearing pointers before clearing list
	for (list<ImageRow>::iterator i = ImageLines.begin(); i != ImageLines.end(); i++)
	{
		for (ImageRow::iterator j = i->begin(); j != i->end(); j++)
			delete *j;
	}
	ImageLines.clear();
}

pair<int, int> TextLineWrap::WordWidth(string word, RendererImage * & RI,
							TextStyle * pTextStyle, bool NewSurface)
{
	SDL_RenderText text_renderer(m_FontPath);
	return text_renderer.GetWordWidth(word, pTextStyle, RI, NewSurface);
}

void TextLineWrap::AddRow(Row line, ImageRow ImageLine, LineAttr LA)
{
	if (line.size() > 0)
	{
		lines.insert(lines.end(), line);
		ImageLines.insert(ImageLines.end(), ImageLine);
		LineAttrData.insert(LineAttrData.end(), LA);
	}
}

void TextLineWrap::AddImageWord(ImageRow & line, RendererImage * word)
{
	line.insert(line.end(), word);
}

list<Row> & TextLineWrap::Wrap(string text, int atX, int atY, int W, int H,
							   string FontPath, TextStyle * pDefaultTextStyle, int PK_HorizAlignment,int PK_VertAlignment)
{
	string flags("");

	switch (PK_HorizAlignment)
	{
		case HORIZALIGNMENT_Center_CONST: flags += "c"; break;
		case HORIZALIGNMENT_Right_CONST: flags += "r"; break;
	}

	switch (PK_VertAlignment)
	{
		case VERTALIGNMENT_Middle_CONST: flags += "m"; break;
		case VERTALIGNMENT_Bottom_CONST: flags += "b"; break;
	}

	// prepend the default alignments if any
	string actualText = (flags != "" ? "~" + flags + "~" : "") + text;

	list<Row> origLines = T.Extract(actualText);
	list<Word> line;
	ImageRow ImageLine;
	RendererImage * RI;
	bool statU = pDefaultTextStyle->m_bUnderline;
	bool statI = pDefaultTextStyle->m_bItalic;
	bool statB = pDefaultTextStyle->m_bBold;
	LineAttr LAttr;
	pair<int, int> WW; // used to stand for "Word Width", but now it's "Word Dimensions" (first: Width; second: Height)
    int lastY = 0;

	m_FontPath = FontPath;
	m_FontFile = pDefaultTextStyle->m_sFont;
	m_FontSize = pDefaultTextStyle->m_iPixelHeight;

	Width = W; Height = H;
	origX = atX; origY = atY;
	lastX = 0;
	string Space = "";

	for (list<Row>::iterator i = origLines.begin(); i != origLines.end(); i++)
	{
		Row row = *i;
        TextStyle* pCurrentTextStyle = pDefaultTextStyle;
        
        for (list<Word>::iterator j = row.begin(); j != row.end(); j++)
		{
			bool bGotTextStyleTokens = false;
			bool Not = false;
			string sCurrentWord = *j;
			size_t nLastPosition = 0;
			
			while(nLastPosition < sCurrentWord.length() - 1)
			{
				if (sCurrentWord[nLastPosition] == '~')
				{
					size_t nPos = sCurrentWord.find('~', nLastPosition + 1);
					if(nPos != string::npos)
					{
						bGotTextStyleTokens = true;
						string sToken = sCurrentWord.substr(nLastPosition + 1, nPos - 1);
						nLastPosition = nPos + 1;
						if(sToken[0] == 'S')
						{
							string sTextStyleNumber = sToken.substr(1);
							int nTextStyleIndex = atoi(sTextStyleNumber.c_str());

							if(NULL != m_pmapTextStyle)
							{
								MapTextStyle::iterator it = m_pmapTextStyle->find(nTextStyleIndex);
								if(it != m_pmapTextStyle->end())
									pCurrentTextStyle = (*it).second;
							}
						}
						else
						{
							for(string::iterator it_char = sToken.begin(); it_char != sToken.end(); ++it_char)
							{
								char ch = *it_char; 
								switch(ch)
								{
									case 'x':
									case 'X': Not = true; break; 
									case 'B': statB = Not ^ true; break;
									case 'I': statI = Not ^ true; break;
									case 'U': statU = Not ^ true; break;
									case 'l': LAttr.HAlign = HORIZALIGNMENT_Left_CONST; break;
									case 'c': LAttr.HAlign = HORIZALIGNMENT_Center_CONST; break;
									case 'r': LAttr.HAlign = HORIZALIGNMENT_Right_CONST; break;
									case 't': LAttr.VAlign = VERTALIGNMENT_Top_CONST; break;
									case 'm': LAttr.VAlign = VERTALIGNMENT_Middle_CONST; break;
									case 'b': LAttr.VAlign = VERTALIGNMENT_Bottom_CONST; break;
								}
							}
						}
					}
					else
						break;
				}
				else
					break;
			}

			if(bGotTextStyleTokens)
				continue;

			bool statB_bk = pCurrentTextStyle->m_bBold;
			bool statU_bk = pCurrentTextStyle->m_bUnderline;
			bool statI_bk = pCurrentTextStyle->m_bItalic;

            if(pCurrentTextStyle == pDefaultTextStyle)
            {
                pCurrentTextStyle->m_bBold = statB;
                pCurrentTextStyle->m_bUnderline = statU;
                pCurrentTextStyle->m_bItalic = statI;
            }

			WW = WordWidth(Space + sCurrentWord, RI, pCurrentTextStyle);

			if (lastX + WW.first >= Width)
			{
				LAttr.Width = lastX;
				LAttr.Height = WW.second;
				AddRow(line, ImageLine, LAttr);
                lastY += WW.second;
				line.clear();
				ImageLine.clear();
				LAttr.Width = lastX = 0;
				
				// re-render last word because it is going to start a new line
				WW = WordWidth(sCurrentWord, RI, pCurrentTextStyle, false);
			}

			pCurrentTextStyle->m_bBold = statB_bk;
			pCurrentTextStyle->m_bUnderline = statU_bk;
			pCurrentTextStyle->m_bItalic = statI_bk;

			if (lastY + WW.second >= Height)
				break;

			T.AddWord(line, sCurrentWord);
			AddImageWord(ImageLine, RI);
			lastX += WW.first;
			//Space = " ";
		}
		LAttr.Width = lastX;
		LAttr.Height = WW.second;

	if (lastY + WW.second < Height)
		AddRow(line, ImageLine, LAttr);
	else
		break;

        lastY += WW.second;
		line.clear();
		ImageLine.clear();
		lastX = 0;
		Space = "";
	}

	return lines;
}

void TextLineWrap::RenderToSurface(SDL_Surface * Surface)
{
	list<ImageRow>::iterator i1; list<LineAttr>::iterator i2;
	list<ImageRow>::reverse_iterator j1; list<LineAttr>::reverse_iterator j2;
	int X = 0, Y = 0;

	list<ImageRow> TopLines, MiddleLines, BottomLines;
	list<LineAttr> TopAttr, MiddleAttr, BottomAttr;

	for (i1 = ImageLines.begin(), i2 = LineAttrData.begin(); i1 != ImageLines.end(); i1++, i2++)
	{
		switch (i2->VAlign)
		{
			case VERTALIGNMENT_Top_CONST:
				TopLines.insert(TopLines.end(), * i1);
				TopAttr.insert(TopAttr.end(), * i2);
				break;

			case VERTALIGNMENT_Middle_CONST:
				MiddleLines.insert(MiddleLines.end(), * i1);
				MiddleAttr.insert(MiddleAttr.end(), * i2);
				break;

			case VERTALIGNMENT_Bottom_CONST:
				BottomLines.insert(BottomLines.end(), * i1);
				BottomAttr.insert(BottomAttr.end(), * i2);
				break;
		}
	}

	int TopBottomY, BottomTopY;

	for (i1 = TopLines.begin(), i2 = TopAttr.begin(); i1 != TopLines.end(); i1++, i2++)
	{
		switch (i2->HAlign)
		{
			case HORIZALIGNMENT_Left_CONST: X = 0; break;
			case HORIZALIGNMENT_Center_CONST: X = int(Width / 2.0 - i2->Width / 2.0); break;
			case HORIZALIGNMENT_Right_CONST: X = Width - i2->Width; break;
		}
		
		DoRenderToSurface(Surface, * i1, origX + X, origY + Y);
		Y += i2->Height;
	}
	TopBottomY = Y;

	Y = Height;

	for (j1 = BottomLines.rbegin(), j2 = BottomAttr.rbegin(); j1 != BottomLines.rend(); j1++, j2++)
	{
		Y -= j2->Height;
		switch (j2->HAlign)
		{
			case HORIZALIGNMENT_Left_CONST: X = 0; break;
			case HORIZALIGNMENT_Center_CONST: X = int(Width / 2.0 - (* j2).Width / 2.0); break;
			case HORIZALIGNMENT_Right_CONST: X = Width - (* j2).Width; break;
		}
		DoRenderToSurface(Surface, * j1, origX + X, origY + Y);
	}
	BottomTopY = Y;

	int MiddleHeight = 0;
	for (i2 = MiddleAttr.begin(); i2 != MiddleAttr.end(); i2++)
	{
		MiddleHeight += (* i2).Height;
	}

	Y = TopBottomY + int((BottomTopY - TopBottomY) / 2.0 - MiddleHeight / 2.0);
	for (i1 = MiddleLines.begin(), i2 = MiddleAttr.begin(); i1 != MiddleLines.end(); i1++, i2++)
	{
		switch (i2->HAlign)
		{
			case HORIZALIGNMENT_Left_CONST: X = 0; break;
			case HORIZALIGNMENT_Center_CONST: X = int(Width / 2.0 - i2->Width / 2.0); break;
			case HORIZALIGNMENT_Right_CONST: X = Width - i2->Width; break;
		}

		if(origX + X < 0) 
			X = 0;
			
		if(origY + Y < 0)
			Y = 0;

		DoRenderToSurface(Surface, * i1, origX + X, origY + Y);

		Y += i2->Height;
	}
}

void WrapAndRenderText(SDL_Surface * Surface, string text, int X, int Y, int W, int H,
					   string FontPath, TextStyle *pTextStyle,int PK_HorizAlignment,int PK_VertAlignment,
                       MapTextStyle *pmapTextStyle/* = NULL*/)
{
    //we'll also give it the map with text styles
	TextLineWrap T(pmapTextStyle);

    //the text won't be rendered directly on the original surface
    //no line will be removed from the list with lines (no height limit)
	T.Wrap(text, 0, 0, W, H, FontPath, pTextStyle,PK_HorizAlignment,PK_VertAlignment);

    //creates a surface with the size of the text
    SDL_Rect rect = {X, Y, W, H};
    SDL_Surface* pTextSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, W, H, 32, rmask, gmask, bmask, amask);
	if( !pTextSurface )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WrapAndRenderText pTextSurface is NULL for %s",text.c_str());
		return;
	}

    //crops the text's rectangle from the source surface
    bool WasSrcAlpha = (Surface->flags & SDL_SRCALPHA) != 0;
    Uint8 WasAlpha = Surface->format->alpha;
	SDL_SetAlpha(Surface, 0, 0);
    SDL_BlitSurface(Surface, &rect, pTextSurface, NULL);
	SDL_SetAlpha(Surface, WasSrcAlpha ? SDL_SRCALPHA : 0, WasAlpha);

    //renders the text on the new surface, relative to 0, 0
    T.RenderToSurface(pTextSurface);

    //blits the text surface to the original surface
	SDL_SetAlpha(pTextSurface, 0, 0);
    SDL_BlitSurface(pTextSurface, NULL, Surface, &rect);
    SDL_FreeSurface(pTextSurface);
}
