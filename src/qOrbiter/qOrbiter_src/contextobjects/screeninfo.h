#ifndef SCREENINFO_H
#define SCREENINFO_H

#include <QObject>
#include <QScreen>
#include <QMap>
#include <QDebug>

class ScreenData : public QObject
{
    Q_OBJECT
public:
    ScreenData() {}
    enum PixelDensityScale{
        PixelDensity_LOW=120,
        PixelDensity_MEDIUM=160,
        PixelDensity_TV=213,
        PixelDensity_HDPI=240,
        PixelDensity_XHDPI=320,
        PixelDensity_XXHDPI= 480,
        PixelDensity_XXXHDPI= 640
    };

    Q_ENUMS(PixelDensityScale)

    enum DeviceSizes{
        Device_240,
        Device_320,
        Device_480,
        Device_540,
        Device_600,
        Device_768,
        Device_800,
        Device_960,
        Device_1024,
        Device_1280
    };

    Q_ENUMS(DeviceSizes)
    virtual ~ScreenData() {}
};

class ScreenObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double logicalDpi READ logicalDpi()  NOTIFY logicalDpiChanged)
    Q_PROPERTY(double physicalDpi READ physicalDpi() NOTIFY physicalDpiChanged)
    Q_PROPERTY(QString orientation READ orientation()  NOTIFY orientationChanged)
    Q_PROPERTY(QString screenName READ screenName()  NOTIFY screenNameChanged)
    Q_PROPERTY(QString primaryOrientation READ primaryOrientation NOTIFY primaryOrientationChanged)
    Q_PROPERTY(QString nativeOrientation READ nativeOrientation() NOTIFY nativeOrientationChanged)
    Q_PROPERTY(int heightMM READ heightMM() NOTIFY measurementChanged  )
    Q_PROPERTY(int widthMM READ widthMM() NOTIFY measurementChanged)
    Q_PROPERTY(int width READ width() NOTIFY sizeChanged)
    Q_PROPERTY(int height READ height() NOTIFY sizeChanged)

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
        m_width(pixel_width)
    {}

    virtual ~ScreenObject() {}

    double logicalDpi(){return m_logicalDpi;}
    double physicalDpi(){return m_phisicalDpi;}
    int height(){return m_height;}
    int width() {return m_width;}
    QString screenName(){return m_screenName;}
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

    int heightMM(){return m_heightMM;}
    int widthMM(){return m_widthMM;}

    QString orientation() {
        return getOrientationString(m_orientation);
    }
    QString primaryOrientation(){
        return getOrientationString(m_primaryOrientation);
    }
    QString nativeOrientation(){
        return getOrientationString(m_nativeOrientation);
    }

public slots:
    void setScreenData(double log, double phys, Qt::ScreenOrientation ori, QString na){

        m_logicalDpi=log;
        emit logicalDpiChanged();

        m_phisicalDpi = phys;
        emit physicalDpiChanged();

        m_orientation = ori;
        emit orientationChanged();

        m_screenName = na;
        emit screenChanged();
    }
    void setScreenGeometry(QRect s){m_height= s.height(); m_width = s.width(); emit geometryChanged(); }
    void setOrientation(Qt::ScreenOrientation e) {m_orientation = e; emit orientationChanged();}
    void setRefreshRate(qreal r) {m_refreshRate = r; emit refreshRateChanged();}

signals:
    void logicalDpiChanged();
    void physicalDpiChanged();
    void orientationChanged();
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
private slots:



private:
    /** Current Screen */
    double m_logicalDpi;
    double m_phisicalDpi;
    Qt::ScreenOrientation m_orientation;
    QString m_screenName;
    Qt::ScreenOrientation m_primaryOrientation;
    Qt::ScreenOrientation m_nativeOrientation;
    qreal m_refreshRate;
    int m_height;
    int m_width;
    int m_heightMM;
    int m_widthMM;
    int m_physicalDpiX;
    int m_physicalDpiY;
    int m_logicalDpiX;
    int m_logicalDpiY;

};

class ScreenInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ScreenObject* primaryScreen READ primaryScreen() NOTIFY primaryScreenChanged)
    Q_PROPERTY(QString screenName READ screenName() NOTIFY primaryScreenChanged)
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


public slots:
    Q_INVOKABLE void setCurrentScreen(QString name){
        if(m_screenList.contains(name)){
            qDebug() << "Poof";
            m_primaryScreen = m_screenList.find(name).value();
            emit primaryScreenChanged();
            qDebug() << "begone";
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
