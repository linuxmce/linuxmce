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
/**
 *
 * @file ImageDefs.h
 * @brief header file 
 * @author
 * @todo ask
 *
 */

#ifndef _IMAGE_DEFS_H_
#define _IMAGE_DEFS_H_

#include <map>
#include "ximajpg.h"
#include "ImageBaseDefs.h" 

using namespace std;

/** linking automatically with CXImage.lib static library */

#ifdef _DEBUG
	#pragma comment(lib, "../../libs/Libraries/CXImage/CXImage/Debug/cximage.lib")
	#pragma comment(lib, "../../libs/Libraries/CXImage/png/Debug/png.lib")
	#pragma comment(lib, "../../libs/Libraries/CXImage/zlib/Debug/zlib.lib")
#else
	#pragma comment(lib, "../../libs/Libraries/CXImage/CXImage/Release/cximage.lib")
	#pragma comment(lib, "../../libs/Libraries/CXImage/png/Release/png.lib")
	#pragma comment(lib, "../../libs/Libraries/CXImage/zlib/Release/zlib.lib")
#endif

/** size of every images in image list */
#define THUMBNAIL_HEIGHT		45	//45 px
#define THUMBNAIL_WIDTH			45	//45 px

#ifndef GRAPHICTYPE_BMP
#define GRAPHICTYPE_BMP			5
#endif

#define CUSTOMER_IMAGE_GAP		20	// 20 px

/** @test extern CImageList		PlutoVIPImageList; */

typedef struct tagPlutoCustomer
{
	int ImageType;
	int ImageNumber;				/**< image number in ImageList */
	BOOL DefaultBitmap;				/** TRUE=has no thumbnail, FALSE=has thumbnail */
	CTime Time;
} PlutoCustomer;

extern map<unsigned long  /** @test PlutoID*/, PlutoCustomer*> mapPlutoImageList;

CString FindCustomerImageFileFromID(ULONG ID, int ImageType);

#define THUMBNAIL_IMAGE_DIRECTORY		_T("Images")
#endif //_IMAGE_DEFS_H_
