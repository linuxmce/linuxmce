// Console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "ximage.h"

CString FindExtension(const CString& name)
{
	int len = name.GetLength();
	int i;
	for (i = len-1; i >= 0; i--){
		if (name[i] == '.'){
			return name.Mid(i+1);
		}
	}
	return CString("");
}

int main(int argc, char* argv[])
{

    if (argc<3) {
        fprintf(stderr, "CxImage 5.00 - Console demo\n");
        fprintf(stderr, "usage: %s input-file output-file\n", argv[0]);
        return 1;
    }

	CString filein(argv[1]);
	CString extin(FindExtension(filein));
	extin.MakeLower();
	int typein = 0;
	if (extin == "bmp")						  typein = CXIMAGE_FORMAT_BMP;
	else if (extin == "gif")				  typein = CXIMAGE_FORMAT_GIF;
	else if (extin == "ico")				  typein = CXIMAGE_FORMAT_ICO;
	else if (extin == "tga")				  typein = CXIMAGE_FORMAT_TGA;
	else if (extin == "jpg")				  typein = CXIMAGE_FORMAT_JPG;
    else if (extin == "tif" || extin=="tiff") typein = CXIMAGE_FORMAT_TIF;
    else if (extin == "png")                  typein = CXIMAGE_FORMAT_PNG;
    else if (extin == "wbmp")                 typein = CXIMAGE_FORMAT_WBMP;
	else if (extin == "wmf" || extin =="emf") typein = CXIMAGE_FORMAT_WMF;
    else if (extin == "pcx")                  typein = CXIMAGE_FORMAT_PCX;
    //else if (extin == "j2k" || extin =="jp2") typein = CXIMAGE_FORMAT_J2K;
    //else if (extin == "jbg")                  typein = CXIMAGE_FORMAT_JBG;
	else {
        fprintf(stderr, "unknown extension for %s\n", argv[1]);
        return 1;
	}

	CString fileout(argv[2]);
	CString extout(FindExtension(fileout));
	extout.MakeLower();
	int typeout = 0;
	if (extout == "bmp")					  typeout = CXIMAGE_FORMAT_BMP;
	else if (extout == "gif")				  typeout = CXIMAGE_FORMAT_GIF;
	else if (extout == "ico")				  typeout = CXIMAGE_FORMAT_ICO;
	else if (extout == "tga")				  typeout = CXIMAGE_FORMAT_TGA;
	else if (extout == "jpg")				  typeout = CXIMAGE_FORMAT_JPG;
    else if (extout == "tif"||extout=="tiff") typeout = CXIMAGE_FORMAT_TIF;
    else if (extout == "png")                 typeout = CXIMAGE_FORMAT_PNG;
    else if (extout == "wbmp")                typeout = CXIMAGE_FORMAT_WBMP;
    else if (extout == "pcx")                 typeout = CXIMAGE_FORMAT_PCX;
    //else if (extout == "j2k"||extout== "jp2") typeout = CXIMAGE_FORMAT_J2K;
    //else if (extout == "jbg")                 typeout = CXIMAGE_FORMAT_JBG;
	else {
        fprintf(stderr, "unknown extension for %s\n", argv[2]);
        return 1;
	}

	CxImage image;

	if (!image.Load(argv[1],typein)){
        fprintf(stderr, "%s\n", image.GetLastError());
        fprintf(stderr, "error loading %s\n", argv[1]);
        return 1;
	}

	if (!image.Save(argv[2],typeout)){
        fprintf(stderr, "%s\n", image.GetLastError());
        fprintf(stderr, "error saving %s\n", argv[2]);
        return 1;
	}

	printf("Done!\n");
	return 0;
}

