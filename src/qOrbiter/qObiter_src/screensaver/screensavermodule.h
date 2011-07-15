#ifndef SCREENSAVERMODULE_H
#define SCREENSAVERMODULE_H

#include <QDeclarativeItem>

class ScreenSaverModule : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QUrl image
                READ image
                WRITE setImage
                NOTIFY imageChanged)
public:
    explicit ScreenSaverModule(QObject *parent = 0);


signals:
    void imageChanged();
    void loadingError();

public slots:
    QUrl image( ) {
                    QUrl local2;
                    local2 = m_imgUrl->toString();
                    return local2;
                   }

  void setImage(const QUrl &q)  {
                        QUrl* local = new QUrl(q);
                        m_imgUrl = local;
                        emit imageChanged();
                                 }

private:
    QUrl *m_imgUrl;
};

#endif // SCREENSAVERMODULE_H
