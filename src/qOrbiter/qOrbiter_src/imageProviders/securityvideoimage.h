#ifndef SECURITYVIDEOIMAGE_H
#define SECURITYVIDEOIMAGE_H

#include <QDeclarativeImageProvider>
#include <contextobjects/securityvideoclass.h>
#include <QDebug>

class SecurityVideoImage : public QObject, public QDeclarativeImageProvider
{
    Q_OBJECT
public:
    explicit SecurityVideoImage(SecurityVideoClass *ref);

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize)
    {

        QImage result;
        QImage key = securityreference->currentFrame;

        if (requestedSize.isValid()) {
            result = key.scaled(requestedSize);
        } else {
            result = key;
        }
        return result;

    }

signals:

public slots:

private:
    SecurityVideoClass *securityreference;
};

#endif // SECURITYVIDEOIMAGE_H
