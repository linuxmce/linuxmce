#ifndef TIMECODEMANAGER_H
#define TIMECODEMANAGER_H
/*!
 *
 *\brief This is a c++ based class that is responsible for both consuming and producing timecode.
 *
 *\ingroup context_objects
 *
 *The timecodemanager class has two responsibilities. To communicate with the currently playing media player if possible,
 *and get the timecode, using a tcp socket. It is also responsible for setting up a tcp server in the event that the qOrbiter
 *it is managed by becomes the media playing device.
 *
 *
 */

#include <QtGlobal>
#ifdef QT5
#include <QtQuick/QQuickItem>
#else
#include <QtDeclarative/QDeclarativeItem>
#endif
#include <QtNetwork/QTcpSocket>
#include <QFile>
#include <QStringList>
#include <QRegExp>
#include <QTime>
#include <QVector>
#include "entertainareatimecode.h"

/*!
 * \brief The TimeCodeManager class processes timecode from other media players.
 * The TimeCodeManager reciecves timecode from other LinuxMCE media player and makes it availible for QML to utilize.
 * The timecode is provided in many formats to construct scroll bars, or standard displays.
 * \ingroup context_objects
 */
#ifdef QT5
class TimeCodeManager : public QQuickItem
#else
class TimeCodeManager : public QDeclarativeItem
#endif
{
    Q_OBJECT

    Q_PROPERTY (int tcTotalTime READ getTotalTime WRITE setTotalTime NOTIFY totalTimeChanged)/*!< \brief Access via timecode_properties. Returns QString time in HH:MM:SS:MMM \ingroup timecode_properties */
    Q_PROPERTY (int tcCurrentTime READ getTime WRITE setTime NOTIFY timeChanged)/*!< \brief returns current timecode in mSec  \ingroup timecode_properties*/
    Q_PROPERTY (double runningTimer READ getProgressBar WRITE setProgressBar NOTIFY positionChanged)/*!< \brief Sets a percentage for any connected scrollbars to connect to.  \ingroup timecode_properties*/
    Q_PROPERTY (QString dragTime READ getDragTime WRITE setDragTime NOTIFY dragtimeChanged)/*!< \brief Displays the result of showDragTime  \ingroup timecode_properties*/
    Q_PROPERTY (QString qsTotalTime READ getStringTotalTime WRITE setStringTotalTime NOTIFY totalStringTimeChanged) /*!< \brief The string value of total time in HH:MM:SS  \ingroup timecode_properties*/
    Q_PROPERTY (QString qsCurrentTime READ getStringTime WRITE setStringTime NOTIFY stringTimeChanged)/*!< \brief The string value of the current time in HH:MM:SS  \ingroup timecode_properties*/
    Q_PROPERTY (QString stringPlaybackSpeed READ getStringPlaybackSpeed WRITE setStringPlaybackSpeed NOTIFY sPlaybackSpeedChanged)/*!< \brief String value of playback speed \ingroup timecode_properties*/
    Q_PROPERTY (int playbackSpeed READ getSpeed WRITE setSpeed NOTIFY playbackSpeedChanged)/*!< \brief int value of the playback speed.  \ingroup timecode_properties*/
    Q_PROPERTY(bool portConnected READ getPortConnected WRITE setPortConnected NOTIFY portConnectedChanged)
public:
#ifdef QT5
    explicit TimeCodeManager(QQuickItem *parent = 0);
#else
    explicit TimeCodeManager(QDeclarativeItem *parent = 0);
#endif

    //these ints are used to represent the beginning and end of media in a format for consumption by a scroller
    //it needs to report the positon of the marker, and provide the time readout converted back to the proper HH::MM:SS format
    int tcTotalTime;
    int tcCurrentTime;
    double runningTimer;
    bool portConnected;

    QString qsTotalTime;
    QString qsCurrentTime;

    int playbackSpeed;
    QString stringPlaybackSpeed;
    bool paused;

    QByteArray socketData;

    QString mediaPlayerIp;
    QTcpSocket *dceMediaSocket;
    int port;

    QString qsTitle;
    QString qsChapter;

    QString dragTime;
    int i_dragTime;

    bool reverse;

    QString ObjectToString(const QObject * obj);
signals:
    void totalStringTimeChanged();
    void stringTimeChanged();
    void totalTimeChanged();
    void timeChanged();
    void positionChanged();
    void playbackSpeedChanged();
    void sPlaybackSpeedChanged();
    void tcMessage();
    void seekToTime(QString time);
    void dragtimeChanged();
    void portConnectedChanged();

    
public slots:

    void eventTimeCode(QString time);

    //! Starts the timecode server.
    /*!
     * \brief Starts the timecodeserver with the params.
     * \param server : IP Address to connect to.
     * \param iport : Port of media player to connect to
     */
    void start(QString server, int iport);

    //! Stops the timecode server
    /*!
     * \brief This tells the timecode server to disconnect and close the socket. Occurs when media is
     *stopped or when changing EA's
     */
    void stop();
    void restart();
    void updateTimeCode();
    void convertToSeconds();
    void setPosition();
    /*!
     * \brief showDragTime - Displays the result of seconds passed in.
     * \param seconds
     * This function takes seconds as an input. This makes passing in the current or dragged position of a slider easier than needing to convert.
     * It converts the seconds to user friendly HH:MM::SS and the setDragTime function is called to emit the change for the user interface.
     * Use this in conjunction with finishDragging to update the media position.
     * \ingroup timecode_properties
     */
    void showDragTime(int seconds);


    void setDragTime(QString dtime) {dragTime = dtime; emit dragtimeChanged();}
    QString getDragTime () {return dragTime;}

    void setStringTotalTime(QString qstTime) {qsTotalTime = qstTime; emit totalStringTimeChanged(); }
     QString getStringTotalTime(){return qsTotalTime;}

     void setStringTime(QString qstime) { qsCurrentTime = qstime; setPosition(); emit stringTimeChanged(); }
     QString getStringTime () {return qsCurrentTime;}

    void setTotalTime(int tTime) {tcTotalTime = tTime; emit totalTimeChanged();}
    int getTotalTime(){return tcTotalTime;}

    void setTime(int time) { tcCurrentTime = time; emit timeChanged();}
    int getTime () {return tcCurrentTime;}

    void setSpeed (int speed) { playbackSpeed = speed; emit playbackSpeedChanged();}
    int getSpeed () { return playbackSpeed; }

    void setStringPlaybackSpeed(QString pbs ) {stringPlaybackSpeed = pbs; emit sPlaybackSpeedChanged();}
    QString getStringPlaybackSpeed() {return stringPlaybackSpeed ; }

    void setProgressBar(int progress) {runningTimer = progress; emit positionChanged();}
    int getProgressBar() {return runningTimer;}

    /*!
     * \brief finishDragging - this function takes the last set i_dragTime and sets the media position there.
     * \ingroup timecode_properties
     * \sa MediaScrollBar
     */
    void finishDragging() {
                emit seekToTime(QString::number(i_dragTime));
    }
    

    void setPortConnected (bool isConnected) { if(port != isConnected ) {portConnected= isConnected; emit portConnectedChanged(); } }
    bool getPortConnected() {return portConnected;}

    void setEventTimeCode(QString eaName, QString qsTime, QString qsTotalTime);




};

#endif // TIMECODEMANAGER_H
