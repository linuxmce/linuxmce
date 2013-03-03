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

    This class is another specific model/item pair with the child being climatescenarioitem It is implemented as a specific class
    as opposed to abstracted because each home automation category is unique, and it seemed more logical to provide custom classes
    for the QAbstractlistmodel base than further abstracting that model interface.

*/

#ifndef CLIMATESCENARIOMODEL_H
#define CLIMATESCENARIOMODEL_H

#include <QAbstractListModel>
#include <datamodels/climatescenarioitem.h>
#include <QList>
#include <QVariant>

class ClimateScenarioItem;

/*!
 * \brief The ClimateScenarioModel class
 * \ingroup data_model
 */
class ClimateScenarioModel : public QAbstractListModel
{
  Q_OBJECT
public:

  explicit ClimateScenarioModel(ClimateScenarioItem *prototype, QObject* parent = 0);
  ~ClimateScenarioModel();
#ifdef QT5
   QHash<int, QByteArray> roleNames() const;
#endif
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  void appendRow(ClimateScenarioItem* item);
  void appendRows(const QList<ClimateScenarioItem*> &items);
  void insertRow(int row, ClimateScenarioItem* item);
  bool removeRow(int row, const QModelIndex &parent = QModelIndex());
  bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
  ClimateScenarioItem* takeRow(int row);
  ClimateScenarioItem* find(const QString &id) const;
  QModelIndex indexFromItem( const ClimateScenarioItem* item) const;
  void sortModel(int column, Qt::SortOrder order);
  void clear();



private slots:
  void handleItemChange();

private:
  ClimateScenarioItem* m_prototype;
  QList<ClimateScenarioItem*> m_list;

};

#endif // CLIMATESCENARIOMODEL_H
