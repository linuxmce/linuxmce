#ifndef QORBITERLOGGER_H
#define QORBITERLOGGER_H

#include <QObject>
#include <QFile>
#ifndef QT5
#include <QDeclarativeError>
#else
#include <QtQml/QQmlError>
#endif
/*!
 * \brief qorbiterLogger - C++ logging context object.

 *\ingroup qt_controllers
 *This class is responsible for writing logs of activity as QOrbiter runs. It is still quite new and under active development. It
 *automatically begins on desktop builds, but on android it must be started after the application is running.
 */
class QOrbiterLogger : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString logLocation READ getLogLocation WRITE setLogLocation NOTIFY logLocationChanged)/*! The location of the log \ingroup qml_properties */
    Q_PROPERTY(QString userLogMsg READ getUserLogMsg WRITE setUserLogMsg NOTIFY userLogMsgChanged )
    Q_PROPERTY(bool commandLogState READ getCommandLogStatus WRITE setCommandLogStatus NOTIFY cmdLogStateChanged)
    Q_PROPERTY(bool guiLogState READ getGuiLogStatus WRITE setGuiLogStatus NOTIFY guiLogStateChanged)
    Q_PROPERTY(bool qtLogState READ getQtLogState WRITE setQtLogStatus NOTIFY qtLogStateChanged)
public:
    explicit QOrbiterLogger(QObject *parent = 0);

    QString logLocation;
    QString userLogMsg;

    QFile commandFile;
    QFile guiFile;
    QFile skinFile;

    QString commandMsg;
    QString guiMsg;
    QString qtMsg;
    QString skinMsg;
    QString mediaMsg;

    bool commandLogState;
    bool guiLogState;
    bool qtLogState;
    bool loggingEnabled;
    
signals:
    void logLocationChanged(QString l);
    void loggingReady(bool);
    void commandMessageRecieved(QString m);
    void guiMessageRecieved(QString m);
    void qtMessageRecieved(QString m);
    void mediaMessageRecieved(QString m);
    void skinMessageRecieved(QString m);

    void userLogMsgChanged(QString m);

    void cmdLogStateChanged(bool);
    void guiLogStateChanged(bool);
    void qtLogStateChanged(bool);


    
public slots:
    /*!
     * \brief setUserLogMsg - Allows the user to write messages to the log from QML.
     * \param m - The Message you want written in string format.
     *Access this function by using \code logger.setUserLogMsg(msg) \endcode
     * \ingroup qorbiter_commands
     */
    void setUserLogMsg(QString m) {userLogMsg = m; emit userLogMsgChanged(userLogMsg); logUserMessage(userLogMsg);}
    QString getUserLogMsg() {return userLogMsg;}

    void setCommandLogStatus(bool s) {commandLogState = s; emit cmdLogStateChanged(commandLogState);}
    bool getCommandLogStatus() {return commandLogState;}

    void setGuiLogStatus(bool s) {guiLogState = s; emit guiLogStateChanged(guiLogState);}
    bool getGuiLogStatus() {return guiLogState;}

    void setQtLogStatus(bool s) {qtLogState = s; emit qtLogStateChanged(qtLogState);}
    bool getQtLogState(){return qtLogState;}

    void logCommandMessage(QString message);
    void logGuiMessage(QString message);
    void logQtMessage(QString message);
    void logSkinMessage(QString message);
    void logMediaMessage(QString message);

#ifndef QT5
    void logQmlErrors(QList<QDeclarativeError> e );
#else
    void logQmlErrors(QList<QQmlError> e);
#endif

    void logUserMessage(QString message);

    void setLogLocation(QString l);
    QString getLogLocation() {return logLocation;}

private:
    bool writeToFile(QString msg);
    bool initializeCommandFile();
    bool initializeGuiFile();
    bool initializeSkinFile();

    bool writeCommandMessage(QString m);
    bool writeGuiMessage(QString m);
    bool writeSkinMessage(QString m);
    void initLogs();

};

#endif // QORBITERLOGGER_H
