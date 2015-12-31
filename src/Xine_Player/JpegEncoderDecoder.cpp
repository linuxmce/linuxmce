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
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "JpegEncoderDecoder.h"

#include <stdlib.h>
#include <stdio.h>

#include <jpeglib.h>

#include <string.h>

JpegEncoderDecoder::JpegEncoderDecoder()
{

}

JpegEncoderDecoder::~JpegEncoderDecoder()
{

}

static void init_image_compression( j_compress_ptr cinfo )
{
	// printf("JpegEncoderDecoder:-:init_image_compression(): starting.\n");

	JpegEncoderDecoder *pJpegDecoder = (JpegEncoderDecoder*) cinfo->client_data;

	pJpegDecoder->allocateBuffer(cinfo->image_width, cinfo->image_height, cinfo->input_components);

	cinfo->dest->next_output_byte  = pJpegDecoder->getAllocatedBuffer();
	cinfo->dest->free_in_buffer    = pJpegDecoder->getBufferSize();
}

int clear_compression_buffer( j_compress_ptr cinfo )
{
	// printf("JpegEncoderDecoder:-:clear_compression_buffer(): Clearing.\n");
	JpegEncoderDecoder *pJpegDecoder = (JpegEncoderDecoder*) cinfo->client_data;

	cinfo->dest->next_output_byte = pJpegDecoder->getAllocatedBuffer();
  	cinfo->dest->free_in_buffer   = pJpegDecoder->getBufferSize();

	return TRUE;
}

void fini_image_compression( j_compress_ptr cinfo )
{
	// printf("JpegEncoderDecoder:-:fini_image_compression(): starting.\n");

	JpegEncoderDecoder *pJpegDecoder = (JpegEncoderDecoder*) cinfo->client_data;

	pJpegDecoder->setCompressedSize(cinfo->dest->next_output_byte - pJpegDecoder->getAllocatedBuffer());
}

bool JpegEncoderDecoder::encodeIntoBuffer(char *buffer, int width, int height, int bytesPerPixel, char* &compressedBuffer, int &compressedSize)
{
   /**
	* This struct contains the JPEG compression parameters and pointers to
	* working space (which is allocated as needed by the JPEG library).
	* It is possible to have several such structures, representing multiple
	* compression/decompression processes, in existence at once.  We refer
	* to any one struct (and its associated working data) as a "JPEG object".
	*/
	struct jpeg_compress_struct compress_info;

   /** This struct represents a JPEG error handler.  It is declared separately
	* because applications often want to supply a specialized error handler
	* (see the second half of this file for an example).  But here we just
	* take the easy way out and use the standard error handler, which will
	* print a message on stderr and call exit() if compression fails.
	* Note that this struct must live as long as the main JPEG parameter
	* struct, to avoid dangling-pointer problems.
	*/
	struct jpeg_error_mgr jerr;

	/* Step 1: allocate and initialize JPEG compression object */

	/* We have to set up the error handler first, in case the initialization
	* step fails.  (Unlikely, but it could happen if you are out of memory.)
	* This routine fills in the contents of struct jerr, and returns jerr's
	* address which we place into the link field in cinfo.
	*/
	compress_info.err = jpeg_std_error(&jerr);

	/* Now we can initialize the JPEG compression object. */
	jpeg_create_compress(&compress_info);

	jpeg_destination_mgr jpg_comp_dest_manager;

	jpg_comp_dest_manager.init_destination 		= init_image_compression;
	jpg_comp_dest_manager.empty_output_buffer 	= clear_compression_buffer;
	jpg_comp_dest_manager.term_destination 		= fini_image_compression;

	compress_info.dest = &jpg_comp_dest_manager;

	compress_info.image_width = width;
	compress_info.image_height = height;
	compress_info.input_components = bytesPerPixel;

	if ( bytesPerPixel == 1 )
  		compress_info.in_color_space = JCS_GRAYSCALE;
	else
		compress_info.in_color_space = JCS_RGB;

  	jpeg_set_defaults( &compress_info );
  	jpeg_set_quality( &compress_info, 80, TRUE );
	compress_info.client_data = this;

	// compression related data
 	JSAMPROW row_pointer[1];
  	int row_stride = bytesPerPixel;

  	jpeg_start_compress( &compress_info, TRUE );
	// printf("Starting compression\n");
  	while( compress_info.next_scanline < compress_info.image_height )
	{
		// printf("reading row: %d\n", compress_info.next_scanline);
    	row_pointer[0] = (JSAMPLE*)buffer + compress_info.next_scanline * row_stride * compress_info.image_width;
    	jpeg_write_scanlines( &compress_info, row_pointer, 1 );
  	}

	// printf("done compressing\n");
  	jpeg_finish_compress( &compress_info);

	// printf("Compressed size is: %d\n", finalBufferSize);

	compressedSize = finalBufferSize;
	delete [] compressedBuffer;
	compressedBuffer = new char[compressedSize];

	memcpy(compressedBuffer, workingBuffer, compressedSize);
	delete [] workingBuffer;
	workingBuffer = NULL;
	workingBufferSize = 0;
	return true;
}

bool JpegEncoderDecoder::allocateBuffer(int width, int height, int bpp)
{
	workingBufferSize = width * height * bpp;
	// printf("JpegEncoderDecoder::allocateBuffer(): allocating a %d buffer\n", workingBufferSize);
	workingBuffer = new unsigned char[workingBufferSize];
	return true;
}

unsigned char *JpegEncoderDecoder::getAllocatedBuffer()
{
	// printf("JpegEncoderDecoder::getAllocatedBuffer(): returning the buffer.\n");
	return workingBuffer;
}

int JpegEncoderDecoder::getBufferSize()
{
	// printf("JpegEncoderDecoder::getBufferSize(): returning the buffer size %d.\n", workingBufferSize);
	return workingBufferSize;
}

void JpegEncoderDecoder::setCompressedSize(int finalSize)
{
	finalBufferSize = finalSize;
}
