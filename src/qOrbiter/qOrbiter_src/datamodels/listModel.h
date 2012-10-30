#ifndef LISTMODEL_H
#define LISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <datamodels/gridItem.h>
#include <qorbitermanager.h>

class ListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY (double progress READ getProgress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY (bool loadingStatus READ getLoadingStatus WRITE setLoadingStatus NOTIFY loadingStatusChanged)
    Q_PROPERTY (int totalcells READ getTotalCells WRITE setTotalCells NOTIFY sizeChanged)
    Q_PROPERTY (int currentCells READ getCurrentCells WRITE setCurrentCells NOTIFY cellsChanged)
    Q_PROPERTY (int totalPages READ getTotalPages WRITE setTotalPages NOTIFY totalPagesChanged())


public:
    explicit ListModel(gridItem* prototype, QObject* parent = 0);
    ~ListModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    //int columnCount(const QModelIndex &parent) const {return 1;}
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
#ifdef QT5
 QHash<int, QByteArray> roleNames() const;
#endif

    void appendRows(const QList<gridItem*> &items);
  Q_INVOKABLE  void appendRow(gridItem* item);
   // bool insertRows(int row, int count, gridItem *item);
    void insertRow(int row, gridItem* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    gridItem* takeRow(int row);
    gridItem* find(const QString &id) const;
    Q_INVOKABLE QVariant get(int index, const QString &name) const;
    QModelIndex indexFromItem( const gridItem* item) const;
    gridItem* currentRow();
    void sortModel(int column, Qt::SortOrder order);

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
//    virtual void beginResetModel();
//    virtual void endResetModel();
private:
    bool resetInternalData();
    gridItem* m_prototype;
    QList<gridItem*> m_list;
    // qorbiterManager *manager_ref;
};

#endif // LISTMODEL_H
