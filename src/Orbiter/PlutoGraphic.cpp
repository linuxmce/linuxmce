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
#include "PlutoGraphic.h"
#include "../PlutoUtils/FileUtils.h"
#include "OrbiterRenderer.h"
#include "Orbiter.h"
#include "../Gen_Devices/AllCommandsRequests.h"
//=======================================================================================================
//Generic PlutoGraphic class methods
//-------------------------------------------------------------------------------------------------------
PlutoGraphic::PlutoGraphic(): m_nReferenceCount(1)
{
	Initialize();

	m_nWidth = 0;
	m_nHeight = 0;
}
//-------------------------------------------------------------------------------------------------------
PlutoGraphic::PlutoGraphic(OrbiterRenderer *pOrbiterRenderer): m_nReferenceCount(1)
{
	Initialize();

	m_pOrbiterRenderer = pOrbiterRenderer;
	m_nWidth = 0;
	m_nHeight = 0;
}

//-------------------------------------------------------------------------------------------------------
PlutoGraphic::PlutoGraphic(string Filename, eGraphicManagement GraphicManagement, 
						   OrbiterRenderer *pOrbiterRenderer): m_nReferenceCount(1)
{
	Initialize();

	m_pOrbiterRenderer = pOrbiterRenderer;
	m_Filename = Filename;
	m_GraphicManagement = GraphicManagement;
	m_nWidth = 0;
	m_nHeight = 0;
};
//-------------------------------------------------------------------------------------------------------
/*virtual*/ PlutoGraphic::~PlutoGraphic()
{
	delete[] m_pGraphicData;
}
//-------------------------------------------------------------------------------------------------------
/*virtual*/ void PlutoGraphic::AddRef()
{
	++m_nReferenceCount;
}
//-------------------------------------------------------------------------------------------------------
/*virtual*/ void PlutoGraphic::Release()
{
	if(0 == --m_nReferenceCount)
		delete this;
}
//-------------------------------------------------------------------------------------------------------
/*virtual*/ void PlutoGraphic::Initialize() 
{ 
	m_GraphicManagement = GR_KEEPCOMPRESSED; 
	m_GraphicFormat = GR_UNKNOWN; 
	m_Filename = "";
	m_pOrbiterRenderer = NULL;
	m_pGraphicData = NULL;
	m_GraphicLength = 0;
}
//-------------------------------------------------------------------------------------------------------

bool PlutoGraphic::LoadGraphicFile(const char *ImageFilename, int iRotation)
{
	m_pGraphicData = m_pOrbiterRenderer->m_pOrbiter->ReadFileIntoBuffer(ImageFilename, m_GraphicLength);
	if (m_GraphicLength == 0)
		return false;
	
	m_GraphicFormat = GR_JPG; // todo: get format by filename

	return LoadGraphic(m_pGraphicData, m_GraphicLength, iRotation);
}
