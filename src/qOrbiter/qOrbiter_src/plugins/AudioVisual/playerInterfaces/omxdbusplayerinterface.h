#ifndef OMXDBUSPLAYERINTERFACE_H
#define OMXDBUSPLAYERINTERFACE_H
#include "../mediabase/mediamanagerbase.h"
#include "omxinterface.h"

class omxdbusplayerinterface : public MediaManagerBase
{
public:
    omxdbusplayerinterface();

  public slots:
    QImage getScreenShot();

private slots:
    void setConnections();

private:
    OmxInterface *m_omxProxy;

private:

};

#endif // OMXDBUSPLAYERINTERFACE_H
