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
#ifndef RENDERER_H
#define RENDERER_H
//---------------------------------------------------------------------------------------------------
#include <string>
using namespace std;
#include "SerializeClass/ShapesColors.h"
//---------------------------------------------------------------------------------------------------
struct SDL_RWops;
struct SDL_Surface;
//---------------------------------------------------------------------------------------------------
class RendererImage;
class DesignObj_Generator;
class RendererMNG;
class DesignObjText;
class TextStyle;
//---------------------------------------------------------------------------------------------------
class TextObj
{
    public:
        enum AlignFlags {Left = 0, Center = 1, Right = 2, Top = 4, Middle = 8, Bottom = 16};

        PlutoPoint position;
        string text;
        bool bold, italic, underline;
        AlignFlags Align;

        TextObj() : position(0, 0), text(""), bold(false), italic(false), underline(false) {}
};
//---------------------------------------------------------------------------------------------------
class Renderer
{
private:

	Renderer();
	~Renderer();

	string m_sFontPath,m_sOutputDirectory;
	int m_Width,m_Height;
	int m_Rotate;
	SDL_Surface *m_pScreen;

	// This comes from the Size table,ScaleMenuBg & ScaleOtherGraphics.  For both, options are: 'Y', make the Y axis fit, crop the X, 'X' is opposite
	// 'S', means scale according to the ScaleX and ScaleY, which come from the table
	// 'F', means stretch the image so it fits
	char m_cDefaultScaleForMenuBackground,m_cDefaultScaleForOtherGraphics;
	float m_fScaleX,m_fScaleY;
	bool m_bUseAlphaBlending, m_bCreateMask;

	static Renderer m_Instance;

	//helper function
	bool SaveSurfaceToXbmMaskFile(SDL_Surface *pSurface, int nMaxOpacity, const string &sFileName);

public:

	static Renderer &GetInstance() { return m_Instance; }

	//setup function
	void Setup(string FontPath, string OutputDirectory, int Width,int Height, 
		bool bUseAlphaBlending, char cDefaultScaleForMenuBackground, char cDefaultScaleForOtherGraphics,
		float fScaleX, float fScaleY, int Rotate, bool bCreateMask);

	//rendering methods
	void RenderObject(RendererImage *pRenderImage, DesignObj_Generator *pDesignObj_Generator,
		PlutoPoint Position, int iRenderStandard, int iOnlyVersion = -999);
	void RenderObjectsChildren(RendererImage *pRenderImage, DesignObj_Generator *pDesignObj_Generator, 
		PlutoPoint pos,int iOnlyVersion);
	void RenderObjectsText(RendererImage *pRenderImage, DesignObj_Generator *pDesignObj_Generator, 
		PlutoPoint pos, int iIteration);

	//helpers
	void SaveImageToFile(RendererImage * pRendererImage, string sSaveToFile, bool bUseOCG=false);
	void SaveImageToPNGFile(RendererImage * pRendererImage, FILE * File, string sFilename, bool Signature = true);
	void SaveMNGToFile(string FileName, RendererMNG * MNG);

	void RenderText(RendererImage *pRenderImage, DesignObjText *pDesignObjText, TextStyle *pTextStyle, DesignObj_Generator *pDesignObj_Generator, PlutoPoint pos);

	RendererImage *Subset(RendererImage *pRenderImage,PlutoRectangle rect);
	RendererMNG * CreateMNGFromFile(string FileName, PlutoSize Size);
	RendererMNG * CreateMNGFromFiles(const vector<string> & FileNames, PlutoSize Size);
};
//---------------------------------------------------------------------------------------------------
#endif

