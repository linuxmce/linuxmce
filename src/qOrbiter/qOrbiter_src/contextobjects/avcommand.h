#ifndef AVCOMMAND_H
#define VCOMMAND_H

#include <QObject>
#include <QHash>
#include <QVariant>

class AvCommand : public QObject
{
    Q_OBJECT
    enum Roles {
        NameRole = Qt::UserRole+1,
        ParentRole =Qt::UserRole+2,
        StatusRole= Qt::DisplayRole+3,
        CommandRole = Qt::DisplayRole
    };

public:
    AvCommand() {}
    explicit AvCommand(int inc_command, QString command_name, bool state, int iParent, QObject *parent = 0);
    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;

    int command() const { return i_commandnum;}
    int cmdParent() const { return i_parentDevice;}
    bool learnState() const {return learning;}
    QString name() const {return qs_command;}

signals:
    void dataChanged();

public slots:

 private:
    int i_commandnum;
    int i_parentDevice;
    bool learning;
    QString qs_command;

};

#endif // AvCommand_H
