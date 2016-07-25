#ifndef SCREENINFO_H
#define SCREENINFO_H

#include <QObject>
#include <QScreen>
#include <QMap>
#include <QDebug>
#include "math.h"
/*!
 * \brief The ScreenData class
 * \ingroup fileselectors
 */
class ScreenData : public QObject
{
    Q_OBJECT
public:
    ScreenData() {}
    enum PixelDensityScale{
        PixelDensity_LOW=120,
        PixelDensity_MEDIUM=160, // baseline dpi
        PixelDensity_TV=213,
        PixelDensity_HDPI=240,
        PixelDensity_XHDPI=320,
        PixelDensity_XXHDPI= 480,
        PixelDensity_XXXHDPI= 640
    };

    Q_ENUMS(PixelDensityScale)

    enum DeviceSizes{ //portrait width
        Device_240 =240,  //* Small
        Device_320 =320,
        Device_480 =480,
        Device_540 =540,  //* Medium
        Device_600 =600,
        Device_768 =768,
        Device_800 =800,
        Device_960 =960,  //* large
        Device_1024=1024,
        Device_1280=1280
    };

    Q_ENUMS(DeviceSizes)

    enum DeviceRange{
        Device_FullScreen=0,
        Device_Small    = 4,    //Phones small
        Device_Medium   = 7,    //Normal Tablets
        Device_Large    = 10,   //Big Tablets
        Device_XLarge   = 13    //Computer Monitors, tvs, etc
    };

    virtual ~ScreenData() {}
};

/*!
 * \brief The ScreenObject class
 * \ingroup fileselectors
 */
class ScreenObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double logicalDpi READ logicalDpi  NOTIFY logicalDpiChanged)
    Q_PROPERTY(double physicalDpi READ physicalDpi NOTIFY physicalDpiChanged)
    Q_PROPERTY(QString orientation READ orientation  NOTIFY orientationChanged)
    Q_PROPERTY(QString screenName READ screenName  NOTIFY screenNameChanged)
    Q_PROPERTY(QString primaryOrientation READ primaryOrientation NOTIFY primaryOrientationChanged)
    Q_PROPERTY(QString nativeOrientation READ nativeOrientation NOTIFY nativeOrientationChanged)
    Q_PROPERTY(int heightMM READ heightMM NOTIFY measurementChanged  )
    Q_PROPERTY(int widthMM READ widthMM NOTIFY measurementChanged)
    Q_PROPERTY(int heightInches READ heightInches() NOTIFY measurementChanged  )
    Q_PROPERTY(int widthInches READ widthInches NOTIFY measurementChanged  )
    Q_PROPERTY(QString stringDeviceSize READ stringDeviceSize()  NOTIFY diagonalSizeChanged)
    Q_PROPERTY(double diagonalSize READ diagonalSize NOTIFY diagonalSizeChanged)
    Q_PROPERTY(double diagonalInches READ diagonalInches NOTIFY diagonalSizeChanged)
    Q_PROPERTY(int width READ width NOTIFY sizeChanged)
    Q_PROPERTY(int height READ height NOTIFY sizeChanged)
    Q_PROPERTY(double pixelRatio READ pixelRatio NOTIFY pixelRatioChanged)
    Q_PROPERTY(QString pixelDensityString READ pixelDensityString NOTIFY pixelRatioChanged)

public:
    ScreenObject():
        m_logicalDpi(0.0),
        m_phisicalDpi(0.0),
        m_orientation(Qt::PortraitOrientation),
        m_screenName("undefined"),
        m_primaryOrientation(Qt::PortraitOrientation),
        m_nativeOrientation(Qt::PortraitOrientation)
    {}

    ScreenObject(QString name,
                 double logDpi, double physDpi,
                 Qt::ScreenOrientation oriented,Qt::ScreenOrientation primary_orientation, Qt::ScreenOrientation native_orientation,
                 int height_mm, int width_mm,
                 int pixel_height, int pixel_width
                 ):
        m_logicalDpi(logDpi),
        m_phisicalDpi(physDpi),
        m_orientation(oriented),
        m_screenName(name),
        m_primaryOrientation(primary_orientation),
        m_nativeOrientation(native_orientation),
        m_heightMM(height_mm),
        m_widthMM(width_mm),
        m_height(pixel_height),
        m_width(pixel_width),
        m_deviceSize(ScreenData::Device_Small),
        m_devicePixelSize(ScreenData::Device_240),
        m_pixelScale(ScreenData::PixelDensity_LOW),
        qs_deviceSize("small")
    {
        setDiagonalSize(sqrt( pow( (double)m_heightMM,2.0)+pow((double)m_widthMM, 2.0) ));

        qDebug() << " Handling Device " <<  heightInches() << "h x " << widthInches() << "w  device measured at " << diagonalInches() << " descriptively";
        if(  diagonalInches() <= 5.2 ){
            setStringDeviceSize(tr("Small"));
            setScreenSize(ScreenData::Device_Small);

            qDebug() << "Set Size Selector to small";

        } else if (diagonalInches() <= 7){
            setStringDeviceSize(tr("Medium"));
            setScreenSize(ScreenData::Device_Medium);            
            qDebug() << "Set Size Selector to medium";

        } else if (diagonalInches() <= 10 ) {
            setStringDeviceSize(tr("Large"));
            setScreenSize(ScreenData::Device_Large);
            qDebug() << "Set Size Selector to large";

        } else if ( diagonalInches() <= 12 ) {
            setStringDeviceSize(tr("XLarge"));
            setScreenSize(ScreenData::Device_XLarge);
            qDebug() << "Set Size Selector to xlarge";
        } else {
            setStringDeviceSize(tr("XLarge"));
            setScreenSize(ScreenData::Device_XLarge);
            qDebug() << "Set Size Selector to xlarge";
        }

        if (m_logicalDpi <= ScreenData::PixelDensity_LOW){ setPixelScale(ScreenData::PixelDensity_LOW);}
        else if (m_logicalDpi <= ScreenData::PixelDensity_MEDIUM){ setPixelScale(ScreenData::PixelDensity_MEDIUM);}
        else if (m_logicalDpi <= ScreenData::PixelDensity_TV){ setPixelScale(ScreenData::PixelDensity_TV);}
        else if (m_logicalDpi <= ScreenData::PixelDensity_HDPI){ setPixelScale(ScreenData::PixelDensity_HDPI);}
        else if (m_logicalDpi <= ScreenData::PixelDensity_XHDPI){ setPixelScale(ScreenData::PixelDensity_XHDPI);}
        else if (m_logicalDpi <= ScreenData::PixelDensity_XXHDPI){ setPixelScale(ScreenData::PixelDensity_XXHDPI);}
        else if (m_logicalDpi <= ScreenData::PixelDensity_XXXHDPI){ setPixelScale(ScreenData::PixelDensity_XXXHDPI);}


        int cmp = m_height < m_width ? m_height : m_width;

        if(cmp <= ScreenData::Device_240) { m_devicePixelSize = ScreenData::Device_240; }
        else  if(cmp <= ScreenData::Device_320) { m_devicePixelSize = ScreenData::Device_320; }
         else  if(cmp <= ScreenData::Device_480) { m_devicePixelSize = ScreenData::Device_480; }
         else  if(cmp <= ScreenData::Device_540) { m_devicePixelSize = ScreenData::Device_540; }
         else  if(cmp <= ScreenData::Device_600) { m_devicePixelSize = ScreenData::Device_600; }
         else  if(cmp <= ScreenData::Device_768) { m_devicePixelSize = ScreenData::Device_768; }
         else  if(cmp <= ScreenData::Device_800) { m_devicePixelSize = ScreenData::Device_800; }
         else  if(cmp <= ScreenData::Device_960) { m_devicePixelSize = ScreenData::Device_960; }
         else  if(cmp <= ScreenData::Device_1024) { m_devicePixelSize = ScreenData::Device_1024; }
         else  if(cmp <= ScreenData::Device_1280) { m_devicePixelSize = ScreenData::Device_1280; }


    }

    virtual ~ScreenObject() {}

    double pixelRatio(){return m_pixelRatio;}
    double diagonalSize(){return m_diagonalSize;}
    double logicalDpi(){return m_logicalDpi;}
    double physicalDpi(){return m_phisicalDpi;}
    int height(){return m_height;}
    int width() {return m_width;}
    QString screenName(){return m_screenName;}
    int heightMM(){return m_heightMM;}
    int widthMM(){return m_widthMM;}
    int heightInches(){ return m_heightMM *0.0393701; }
    int widthInches(){ return m_widthMM * 0.0393701; }
    double diagonalInches(){return m_diagonalSize*0.0393701;}


    QString stringDeviceSize(){return qs_deviceSize;}
    QString orientation() { return getOrientationString(m_orientation); }
    QString primaryOrientation(){ return getOrientationString(m_primaryOrientation); }
    QString nativeOrientation(){ return getOrientationString(m_nativeOrientation); }

    QString deviceSizeString(){ return deviceSizeToString(m_deviceSize);}
    QString pixelDensityString(){return pixelDensityToString(m_pixelScale);}
    QString resolutionString() {return deviceResolutionToString(m_devicePixelSize);}

    ScreenData::DeviceRange deviceSize(){return m_deviceSize;}
    QStringList DeviceSizeList(){ return QStringList(); }

public slots:

    QString deviceResolutionToString(ScreenData::DeviceSizes sz){
        switch (sz) {
        case ScreenData::Device_240: return "240";break;
        case ScreenData::Device_320: return "320";break;
        case ScreenData::Device_480: return "480";break;
        case ScreenData::Device_540: return "540";break;
        case ScreenData::Device_600: return "600";break;
        case ScreenData::Device_768: return "768";break;
        case ScreenData::Device_800: return "800";break;
        case ScreenData::Device_960: return "960";break;
        case ScreenData::Device_1024: return "1024";break;
        case ScreenData::Device_1280: return "1280";break;
        default:
            return "240";
            break;
        }
    }

    QString deviceSizeToString(ScreenData::DeviceRange d){
        switch(d){
        case ScreenData::Device_Small: return "small"; break;
        case ScreenData::Device_Medium: return "medium"; break;
        case ScreenData::Device_Large: return "large"; break;
        case ScreenData::Device_XLarge: return "xlarge"; break;
        default: return "Unknown";     break;
        }
    }

    QString deviceSizeToString(int d){
        return deviceSizeToString((ScreenData::DeviceRange)d);
    }

    QString pixelDensityToString(ScreenData::PixelDensityScale sz){
        switch (sz) {
        case ScreenData::PixelDensity_LOW: return "ldpi";break;
        case ScreenData::PixelDensity_MEDIUM: return "mdpi";break;
        case ScreenData::PixelDensity_TV: return "tvdpi";break;
        case ScreenData::PixelDensity_HDPI: return "hdpi";break;
        case ScreenData::PixelDensity_XHDPI: return "xhdpi";break;
        case ScreenData::PixelDensity_XXHDPI: return "xxhdpi";break;
        case ScreenData::PixelDensity_XXXHDPI: return "xxxhdpi";break;
        default:
            return "+ldpi";
            break;
        }
    }

    QString getOrientationString(Qt::ScreenOrientation o){
        switch (o) {
        case Qt::PrimaryOrientation: return tr("Primary"); break;
        case Qt::LandscapeOrientation:return tr("Landscape"); break;
        case Qt::PortraitOrientation: return tr("Portrait"); break;
        case Qt::InvertedLandscapeOrientation: return tr("Inverted Landscape");break;
        case Qt::InvertedPortraitOrientation: return tr("Inverted Portrait"); break;
        default:return "Unknown"; break;
        }
    }

    void setPixelRatio(){
        m_pixelRatio =  ( (double) m_phisicalDpi / m_logicalDpi) ;
        emit pixelRatioChanged();
        qDebug() << Q_FUNC_INFO << "Processed Screen Pixel Ratio::" << m_pixelRatio;
    }


signals:
    void logicalDpiChanged();
    void physicalDpiChanged();
    void orientationChanged(ScreenObject *scrn, Qt::ScreenOrientation o);
    void screenNameChanged();
    void screenChanged();
    void refreshRateChanged();
    void geometryChanged();
    void heightChanged();
    void widthChanged();
    void nativeOrientationChanged();
    void primaryOrientationChanged();
    void measurementChanged();
    void sizeChanged();
    void screenSizeChanged();
    void pixelDensityChanged();
    void diagonalSizeChanged();
    void pixelRatioChanged();

private slots:
    void setDiagonalSize(double dg){m_diagonalSize=dg; emit diagonalSizeChanged(); }
    void setScreenSize(ScreenData::DeviceRange sz){m_deviceSize=sz; emit screenSizeChanged();  }
    void setPixelScale(ScreenData::PixelDensityScale pz){m_pixelScale = pz; emit pixelDensityChanged(); }
    void setScreenGeometry(QRect s){m_height= s.height(); m_width = s.width(); emit geometryChanged(); }
    void setOrientation(Qt::ScreenOrientation e) {m_orientation = e; emit orientationChanged(this, e);}
    void setRefreshRate(qreal r) {m_refreshRate = r; emit refreshRateChanged();}
    void setStringDeviceSize(QString s){qs_deviceSize = s; }

private:
    /** Current Screen */
    double m_logicalDpi;
    double m_phisicalDpi;
    Qt::ScreenOrientation m_orientation;
    QString m_screenName;
    Qt::ScreenOrientation m_primaryOrientation;
    Qt::ScreenOrientation m_nativeOrientation;
    QString qs_deviceSize;
    qreal m_refreshRate;
    int m_height;
    int m_width;
    int m_heightMM;
    int m_widthMM;
    int m_physicalDpiX;
    int m_physicalDpiY;
    int m_logicalDpiX;
    int m_logicalDpiY;
    double m_diagonalSize;
    double m_pixelRatio;

    ScreenData::DeviceRange m_deviceSize;
    ScreenData::DeviceSizes m_devicePixelSize;
    ScreenData::PixelDensityScale m_pixelScale;
};

/*!
 * \brief The ScreenInfo class
 * \ingroup fileselectors
 */
class ScreenInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ScreenObject* primaryScreen READ primaryScreen NOTIFY primaryScreenChanged)
    Q_PROPERTY(QString screenName READ screenName NOTIFY primaryScreenChanged)
public:
    explicit ScreenInfo(QObject *parent = 0);
    ScreenObject* primaryScreen(){return m_primaryScreen;}
    int screenCount(){ return m_screenList.count()-1;}
    QList<QString> screenList(){return m_screenList.keys();}
    QString screenName(){return m_primaryScreen->screenName();}
    double logicalDpi(){return m_primaryScreen->logicalDpi();}
    double physicalDpi(){return m_primaryScreen->physicalDpi();}
    double dp(){ return ( m_primaryScreen->physicalDpi() / 160); }

signals:
    void primaryScreenChanged();
    void screenCountChanged();
    void screenListChanged();
    void screenSizeChanged();
    void screenOrientationChanged(Qt::ScreenOrientation o);


public slots:
    Q_INVOKABLE void setCurrentScreen(QString name){
        if(m_screenList.contains(name)){
            qDebug() << "Set current screen to " << name;
            m_primaryScreen = m_screenList.find(name).value();
            emit primaryScreenChanged();         
        }
    }

   Q_INVOKABLE QString deviceSizeToString(int d){
        switch(d){
        case 4:     return "small"; break;
        case 7:     return "medium"; break;
        case 10:    return "large"; break;
        case 13:    return "xlarge"; break;
        default:    return ""; break;
        }
    }

    void handleScreenObjectOrientationChanged(ScreenObject *obj, Qt::ScreenOrientation o){        
        if(m_screenList.count() == 1){
            emit screenOrientationChanged(o);
        } else{
            if(primaryScreen()->screenName() == obj->screenName()){
                emit screenOrientationChanged(o);
            }
        }
    }

private:
    ScreenObject* m_primaryScreen;
    int m_screenCount;
    QMap<QString, ScreenObject*> m_screenList;
    double m_logicalDpi;
    double m_phisicalDpi;
    QString m_orientation;
    QString m_screenName;


};

#endif // SCREENINFO_H
