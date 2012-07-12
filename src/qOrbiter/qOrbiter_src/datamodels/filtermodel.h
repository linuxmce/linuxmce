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

   The status of this class is unknown (yes i know i wrote it). It may be deleted or not

   This is another example of a model/item pair as its child is filtermodelitem. These two classes work inconjunction to manage the
   states of the selected type of filter (attributes) items in which it is chose to hold.

*/

#ifndef FILTERMODEL_H
#define FILTERMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <datamodels/DataModelItems/filtermodelitem.h>
#include <QStringList>

class FilterModelItem;

class FilterModel: public QAbstractListModel
{
  Q_OBJECT


public:
  explicit FilterModel(FilterModelItem* prototype, QObject* parent = 0);
  ~FilterModel();
  Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  void appendRow(FilterModelItem* item);
  void appendRows(const QList<FilterModelItem*> &items);
  void insertRow(int row, FilterModelItem* item);
  bool removeRow(int row, const QModelIndex &parent = QModelIndex());
  bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
  FilterModelItem* takeRow(int row);
  FilterModelItem* find(const QString &id) const;
  QModelIndex indexFromItem( const FilterModelItem* item) const;
  FilterModelItem* currentRow();
  void sortModel(int column, Qt::SortOrder order);
  void clear();
  void ReturnSelectedItems();
  QModelIndex lastrow;
  QString ident;  

  Q_INVOKABLE bool setSelectionStatus(QString format);
  Q_INVOKABLE bool getSelectionStatus(QString format);

signals:
  void ItemAdded();
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const bool &state);
  void SetTypeSort(int pos, QString type);
private slots:
  void handleItemChange();

private:
  FilterModelItem* m_prototype;
  QList<FilterModelItem*> m_list;
};

#endif // FILTERMODEL_H
