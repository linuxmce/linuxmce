#ifndef SCREENSAVER_H
#define SCREENSAVER_H

#include <QDeclarativeItem>
#include <QTimer>
#include <QList>
#include <QtNetwork/QNetworkReply>

class ScreenSaver : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY (QString currentImage READ getImage WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY (bool isReady READ getStatus WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY (int transitionDuration READ getDuration WRITE setDuration NOTIFY durationChanged )
    Q_PROPERTY (int timerInterval READ getInterval WRITE setInterval NOTIFY intervalChanged)
    Q_PROPERTY (bool active READ getActive WRITE setActive NOTIFY activatedChanged)
    Q_PROPERTY(QString newImageUrl READ getNewImageUrl WRITE setNewImageUrl NOTIFY newImageUrlChanged)
public:
    explicit ScreenSaver(QDeclarativeItem *parent = 0);
    QStringList images;

    bool active;
    int transitionDuration;
    bool isReady;
    bool primary;
   QColor color;

    QImage qi_currentImage;
    QString currentImage;
    QString newImageUrl;

      QTimer picChanger;
      int timerInterval;

signals:
    void imageChanged();
    void screenSaverReady();
    void statusChanged();
    void requestNewImage();
    void stateChanged();
    void durationChanged();
    void activatedChanged();
    void intervalChanged();
    void newImageUrlChanged();

public slots:


    void setImageData(QByteArray buffer);
    void setActive(bool state);
    bool getActive() {return active;}

    QImage getImageData() {return qi_currentImage;}
    QString getImage();
    QImage qmlImage() { return qi_currentImage;}

    void setImage(QString imgUrl);

    bool getStatus();
    void setStatus(bool status);

    void pickImage();

    void selectNew();
    int getInterval() {return timerInterval;}
    void setInterval (int ms) {timerInterval = ms; picChanger.stop(); picChanger.start(timerInterval); emit intervalChanged();}

    void setDuration(int d) {transitionDuration = d; picChanger.stop(); picChanger.start(transitionDuration); emit durationChanged();}
    int getDuration() {return transitionDuration;}

    void setNewImageUrl(QString l){ newImageUrl = l; fetchImageData(); emit newImageUrlChanged();}
    QString getNewImageUrl(){return newImageUrl;}

    void fetchImageData();
    void processImageData(QNetworkReply *imgData);

protected:
   void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
};

#endif // SCREENSAVER_H
