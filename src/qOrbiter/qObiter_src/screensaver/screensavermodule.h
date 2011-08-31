#ifndef SCREENSAVERMODULE_H
#define SCREENSAVERMODULE_H

#include <QObject>
#include <QUrl>
#include <QStringList>


class ScreenSaverModule : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QUrl current_image READ getImage WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY (QString testtext READ getTest)

public:
    explicit ScreenSaverModule(QObject *parent = 0);

    QUrl current_image;
    QStringList imageList;
    QString testtext;
    QString getTest() {return testtext;}

signals:
    void imageChanged();
    void loadingError();

public slots:

    void nextImage();
    QUrl getImage( ) {return current_image;}
    void setImage(const QUrl &q)  { current_image = q; emit imageChanged(); }
    int getNext();


private:
    QUrl *m_imgUrl;

};

#endif // SCREENSAVERMODULE_H
