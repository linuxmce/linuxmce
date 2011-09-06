#include "securityvideoimage.h"

SecurityVideoImage::SecurityVideoImage(SecurityVideoClass *ref) :
    QDeclarativeImageProvider(QDeclarativeImageProvider::Image) ,securityreference(ref)
{
}
