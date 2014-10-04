#ifndef LINUXMCEDATA_H
#define LINUXMCEDATA_H

#include <QObject>

class LinuxmceData : public QObject{
 Q_OBJECT

public:
    explicit LinuxmceData(QObject *parent =0);

};

/*!
 * \brief The HostSystemData class is used as a simple enum to provide
 *  a qml enum for the host operating system.
 * import into qml with statement: import org.linuxmce.enums
 * use: HostDevices.OSX_DESKTOP
 */
class HostSystemData : public QObject{
    Q_OBJECT
public:

    HostSystemData(){}
    virtual ~HostSystemData(){}
    enum HostTypes{
        RASPBERRY_PI=1,
        OSX_DESKTOP=2,
        LINUX_DESKTOP=3,
        WINDOWS_DESKTOP=4,
        ANDROID_PHONE=5,
        ANDROID_TABLET=6,
        IOS_PHONE=7,
        IOS_TABLET=8,
        OTHER_EMBEDDED=9,
        SAILFISH_OS=10
    };
    Q_ENUMS(HostTypes)


signals:

public slots:

};

#endif // LINUXMCEDATA_H
