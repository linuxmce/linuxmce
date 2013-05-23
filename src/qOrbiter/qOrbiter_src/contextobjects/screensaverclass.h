#ifndef SCREENSAVERCLASS_H
#define SCREENSAVERCLASS_H

#include <QObject>
#include <QStringList>
#include <QImage>
#include <QTimer>

class ScreenSaverClass : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QString currentImage READ getImage WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY (bool isReady READ getStatus WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY (int transitionDuration READ getDuration WRITE setDuration NOTIFY durationChanged )
    Q_PROPERTY (int timerInterval READ getInterval WRITE setInterval NOTIFY intervalChanged)
    Q_PROPERTY (bool active READ getActive WRITE setActive NOTIFY activatedChanged)

public:
    explicit ScreenSaverClass(QObject *parent = 0);
    QStringList images;
    QString currentImage;
    QImage qi_currentImage;  

    bool active;
    int transitionDuration;
    bool isReady;
    bool primary;

    int timerInterval;

     QTimer picChanger;


signals:
    void imageChanged();
    void screenSaverReady();
    void statusChanged();
    void requestNewImage(QString url);
    void stateChanged();
    void durationChanged();
    void activatedChanged();
    void intervalChanged();

public slots:
    void setImageList(QStringList imgList);
    void clearImageList();
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
    void setInterval (int ms) {timerInterval = ms; emit intervalChanged();}

    void setDuration(int d) {transitionDuration = d; picChanger.stop(); picChanger.start(transitionDuration); emit durationChanged();}
    int getDuration() {return transitionDuration;}

};

#endif // SCREENSAVERCLASS_H
