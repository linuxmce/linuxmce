/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//
// C++ Interface: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef JPEGENCODERDECODER_H
#define JPEGENCODERDECODER_H

/**
*/
class JpegEncoderDecoder
{
private:
	unsigned char *workingBuffer;
	int workingBufferSize;
	int finalBufferSize;

public:
    JpegEncoderDecoder();

    ~JpegEncoderDecoder();

	bool encodeIntoBuffer(char *buffer, int width, int height, int bytesPerPixel, char* &compressedBuffer, int &compressedSize);

	bool allocateBuffer(int width, int height, int bpp);

	unsigned char *getAllocatedBuffer();

	int getBufferSize();

	void setCompressedSize(int finalSize);
};

#endif
