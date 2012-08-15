#ifndef SCREENSAVERCLASS_H
#define SCREENSAVERCLASS_H

#include <QObject>
#include <QStringList>
#include <QImage>

class ScreenSaverClass : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QString currentImage READ getImage WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY (bool isReady READ getStatus WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY (int transitionDuration READ getDuration WRITE setDuration NOTIFY durationChanged )

public:
    explicit ScreenSaverClass(QObject *parent = 0);
    QStringList images;
    QString currentImage;
    QImage qi_currentImage;
    QImage qi_newImage;

    bool active;
    int transitionDuration;
    bool isReady;
    bool primary;
    
signals:
    void imageChanged();
    void screenSaverReady();
    void statusChanged();
    void requestNewImage(QString url);
    void stateChanged();
    void durationChanged();

public slots:
    void setImageList(QStringList imgList);
    void clearImageList();
    void setImageData(const uchar* data,int iData_size);
    void setActive(bool state);

    QImage getImageData() {return qi_currentImage;}
    QImage getSecondImageData() {return qi_newImage;}

    QString getImage();

    void setImage(QString imgUrl);

    bool getStatus();
    void setStatus(bool status);

    void pickImage();

    void selectNew();

    void setDuration(int d) {transitionDuration = d; emit durationChanged();}
    int getDuration() {return transitionDuration;}
    
};

#endif // SCREENSAVERCLASS_H
