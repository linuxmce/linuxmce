//-------------------------------------
//  JPGFILE.C -- Image File Functions
//-------------------------------------

#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>

#include "jpeglib.h"
#include "JpegFile.h"

static OPENFILENAME ofn;

// Windows open-file functions

void JpegFileInitialize (HWND hwnd)
{
    static WCHAR szFilter[] = TEXT ("JPG Files (*.JPG)\0*.jpg\0")
        TEXT ("All Files (*.*)\0*.*\0\0");

    ofn.lStructSize       = sizeof (OPENFILENAME);
    ofn.hwndOwner         = hwnd;
    ofn.hInstance         = NULL;
    ofn.lpstrFilter       = szFilter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter    = 0;
    ofn.nFilterIndex      = 0;
    ofn.lpstrFile         = NULL;          // Set in Open and Close functions
    ofn.nMaxFile          = MAX_PATH;
    ofn.lpstrFileTitle    = NULL;          // Set in Open and Close functions
    ofn.nMaxFileTitle     = MAX_PATH;
    ofn.lpstrInitialDir   = NULL;
    ofn.lpstrTitle        = NULL;
    ofn.Flags             = 0;             // Set in Open and Close functions
    ofn.nFileOffset       = 0;
    ofn.nFileExtension    = 0;
    ofn.lpstrDefExt       = TEXT ("jpg");
    ofn.lCustData         = 0;
    ofn.lpfnHook          = NULL;
    ofn.lpTemplateName    = NULL;
}

BOOL JpegFileOpenDlg (HWND hwnd, LPWSTR pstrFileName, LPWSTR pstrTitleName)
{
    ofn.hwndOwner         = hwnd;
    ofn.lpstrFile         = pstrFileName;
    ofn.lpstrFileTitle    = pstrTitleName;
    ofn.Flags             = OFN_HIDEREADONLY;

    return GetOpenFileName (&ofn);
}

// JPG image handler functions

BOOL JpegLoadImage (LPWSTR pstrFileName, BYTE **ppbImageData,
                   int *piWidth, int *piHeight, int *piChannels)
{
    FILE           *pfFile;
    int             iBitDepth;

    // open the JPG input file

    if (!pstrFileName)
    {
        *ppbImageData = NULL;
        return FALSE;
    }

    if (!(pfFile = _wfopen(pstrFileName, L"rb")))
    {
        *ppbImageData = NULL;
        return FALSE;
    }

    BOOL result = JpegLoadImageFilePtr(pfFile, ppbImageData, piWidth, piHeight, piChannels);

    // yepp, done

    fclose (pfFile);

    return result;
}

BOOL JpegLoadImageFilePtr (FILE *pfFile, BYTE **ppbImageData,
                   int *piWidth, int *piHeight, int *piChannels)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, pfFile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    *piWidth = cinfo.output_width;
    *piHeight = cinfo.output_height;

    *piChannels = cinfo.output_components;

    // now we can allocate memory to store the image

    if (*ppbImageData)
    {
        free (*ppbImageData);
        *ppbImageData = NULL;
    }
    *ppbImageData = (BYTE *) malloc((*piWidth) * (*piChannels) * (*piHeight) * sizeof(BYTE));

    while (cinfo.output_scanline < cinfo.output_height)
    {
         JSAMPROW pImageRow[1] = {(*ppbImageData) + cinfo.output_scanline * (*piWidth) * (*piChannels) * sizeof(BYTE)};
         jpeg_read_scanlines(&cinfo, pImageRow, 1);
    }

    // and we're done

    jpeg_destroy_decompress(&cinfo);

    return TRUE;
}

//-----------------
//  end of source
//-----------------


