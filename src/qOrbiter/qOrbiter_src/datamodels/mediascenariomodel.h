#ifndef MEDIASCENARIOMODEL_H
#define MEDIASCENARIOMODEL_H

#include <QAbstractListModel>
#include <datamodels/mediascenarioitem.h>
#include <QList>
#include <QVariant>

class MediaScenarioItem;


class MediaScenarioModel : public QAbstractListModel
{
  Q_OBJECT

public:

  explicit MediaScenarioModel(MediaScenarioItem *prototype, QObject* parent = 0);
  ~MediaScenarioModel();

#ifdef QT5
   QHash<int, QByteArray> roleNames() const;
#endif

  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  void appendRow(MediaScenarioItem* item);
  void appendRows(const QList<MediaScenarioItem*> &items);
  void insertRow(int row, MediaScenarioItem* item);
  bool removeRow(int row, const QModelIndex &parent = QModelIndex());
  bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
  MediaScenarioItem* takeRow(int row);
  MediaScenarioItem* find(const QString &id) const;
  QModelIndex indexFromItem( const MediaScenarioItem* item) const;
  void sortModel(int column, Qt::SortOrder order);
  void clear();

private slots:
  void handleItemChange();

private:
  MediaScenarioItem* m_prototype;
  QList<MediaScenarioItem*> m_list;

signals:
void  DataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

};



#endif // MEDIASCENARIOMODEL_H
