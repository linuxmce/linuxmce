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

#ifndef PLAYLISTITEMCLASS_H
#define PLAYLISTITEMCLASS_H

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

The purpose of this class is to keep a reference to all items in the current playlist, to notify the ui of playlist changes,
  and to eventually allow the user to change the order of the playlist on the fly. role names subject to change
  */


#include <QVariant>
#include <QImage>
#include <QMap>
#include <QDebug>
#ifdef debug
#include <QDebug>
#endif

class PlaylistItemClass: public QObject
{
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::DisplayRole+1,
        IndexRole =Qt::DisplayRole+2,
        PathRole= Qt::DisplayRole+3,
        FKRole = Qt::DisplayRole+4,
        StateRole = Qt::DisplayRole+5

    };

public:
    PlaylistItemClass() {}
    explicit PlaylistItemClass( QString ident,  QString path,  int index);
    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;

    inline QString id() const {  return QString::number(m_index); }
    inline QString name() const { return m_name; }
    inline int index() const { return m_index; }
    inline QString path() const { return m_fk_file; }

    inline QMap <QString*, int> attributes() const {return m_mapAttrib;}

    inline void setActive(bool b) { isActive = b;}
    Q_INVOKABLE inline bool getActive () {return isActive;}
    bool isActive;

public slots:
    void addedToModel();
    void destruct() {
//        qDebug() << "calling destructor for item.";
       this->deleteLater();
    }

private:
    QString m_name;
    int m_index;
    QString m_path;
    QString m_fk_file;
    QMap <QString*, int> m_mapAttrib;


signals:
    void imageChanged();
    void dataChanged();
};

#endif // PLAYLISTITEMCLASS_H
