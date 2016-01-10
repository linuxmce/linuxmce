#ifndef BOOKMARKITEM_H
#define BOOKMARKITEM_H

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

  the purpose of this class is to enable a representation of floorplan devices for qml
  it will allow an abstraction of all aspects of the device and allow for flexibility
  in both the display and use of floorplan devices.
  */

#include <QImage>
class BookmarkItem : public QObject
{
    Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
    Q_OBJECT
public:
    explicit BookmarkItem(QString incTitle, QImage incMediaImage, QImage incBookmarkImage, QObject *parent = 0);
    QString title;
    QImage mediaImage;
    QImage bookmarkImage;
signals:
    void titleChanged();
    void mediaImageChanged();
    void bookmarkImageChanged();
public slots:
    void setTitle(QString inc_title) {title = inc_title; emit titleChanged();}
    QString getTitle() {return title;}

    void setMediaImage (QImage inc_mediaImage) {mediaImage = inc_mediaImage;}

    void setBookmarkImage (QImage inc_bookmark) {bookmarkImage = inc_bookmark;}
    
};

#endif // BOOKMARKITEM_H
