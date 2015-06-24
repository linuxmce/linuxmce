#ifndef CLOUDINTERFACECOTROLLER_H
#define CLOUDINTERFACECOTROLLER_H

#include <QObject>
#include <QMap>
#include "abstractcloudprovider.h"

/*!
 * \brief The CloudInterfaceCotroller class is designed to be the interface between
 * LinuxMCE and various cloud services, implemented as plugins.
 */
class CloudInterfaceController : public QObject
{
    Q_OBJECT
public:
    explicit CloudInterfaceController(QObject *parent = 0);

signals:

public slots:

private:
    QMap <QString, CloudServiceInterface*> m_plugins;

};

#endif // CLOUDINTERFACECOTROLLER_H
