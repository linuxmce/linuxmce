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

 The purpose of this class is to be a c++ representation of the av codes grid for any given dce object(s) that can have
  avcodes and or other manner of sending it devices.
*/

#ifndef AVCODEGRID_H
#define AVCODEGRID_H

#include <QAbstractItemModel>
#include <contextobjects/avcommand.h>
#include <QModelIndex>

class AvCodeGrid : public QAbstractListModel
{
    Q_OBJECT
public:
#ifdef QT5
    QHash<int, QByteArray> roleNames() const;
   #endif

    explicit AvCodeGrid(AvCommand *m_prototype, QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void appendRow(AvCommand* item);
    void appendRows(const QList<AvCommand*> &items);
    void insertRow(int row, AvCommand* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    AvCommand* takeRow(int row);
    AvCommand* find(const QString &id) const;
    QModelIndex indexFromItem( const AvCommand* item) const;
    AvCommand* currentRow();

signals:
    void deviceAdded();
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const int &sRow);
    void modelAboutToBeReset();
    void modelReset();

public slots:
    void clear();
    void sortModel(int column, Qt::SortOrder order);

private slots:
    void handleItemChange();
    void reset();
    bool resetInternalData();

private:
    AvCommand* m_prototype;
    QList<AvCommand*> m_list;

};

#endif // AVCODEGRID_H
