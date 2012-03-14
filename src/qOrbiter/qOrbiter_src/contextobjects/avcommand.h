#ifndef AVCOMMAND_H
#define VCOMMAND_H

#include <QObject>

class AvCommand : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QString qs_command READ getCommandName WRITE setCommandName NOTIFY commandNameSet )
    Q_PROPERTY (bool learning READ getLearnState WRITE setLearnState NOTIFY learningChanged)
    Q_PROPERTY (int i_commandnum READ getCommandNum WRITE setCommandNum NOTIFY commandSet)
public:
    explicit AvCommand(int inc_command, QString command_name, bool state, QObject *parent = 0);
    int i_commandnum;
    bool learning;
    QString qs_command;

signals:
    void commandSet();
    void learningChanged();
    void commandNameSet();

public slots:
    void setCommandNum(int d) {i_commandnum=d; emit commandSet();}
    int getCommandNum() { return i_commandnum;}

    void setLearnState(bool state) {learning = state; emit learningChanged();}
    bool getLearnState() {return learning;}

    void setCommandName(QString d) {qs_command = d; emit commandNameSet();}
    QString getCommandName() {return qs_command;}

};

#endif // AvCommand_H
