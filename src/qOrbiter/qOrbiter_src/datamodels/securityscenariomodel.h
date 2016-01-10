#ifndef SECURITYSCENARIOMODEL_H
#define SECURITYSCENARIOMODEL_H
#include <QAbstractListModel>
#include <datamodels/securityscenarioitem.h>
#include <QList>
#include <QVariant>

class SecurityScenarioItem;


class SecurityScenarioModel : public QAbstractListModel
{
  Q_OBJECT
public:

  explicit SecurityScenarioModel(SecurityScenarioItem *prototype, QObject* parent = 0);
  ~SecurityScenarioModel();

#ifdef QT5
   QHash<int, QByteArray> roleNames() const;
#endif
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  void appendRow(SecurityScenarioItem* item);
  void appendRows(const QList<SecurityScenarioItem*> &items);
  void insertRow(int row, SecurityScenarioItem* item);
  bool removeRow(int row, const QModelIndex &parent = QModelIndex());
  bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
  SecurityScenarioItem* takeRow(int row);
  SecurityScenarioItem* find(const QString &id) const;
  QModelIndex indexFromItem( const SecurityScenarioItem* item) const;
  void sortModel(int column, Qt::SortOrder order);
  void clear();



private slots:
  void handleItemChange();

private:
  SecurityScenarioItem* m_prototype;
  QList<SecurityScenarioItem*> m_list;

};


#endif // SECURITYSCENARIOMODEL_H
