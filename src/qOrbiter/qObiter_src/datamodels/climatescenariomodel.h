#ifndef CLIMATESCENARIOMODEL_H
#define CLIMATESCENARIOMODEL_H

#include <QAbstractListModel>
#include <datamodels/climatescenarioitem.h>
#include <QList>
#include <QVariant>

class ClimateScenarioItem;


class ClimateScenarioModel : public QAbstractListModel
{
  Q_OBJECT
public:

  explicit ClimateScenarioModel(ClimateScenarioItem *prototype, QObject* parent = 0);
  ~ClimateScenarioModel();

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
  void clear();



private slots:
  void handleItemChange();

private:
  ClimateScenarioItem* m_prototype;
  QList<ClimateScenarioItem*> m_list;

};

#endif // CLIMATESCENARIOMODEL_H
