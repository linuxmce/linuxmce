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

  This class acts as a controller for managing playlist items currently. It will be expanded to encompass the
  existing orbiter playlist management functions like saving to public and private users, reordering, etc.
  */

#ifndef PLAYLISTCLASS_H
#define PLAYLISTCLASS_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <contextobjects/playlistitemclass.h>

class PlaylistClass: public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int currentIndex READ getCurrentIndex WRITE setCurrentIndex NOTIFY activeItemChanged)
    Q_PROPERTY(int currentPosition READ getCurrentPosition NOTIFY currentPositionChanged)

public:

    explicit PlaylistClass(PlaylistItemClass* prototype, QObject* parent = 0);


    ~PlaylistClass();
#ifdef QT5
    QHash <int, QByteArray> roleNames() const ;
#endif
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void clear();
    void appendRows(const QList<PlaylistItemClass*> &items);
    void insertRow(int row, PlaylistItemClass* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    PlaylistItemClass* takeRow(int row);
    PlaylistItemClass* find(const QString &id) const;
    QModelIndex indexFromItem( const PlaylistItemClass* item) const;
    PlaylistItemClass* currentRow();
    int currentIndex;
    int totalSize;
    int currentPosition;
    void sortModel(int column, Qt::SortOrder order);


signals:
    void ItemAdded();
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const int &sRow);
    void playlistReady();
    void activeItemChanged();
    void modelAboutToBeReset();
    void modelReset();
    void currentPositionChanged();
    void totalSizeChanged();

public slots:
    bool checkDupe(QString name, int position);
    void setItemStatus(int b);
    void setDceSize(int d){totalSize=d;emit totalSizeChanged();}
    int count() {return m_list.count();}

    void setCurrentIndex(int i);
    int getCurrentIndex();

    void appendRow(PlaylistItemClass *item);
    void populate();
    int getCurrentPosition(){return currentPosition;}
private slots:
    void setCurrentPosition(int t){currentPosition = t; emit currentPositionChanged();}
    void handleItemChange();
    void itemDeletion() { /*qDebug() << "Playlist Item Destroyed::" << m_list.size();*/ }

private:
    PlaylistItemClass* m_prototype;
    QList<PlaylistItemClass*> m_list;
     bool resetInternalData();
};

#endif // PLAYLISTCLASS_H
