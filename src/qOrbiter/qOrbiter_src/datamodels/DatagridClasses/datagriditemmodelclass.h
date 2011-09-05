#ifndef DATAGRIDITEMMODELCLASS_H
#define DATAGRIDITEMMODELCLASS_H

/*
  This class is to be the main handler for the media related datagrids. It is being created from the abstract subclass
  QAbstractItemModel because this is the best abstract class of the various QAbstractModel classes, namely for its item based
  approach which is more important for encapsulating individual file references to be stand alone objects if needed.
  This class will eventually require an imageprovider.
  */

#include <QAbstractItemModel>
#include <datamodels/DatagridClasses/datagriditem.h>
#include <QModelIndex>

class DataGridItemModelClass : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit DataGridItemModelClass(DataGridItem *m_prototype, QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void appendRow(DataGridItem* item);
    void appendRows(const QList<DataGridItem*> &items);
    void insertRow(int row, DataGridItem* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    DataGridItem* takeRow(int row);
    DataGridItem* find(const QString &id) const;
    QModelIndex indexFromItem( const DataGridItem* item) const;
    DataGridItem* currentRow();
    void handleItemChange();
    void clear();


signals:

public slots:

private:
    DataGridItem* m_prototype;
    QList<DataGridItem*> m_list;

};

#endif // DATAGRIDITEMMODELCLASS_H
