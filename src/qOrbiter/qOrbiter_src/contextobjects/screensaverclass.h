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

public:
    explicit ScreenSaverClass(QObject *parent = 0);
    QStringList images;
    QString currentImage;
    QImage qi_currentImage;
    bool active;

    bool isReady;
    
signals:
    void imageChanged();
    void screenSaverReady();
    void statusChanged();
    void requestNewImage(QString url);
    void stateChanged();
    
public slots:
    void setImageList(QStringList imgList);
    void clearImageList();
    void setImageData(const uchar* data,int iData_size);
    void setActive(bool state);
    QImage getImageData() {return qi_currentImage;}
    QString getImage();
    void setImage(QString imgUrl);

    bool getStatus();
    void setStatus(bool status);

    void pickImage();

    void selectNew();
    
};

#endif // SCREENSAVERCLASS_H
