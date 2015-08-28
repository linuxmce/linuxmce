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

    This is a specialized class for dealing specifically with the attribute sort items that are part of the filtering
    variable for LinuxMCE's Datagrid generator. The Items in this class should be unique and special cases should only be
    made in the actual calls done in the request in qOrbiter.cpp
*/
#ifndef ATTRIBUTESORTMODEL_H
#define ATTRIBUTESORTMODEL_H


#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <datamodels/DataModelItems/attributesortitem.h>
#include <QStringList>
#include <QDebug>

class AttributeSortItem;

class AttributeSortModel: public QAbstractListModel
{
  Q_OBJECT
    Q_PROPERTY(bool allowMulti READ allowMulti NOTIFY allowMultiChanged)

public:
  explicit AttributeSortModel(AttributeSortItem* prototype, int filterNumber, bool exclusive ,QObject* parent = 0);
  ~AttributeSortModel();
#ifdef QT5
   QHash<int, QByteArray> roleNames() const;
#endif
  Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  AttributeSortItem* takeRow(int row);
  AttributeSortItem* find(const QString &id) const;
  QModelIndex indexFromItem( const AttributeSortItem* item) const;
  AttributeSortItem* currentRow();
  void sortModel(int column, Qt::SortOrder order);
  void ReturnSelectedItems();
 int filterLevel;
  QModelIndex lastrow;
  QString ident;

  Q_INVOKABLE void setSelectionStatus(QString format);
  Q_INVOKABLE bool getSelectionStatus(QString format);

  Q_INVOKABLE bool clearSelections();

  bool allowMulti() const;
  void setAllowMulti(bool allowMulti);

signals:
  void ItemAdded();
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const bool &state);
   void SetTypeSort(int pos, QString type);
   void modelAboutToBeReset();
   void modelReset();
   void allowMultiChanged();

public slots:
   void resetStates();
   void appendRow(AttributeSortItem* item);
   void appendRows(const QList<AttributeSortItem*> &items);
   void insertRow(int row, AttributeSortItem* item);
   bool removeRow(int row, const QModelIndex &parent = QModelIndex());
   bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
   void clear();
   void itemDeletion() { /*qDebug() << "Attribute Sort Item Destroyed"  ;*/}
   void objectDestroyed(QObject*){
//       qDebug()<< "Attribute QObject* item destroyed";
}

private slots:
  void handleItemChange();
  void reset();

private:
  bool resetInternalData();
  bool m_allowMulti;

  AttributeSortItem* m_prototype;
  QList<AttributeSortItem*> m_list;
};

#endif // ATTRIBUTESORTMODEL_H
