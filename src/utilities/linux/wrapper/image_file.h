//
// Author : Remus C.
//
// Changed by : ...
//

// x11 image type : file access routines
// which does not need the X11 library
//
// may need change in the future
// needed by wrapper_x11 & other

#ifndef _IMAGE_FILE_H_
#define _IMAGE_FILE_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "image_file.h"
#endif

#include <string>
#include <iostream>
using namespace std;

// write a xbm-type of file
// does not need X routines
// x_hot and y_hot value -1 means: not used
// pRawBoolData is an array[width*height] of boolean values for every pixel
bool Xbm_WriteFile(const string &sPath, const char *pRawBoolData, unsigned int width, unsigned int height, int x_hot=-1, int y_hot=-1);

#endif
// _IMAGE_FILE_H_
