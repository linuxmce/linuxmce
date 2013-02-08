#ifndef TELECOMSCENARIOMODEL_H
#define TELECOMSCENARIOMODEL_H
#include <QAbstractListModel>
#include <datamodels/telecomscenarioitem.h>
#include <QList>
#include <QVariant>

class TelecomScenarioItem;


class TelecomScenarioModel : public QAbstractListModel
{
  Q_OBJECT
public:

  explicit TelecomScenarioModel(TelecomScenarioItem *prototype, QObject* parent = 0);
  ~TelecomScenarioModel();

#ifdef QT5
   QHash<int, QByteArray> roleNames() const;
#endif
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  void appendRow(TelecomScenarioItem* item);
  void appendRows(const QList<TelecomScenarioItem*> &items);
  void insertRow(int row, TelecomScenarioItem* item);
  bool removeRow(int row, const QModelIndex &parent = QModelIndex());
  bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
  TelecomScenarioItem* takeRow(int row);
  TelecomScenarioItem* find(const QString &id) const;
  QModelIndex indexFromItem( const TelecomScenarioItem* item) const;
  void sortModel(int column, Qt::SortOrder order);
  void clear();



private slots:

  void handleItemChange();

private:
  TelecomScenarioItem* m_prototype;
  QList<TelecomScenarioItem*> m_list;

};


#endif // TELECOMSCENARIOMODEL_H
