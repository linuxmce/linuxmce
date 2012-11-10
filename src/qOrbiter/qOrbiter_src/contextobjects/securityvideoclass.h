#ifndef SECURITYVIDEOCLASS_H
#define SECURITYVIDEOCLASS_H

#include <QObject>
#include <QMap>
#include <QTimer>
#include <QImage>
#include <QTime>
#include <QImage>


class SecurityVideoClass : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QString timestamp WRITE setTimeStamp READ getTimeStamp NOTIFY imageUpdated())


public:
    explicit SecurityVideoClass(QObject *parent = 0);
    QMap<int, QImage> cameras;
    QTimer *requestInterval;
    QImage currentFrame;
    QString timestamp;

    QString getTimeStamp () {return QTime::currentTime().toString(); }
    void setTimeStamp(QString t) {timestamp = QTime::currentTime().toString(); emit imageUpdated();}


signals:
    void imageUpdated();

public slots:
    void setCameraImage(int cam, QImage img) { if(!img.isNull()) {cameras.find(cam).value()= img;}  }


};

#endif // SECURITYVIDEOCLASS_H
