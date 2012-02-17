/*
 * Author: Christophe Dumez <dchris@gmail.com>
 * License: Public domain (No attribution required)
 * Website: http://cdumez.blogspot.com/
 * Version: 1.0
 */

#ifndef LISTMODEL_H
#define LISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <datamodels/gridItem.h>
#include <qorbitermanager.h>


class qorbiterManager;

class ListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ListModel(gridItem* prototype, QObject* parent = 0);
    ~ListModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void appendRows(const QList<gridItem*> &items);
    void insertRow(int row, gridItem* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    gridItem* takeRow(int row);
    gridItem* find(const QString &id) const;
    Q_INVOKABLE QVariant get(int index, const QString &name) const;
    QModelIndex indexFromItem( const gridItem* item) const;
    gridItem* currentRow();

    int totalcells;
    bool loadingStatus;
    int gridType;




signals:
    void ItemAdded();
    void gimmieData(int type);
    void sizeChanged(int size);
    void gridTypeChanged(int type);
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const int &sRow);

public slots:
    void checkForMore();
    void appendRow(gridItem* item);
    void populateGrid(int mediaType);
    void setTotalCells(int cells);
    int getTotalCells();
    void setGridType( int type);
    int getGridType();
    void clear();

private slots:
    void handleItemChange();

private:
    gridItem* m_prototype;
    QList<gridItem*> m_list;
    // qorbiterManager *manager_ref;
};

#endif // LISTMODEL_H
