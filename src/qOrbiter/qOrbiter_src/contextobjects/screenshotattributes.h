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
*/
#ifndef SCREENSHOTATTRIBUTES_H
#define SCREENSHOTATTRIBUTES_H

#include <QObject>

class screenshotAttributes : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QString attributeName READ getAttributeName NOTIFY nameChanged)
    Q_PROPERTY (QString attributeType READ getAttributeType NOTIFY typeChanged)
    Q_PROPERTY (QString attributeNo READ getAttributeNo NOTIFY idChanged)

public:
    explicit screenshotAttributes(QString name, QString type, QString no,QObject *parent = 0);
    QString attributeName;
    QString attributeType;
    QString attributeNo;

signals:
    void nameChanged();
    void typeChanged();
    void idChanged();


public slots:
    QString getAttributeName () {return attributeName;}
    void setAttributeName(QString incAtr) {attributeName = incAtr;}

    QString getAttributeType() {return attributeType;}
    void setAttributeType (QString incType) {attributeType = incType;}

    QString getAttributeNo( ) {return attributeNo;}
    void setAttributeNo(QString attno){attributeNo = attno;}

};

#endif // SCREENSHOTATTRIBUTES_H
