#ifndef DCESCREENSAVER_H
#define DCESCREENSAVER_H

#ifdef QT5
#include <QQuickPaintedItem>
#include <QSGNode>
#else
#include <QDeclarativeItem>
#endif

#include <QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QPixmap>
#include <QPropertyAnimation>

class DceScreenSaver :
        #ifdef QT5
        public QQuickPaintedItem
        #else
        public QDeclarativeItem
        #endif
{
    Q_OBJECT
    Q_DISABLE_COPY(DceScreenSaver)

    Q_PROPERTY(QString requestUrl READ getRequestUrl WRITE setRequestUrl NOTIFY requestUrlChanged)
    Q_PROPERTY(bool active READ getActive WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool running READ getRunning NOTIFY runningChanged)
    Q_PROPERTY(bool ready READ getReady NOTIFY readyChanged)
    Q_PROPERTY(int interval READ getInterval WRITE setInterval NOTIFY intervalChanged)
    Q_PROPERTY(int pictureCount READ getPictureCount NOTIFY pictureCountChanged)
    Q_PROPERTY(double fadeOpacity READ getFadeOpacity WRITE setFadeOpacity NOTIFY fadeOpacityChanged)
    Q_PROPERTY(double currentScale READ getCurrentScale WRITE setCurrentScale NOTIFY currentScaleChanged)

public:
#ifdef QT5
    DceScreenSaver(QQuickPaintedItem *parent=0);
#else
    DceScreenSaver(QDeclarativeItem *parent = 0);
#endif
    ~DceScreenSaver();


    int opacity;
    bool active;
    bool running;
    bool ready;
    int interval;
    double fadeOpacity;
    double currentScale;
    QString requestUrl;
    int pictureCount;
    QStringList urlList;

    QPixmap currentImage;
    QPixmap surface;
    QPropertyAnimation *fadeAnimation;
    QPropertyAnimation *panAnimation;
    QPropertyAnimation *zoomAnimation;
    QSize endSize;
    int m_animationTimer;
    int tick;


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
    void fadeOpacityChanged();
    void currentScaleChanged();

public slots:

    void setCurrentScale(double d){ currentScale =d;}
    double getCurrentScale(){return currentScale;}

    void setFadeOpacity(double d){ fadeOpacity = d; emit fadeOpacityChanged();}
    double getFadeOpacity(){return fadeOpacity;}

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
    void forceUpdate(){
#ifdef __ANDROID__
        QPixmap t = currentImage.scaled(width(), height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
#else
         QPixmap t = currentImage.scaled(width(), height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
#endif

        if(!t.isNull()){
            currentImage = t;
        }
       #ifdef QT5
        update();
#else
        update(0,0,width(), height());
        qWarning() << "Update forced";
        t=NULL;
#endif
    }
    Q_INVOKABLE  void setImageList(QStringList l);
private:
    void requestImage(QString img);
    QNetworkAccessManager *requestManager;
    QTimer *intervalTimer;
    void beginZoom();
    int scaleFactor();
    QString currentUrl;

private slots:
    void resetPicture() {endSize = currentImage.size();  }

protected:

    void startFadeTimer(int time);
    void stopFadeTimer();


#ifdef QT4
    void paint(QPainter *p ,const QStyleOptionGraphicsItem *option, QWidget *widget );
#elif QT5
   void paint(QPainter *painter);
#endif
    void timerEvent(QTimerEvent *event);



};

QML_DECLARE_TYPE(DceScreenSaver)

#endif // DCESCREENSAVER_H

