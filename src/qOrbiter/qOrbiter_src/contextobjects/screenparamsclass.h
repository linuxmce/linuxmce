/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
   Langston Ball  golgoj4@gmail.com
    QOrbiter is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QOrbiter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QOrbiter.  If not, see <http://www.gnu.org/licenses/>.

The purpose of this class is to track screen parameters for certain screens. Params are not always used and/or relevant
to a screen, but in the case they are, there should be proper handling. The purpose of this class is to be an independant
reference to the current screen params and relevant getters and setters. Also, will be extended to deal with message passing
back to the router in the case of special buttons like the 'yes and no' prompts or any of the other prompts.
It needs to be expanded to create device numberlists for now playing and other situations where the contextual data may be relevant
 */

#ifndef SCREENPARAMSCLASS_H
#define SCREENPARAMSCLASS_H

#include <QObject>
#include <QMap>
#ifdef debug
#include <QDebug>
#endif


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

    Q_INVOKABLE  QString getParam(int key) {return paramDevices.value(key);}

    int getSender() {return deviceFrom;}

    int deviceTemplate;
    int deviceCategory;
    int currentScreen;
    QMap <int, QString> paramDevices;

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
    void setSender(long sender) { sender = deviceFrom; emit senderChanged();}
    void addParam(QString val, int key) {paramDevices.insert(key, val); }
    void clear();

};

#endif // SCREENPARAMSCLASS_H
