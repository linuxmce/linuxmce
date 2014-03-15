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
#include <QReadWriteLock>

class GenericFlatListModel : public QAbstractListModel
{
    Q_OBJECT

    mutable QReadWriteLock lock;

    Q_PROPERTY (double progress READ getProgress WRITE setProgress NOTIFY progressChanged)
    Q_PROPERTY (bool loadingStatus READ getLoadingStatus WRITE setLoadingStatus NOTIFY loadingStatusChanged)
    Q_PROPERTY (int currentCells READ getCurrentCells WRITE setCurrentCells NOTIFY cellsChanged)
    Q_PROPERTY (int totalPages READ getTotalPages WRITE setTotalPages NOTIFY totalPagesChanged())

public:
    explicit GenericFlatListModel(QObject *parent = 0);
    GenericFlatListModel(GenericModelItem *prototypeItem, QObject *parent = 0);
    ~GenericFlatListModel();
    
    void setPrototype(GenericModelItem* pItem);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    // Q_INVOKABLE to be able to call it from QML
    Q_INVOKABLE bool setData(const int index, const QString roleName, const QVariant & value);

    virtual Qt::ItemFlags flags(const QModelIndex & index ) const;
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

    void resetWindow();
    QModelIndex indexFromItem( const GenericModelItem* item) const;
    void updateItemData(int row, int role, QVariant value);
    void setModelName(QString s) { modelName = s; }
    void setPK_DataGrid(int PK_DataGrid) { m_PK_DataGrid = PK_DataGrid; }
    void setOption(QString option) { m_option = option; }
    void seek(QString seek);
    void seekResult(int row);
    bool isSeeking() { return m_seek; }

    QString modelName;
    // Values we need when requesting data
    QString m_dgName;
    int m_PK_DataGrid;
    QString m_option;
    bool m_seek;
    bool loaded;

    int m_windowStart; // item number at start of the window
    int m_bForward; // forward(true) or backward request(false)
    int m_windowSize; // The size of the window of items to display
    int m_iLastRow; // last row accessed, to be able to see what direction we are scrolling
    int m_requestStart; // start of the requested range
    int m_requestEnd; // end of the requested range
    

    int totalRows;
    int m_totalCols;

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
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void progressChanged(double p);
    void ready(int type);
    void statusMessage(QString msg);

    void modelAboutToBeReset();
    void modelReset();
    void cellsChanged();
    void pagingCleared();
    void totalPagesChanged();
    void scrollToItem(int item);

public slots:
     QVariant get(int index, const QString &name) const;
     void setTotalPages(int p){totalPages = p;qDebug() << "New page count " << totalPages; emit totalPagesChanged();}
     int  getTotalPages() {return totalPages;}
     void setSeperator(int s) {seperator = s;}
     void checkForMore();
     void populateGrid(int mediaType);

     void setWindowSize(int windowSize) { m_windowSize = windowSize; }
     void setTotalRows(int rows);
     void setTotalCols(int cols) { m_totalCols = cols; }
     int getTotalRows();
     void setDgName(QString dgName) { m_dgName = dgName; }
     QString getDGName() { return m_dgName; }
     void setRequestedRows(int start, int end) { m_requestEnd = end; m_requestStart = start; }
     bool isForwardRequest() { return m_bForward; }

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
    Q_INVOKABLE void refreshData();
    void requestMoreData(int row, int direction);

    void removeComplete();


private slots:
    void handleItemChange();
    void reset();

private:
    bool resetInternalData();
    GenericModelItem* m_prototype;
    QList<GenericModelItem*>m_list;
    
};

#endif // GENERICFLATLISTMODEL_H
