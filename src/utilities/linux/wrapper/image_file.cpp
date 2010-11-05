//
// Author : Remus C.
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "image_file.h"
#endif

#include "image_file.h"
#include <stdio.h>

bool Xbm_WriteFile(const string &sPath, const char *pRawData, unsigned int width, unsigned int height, int x_hot/*=-1*/, int y_hot/*=-1*/)
{
    //fprintf(stderr,
    //        "sPath=='%s', pRawData==%p, width==%d, height==%d, x_hot==%d, y_hot==%d\n",
    //        sPath.c_str(), pRawData, width, height, x_hot, y_hot
    //        );
    FILE *pFile = fopen(sPath.c_str(), "w");
    if (pFile == NULL)
    {
        fprintf(stderr, "cannot write into '%s'\n", sPath.c_str());
        return false;
    }
    fprintf(pFile, "#define name_width %d\n", width);
    fprintf(pFile, "#define name_height %d\n", height);
    if (x_hot != -1)
        fprintf(pFile, "#define name_x_hot %d\n", x_hot);
    if (y_hot != -1)
        fprintf(pFile, "#define name_y_hot %d\n", y_hot);
    fprintf(pFile, "static unsigned char name_bits[] = {\n");
    unsigned char packed_byte = 0;
    unsigned int pos_in_byte = 0;
    for (unsigned int y=0; y<height; ++y)
    {
        for (unsigned int x=0; x<width; ++x)
        {
            unsigned char value = (*(pRawData + y*width + x) ) ? 1 : 0;
            packed_byte |= value << pos_in_byte;
            pos_in_byte++;
            if (pos_in_byte == 8)
            {
                fprintf(pFile, " 0x%02x", packed_byte);
                if ( (x != width-1) || (y != height-1) )
                    fprintf(pFile, ",");
                pos_in_byte = 0;
                packed_byte = 0;
            }
        }
        if (pos_in_byte != 0)
        {
            fprintf(pFile, " 0x%02x", packed_byte);
            pos_in_byte = 0;
            if (y != height-1)
                fprintf(pFile, ",");
            pos_in_byte = 0;
            packed_byte = 0;
        }
        fprintf(pFile, "\n");
    }
    fprintf(pFile, "}\n");
    fclose(pFile);
    return true;
}
