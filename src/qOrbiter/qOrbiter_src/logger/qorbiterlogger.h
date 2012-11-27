#ifndef QORBITERLOGGER_H
#define QORBITERLOGGER_H

#include <QObject>
#include <QFile>
#include <QDeclarativeError>
class QOrbiterLogger : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString logLocation READ getLogLocation WRITE setLogLocation NOTIFY logLocationChanged)
public:
    explicit QOrbiterLogger(QObject *parent = 0);

    QString logLocation;
    QFile commandFile;
    QFile guiFile;
    QFile skinFile;

    QString commandMsg;
    QString guiMsg;
    QString qtMsg;
    QString skinMsg;
    QString mediaMsg;
    
signals:
    void logLocationChanged(QString l);
    void loggingReady(bool);
    void commandMessageRecieved(QString m);
    void guiMessageRecieved(QString m);
    void qtMessageRecieved(QString m);
    void mediaMessageRecieved(QString m);
    void skinMessageRecieved(QString m);
    
public slots:
    void logCommandMessage(QString message);
    void logGuiMessage(QString message);
    void logQtMessage(QString message);
    void logSkinMessage(QString message);
    void logMediaMessage(QString message);
    void logQmlErrors(QList<QDeclarativeError> e );

    void setLogLocation(QString l) {logLocation = l; emit logLocationChanged(logLocation);}
    QString getLogLocation() {return logLocation;}

private:
    bool writeToFile(QString msg);
    bool initializeCommandFile();
    bool initializeGuiFile();
    bool initializeSkinFile();

    bool writeCommandMessage(QString m);
    bool writeGuiMessage(QString m);
    bool writeSkinMessage(QString m);
};

#endif // QORBITERLOGGER_H
