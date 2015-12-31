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
#include "GraphicBuilder.h"
#include "RendererMNG.h"
#include "DCE/Logger.h"

#if defined(POCKETFROG)
	#include "PocketFrog/PocketFrogGraphic.h"
#elif defined(ORBITER_OPENGL)	
	#include "OpenGL/OpenGLGraphic.h"
#else
	#include "SDL/SDLGraphic.h"
#endif
//-------------------------------------------------------------------------------------------------------
bool IsMNG(string sFileName)
{
	string sExtension = FileUtils::FindExtension(sFileName);

	return "MNG" == sExtension || "mng" == sExtension;
}
//-------------------------------------------------------------------------------------------------------
bool IsPNG(string sFileName)
{
	string sExtension = FileUtils::FindExtension(sFileName);

	return "PNG" == sExtension || "png" == sExtension;
}
//-------------------------------------------------------------------------------------------------------
bool IsOCG(string sFileName)
{
	string sExtension = FileUtils::FindExtension(sFileName);

	return "OCG" == sExtension || "ocg" == sExtension;
}
//-------------------------------------------------------------------------------------------------------
bool IsPFG(string sFileName)
{
	string sExtension = FileUtils::FindExtension(sFileName);

	return "PFG" == sExtension || "pfg" == sExtension;
}

PlutoGraphic *CreatePlutoGraphic(GraphicType Type, string Filename, 
						 eGraphicManagement GraphicManagement, OrbiterRenderer *pOrbiterRenderer)
{

	eGraphicFormat eGF = GR_UNKNOWN;

	if(IsMNG(Filename))
		eGF = GR_MNG;
	else if(IsPNG(Filename))
		eGF = GR_PNG;
	else if(IsOCG(Filename))
		eGF = GR_OCG;

	PlutoGraphic *pGraphic
#if defined(POCKETFROG)
		= new PocketFrogGraphic(pOrbiterRenderer);
#elif defined(ORBITER_OPENGL)
		= new OpenGLGraphic(pOrbiterRenderer);
#else
		= new SDLGraphic(pOrbiterRenderer); //we won't load the graphic yet
#endif
	if (pGraphic!=NULL)
	{
		pGraphic->m_GraphicFormat = eGF;
		pGraphic->m_GraphicManagement = GraphicManagement;
		pGraphic->m_Filename = Filename;
	}	
	return pGraphic;
}

//-------------------------------------------------------------------------------------------------------
void CreateVectorGraphic(VectorPlutoGraphic& vectPlutoGraphic, GraphicType Type, string Filename, 
						 eGraphicManagement GraphicManagement, OrbiterRenderer *pOrbiterRenderer)
{
	PlutoGraphic *pGraphic = CreatePlutoGraphic(Type, Filename, GraphicManagement, pOrbiterRenderer); 

	if(NULL != pGraphic)
	{
		vectPlutoGraphic.push_back(pGraphic);
	}
}
//-------------------------------------------------------------------------------------------------------
