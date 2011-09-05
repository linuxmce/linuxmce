#ifndef SCREENPARAMSCLASS_H
#define SCREENPARAMSCLASS_H

/*
The purpose of this class is to track screen parameters for certain screens. Params are not always used and/or relevant
to a screen, but in the case they are, there should be proper handling. The purpose of this class is to be an independant
reference to the current screen params and relevant getters and setters. Also, will be extended to deal with message passing
back to the router in the case of special buttons like the 'yes and no' prompts or any of the other prompts.
getter and setter un
  */

#include <QObject>
#include <QMap>

class ScreenParamsClass : public QObject
{
    Q_OBJECT
    Q_PROPERTY (int deviceTo READ getReciever WRITE setReciever NOTIFY recieverChanged)
    Q_PROPERTY (int deviceFrom READ getSender WRITE setSender NOTIFY senderChanged)

public:
    explicit ScreenParamsClass(QObject *parent = 0);

    int deviceTo;
    void setReciever(long devto) { devto = deviceTo; emit recieverChanged();}
    int getReciever() {return deviceTo;}

    int deviceFrom;
    void setSender(long sender) { sender = deviceFrom; emit senderChanged();}
    int getSender() {return deviceFrom;}

    int deviceTemplate;
    int deviceCategory;
    int currentScreen;
    QMap <int, int> paramDevices;

    int Param1;
    int Param2;
    int Param3;
    int Param4;
    QString qs_Param1;
    QString qs_Param2;
    QString qs_Param3;
    QString qs_Param4;


signals:
    void recieverChanged();
    void senderChanged();

public slots:


};

#endif // SCREENPARAMSCLASS_H
