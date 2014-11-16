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

   Q_INVOKABLE static const QString getSystemName(int s){
        switch(s){
        case RASPBERRY_PI:
            return "Raspberry Pi";
            break;
        case OSX_DESKTOP:
            return "OS X Desktop";
            break;
        case LINUX_DESKTOP:
            return "Linux Desktop";
            break;
        case ANDROID_TABLET:
            return "Android Tablet";
            break;
        case ANDROID_PHONE:
            return "Android Phone";
            break;
        default:
            return "Dunno, Windows 98?";
            break;
        }
    }

};
class HouseModesHelper : public QObject
{
    Q_OBJECT
public:
    HouseModesHelper() {}
    virtual ~HouseModesHelper() {}
    enum HouseModes{
        Unarmed_at_home=1,
        Armed_away=2,
        Armed_at_home=3,
        Sleeping_CONST=4,
        Entertaining=5,
        Armed_Extended_away=6
    };
    Q_ENUMS(HouseModes)
};

#endif // LINUXMCEDATA_H
