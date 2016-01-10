#ifndef DCESCREENSAVER_H
#define DCESCREENSAVER_H

#ifdef QT5
#include <QQuickPaintedItem>
#include <QSGNode>
#else
#include <QDeclarativeItem>
#endif

#include <QTimer>
#include <QTime>
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
    Q_PROPERTY(bool active READ getActive WRITE setScreenSaverActive NOTIFY activeChanged)
    Q_PROPERTY(bool running READ getRunning NOTIFY runningChanged)
    Q_PROPERTY(bool ready READ getReady NOTIFY readyChanged)
    Q_PROPERTY(int interval READ getInterval WRITE setInterval NOTIFY intervalChanged)
    Q_PROPERTY(int pictureCount READ getPictureCount NOTIFY pictureCountChanged)
    Q_PROPERTY(double fadeOpacity READ getFadeOpacity WRITE setFadeOpacity NOTIFY fadeOpacityChanged)
    Q_PROPERTY(double currentScale READ getCurrentScale WRITE setCurrentScale NOTIFY currentScaleChanged)
    Q_PROPERTY(QString currentImageName READ getCurrentImageName NOTIFY currentImageNameChanged)
    Q_PROPERTY(bool useAnimation READ getUseAnimation WRITE setUseAnimation NOTIFY useAnimationChanged)
    Q_PROPERTY(QString debugInfo READ getDebugInfo WRITE setDebugInfo NOTIFY debugInfoChanged)
    Q_PROPERTY(bool enableDebug READ getEnableDebug WRITE setEnableDebug NOTIFY enableDebugChanged)
    Q_PROPERTY(bool fadeEnabled READ getFadeEnabled WRITE setFadeEnabled NOTIFY fadeEnabledChanged)
    Q_PROPERTY(bool zoomEnabled READ getZoomEnabled WRITE setZoomEnabled NOTIFY zoomEnabledChanged)
public:
#ifdef QT5
    DceScreenSaver(QQuickPaintedItem *parent=0);
#else
    DceScreenSaver(QDeclarativeItem *parent = 0);
#endif
    ~DceScreenSaver();

    QString debugInfo;


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
    QString currentImageName;
    bool fadeEnabled;
    bool zoomEnabled;
    bool useAnimation;
    int transitionTime;

    int incomingTransition_x;
    int incomingTransition_y;

    int currentTransition_x;
    int currentTransition_y;

    QPixmap currentImage;
    QPixmap surface;
    QPropertyAnimation *fadeAnimation;
    QPropertyAnimation *panAnimation;
    QPropertyAnimation *zoomAnimation;
    QSize endSize;
    int m_animationTimer;
    int tick;
    bool imgSet;
    bool enableDebug;


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
    void currentImageNameChanged();
    void useAnimationChanged();
    void debugInfoChanged();
    void enableDebugChanged();
    void fadeEnabledChanged();
    void zoomEnabledChanged();
    void transitionFinished();
public slots:

    void setZoomEnabled (bool z){ if(z!=zoomEnabled) { zoomEnabled = z; emit zoomEnabledChanged();}  }
    bool getZoomEnabled(){return zoomEnabled;}

    void setFadeEnabled(bool f){ if(fadeEnabled !=f) {fadeEnabled = f; emit fadeEnabledChanged();}  }
    bool getFadeEnabled(){return fadeEnabled;}

    void setDebugInfo(QString i){ if( enableDebug && debugInfo != i) { debugInfo = QTime::currentTime().toString()+"DceScreenSaver::"+i; emit debugInfoChanged();}  }
    QString getDebugInfo(){return debugInfo;}

    void setEnableDebug(bool e){if (enableDebug != e) { enableDebug =e; emit enableDebugChanged(); }  }
    bool getEnableDebug(){ return enableDebug;}

    void setUseAnimation(bool ani){ if(ani!= useAnimation) {useAnimation = ani; emit useAnimationChanged(); }   }
    bool getUseAnimation(){return useAnimation;}

    void setCurrentImageName(QString n) {
        if(!n.isEmpty() ){
            n = currentImageName;
        }                                        }
    QString getCurrentImageName(){return currentImageName;}

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

    void setScreenSaverActive(bool b){active = b;
                                      if(!b){
                                          intervalTimer->stop();

                                          zoomAnimation->stop();
                                          fadeAnimation->stop();
                                          setRunning(false);
                                      } else {
                                          setRunning(true);
                                          intervalTimer->start(interval);

                                      }
                                                                    emit activeChanged();
                                     }
    bool getActive(){return active;}

    void setInterval(int i){if(interval!=i) {interval = i; intervalTimer->start(i); startFadeTimer(); emit intervalChanged();}}
    int getInterval(){return interval;}

    bool getRunning(){return running;}
    void setRunning(bool r) {running = r; emit runningChanged(); intervalTimer->start(); }

    void setReady(bool r){ready = r; readyChanged();}
    bool getReady() {return ready;}
    void forceUpdate(){
        qDebug() <<"dcescreensaver::size changed";
        surface= currentImage.scaled(this->width(), this->height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        setDebugInfo("Update forced");
        this->update();
    }
    Q_INVOKABLE  void setImageList(QStringList l);
private:
    void requestImage(QString img);
    QNetworkAccessManager *requestManager;
    QTimer *intervalTimer;

    int scaleFactor();
    QString currentUrl;

private slots:
    void resetPicture() { endSize = currentImage.size();  }
    void updateZoom (QVariant zoomVal){ setCurrentScale(zoomVal.toDouble()); this->update();}

protected:

    void startFadeTimer();
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

