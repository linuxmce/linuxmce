#ifndef AVCODEGRID_H
#define AVCODEGRID_H
/*
  The purpose of this class is to be a c++ representation of the av codes grid for any given dce object(s) that can have
  avcodes and or other manner of sending it devices.

  */
#include <QAbstractItemModel>
#include <avitem.h>
#include <QModelIndex>

class AvCodeGrid : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit AvCodeGrid(AvItem *m_prototype, QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void appendRow(AvItem* item);
    void appendRows(const QList<AvItem*> &items);
    void insertRow(int row, AvItem* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    AvItem* takeRow(int row);
    AvItem* find(const QString &id) const;
    QModelIndex indexFromItem( const AvItem* item) const;
    AvItem* currentRow();
    void handleItemChange();
    void clear();


signals:
    void deviceAdded();

public slots:

private:
    AvItem* m_prototype;
    QList<AvItem*> m_list;
};

#endif // AVCODEGRID_H
