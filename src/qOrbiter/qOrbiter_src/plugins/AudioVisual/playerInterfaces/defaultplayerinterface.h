#ifndef DEFAULTPLAYERINTERFACE_H
#define DEFAULTPLAYERINTERFACE_H
#include "../mediabase/mediamanagerbase.h"


class DefaultPlayerInterface : public MediaManagerBase
{
public:
    DefaultPlayerInterface();

public slots:
    QImage getScreenShot();
};

#endif // DEFAULTPLAYERINTERFACE_H
