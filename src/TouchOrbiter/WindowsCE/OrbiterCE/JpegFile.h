#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

void JpegFileInitialize (HWND hwnd) ;
BOOL JpegFileOpenDlg (HWND hwnd, LPWSTR pstrFileName, LPWSTR pstrTitleName) ;

BOOL JpegLoadImage (LPWSTR pstrFileName, BYTE **ppbImageData, 
                   int *piWidth, int *piHeight, int *piChannels);
BOOL JpegLoadImageFilePtr (FILE *pfFile, BYTE **ppbImageData,
                   int *piWidth, int *piHeight, int *piChannels);
