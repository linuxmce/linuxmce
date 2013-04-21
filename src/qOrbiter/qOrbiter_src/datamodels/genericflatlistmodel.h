#ifndef GENERICFLATLISTMODEL_H
#define GENERICFLATLISTMODEL_H


#include <QAbstractListModel>
#include <datamodels/genericmodelitem.h>
#ifdef QT5
#include <QtWidgets/QApplication>
#else
#include <QApplication>
#endif
#include <QHash>
#include <QList>
#include <QVariant>
#include <QDebug>

class GenericFlatListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY (double progress READ getProgress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY (bool loadingStatus READ getLoadingStatus WRITE setLoadingStatus NOTIFY loadingStatusChanged)
    Q_PROPERTY (int totalcells READ getTotalCells WRITE setTotalCells NOTIFY sizeChanged)
    Q_PROPERTY (int currentCells READ getCurrentCells WRITE setCurrentCells NOTIFY cellsChanged)
    Q_PROPERTY (int totalPages READ getTotalPages WRITE setTotalPages NOTIFY totalPagesChanged())

public:
    explicit GenericFlatListModel(GenericModelItem *prototypeItem, QObject *parent = 0);
    ~GenericFlatListModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QHash<int, QByteArray> roleNames() const;

    void appendRows(const QList<GenericModelItem*> &items);
    void appendRow(GenericModelItem* item);
    void insertRows(int row, int count, GenericModelItem *item);
    void insertRow(int row, GenericModelItem* item);
    void sortModel(int column, Qt::SortOrder order);

    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    GenericModelItem* takeRow(int row);
    GenericModelItem* find(const QString &id) const;
    GenericModelItem* currentRow();

    QModelIndex indexFromItem( const GenericModelItem* item) const;

    int totalcells;
    int seperator;
    bool loadingStatus;
    int gridType;
    double progress;
    int currentCells;
    bool clearing;
    int totalPages;

    
signals:
    void itemAdded(int row);
    void gimmieData(int type);
    void loadingStatusChanged(bool state);
    void sizeChanged(int size);
    void gridTypeChanged(int type);
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const int &sRow);
    void progressChanged(double p);
    void ready(int type);
    void statusMessage(QString msg);

    void modelAboutToBeReset();
    void modelReset();
    void cellsChanged();
    void pagingCleared();
    void totalPagesChanged();
    
public slots:
     QVariant get(int index, const QString &name) const;
     void setTotalPages(int p){totalPages = p;qDebug() << "New page count " << totalPages; emit totalPagesChanged();}
     int  getTotalPages() {return totalPages;}
     void setSeperator(int s) {seperator = s;}
     void checkForMore();
     void populateGrid(int mediaType);
     void setTotalCells(int cells);
     int getTotalCells();
     void setGridType( int type);
     int getGridType();
     void clear();
     void setLoadingStatus(bool b);
     bool getLoadingStatus();
     void setProgress(double n_progress);
     double getProgress();
     void attributeSort();
     void clearAndRequest( int );
     void setCurrentCells (int i) {currentCells = i; emit cellsChanged();}
     int getCurrentCells () {return currentCells;}
     void clearForPaging();


private slots:
    void handleItemChange();
    void reset();

private:
    bool resetInternalData();
    GenericModelItem* m_prototype;
    QList<GenericModelItem*>m_list;
    
};

#endif // GENERICFLATLISTMODEL_H
