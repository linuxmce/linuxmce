#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

#include <QObject>
#include <qdeclarative.h>
#include <QDeclarativeItem>
#include <qMediaPlayer/qMediaPlayer.h>
#include "../qOrbiter/qOrbiter_src/plugins/AudioVisual/videoplayerbase.h"
#include "../qOrbiter/qOrbiter_src/plugins/AudioVisual/audioplayerbase.h"
#include <colorfilterproxywidget.h>
#include <QTime>

using namespace DCE;
using namespace Qt;

class MediaManager : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ getConnectionStatus WRITE setConnectionStatus NOTIFY connectedChanged)
    Q_PROPERTY(QString currentStatus READ getCurrentStatus WRITE setCurrentStatus NOTIFY currentStatusChanged)
public:
    explicit MediaManager(QDeclarativeItem *parent = 0);

    qMediaPlayer *mediaPlayer;
    QString currentStatus;
    QWidget *window;
    VideoPlayerBase *videoOutput;
    AudioPlayerBase *audioOutput;
    ColorFilterProxyWidget *filterProxy;

    bool connected;
    QString serverAddress;
    int deviceNumber;

signals:
    bool connectedChanged();
    QString currentStatusChanged();


public slots:

    void setCurrentStatus(QString s) {currentStatus = QTime::currentTime().toString()+"::"+s; emit currentStatusChanged();}
    QString getCurrentStatus() {return currentStatus;}

    void setConnectionDetails(int t, QString r);

    void setConnectionStatus(bool stat){connected = stat; emit connectedChanged();}
    bool getConnectionStatus(){return connected;}

private:
 void initializePlayer();
};

#endif // MEDIAMANAGER_H
