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
#ifndef PLUTOGRAPHIC_H
#define PLUTOGRAPHIC_H

#define GRAPHIC_NORMAL		 0
#define GRAPHIC_SELECTED	-1
#define GRAPHIC_HIGHLIGHTED	-2
//-------------------------------------------------------------------------------------------------------
#include <algorithm>
#include <string>
#include <list>
#include <vector>
using namespace std;
//-------------------------------------------------------------------------------------------------------
#include "PlutoUtils/GraphicFormat.h"
//-------------------------------------------------------------------------------------------------------
// Graphic memory management.  GR_DYNAMIC means that this is a dynamic image
// and there's no reason to ever keep it.  Discardonchange may keep the image
// if there's enough system memory. 
//-------------------------------------------------------------------------------------------------------
enum eGraphicManagement { GR_KEEPUNCOMPRESSED, GR_KEEPCOMPRESSED, GR_DISCARDONCHANGE, GR_DYNAMIC, GR_STORAGE };

// Windows-style RGB management
//-------------------------------------------------------------------------------------------------------
#ifndef RGB
	#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

	#ifndef _VC80_UPGRADE
		#define GetRValue(rgb)      ((BYTE)(rgb))
		#define GetGValue(rgb)      ((BYTE)(((WORD)(rgb)) >> 8))
		#ifndef ARMV4I
			#define GetBValue(rgb)      ((BYTE)((rgb)>>16))
		#endif
	#endif

#endif
//-------------------------------------------------------------------------------------------------------
enum GraphicType
{
	gtSDLGraphic,
	gtPocketFrogGraphic,
	gtOpenGLGraphic
};
//-------------------------------------------------------------------------------------------------------
class OrbiterRenderer;
//-------------------------------------------------------------------------------------------------------
#define RELEASE_GRAPHIC(x) { if(NULL != (x)) { (x)->Release(); (x) = NULL; } }
//-------------------------------------------------------------------------------------------------------
class PlutoGraphic
{
public:
	PlutoGraphic();
	PlutoGraphic(OrbiterRenderer *pOrbiterRenderer);
	PlutoGraphic(string Filename, eGraphicManagement GraphicManagement, OrbiterRenderer *pOrbiterRenderer);

	//reference counting methods
	virtual void AddRef();
	virtual void Release();

	virtual PlutoGraphic* Clone() = 0; //Must be implemented

	OrbiterRenderer *m_pOrbiterRenderer;

	virtual void Initialize();
	virtual GraphicType GraphicType_get()=0;  // Must be implemented
	virtual bool IsEmpty()=0; //Must be implemented
	virtual bool LoadGraphic(char *pData, size_t iSize,int iRotation=0) = 0; //Must be implemented
	virtual bool LoadGraphicFile(const char *ImageFilename, int iRotation=0);
	virtual void Clear() = 0; //Must be implemented
	virtual PlutoGraphic *GetHighlightedVersion() { return NULL; } // Return the same object in a highlighted state
	virtual bool GetInMemoryBitmap(char*& pRawBitmapData, size_t& ulSize) = 0;
	
	enum eGraphicManagement m_GraphicManagement;
	enum eGraphicFormat m_GraphicFormat;

	char *m_pGraphicData;
	size_t m_GraphicLength;

	int m_nWidth, m_nHeight;
	string m_Filename;

protected:

	virtual ~PlutoGraphic();

private:

	int m_nReferenceCount;
};
//-------------------------------------------------------------------------------------------------------
typedef list<PlutoGraphic*> PlutoGraphicList, AsyncLoadList;
typedef vector<PlutoGraphic *> VectorPlutoGraphic;
//-------------------------------------------------------------------------------------------------------
#endif
