/*
 * File:	ximatif.cpp
 * Purpose:	Platform Independent TIFF Image Class Loader and Writer
 * 07/Aug/2001 <ing.davide.pizzolato@libero.it>
 * CxImage version 5.00 23/Aug/2002
 */

#include "ximatif.h"

#if CXIMAGE_SUPPORT_TIF

#include "../tiff/tiffio.h"

#define CVT(x)			(((x) * 255L) / ((1L<<16)-1))
#define	SCALE(x)		(((x)*((1L<<16)-1))/255)
#define CalculateLine(width,bitdepth)	(((width * bitdepth) + 7) / 8)
#define CalculatePitch(line)	(line + 3 & ~3)

extern "C" TIFF* TIFFOpenEx(CxFile* stream, const char* mode);

////////////////////////////////////////////////////////////////////////////////
bool CxImageTIF::Decode(CxFile * hFile)
{
	//Comment this line if you need more information on errors
	// TIFFSetErrorHandler(NULL);	//<Patrick Hoffmann>

	//Open file and fill the TIFF structure
	// m_tif = TIFFOpen(imageFileName,"rb");
	TIFF* m_tif = TIFFOpenEx(hFile, "rb");

	uint32 height=0;
	uint32 width=0;
	uint16 bitspersample=1;
	uint16 samplesperpixel=1;
	uint32 rowsperstrip=(DWORD)-1;
	uint16 photometric=0;
	uint16 compression=1;
	uint16 orientation=ORIENTATION_TOPLEFT; //<vho>
	uint32 x, y;
	float resolution, offset;
	BOOL isRGB;
	BYTE *bits;		//pointer to source data
	BYTE *bits2;	//pointer to destination data

  try{
	//check if it's a tiff file
	if (!m_tif)
		throw "Error encountered while opening TIFF file";

	info.nNumFrames=0;
	while(TIFFSetDirectory(m_tif,(uint16)info.nNumFrames)) info.nNumFrames++;

	if (!TIFFSetDirectory(m_tif, (uint16)info.nFrame))
		throw "Error: page not present in TIFF file";			

	//get image info
	TIFFGetField(m_tif, TIFFTAG_COMPRESSION, &compression);
//	if (compression == COMPRESSION_LZW)
//		throw "LZW compression is no longer supported due to Unisys patent enforcement";			

	TIFFGetField(m_tif, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(m_tif, TIFFTAG_IMAGELENGTH, &height);
	TIFFGetField(m_tif, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
	TIFFGetField(m_tif, TIFFTAG_BITSPERSAMPLE, &bitspersample);
	TIFFGetField(m_tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);   
	TIFFGetField(m_tif, TIFFTAG_PHOTOMETRIC, &photometric);
	TIFFGetField(m_tif, TIFFTAG_ORIENTATION, &orientation);

	TIFFGetField(m_tif, TIFFTAG_XRESOLUTION, &resolution);
	SetXDPI((long)resolution);
	TIFFGetField(m_tif, TIFFTAG_YRESOLUTION, &resolution);
	SetYDPI((long)resolution);
	TIFFGetField(m_tif, TIFFTAG_XPOSITION, &offset);
	info.xOffset = (long)offset;
	TIFFGetField(m_tif, TIFFTAG_YPOSITION, &offset);
	info.yOffset = (long)offset;

	head.biWidth  = width;
	head.biHeight = height;
	head.biClrUsed=0;
	info.nBkgndIndex =-1;

	if (rowsperstrip>height){
		rowsperstrip=height;
		TIFFSetField(m_tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
	}

	isRGB = (bitspersample >= 8) &&
		(photometric == PHOTOMETRIC_RGB) ||
		(photometric == PHOTOMETRIC_YCBCR) ||
		(photometric == PHOTOMETRIC_SEPARATED) ||
		(photometric == PHOTOMETRIC_LOGL) ||
		(photometric == PHOTOMETRIC_LOGLUV);

	if (isRGB){
		head.biBitCount=24;
	}else{
		if ((photometric==PHOTOMETRIC_MINISBLACK)||(photometric==PHOTOMETRIC_MINISWHITE)){
			if	(bitspersample == 1){
				head.biBitCount=1;		//B&W image
				head.biClrUsed =2;
			} else if (bitspersample == 4) {
				head.biBitCount=4;		//16 colors gray scale
				head.biClrUsed =16;
			} else {
				head.biBitCount=8;		//gray scale
				head.biClrUsed =256;
			}
		} else if (bitspersample == 4) {
			head.biBitCount=4;			// 16 colors
			head.biClrUsed=16;
		} else {
			head.biBitCount=8;			//256 colors
			head.biClrUsed=256;
		}
	}

	if (info.nEscape) throw "Cancelled"; // <vho> - cancel decoding

	Create(head.biWidth,head.biHeight,head.biBitCount,CXIMAGE_FORMAT_TIF);	//image creation
#if CXIMAGE_SUPPORT_ALPHA
	if (samplesperpixel==4) AlphaCreate();	//add alpha support for 32bpp tiffs
	if (samplesperpixel==2 && bitspersample==8) AlphaCreate();	//add alpha support for 8bpp + alpha
#endif //CXIMAGE_SUPPORT_ALPHA

	if (isRGB) {
		// Read the whole image into one big RGBA buffer using
		// the traditional TIFFReadRGBAImage() API that we trust.
		uint32* raster;		// retrieve RGBA image
		uint32 *row;

		raster = (uint32*)_TIFFmalloc(width * height * sizeof (uint32));
		if (raster == NULL) throw "No space for raster buffer";
			
		// Read the image in one chunk into an RGBA array
		if(!TIFFReadRGBAImage(m_tif, width, height, raster, 1)) {
				_TIFFfree(raster);
				throw "Corrupted TIFF file!";
		}

		// read the raster lines and save them in the DIB
		// with RGB mode, we have to change the order of the 3 samples RGB
		row = &raster[0];
		bits2 = info.pImage;
		for (y = 0; y < height; y++) {

			if (info.nEscape){ // <vho> - cancel decoding
				_TIFFfree(raster);
				throw "Cancelled";
			}

			bits = bits2;
			for (x = 0; x < width; x++) {
				*bits++ = (BYTE)TIFFGetB(row[x]);
				*bits++ = (BYTE)TIFFGetG(row[x]);
				*bits++ = (BYTE)TIFFGetR(row[x]);
#if CXIMAGE_SUPPORT_ALPHA
				if (samplesperpixel==4) AlphaSet(x,y,(BYTE)TIFFGetA(row[x]));
#endif //CXIMAGE_SUPPORT_ALPHA
			}
			row += width;
			bits2 += info.dwEffWidth;
		}
		_TIFFfree(raster);
	} else {
		RGBQUAD *pal;
		pal=(RGBQUAD*)calloc(256,sizeof(RGBQUAD));
		if (pal==NULL) throw "Unable to allocate TIFF palette";

		// set up the colormap based on photometric	
		switch(photometric) {
			case PHOTOMETRIC_MINISBLACK:	// bitmap and greyscale image types
			case PHOTOMETRIC_MINISWHITE:
				if (bitspersample == 1) {	// Monochrome image
					if (photometric == PHOTOMETRIC_MINISBLACK) {
						pal[1].rgbRed = pal[1].rgbGreen = pal[1].rgbBlue = 255;
					} else {
						pal[0].rgbRed = pal[0].rgbGreen = pal[0].rgbBlue = 255;
					}
				} else {		// need to build the scale for greyscale images
					if (photometric == PHOTOMETRIC_MINISBLACK) {
						for (UINT i=0; i<head.biClrUsed; i++){
							pal[i].rgbRed = pal[i].rgbGreen = pal[i].rgbBlue = (BYTE)(i*(255/(head.biClrUsed-1)));
						}
					} else {
						for (UINT i=0; i<head.biClrUsed; i++){
							pal[i].rgbRed = pal[i].rgbGreen = pal[i].rgbBlue = (BYTE)(255-i*(255/(head.biClrUsed-1)));
						}
					}
				}
				break;
			case PHOTOMETRIC_PALETTE:	// color map indexed
				uint16 *red;
				uint16 *green;
				uint16 *blue;
				TIFFGetField(m_tif, TIFFTAG_COLORMAP, &red, &green, &blue); 

				// Is the palette 16 or 8 bits ?
				BOOL Palette16Bits = FALSE;
				int n=1<<bitspersample;
				while (n-- > 0) {
					if (red[n] >= 256 || green[n] >= 256 || blue[n] >= 256) {
						Palette16Bits=TRUE;
						break;
					}
				}

				// load the palette in the DIB
				for (int i = (1 << bitspersample) - 1; i >= 0; i--) {
					if (Palette16Bits) {
						pal[i].rgbRed =(BYTE) CVT(red[i]);
						pal[i].rgbGreen = (BYTE) CVT(green[i]);
						pal[i].rgbBlue = (BYTE) CVT(blue[i]);           
					} else {
						pal[i].rgbRed = (BYTE) red[i];
						pal[i].rgbGreen = (BYTE) green[i];
						pal[i].rgbBlue = (BYTE) blue[i];        
					}
				}
				break;
		}
		SetPalette(pal,head.biClrUsed);	//palette assign
		free(pal);

		// read the tiff lines and save them in the DIB
		uint32 nrow;
		uint32 ys;
		int line = CalculateLine(width, bitspersample * samplesperpixel);
		long bitsize= TIFFStripSize(m_tif);
		//verify bitsize: could be wrong if StripByteCounts is missing.
		if (bitsize>(long)(head.biSizeImage)) bitsize=head.biSizeImage;
		bits = (BYTE*)malloc(bitsize);

		for (ys = 0; ys < height; ys += rowsperstrip) {

			if (info.nEscape){ // <vho> - cancel decoding
				free(bits);
				throw "Cancelled";
			}

			nrow = (ys + rowsperstrip > height ? height - ys : rowsperstrip);

			if (TIFFReadEncodedStrip(m_tif, TIFFComputeStrip(m_tif, ys, 0), bits, nrow * line) == -1) {
				free(bits);
				throw "Corrupted TIFF file!";
			}
			for (y = 0; y < nrow; y++) {
				long offset=(nrow-y-1)*line;
				if (bitspersample==16) for (DWORD xi=0;xi<width;xi++) bits[xi+offset]=bits[xi*2+offset+1];
				if (samplesperpixel==1) { //simple 8bpp image
					memcpy(info.pImage+info.dwEffWidth*(height-ys-nrow+y),bits+offset,info.dwEffWidth);
				} else {	//8bpp image with alpha layer
					int xi=0;
					int ii=0;
					int yi=height-ys-nrow+y;
					while (ii<line){
						SetPixelIndex(xi,yi,bits[ii*samplesperpixel+offset]);
#if CXIMAGE_SUPPORT_ALPHA
						AlphaSet(xi,yi,bits[ii*samplesperpixel+offset+1]);
#endif //CXIMAGE_SUPPORT_ALPHA
						ii++;
						xi++;
						if (xi>=(int)width){
							yi--;
							xi=0;
						}
					}
				}
			}
			/*if (head.biClrUsed==2){
				for (y = 0; y < nrow; y++) { for (x = 0; x < width; x++) {
					SetPixelIndex(x,y+ys,(bits[y*line+(x>>3)]>>(7-x%8))&0x01);
			}}}*/
		}
		free(bits);

		switch(orientation){
		case ORIENTATION_TOPRIGHT: /* row 0 top, col 0 rhs */
			Mirror();
			break;
		case ORIENTATION_BOTRIGHT: /* row 0 bottom, col 0 rhs */
			Flip();
			Mirror();
			break;
		case ORIENTATION_BOTLEFT: /* row 0 bottom, col 0 lhs */
			Flip();
			break;
		case ORIENTATION_LEFTTOP: /* row 0 lhs, col 0 top */
			RotateRight();
			Mirror();
			break;
		case ORIENTATION_RIGHTTOP: /* row 0 rhs, col 0 top */
			RotateLeft();
			break;
		case ORIENTATION_RIGHTBOT: /* row 0 rhs, col 0 bottom */
			RotateLeft();
			Mirror();
			break;
		case ORIENTATION_LEFTBOT: /* row 0 lhs, col 0 bottom */
			RotateRight();
			break;
		}

	}
  } catch (char *message) {
	  strncpy(info.szLastError,message,255);
	  if (m_tif) TIFFClose(m_tif);
	  return FALSE;
  }
	TIFFClose(m_tif);
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
bool CxImageTIF::Encode(CxFile * hFile, bool bAppend)
{
  try{
	if (hFile==NULL) throw "invalid file pointer";

	if (m_tif2==NULL) m_tif2=TIFFOpenEx(hFile, "w+b");
	if (m_tif2==NULL) throw "initialization fail";

	if (bAppend || m_pages) m_multipage=true;
	m_pages++;

	if (!EncodeBody(m_tif2,m_multipage,m_pages,m_pages)) throw "Error saving TIFF file";
	if (bAppend) {
		if (!TIFFWriteDirectory(m_tif2)) throw "Error saving TIFF directory";
	}
  } catch (char *message) {
	  strncpy(info.szLastError,message,255);
	  if (m_tif2){
		  TIFFClose(m_tif2);
		  m_tif2=NULL;
		  m_multipage=false;
		  m_pages=0;
	  }
	  return false;
  }
	if (!bAppend){
		TIFFClose(m_tif2);
		m_tif2=NULL;
		m_multipage=false;
		m_pages=0;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
// Thanks to Abe <God.bless@marihuana.com>
bool CxImageTIF::Encode(CxFile * hFile, CxImage ** pImages, int pagecount)
{
	TIFF *m_tif=NULL;
  try{
	if (hFile==NULL) throw "invalid file pointer";
	if (pImages==NULL || pagecount==0) throw "multipage TIFF, no images!";

	m_tif=TIFFOpenEx(hFile, "w+b");
	if (m_tif==NULL) throw "initialization fail";

	CxImageTIF ghost;
	for (int i=1; i<=pagecount; i++){
		if (pImages[i-1]==NULL) throw "Bad image pointer";
		ghost.Ghost(pImages[i-1]);
		if (!ghost.EncodeBody(m_tif,true,i,pagecount)) throw "Error saving TIFF file";
		//if (!((CxImageTIF*)pImages[i-1])->EncodeBody(m_tif,true,i,pagecount)) throw "Error saving TIFF file";
		if (!TIFFWriteDirectory(m_tif)) throw "Error saving TIFF directory";
	}
  } catch (char *message) {
	  strncpy(info.szLastError,message,255);
	  if (m_tif) TIFFClose(m_tif);
	  return false;
  }
	TIFFClose(m_tif);
	return true;
}
////////////////////////////////////////////////////////////////////////////////
bool CxImageTIF::EncodeBody(TIFF *m_tif, bool multipage, int page, int pagecount)
{
	uint32 height=head.biHeight;
	uint32 width=head.biWidth;
	uint16 bitcount=head.biBitCount;
	uint16 bitspersample;
	uint16 samplesperpixel;
	uint16 photometric=0;
	uint16 compression;
//	uint16 pitch;
//	int line;
	uint32 x, y;

	samplesperpixel = ((bitcount == 24) || (bitcount == 32)) ? (BYTE)3 : (BYTE)1;
#if CXIMAGE_SUPPORT_ALPHA
	if (bitcount==24 && HasAlpha()) { bitcount=32; samplesperpixel=4; }
#endif //CXIMAGE_SUPPORT_ALPHA

	bitspersample = bitcount / samplesperpixel;

	//set the PHOTOMETRIC tag
	RGBQUAD *rgb = GetPalette();
	switch (bitcount) {
		case 1:
			if (CompareColors(&rgb[0],&rgb[1])<0) {
				/* <abe> some viewers do not handle PHOTOMETRIC_MINISBLACK:
				 * let's transform the image in PHOTOMETRIC_MINISWHITE
				 */
				//invert the colors
				RGBQUAD tempRGB=GetPaletteColor(0);
				SetPaletteIndex(0,GetPaletteColor(1));
				SetPaletteIndex(1,tempRGB);
				//invert the pixels
				BYTE *iSrc=info.pImage;
				for (unsigned long i=0;i<head.biSizeImage;i++){
					*iSrc=(BYTE)~(*(iSrc));
					iSrc++;
				}
				photometric = PHOTOMETRIC_MINISWHITE;
				//photometric = PHOTOMETRIC_MINISBLACK;
			} else {
				photometric = PHOTOMETRIC_MINISWHITE;
			}
			break;
		case 4:	// Check if the DIB has a color or a greyscale palette
		case 8:
			photometric = PHOTOMETRIC_MINISBLACK; //default to gray scale
			for (x = 0; x < head.biClrUsed; x++) {
				if ((rgb->rgbRed != x)||(rgb->rgbRed != rgb->rgbGreen)||(rgb->rgbRed != rgb->rgbBlue)){
					photometric = PHOTOMETRIC_PALETTE;
					break;
				}
				rgb++;
			}
			break;
		case 24:
		case 32:
			photometric = PHOTOMETRIC_RGB;			
			break;
	}

#if CXIMAGE_SUPPORT_ALPHA
	if (HasAlpha() && bitcount==8) samplesperpixel=2; //8bpp + alpha layer
#endif //CXIMAGE_SUPPORT_ALPHA

//	line = CalculateLine(width, bitspersample * samplesperpixel);
//	pitch = (uint16)CalculatePitch(line);

	//prepare the palette struct
	RGBQUAD pal[256];
	if (GetPalette()){
		BYTE b;
		memcpy(pal,GetPalette(),GetPaletteSize());
		for(WORD a=0;a<head.biClrUsed;a++){	//swap blue and red components
			b=pal[a].rgbBlue; pal[a].rgbBlue=pal[a].rgbRed; pal[a].rgbRed=b;
		}
	}

	// handle standard width/height/bpp stuff
	TIFFSetField(m_tif, TIFFTAG_IMAGEWIDTH, width);
	TIFFSetField(m_tif, TIFFTAG_IMAGELENGTH, height);
	TIFFSetField(m_tif, TIFFTAG_SAMPLESPERPIXEL, samplesperpixel);
	TIFFSetField(m_tif, TIFFTAG_BITSPERSAMPLE, bitspersample);
	TIFFSetField(m_tif, TIFFTAG_PHOTOMETRIC, photometric);
	TIFFSetField(m_tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);	// single image plane 
	TIFFSetField(m_tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(m_tif, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(m_tif, -1));  //<REC> gives better compression

	// handle metrics
	TIFFSetField(m_tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
	TIFFSetField(m_tif, TIFFTAG_XRESOLUTION, (float)info.xDPI);
	TIFFSetField(m_tif, TIFFTAG_YRESOLUTION, (float)info.yDPI);
//	TIFFSetField(m_tif, TIFFTAG_XPOSITION, (float)info.xOffset);
//	TIFFSetField(m_tif, TIFFTAG_YPOSITION, (float)info.yOffset);

	// multi-paging - Thanks to Abe <God.bless@marihuana.com>
	if (multipage)
	{
		char page_number[20];
		sprintf(page_number, "Page %d", page);

		TIFFSetField(m_tif, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
		TIFFSetField(m_tif, TIFFTAG_PAGENUMBER, page,pagecount);
		TIFFSetField(m_tif, TIFFTAG_PAGENAME, page_number);
	} else {
		TIFFSetField(m_tif, TIFFTAG_SUBFILETYPE, 0);
	}

	// palettes (image colormaps are automatically scaled to 16-bits)
	if (photometric == PHOTOMETRIC_PALETTE) {
		uint16 *r, *g, *b;
		r = (uint16 *) _TIFFmalloc(sizeof(uint16) * 3 * 256);
		g = r + 256;
		b = g + 256;

		for (int i = 255; i >= 0; i--) {
			b[i] = (uint16)SCALE((uint16)pal[i].rgbRed);
			g[i] = (uint16)SCALE((uint16)pal[i].rgbGreen);
			r[i] = (uint16)SCALE((uint16)pal[i].rgbBlue);
		}

		TIFFSetField(m_tif, TIFFTAG_COLORMAP, r, g, b);
		_TIFFfree(r);
	}

	// compression
	switch(bitcount) {
		case 1 :
			compression = COMPRESSION_CCITTFAX4; //COMPRESSION_CCITTFAX3;
			break;
		case 8 :
			compression = COMPRESSION_PACKBITS;
			break;
		case 24 :
		case 32 :
			compression = COMPRESSION_JPEG;	//COMPRESSION_PACKBITS;
			break;
		default :
			compression = COMPRESSION_NONE;
			break;
	}
	TIFFSetField(m_tif, TIFFTAG_COMPRESSION, compression);

	// read the DIB lines from bottom to top
	// and save them in the TIF
	// -------------------------------------	
	BYTE *bits;
	switch(bitcount) {				
		case 1 :
		case 4 :
		case 8 :
		{
			if (samplesperpixel==1){
				for (y = 0; y < height; y++) {
					bits= info.pImage + (height - y - 1)*info.dwEffWidth;
					TIFFWriteScanline(m_tif,bits, y, 0);
				}
			}
#if CXIMAGE_SUPPORT_ALPHA
			else { //8bpp + alpha layer
				bits = (BYTE*)malloc(2*width);
				for (y = 0; y < height; y++) {
					for (x=0;x<width;x++){
						bits[2*x]=GetPixelIndex(x,height - y - 1);
						bits[2*x+1]=AlphaGet(x,height - y - 1);
					}
					TIFFWriteScanline(m_tif,bits, y, 0);
				}
				free (bits);
			}
#endif //CXIMAGE_SUPPORT_ALPHA
			break;
		}				
		case 24:
		{
			TIFFSetField(m_tif, TIFFTAG_JPEGQUALITY, info.nQuality); //needed for COMPRESSION_JPEG
			TIFFSetField(m_tif, TIFFTAG_ROWSPERSTRIP, 8); //needed for COMPRESSION_JPEG
			BYTE *buffer = (BYTE *)malloc(info.dwEffWidth);
			for (y = 0; y < height; y++) {
				// get a pointer to the scanline
				memcpy(buffer, info.pImage + (height - y - 1)*info.dwEffWidth, info.dwEffWidth);
				// TIFFs store color data RGB instead of BGR
				BYTE *pBuf = buffer;
				for (x = 0; x < width; x++) {
					BYTE tmp = pBuf[0];
					pBuf[0] = pBuf[2];
					pBuf[2] = tmp;
					pBuf += 3;
				}
				// write the scanline to disc
				TIFFWriteScanline(m_tif, buffer, y, 0);
			}
			free(buffer);
			break;
		}				
		case 32 :
		{
#if CXIMAGE_SUPPORT_ALPHA
			TIFFSetField(m_tif, TIFFTAG_JPEGQUALITY, info.nQuality); //needed for COMPRESSION_JPEG
			TIFFSetField(m_tif, TIFFTAG_ROWSPERSTRIP, 8); //needed for COMPRESSION_JPEG
			BYTE *buffer = (BYTE *)malloc((info.dwEffWidth*4)/3);
			for (y = 0; y < height; y++) {
				// get a pointer to the scanline
				memcpy(buffer, info.pImage + (height - y - 1)*info.dwEffWidth, info.dwEffWidth);
				// TIFFs store color data RGB instead of BGR
				BYTE *pSrc = buffer + 3 * width;
				BYTE *pDst = buffer + 4 * width;
				for (x = 0; x < width; x++) {
					pDst-=4;
					pSrc-=3;
					pDst[3] = AlphaGet(width-x-1,height-y-1);
					pDst[2] = pSrc[0];
					pDst[1] = pSrc[1];
					pDst[0] = pSrc[2];
				}
				// write the scanline to disc
				TIFFWriteScanline(m_tif, buffer, y, 0);
			}
			free(buffer);
#endif //CXIMAGE_SUPPORT_ALPHA
			break;
		}				
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
#endif 	// CXIMAGE_SUPPORT_TIF
