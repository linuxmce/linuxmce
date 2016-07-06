#ifndef OMXDBUSPLAYERINTERFACE_H
#define OMXDBUSPLAYERINTERFACE_H
#include "../mediabase/mediamanagerbase.h"
#include "omxinterface.h"

class omxdbusplayerinterface : public MediaManagerBase
{
    Q_OBJECT
public:
    omxdbusplayerinterface();

  public slots:
    QImage getScreenShot();
    void modifypath(QString file);

private slots:
    void setConnections();


private:
    OmxInterface *m_omxProxy;

private:

};

#endif // OMXDBUSPLAYERINTERFACE_H
