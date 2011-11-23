#ifndef FLOORPLANMODEL_H
#define FLOORPLANMODEL_H

#include <QAbstractListModel>
#include <datamodels/floorplanimageitem.h>
#include <QModelIndex>

class FloorPlanModel : public QAbstractListModel
{
    Q_PROPERTY (QString currentPage READ getCurrentPage WRITE setCurrentPage NOTIFY pageChanged)
    Q_OBJECT
public:
    explicit FloorPlanModel(FloorPlanItem *m_prototype, QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void appendRow(FloorPlanItem* item);
    void appendRows(const QList<FloorPlanItem*> &items);
    void insertRow(int row, FloorPlanItem* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    FloorPlanItem* takeRow(int row);
    FloorPlanItem* find(const QString &id) const;
    QModelIndex indexFromItem( const FloorPlanItem* item) const;
    FloorPlanItem* currentRow();

    void clear();
    QImage getPageImage(QString &id);
    Q_INVOKABLE void setCurrentPage(QString currentPageId);
    QString getCurrentPage() {return currentPage;}

    QString currentPage;


   // QModelIndex index(int row, int column, const QModelIndex &parent) const ;
  //  QModelIndex parent(const QModelIndex &child) const;
  //  int columnCount(const QModelIndex &parent) const;

signals:
   void pageChanged();
public slots:
 void handleItemChange();
private:
    FloorPlanItem* m_prototype;
    QList<FloorPlanItem*> m_list;
};

#endif // FLOORPLANMODEL_H
