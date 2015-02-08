#ifndef SCREENINFO_H
#define SCREENINFO_H

#include <QObject>
#include <QScreen>
#include <QMap>
#include <QDebug>

class ScreenObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double logicalDpi READ logicalDpi()  NOTIFY logicalDpiChanged)
    Q_PROPERTY(double physicalDpi READ physicalDpi() NOTIFY physicalDpiChanged)
    Q_PROPERTY(QString orientation READ orientation()  NOTIFY orientationChanged)
    Q_PROPERTY(QString screenName READ screenName()  NOTIFY screenNameChanged)
public:
    ScreenObject():
        m_logicalDpi(0.0),
        m_phisicalDpi(0.0),
        m_orientation(Qt::PortraitOrientation),
        m_screenName("undefined")
    {}

    ScreenObject(double logDpi, double physDpi, Qt::ScreenOrientation oriented, QString name):
        m_logicalDpi(logDpi),
        m_phisicalDpi(physDpi),
        m_orientation(oriented),
        m_screenName(name)
    {}

    virtual ~ScreenObject() {}

    double logicalDpi(){return m_logicalDpi;}
    double physicalDpi(){return m_phisicalDpi;}
    QString screenName(){return m_screenName;}
    QString orientation() {
        switch (m_orientation) {
        case Qt::PrimaryOrientation:
            return tr("Primary");
            break;
        case Qt::LandscapeOrientation:
            return tr("Landscape");
            break;
        case Qt::PortraitOrientation:
            return tr("Portrait");
            break;
        case Qt::InvertedLandscapeOrientation:
            return tr("Inverted Landscape");
            break;
        case Qt::InvertedPortraitOrientation:
            return tr("Inverted Portrait");
        default:
            return "Unknown";
            break;
        }

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

signals:
    void logicalDpiChanged();
    void physicalDpiChanged();
    void orientationChanged();
    void screenNameChanged();
    void screenChanged();

private:
    /** Current Screen */
    double m_logicalDpi;
    double m_phisicalDpi;
    Qt::ScreenOrientation m_orientation;
    QString m_screenName;
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
