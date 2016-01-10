#include "ImageConvertor.h"

bool ImageConvertor::convertToRgb( ImageConvertor::SourceFormat sourceFormat, unsigned char *sourceBuffer, int sourceWidth, int sourceHight, unsigned char **destinationBuffer )
{
    switch ( sourceFormat )
    {
        case YV12_FORMAT:
            return true;
        default:
            return false;
    };
}
