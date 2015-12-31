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
#include "Renderer.h"

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include "SDL_rotozoom.h"
#include <SDL_image.h>
#include <sge.h>
#include <SDL_ttf.h>
#include <png.h>
#include <math.h>

#include "SDL_Helpers/SDL_Defs.h"
#include "SDL_Helpers/SDL_RenderUtils.h"
#include "SDL_Helpers/SDL_Helpers.h"
#include "SDL_Helpers/RendererImage.h"

#include "Orbiter/TextStyle.h"
#include "Orbiter/SDL/SDLRendererOCGHelper.h"
#include "Orbiter/RendererOCG.h"
#include "Orbiter/RendererMNG.h"
#include "DesignObj_Generator.h"
#include "Splitter/TextWrapper.h"

#include "pluto_main/Define_HorizAlignment.h"
#include "pluto_main/Define_VertAlignment.h"

#include "../utilities/linux/wrapper/image_file.h"
//---------------------------------------------------------------------------------------------------
#include "DCE/Logger.h"
using namespace DCE;
//---------------------------------------------------------------------------------------------------
Renderer Renderer::m_Instance; //the one and only
//---------------------------------------------------------------------------------------------------
Renderer::Renderer() :
	m_Width(0), m_Height(0), m_Rotate(0), m_pScreen(NULL),
	m_cDefaultScaleForMenuBackground(0), m_cDefaultScaleForOtherGraphics(0),
	m_fScaleX(1.0f), m_fScaleY(1.0f),
	m_bUseAlphaBlending(false)
{
}
//---------------------------------------------------------------------------------------------------
void Renderer::Setup(string FontPath, string OutputDirectory, int Width, int Height, 
	bool bUseAlphaBlending, char cDefaultScaleForMenuBackground, char cDefaultScaleForOtherGraphics,
	float fScaleX, float fScaleY, int Rotate, bool bCreateMask)
{
#ifndef WIN32
	setenv("SDL_VIDEODRIVER", "dummy", 1); 
#endif

    m_sFontPath=FontPath;
    m_sOutputDirectory=OutputDirectory;
    m_Width=Width;
    m_Height=Height;
	m_bUseAlphaBlending=bUseAlphaBlending;

	m_cDefaultScaleForMenuBackground=cDefaultScaleForMenuBackground;
	m_cDefaultScaleForOtherGraphics=cDefaultScaleForOtherGraphics;
	m_fScaleX = fScaleX;
	m_fScaleY = fScaleY;
	m_bCreateMask = bCreateMask;

	if(Rotate)
		m_Rotate = 360 - Rotate;  // SDL treats rotation as counter clockwise, we do clockwise
	else
		m_Rotate = 0;

	if (!(SDL_WasInit(SDL_INIT_VIDEO) == SDL_INIT_VIDEO))
	{
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) == -1)
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed initializing SDL: %s",SDL_GetError());

#ifndef WIN32 //linux
    	    string sCmd = "/usr/pluto/bin/Start_X.sh; /usr/pluto/bin/Start_ratpoison.sh";
        	printf("X is not running! Starting X and ratpoison: %s\n", sCmd.c_str());
        	system(sCmd.c_str());
#endif //linux

			exit(1);
		}
		atexit(SDL_Quit);
	}

	if (TTF_Init() == -1)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to init SDL TTF: %s",TTF_GetError());
		return;
	}
}
//---------------------------------------------------------------------------------------------------
Renderer::~Renderer()
{
    TTF_Quit();
	SDL_Quit();
}
//---------------------------------------------------------------------------------------------------
// This takes an incoming pRenderImage of what's been rendered so far, and will add this new object and it's
// children to it, or save them separately if set to can hide, or if rendering a selected, highlighted or alt versions
// If iRenderStandard==1, render the standard version only, if ==0, everything but the standard, if -1, do all

// When we start rendering a non-standard version (selected, highlighted, etc.), we will render all of it's children
// in the same state.  Therefore we also have an "OnlyVersion" version flag.  This means only render the given version.
// A standard version that has non-standard variations, will also want all of it's children to render only the standard version
void Renderer::RenderObject(RendererImage *pRenderImage, DesignObj_Generator *pDesignObj_Generator,
	PlutoPoint Position, int iRenderStandard, int iOnlyVersion)
{
	bool bPreserveAspectRatio = pDesignObj_Generator->m_bPreserveAspectRatio;
    bool bPreserveTransparencies = pDesignObj_Generator->m_bPreserveTransparencies;

	if( pDesignObj_Generator->m_rPosition.Width==0 || pDesignObj_Generator->m_rPosition.Height==0 )
		return; // Nothing to render anyway

    // We have versions for standard, highlighted, selected, and a variable number of alternate versions
    // We'll make a loop that starts from the number of alt versions, counting down to -2, so when it's at
    // 0, it renders the selected, -1 the highlighted, -2 the standard.  Count down because the standard
    // version should be rendered last, since that's the only one that will be composited on top of the background.
    // The others are all output as separate files.  The first time RenderObject is called, pRenderImage is null
    int StartingValue = iRenderStandard!=1 ? (int) pDesignObj_Generator->m_vectOrigAltGraphicFilename.size() : -2;
    int EndingValue = iRenderStandard!=0 ? -2 : -1;
	if( iOnlyVersion!=-999 )
		StartingValue=EndingValue=iOnlyVersion;

    for(int iIteration=StartingValue;iIteration>=EndingValue;--iIteration)
    {
        // Normally we don't create output files for selected/highlighted/alt versions unless there is a corresponding graphic.  There's really nothing
        // to display otherwise.  However, if the user forced this version with the 'visible state' setting, then we should output that state anyway.  Perhaps
        // it only contains text that should be visible in that state
        bool bForceOutput=false;
        string sInputFile=""; // The graphics file we're using this pass
        string sSaveToFile=""; // If this is set to a value, then we will save the current image to a separate file

        // Create another pointer to the render image, because if we create a new image to be saved
        // in the above file, we want the pointer to return to the original one when this loop completes
		// This shadows the pRenderImage passed in
        RendererImage *pRenderImageOriginal=pRenderImage;
        RendererImage *pRenderImage=pRenderImageOriginal; // TROUBLE nr 1: subsequent loops overwrite pRenderImage allocated in loop a few line below this
        PlutoPoint PositionOriginal=Position; // Ditto with the position
        PlutoPoint Position=PositionOriginal;

        if( iIteration==-2 )
            sInputFile = pDesignObj_Generator->m_sOrigBackgroundFile;
        else if( iIteration==-1 )
            sInputFile = pDesignObj_Generator->m_sOrigSelectedFile;
        else if( iIteration==0 )
            sInputFile = pDesignObj_Generator->m_sOrigHighlightGraphicFilename;
        else
            sInputFile = pDesignObj_Generator->m_vectOrigAltGraphicFilename[iIteration-1];

        // If the visible state is specified, skip this screen if it's not specifically in the list
        if( pDesignObj_Generator->m_sVisibleState.length()>0 && pDesignObj_Generator->m_sVisibleState!="T" && (
                (iIteration==-2 && pDesignObj_Generator->m_sVisibleState.find("N")==string::npos) ||
                (iIteration==-1 && pDesignObj_Generator->m_sVisibleState.find("S")==string::npos) ||
                (iIteration==0 && pDesignObj_Generator->m_sVisibleState.find("H")==string::npos) ||
                (iIteration>0 && pDesignObj_Generator->m_sVisibleState.find(StringUtils::itos(iIteration))==string::npos) ) )
            continue;

        // If we're processing the background (-2) we will still render children
        if( sInputFile.length()==0 && iIteration!=-2 )
        {
            // We will render this anyway if the visible state is forced.  It may just be text, for example
            if( pDesignObj_Generator->m_sVisibleState.length()==0 ||
                    (iIteration==-2 && pDesignObj_Generator->m_sVisibleState.find("N")==string::npos) ||
                    (iIteration==-1 && pDesignObj_Generator->m_sVisibleState.find("S")==string::npos) ||
                    (iIteration==0 && pDesignObj_Generator->m_sVisibleState.find("H")==string::npos) ||
                    (iIteration>0 && pDesignObj_Generator->m_sVisibleState.find(StringUtils::itos(iIteration))==string::npos) )
                continue;
            else
                bForceOutput=true;
        }

        bool bIsMenu=false;
		// If iOnlyVersion is -999, then we've already hit an object with non-standard versions, so we're not
		// going to write out children as separate objects anymore
		/* AB 4-20-2007 -- Argh, another issue.  When iOnlyVersion==-999 is there, then on the tv channel grid, where the cell object
		5197 now has selected and highlighted states, the children which can be hidden (favorites check icon) are always merged in. */
        if( /* iOnlyVersion==-999 && */ (iIteration!=-2 || pDesignObj_Generator->m_bCanBeHidden || !pRenderImage ||
			pDesignObj_Generator->m_vectOrigAltGraphicFilename.size() || pDesignObj_Generator->m_sOrigHighlightGraphicFilename.size() || pDesignObj_Generator->m_sOrigSelectedFile.size()) )
        {
            // We're going to save this out as a separate file
            sSaveToFile=pDesignObj_Generator->m_ObjectID;
            if( !pRenderImage )
            {
                // This is a new screen or a pop-up, so start with a clean canvas
				if( pDesignObj_Generator->m_bIsPopup && pDesignObj_Generator->m_rPosition.Width && pDesignObj_Generator->m_rPosition.Height )
	                pRenderImage = CreateBlankCanvas(PlutoSize(pDesignObj_Generator->m_rPosition.Width,pDesignObj_Generator->m_rPosition.Height), sInputFile.length() == 0 && ! pDesignObj_Generator->m_bIsPopup);
				else
	                pRenderImage = CreateBlankCanvas(PlutoSize(m_Width,m_Height), sInputFile.length() == 0); // TROUBLE nr 1: this pointer is lost each time the for loop starts
                bIsMenu=true;
            }
            else
            {
                // This is going to be a "piece" of the existing background
                pRenderImage=Subset(pRenderImage,pDesignObj_Generator->m_rPosition + Position);
                // All the child's objects will have absolute values, so the offset position if we're outputing to a separate file is our absolute position * -1
                Position.X=pDesignObj_Generator->m_rPosition.X*-1; Position.Y=pDesignObj_Generator->m_rPosition.Y*-1;
            }

            // Append to the filename based on what graphic we're outputing
            if( iIteration==-1 )
                sSaveToFile += "_S";
            else if( iIteration==0 )
                sSaveToFile += "_H";
            else if( iIteration>0 )
                sSaveToFile += "_A" + StringUtils::itos(iIteration);
        }

        RendererImage *pRenderImage_Child=NULL; // The new image to be pasted on top
		RendererMNG *pRendererMNG=NULL;
        if( sInputFile.length()>0 )
        {
			if( StringUtils::ToUpper(FileUtils::FindExtension(sInputFile))=="MNG" )
			{
				pRendererMNG = CreateMNGFromFile(sInputFile,pDesignObj_Generator->m_rBackgroundPosition.Size());
				string::size_type pos = sInputFile.rfind('/');
			}
			else
			{
				pRenderImage_Child = CreateFromFile(sInputFile, m_fScaleX, m_fScaleY, pDesignObj_Generator->m_rBackgroundPosition.Size(),bPreserveAspectRatio,false,bIsMenu ? m_cDefaultScaleForMenuBackground : m_cDefaultScaleForOtherGraphics, pDesignObj_Generator->m_rBitmapOffset,!pDesignObj_Generator->m_bContainsFloorplans);
			}

            if( !pRenderImage_Child && !pRendererMNG )
            {
                throw "Failed to open " + sInputFile + " abandoning creation of object " + pDesignObj_Generator->m_ObjectID;
            }

			if( pDesignObj_Generator->m_sVisibleState.find('T')!=string::npos )
			{
				int X = pDesignObj_Generator->m_rBackgroundPosition.Width;
				int Y = pDesignObj_Generator->m_rBackgroundPosition.Height;
				StringUtils::Replace(&pDesignObj_Generator->m_sVisibleState,"T","");
				int LineWidth = X / 8 + 1; // The width of each line
				unsigned char *pHitTest = new unsigned char[LineWidth * Y];
				pDesignObj_Generator->m_dbHitTest.m_dwSize = LineWidth * Y;
				pDesignObj_Generator->m_dbHitTest.m_pBlock = (char *) pHitTest;
				memset(pHitTest,255,LineWidth * Y);

				for(int y=0;y<Y;++y)
				{
					for(int x=0;x<X;++x)
					{
						Uint32 Pixel = getpixel(pRenderImage_Child->m_pSurface, x, y);
						unsigned char *pPixel = (unsigned char *) &Pixel;
						if ( pPixel[3]<128 )
						{
							unsigned char c = pHitTest[ y * LineWidth + x/8 ];
							c ^= 1 << (x%8);
							pHitTest[ y * LineWidth + x/8 ] = c;
						}
					}
				}
			}
        }
		else if( pRenderImageOriginal==NULL && iIteration==-2 )
		{
			if (! pDesignObj_Generator->m_bIsPopup)
			{
				// This is a top-level object (ie the background for a screen), but there is no image
				SDL_FillRect(pRenderImage->m_pSurface, NULL,SDL_MapRGBA(pRenderImage->m_pSurface->format, 0, 0, 0, 255));
			}
			else
			{
				// This is a popup. The is no background image, but it has to be transparent
				SDL_FillRect(pRenderImage->m_pSurface, NULL,SDL_MapRGBA(pRenderImage->m_pSurface->format, 0, 0, 0, 0));
			}
			pRenderImage->NewSurface = false;
		}

		int iOnlyVersion_Children=-999;  // By default render all of our children
		if( iOnlyVersion!=-999 )
			iOnlyVersion_Children = iOnlyVersion;
		else if( iIteration!=-2 || pDesignObj_Generator->m_sOrigHighlightGraphicFilename.size() ||
			pDesignObj_Generator->m_sOrigSelectedFile.size() || pDesignObj_Generator->m_vectOrigAltGraphicFilename.size() )
		{
			iOnlyVersion_Children = iIteration;
		}

		for(size_t iFrame=0; iFrame < (pRendererMNG ? pRendererMNG->count() : 1); iFrame++)
		{
			// If this is multi-frame, we have to repeat, so we'll need a copy of the image
			RendererImage *pRenderImageClone;
			RendererImage *pRenderImageClone_Child;
			if( pRendererMNG )
			{
				pRenderImageClone = DuplicateImage(pRenderImage);
				pRenderImageClone_Child = pRendererMNG->GetFrame(iFrame);
			}
			else
			{
				pRenderImageClone = pRenderImage;
				pRenderImageClone_Child = pRenderImage_Child;
			}

			// See if we're supposed to render our children first
			if( pDesignObj_Generator->m_bChildrenBehind )
			{
				RenderObjectsChildren(pRenderImageClone,pDesignObj_Generator,Position,iOnlyVersion_Children);
				
				if(pRenderImageClone_Child )
					CompositeImage(pRenderImageClone,pRenderImageClone_Child,pDesignObj_Generator->m_rPosition.Location() + Position);

				RenderObjectsText(pRenderImageClone,pDesignObj_Generator,Position,iIteration);
			}
			else // Render our own first
			{
				if(pRenderImageClone_Child )
				{
					CompositeImage(pRenderImageClone,pRenderImageClone_Child,pDesignObj_Generator->m_rPosition.Location() + Position);
				}
				if( pDesignObj_Generator->m_bChildrenBeforeText )
				{
					RenderObjectsChildren(pRenderImageClone,pDesignObj_Generator,Position,iOnlyVersion_Children);
					RenderObjectsText(pRenderImageClone,pDesignObj_Generator,Position,iIteration);
				}
				else
				{
					RenderObjectsText(pRenderImageClone,pDesignObj_Generator,Position,iIteration);
					RenderObjectsChildren(pRenderImageClone,pDesignObj_Generator,Position,iOnlyVersion_Children);
				}
			}
			if( pRendererMNG )
				pRendererMNG->ReplaceFrame(iFrame, pRenderImageClone);
		}

        // If this is a screen (ie top level object) then we should always save something even
        // if there was no input file.
        if( sInputFile.length() || pRenderImageOriginal==NULL || pDesignObj_Generator->m_bCanBeHidden || bForceOutput )
        {
            if( sSaveToFile.length()>0 )
            {
                if(bPreserveTransparencies)
                    SetTransparentColor(pRenderImage->m_pSurface, 0, 0, 0);

				if( pRendererMNG )
					SaveMNGToFile(m_sOutputDirectory + sSaveToFile,pRendererMNG);
				else
	                SaveImageToFile(pRenderImage,m_sOutputDirectory + sSaveToFile,pDesignObj_Generator->m_bUseOCG);
#ifdef OUTPUT_BMP
                sSaveToFile+=".bmp";
#else
				if( pRendererMNG )
	                sSaveToFile+=".mng";
				else if( pDesignObj_Generator->m_bUseOCG )
					sSaveToFile+=".ocg";
				else
	                sSaveToFile+=".png";
#endif
            }

            if( iIteration==-2 )
                pDesignObj_Generator->m_sBackgroundFile = sSaveToFile;
            else if( iIteration==-1 )
                pDesignObj_Generator->m_sSelectedFile = sSaveToFile;
            else if( iIteration==0 )
                pDesignObj_Generator->m_sHighlightGraphicFilename = sSaveToFile;
            else
			{
				// Pad the vector so we can do the assignment
					if( (int)pDesignObj_Generator->m_vectAltGraphicFilename.size()<iIteration )
					for(size_t s = pDesignObj_Generator->m_vectAltGraphicFilename.size(); (int)s<iIteration; ++s)
						pDesignObj_Generator->m_vectAltGraphicFilename.push_back("");

                pDesignObj_Generator->m_vectAltGraphicFilename[iIteration-1] = sSaveToFile;
			}
        }
        delete pRenderImage_Child;
        delete pRendererMNG;
        if( pRenderImageOriginal!=pRenderImage )
            delete pRenderImage;
    }
    //  cout << "Finished Rendering " << pDesignObj_Generator->m_ObjectID << endl;
}
//---------------------------------------------------------------------------------------------------
void Renderer::RenderObjectsChildren(RendererImage *pRenderImage, DesignObj_Generator *pDesignObj_Generator,
									 PlutoPoint pos, int iOnlyVersion)
{
    // We don't support layering.  This isn't normally a problem for pre-rendered graphics.  However, for objects that are rendered
    // seperately, like the selected and highlighted versions, if they have transparency, it's possible that other child objects
    // that should be rendered aren't rendered yet.  Rather than making layers and complicating things, we'll just do 2 passes
    // and render all the objects standard variations without CanHide, and then do a second pass for the others.
    DesignObj_DataList::iterator it;
    for(it=pDesignObj_Generator->m_ChildObjects.begin();it!=pDesignObj_Generator->m_ChildObjects.end();++it)
    {
        DesignObj_Generator *pObj = (DesignObj_Generator *) *it;
		if( iOnlyVersion!=-999 )
		{
	        RenderObject(pRenderImage,(DesignObj_Generator *) pObj,pos,0,iOnlyVersion);  // Standard only
		    continue;
		}

        if( pObj->m_bCanBeHidden || (pObj->m_sVisibleState.length()>0 && pObj->m_sVisibleState.find('N')==string::npos) )
            continue;
        RenderObject(pRenderImage,(DesignObj_Generator *) pObj,pos,1);  // Standard only
        pObj->m_bRendered=true;
    }

	if( iOnlyVersion!=-999 )
		return; // We already rendered all the children

    // Now do the others
    for(it=pDesignObj_Generator->m_ChildObjects.begin();it!=pDesignObj_Generator->m_ChildObjects.end();++it)
    {
        DesignObj_Generator *pObj = (DesignObj_Generator *) *it;
        RenderObject(pRenderImage,(DesignObj_Generator *) pObj,pos,pObj->m_bRendered ? 0 : -1);
    }
}
//---------------------------------------------------------------------------------------------------
void Renderer::RenderObjectsText(RendererImage *pRenderImage, DesignObj_Generator *pDesignObj_Generator, 
	PlutoPoint pos, int iIteration)
{
    for(size_t s=0;s<pDesignObj_Generator->m_vectDesignObjText.size();++s)
    {
        DesignObjText *pDesignObjText = pDesignObj_Generator->m_vectDesignObjText[s];
        if( !pDesignObjText->m_bPreRender )
            continue;

        // Find the style to use
        TextStyle *pTextStyle = pDesignObjText->m_mapTextStyle_Find(iIteration);
        if( !pTextStyle )
            pTextStyle = pDesignObjText->m_mapTextStyle_Find(0);  // The standard one

        if( !pTextStyle )
            throw "Cannot find a style to use for text " + StringUtils::itos(pDesignObjText->m_PK_Text);

        RenderText(pRenderImage,pDesignObjText,pTextStyle,pDesignObj_Generator,pos);
    }
}
//---------------------------------------------------------------------------------------------------
void Renderer::SaveImageToPNGFile(RendererImage * pRendererImage, FILE * File, string sFilename, bool Signature)
{
    // we can't just save NULL pointers...
    if (pRendererImage == NULL || pRendererImage->m_pSurface == NULL)
    {
        throw "Save to image passed in a null pointer";
    }

	SDL_Surface *pSDL_Surface = pRendererImage->m_pSurface;

	if (!m_bUseAlphaBlending)
	{
		if (m_bCreateMask)
			SaveSurfaceToXbmMaskFile(pSDL_Surface, 0, sFilename + ".mask.xbm");

		SetGeneralSurfaceOpacity(pSDL_Surface, SDL_ALPHA_OPAQUE); // remove all transparency from the surface
	}
    
	if( m_Rotate )
		pSDL_Surface = rotozoomSurface (pRendererImage->m_pSurface, m_Rotate, 1,0);

	IMG_SavePNG(pSDL_Surface, File, Signature);

	if( m_Rotate )
	    SDL_FreeSurface(pSDL_Surface);
}
//---------------------------------------------------------------------------------------------------
void Renderer::SaveImageToFile(RendererImage * pRendererImage, string sSaveToFile, bool bUseOCG)
{
    // we can't just save NULL pointers...
    if (pRendererImage == NULL || pRendererImage->m_pSurface == NULL)
    {
        throw "Save to image passed in a null pointer";
    }

#ifdef OUTPUT_BMP

    string FileName = sSaveToFile + ".bmp";
	SDL_Surface *pSDL_Surface = pRendererImage->m_pSurface;
	if( m_Rotate )
		pSDL_Surface = rotozoomSurface (pRendererImage->m_pSurface, m_Rotate, 1,0);
    if (SDL_SaveBMP(pSDL_Surface, FileName.c_str()) == -1)
    {
        throw "Failed to write file: " + FileName + ": " + SDL_GetError();
    }
	if( m_Rotate )
	    SDL_FreeSurface(pSDL_Surface);
#else

	if( bUseOCG )
	{
		string FileName = sSaveToFile + ".ocg";

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Saving %s",FileName.c_str());
		SDL_Surface *pSDL_Surface = pRendererImage->m_pSurface;
		if( m_Rotate )
			pSDL_Surface = rotozoomSurface (pRendererImage->m_pSurface, m_Rotate, 1,0);
		SDL_SaveOCG(pSDL_Surface, FileName);
		if( m_Rotate )
			SDL_FreeSurface(pSDL_Surface);
	}
	else
	{
		string FileName = sSaveToFile + ".png";

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Saving %s",FileName.c_str());
		FILE * File = fopen(FileName.c_str(), "wb");
		SaveImageToPNGFile(pRendererImage, File, FileName);
		fclose(File);
	}

#endif
}
//---------------------------------------------------------------------------------------------------
RendererMNG * Renderer::CreateMNGFromFiles(const vector<string> & FileNames, PlutoSize Size)
{
	MNGHeader Header;
	int kounter = 0;

	RendererMNG * Result = new RendererMNG;

	for (size_t i = 0; i < FileNames.size(); i++)
	{
		RendererImage * pRendererImage = CreateFromFile(FileNames[i], m_fScaleX, m_fScaleY, Size);
		Result->AppendFrame(pRendererImage);
	}

	Header.ticks_per_second = 1000;
	Header.frame_height = Size.Height;
	Header.frame_width = Size.Width;
	Result->SetHeader(Header);
	return Result;
}
//---------------------------------------------------------------------------------------------------
RendererMNG * Renderer::CreateMNGFromFile(string FileName, PlutoSize Size)
{
	FILE * File;
	int RealW, RealH;
	MNGHeader Header;
	int kounter = 0;

	File = fopen(FileName.c_str(), "rb");
	if (! File)
		return NULL;

	char buffer[1024];
	fread(buffer, 1, 8, File); // strip signature

	PNGChunk * chunk;
	bool BuildingPNG = false;
	PNGCatChunks * InMemoryPNG = new PNGCatChunks;

	RendererMNG * Result = new RendererMNG;
	while (ReadChunk(File, chunk = new PNGChunk))
	{
		if (chunk->type == "MHDR")
		{
			ParseMNGHeader(chunk->data, &Header);
			Result->SetHeader(Header);
			RealW = Size.Width == 0 ? Header.frame_width : Size.Width;
			RealH = Size.Height == 0 ? Header.frame_height : Size.Height;
		}

		if (chunk->type == "IHDR")
		{
			BuildingPNG = true;
		}

		if (BuildingPNG)
		{
			InMemoryPNG->AddChunk(chunk);
			if (chunk->type == "IEND")
			{
				BuildingPNG = false;
				char * pGraphicFile;
				size_t iSizeGraphicFile = InMemoryPNG->CatChunks(pGraphicFile);

				SDL_RWops * rw = SDL_RWFromMem(pGraphicFile, (int)iSizeGraphicFile);
				RendererImage * pRendererImage = CreateFromRWops(rw, m_fScaleX, m_fScaleY, true, Size);

				delete [] pGraphicFile;

				Result->AppendFrame(pRendererImage);
				InMemoryPNG->clear();
			}
		}
		else
		{
			delete chunk;
		}
	}

	delete InMemoryPNG;

	fclose(File);

	Header = Result->GetHeader();
	Header.frame_height = RealH;
	Header.frame_width = RealW;
	Result->SetHeader(Header);
	return Result;
}
//---------------------------------------------------------------------------------------------------
void Renderer::SaveMNGToFile(string FileName, RendererMNG * MNG)
{
	FILE * File;

	File = fopen((FileName + ".mng").c_str(), "wb");

	fwrite(MNGsignature, 1, 8, File);
	char * Header;
	MNG->GetHeader().BinaryForm(Header);
	fwrite(Header, 1, 40, File);
	delete [] Header;

	for (size_t i = 0; i < MNG->count() ; i++)
	{
		SaveImageToPNGFile(MNG->GetFrame(i), File, FileName, false);
	}

	const char * MEND = "\x00\x00\x00\x00MEND\x21\x20\xF7\xD5";
	fwrite(MEND, 1, 12, File);

	fclose(File);
}
//---------------------------------------------------------------------------------------------------
void Renderer::RenderText(RendererImage *pRenderImage, DesignObjText *pDesignObjText, TextStyle *pTextStyle, DesignObj_Generator *pDesignObj_Generator, PlutoPoint pos)
{
    PlutoRectangle rect = pDesignObjText->m_rPosition + pos;

    WrapAndRenderText(pRenderImage->m_pSurface, pDesignObjText->m_sText, rect.X, rect.Y, rect.Width, rect.Height,
        m_sFontPath, pTextStyle,pDesignObjText->m_iPK_HorizAlignment,pDesignObjText->m_iPK_VertAlignment);
}
//---------------------------------------------------------------------------------------------------
// Extract a subsurface from a surface
RendererImage * Renderer::Subset(RendererImage *pRenderImage, PlutoRectangle rect)
{
    PlutoSize size(rect.Width, rect.Height);
    RendererImage * SubSurface = CreateBlankCanvas(size);

    if (SubSurface == NULL)
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to create sub-surface. Can't extract subset");
    }

    SDL_Rect SourceRect;
    SourceRect.x = rect.Left(); SourceRect.y = rect.Top();
    SourceRect.w = rect.Width; SourceRect.h = rect.Height;
    SDL_SetAlpha(pRenderImage->m_pSurface, 0, 0);
    SDL_BlitSurface(pRenderImage->m_pSurface, &SourceRect, SubSurface->m_pSurface, NULL);

    SubSurface->NewSurface = false;

    return SubSurface;
}
//---------------------------------------------------------------------------------------------------
bool Renderer::SaveSurfaceToXbmMaskFile(SDL_Surface *pSurface, int nMaxOpacity, const string &sFileName)
{
#ifndef WIN32
	typedef long int COORD_TYPE;
	int width = pSurface->w;
	int height = pSurface->h;
	// size of coordinates in the buffer
	// allocate the buffer
	char *pRawImage = new char[width * height];
	if (pRawImage == NULL)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "cannot allocate memory");
		return false;
	}
	// compute the image
	for(int x = 0; x < width; x++)
	{
		for(int y = 0; y < height; y++)
		{
			char *pD = (char *) pSurface->pixels + y * pSurface->pitch + x * 4;
			*(pRawImage + y*width + x) = (char)(pD[3] < nMaxOpacity);
		}
	}

	bool bResult = Xbm_WriteFile(sFileName, pRawImage, width, height);
	delete pRawImage;
	if (! bResult)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "cannot write to xbm mask file '%s'", sFileName.c_str());
		return false;
	}
#endif

	return true;
}
//---------------------------------------------------------------------------------------------------
