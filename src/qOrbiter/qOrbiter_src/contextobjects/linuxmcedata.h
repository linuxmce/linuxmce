#ifndef LINUXMCEDATA_H
#define LINUXMCEDATA_H

#include <QObject>

class LinuxmceData : public QObject
{
    Q_OBJECT
    /*! \warning I am a bad person for this - Langston */

public:
    enum MediaSubTypes{
        TVSHOWS=1,
        MOVIES=2,
        HOMEVIDEOS=3,
        SPORTSEVENTS=4,
        MUSICVIDEOS=5,
        ALTERNATIVE=6,
        POPULARMUSIC=7,
        CLASSICALMUSIC=8,
        LEARNING=9,
        AUDIOBOOKS=10,
        ARCADE=11,
        CONSOLE=12
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
