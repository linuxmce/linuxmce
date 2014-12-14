#ifndef ANDROIDSYSTEM_H
#define ANDROIDSYSTEM_H

#include <QObject>
#include <jni.h>

#if defined(QT4)
#include <QPlatformNativeInterface>
#elif defined(QT5)
#include <QtAndroidExtras>
#endif
#include <QColor>

/*!
 * \brief The AndroidSystem class
 * This class is a frankenstein of sorts because of the two different ways one can access the JNI. In qt4, its used for the media player
 * and to obtain some system informaton. In Qt5 it is exclusively used for system information at the moment.
 */
class AndroidSystem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int apiLevel READ getApiLevel NOTIFY apiLevelChanged)
    Q_PROPERTY(QString statusMessage READ getStatusMessage  NOTIFY statusMessageChanged)
    Q_PROPERTY(QString codename READ getCodeName NOTIFY codenameChanged)
    Q_PROPERTY(QString deviceName READ getDeviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(QString deviceManufacturer READ getDeviceManufacturer NOTIFY deviceManufacturerChanged)
    Q_PROPERTY(QString deviceBrand READ getDeviceBrand NOTIFY deviceBrandChanged )
    Q_PROPERTY(bool androidReady READ getReadyStatus NOTIFY readyStatusChanged)

    Q_PROPERTY(bool mountStatus READ getMountStatus NOTIFY mountStatusChanged)
    Q_PROPERTY(QString externalStorageLocation READ getExternalStorageLocation NOTIFY externalStorageLocationChanged)

    Q_PROPERTY(QColor blueStandard READ getBlueStandard NOTIFY blueStandardChanged)
    Q_PROPERTY(QColor blueHighlight READ getBlueHighLight NOTIFY blueHightlightChanged)
    Q_PROPERTY(QColor purpleStandard READ getPurpleStandard NOTIFY purpleStandardChanged)
    Q_PROPERTY(QColor purpleHighlight READ getPurpleHighlight NOTIFY purpleHighlightChanged)
    Q_PROPERTY (QColor greenStandard READ getGreenStandard NOTIFY greenStandardChanged)
    Q_PROPERTY(QColor greenHighlight READ getGreenHighlight NOTIFY greenHighlightChanged)
    Q_PROPERTY(QColor orangeHighlight READ getOrangeHighlight NOTIFY orangeHighlightChanged)
    Q_PROPERTY(QColor orangeStandard READ getOrangeStandard NOTIFY orangeStandardChanged)
    Q_PROPERTY (QColor redStandard READ getRedStandard NOTIFY redStandardChanged)
    Q_PROPERTY(QColor redHighlight READ getRedHighlight NOTIFY redHighlightChanged)

public:
    explicit AndroidSystem(QObject *parent = 0);
    int apiLevel;
    QString statusMessage;
    QString codename;
    QString deviceName;
    QString deviceManufacturer;
    QString deviceBrand;
    QString externalStorageLocation;
    bool androidReady;
    bool isTablet;

    bool mountStatus;

    QColor blueStandard;
    QColor blueHighlight;
    QColor purpleStandard;
    QColor purpleHighlight;
    QColor greenStandard;
    QColor greenHighlight;
    QColor orangeStandard;
    QColor orangeHighlight;
    QColor redStandard;
    QColor redHighlight;

signals:
    void mountStatusChanged();
    void externalStorageLocationChanged();
    void apiLevelChanged();
    void statusMessageChanged();
    void codenameChanged();
    void deviceNameChanged();
    void deviceManufacturerChanged();
    void deviceBrandChanged();
    void readyStatusChanged();

    void blueStandardChanged();
    void blueHightlightChanged();
    void purpleStandardChanged();
    void purpleHighlightChanged();
    void greenStandardChanged();
    void greenHighlightChanged();
    void orangeStandardChanged();
    void orangeHighlightChanged();
    void redStandardChanged();
    void redHighlightChanged();
    void isTabletChanged();

public slots:
    void setIsTablet(bool b){if(isTablet !=b){ isTablet = b;} emit isTabletChanged();}
    bool getIsTablet() {return isTablet;}
    void setReadyStatus(bool s) { androidReady = s; emit readyStatusChanged();}
    bool getReadyStatus() { return androidReady;}

    void setMountStatus(bool b) {mountStatus = b; emit mountStatusChanged();}
    bool getMountStatus(){return mountStatus;}

    void setExternalStorageLocation(QString e) { if(e==""){return;}  else  {externalStorageLocation = e; emit externalStorageLocationChanged();}}
    QString getExternalStorageLocation(){return externalStorageLocation;}

    QColor  getBlueStandard() {return blueStandard;}
    QColor getBlueHighLight() {return blueHighlight;}
    QColor getPurpleStandard() {return purpleStandard;}
    QColor getPurpleHighlight() {return purpleHighlight;}
    QColor getGreenStandard() {return greenStandard;}
    QColor getGreenHighlight(){ return greenHighlight;}
    QColor getOrangeStandard(){return orangeStandard;}
    QColor getOrangeHighlight(){return orangeHighlight;}
    QColor getRedStandard() {return redStandard;}
    QColor getRedHighlight() {return redHighlight;}

    void setDeviceBrand(QString b) {deviceBrand = b; emit deviceBrandChanged();}
    QString getDeviceBrand() {return deviceBrand;}

    void setDeviceManufacturer(QString m) {deviceManufacturer = m; emit deviceManufacturerChanged();}
    QString getDeviceManufacturer() {return deviceManufacturer;}

    void setDeviceName(QString d){deviceName = d; emit deviceNameChanged();}
    QString getDeviceName(){return deviceName; }

    void setCodeName(QString c) {codename = c; emit codenameChanged();}
    QString getCodeName(){return codename;}

    void setStatusMessage(QString s) {statusMessage = s; emit statusMessageChanged();}
    QString getStatusMessage() {return statusMessage;}

    void setApiLevel(int lvl){ apiLevel = lvl; emit apiLevelChanged();}
    int getApiLevel(){ return apiLevel;}

   /* Q_INVOKABLE*/ bool updateExternalStorageLocation();
   /* Q_INVOKABLE*/ bool updateBuildInformation();

    bool playMedia(QString url);
    bool stopMedia();    
    bool startAudioService(long addr);
    bool setVolume(double vol);

    void mediaStop();
    void mediaPlay();
    void mediaPause();
    int getMediaPosition();
    void mediaResume();
    void mediaSeek(int s);
    void mediaSetVol(double vol);

private:

    bool findClassIdents();
    bool getDisplayInfo();
    jobject m_qtActivity;
    jobject m_audioService;

    QString lmceDataClass;
    QString locationFunction;
    QString deviceDataClass;

#ifdef QT5
 QAndroidJniObject *j_lmceData;

#endif

};

#endif // ANDROIDSYSTEM_H
