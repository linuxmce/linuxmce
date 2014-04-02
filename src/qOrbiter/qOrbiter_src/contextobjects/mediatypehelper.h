#ifndef MEDIATYPEHELPER_H
#define MEDIATYPEHELPER_H

#include <QObject>

class MediaTypeHelper : public QObject
{
    Q_OBJECT
public:
    enum MediaTypes{
        LINUXMCE_LIVE_TV=1,
        LINUXMCE_CD=2,
        LINUXMCE_DVD,
        STORED_AUDIO=4,
        STORED_VIDEO=5,
        LIVE_RADIO=6,
        LINUXMCE_PICTURES=7,
        NP_LIVETV=11,
        NP_DVD=12,
        NP_OTARADIO=13,
        NP_SAT_RADIO=14,
        NP_CABLE_RADIO=15,
        LINUXMCE_STREAMED_AUDIO=43,
        LINUXMCE_STREAMED_VIDEO=44,
        LINUXMCE_YOUTUBE=63,
        NP_NETWORK_MEDIA=66
    };
    Q_ENUMS(MediaTypes)
    explicit MediaTypeHelper(QObject *parent = 0);

signals:

public slots:

};

#endif // MEDIATYPEHELPER_H
