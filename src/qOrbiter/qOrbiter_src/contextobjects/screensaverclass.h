#ifndef SCREENSAVERCLASS_H
#define SCREENSAVERCLASS_H

#include <QObject>
#include <QStringList>

class ScreenSaverClass : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QString currentImage READ getImage WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY (bool isReady READ getStatus WRITE setStatus NOTIFY statusChanged)

public:
    explicit ScreenSaverClass(QObject *parent = 0);
    QStringList images;
    QString currentImage;
    bool isReady;
    
signals:
    void imageChanged();

    void screenSaverReady();
    void statusChanged();
    
public slots:
    void setImageList(QStringList imgList);
    void clearImageList();

    QString getImage();
    void setImage(QString imgUrl);

    bool getStatus();
    void setStatus(bool status);

    void pickImage();

    void selectNew();
    
};

#endif // SCREENSAVERCLASS_H
