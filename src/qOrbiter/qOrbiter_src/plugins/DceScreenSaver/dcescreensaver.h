#ifndef DCESCREENSAVER_H
#define DCESCREENSAVER_H

#include <QDeclarativeItem>
#include <QTimer>
#include <QtOpenGL>
#include <QtNetwork/QNetworkAccessManager>

class DceScreenSaver : public QDeclarativeItem
{
    Q_OBJECT
    Q_DISABLE_COPY(DceScreenSaver)
    Q_PROPERTY(QString requestUrl READ getRequestUrl WRITE setRequestUrl NOTIFY requestUrlChanged)
    Q_PROPERTY(bool active READ getActive WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool running READ getRunning NOTIFY runningChanged)
    Q_PROPERTY(bool ready READ getReady NOTIFY readyChanged)
    Q_PROPERTY(int interval READ getInterval WRITE setInterval NOTIFY intervalChanged)
    Q_PROPERTY(int pictureCount READ getPictureCount NOTIFY pictureCountChanged)

public:
    DceScreenSaver(QDeclarativeItem *parent = 0);
    ~DceScreenSaver();


    bool active;
    bool running;
    bool ready;
    int interval;
    QString requestUrl;
    int pictureCount;
    QStringList urlList;

    QImage currentImage;

signals:
    void paintedFrame();
    void newPictureRecieved();
    void imageListRead();
    void requestUrlChanged();
    void intervalChanged();
    void activeChanged();
    void runningChanged();
    void readyChanged();
    void pictureCountChanged();
    void urlListReady();

public slots:
void getNextImage();
void processImageData(QNetworkReply*r);

    void setPictureCount(int p){ pictureCount = p; emit pictureCountChanged();}
    int getPictureCount () {return pictureCount;}
    void setRequestUrl(QString s){ requestUrl = s; emit requestUrlChanged();}
    QString getRequestUrl(){return requestUrl;}

    void setActive(bool b){active = b;
                           if(!b){
                               intervalTimer->stop();
                               setRunning(false);
                           } else {
                               setRunning(true);
                               intervalTimer->start(interval);
                           }
                           emit activeChanged();
                          }
    bool getActive(){return active;}

    void setInterval(int i){interval = i; intervalTimer->start(i); emit intervalChanged();}
    int getInterval(){return interval;}

    bool getRunning(){return running;}
    void setRunning(bool r) {running = r; emit runningChanged();}

    void setReady(bool r){ready = r; readyChanged();}
    bool getReady() {return ready;}

  Q_INVOKABLE  void setImageList(QStringList l);
private:
    void requestImage(QString img);
    QNetworkAccessManager *requestManager;
    QTimer *intervalTimer;



    QString currentUrl;


protected:

    void paint(QPainter *p ,const QStyleOptionGraphicsItem *option, QWidget *widget);
    void update();
};

QML_DECLARE_TYPE(DceScreenSaver)

#endif // DCESCREENSAVER_H

