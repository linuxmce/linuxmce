/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef __ICON_DECODER_H__
#define __ICON_DECODER_H__

#include <string>
//--------------------------------------------------------------------------------------------------
char *DecodeIcon(std::string sRawImage, size_t& unIconSize);
//--------------------------------------------------------------------------------------------------
#endif //__ICON_DECODER_H__
