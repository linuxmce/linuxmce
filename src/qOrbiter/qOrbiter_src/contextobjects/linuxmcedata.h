#ifndef LINUXMCEDATA_H
#define LINUXMCEDATA_H

#include <QObject>

class LinuxmceData : public QObject
{
    Q_OBJECT
    /*! \warning I am a bad person for this - Langston */

public:
    enum MediaSubTypes{
        TVSHOWS,
        MOVIES,
        HOMEVIDEOS,
        SPORTSEVENTS,
        MUSICVIDEOS,
        ALTERNATIVE,
        POPULARMUSIC,
        CLASSICALMUSIC,
        LEARNING,
        AUDIOBOOKS,
        ARCADE,
        CONSOLE
    };

    Q_ENUMS(MediaSubTypes)
    explicit LinuxmceData(QObject *parent = 0);    

//    static void declareToQml(){
//        qmlRegisterType<LinuxmceData>("DataEnums",1,0,"LinuxmceData");
//    }

signals:

public slots:

};

#endif // LINUXMCEDATA_H
