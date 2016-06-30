#ifndef OMXDBUSPLAYERINTERFACE_H
#define OMXDBUSPLAYERINTERFACE_H
#include "../mediabase/mediamanagerbase.h"

class omxdbusplayerinterface : public MediaManagerBase
{
public:
    omxdbusplayerinterface();

  public slots:
    QImage getScreenShot();
};

#endif // OMXDBUSPLAYERINTERFACE_H
