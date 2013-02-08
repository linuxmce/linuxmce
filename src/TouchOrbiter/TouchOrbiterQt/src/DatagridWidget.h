#ifndef DATAGRIDWIDGET_H
#define DATAGRIDWIDGET_H
//
#include <QTableWidget>
#include <QString>
#include <QStyledItemDelegate>
#include <QAbstractTableModel>
#include <QPainter>
#include <QMap>
#include <QVector>
//
class DatagridCell : public QObject
{
Q_OBJECT
public:
	DatagridCell(QObject * parent = 0);
	virtual ~DatagridCell();

public:
	QMap<QString, QString> Parameter;
	
	QString Text;
	QString ImagePath;
	QPixmap Image;
	int Row, Column;
	
	class DatagridData *parentTable;
	int parentRow;

signals:
	void CellChanged(class DatagridCell *);

public slots:
	void DownloadImage();
	void ImageDownloadComplete(QPixmap * theImage);

protected:
	bool downloadStarted;
};
//
class DatagridData : public QAbstractTableModel
{
Q_OBJECT
public:
	DatagridData(QObject * parent = 0);
	virtual ~DatagridData();

	virtual void setCell(int Row, DatagridCell *Cell);
	virtual DatagridCell *getCell(int Row) const;
	
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

public:
	class DatagridWidget *Table;
	bool UseCellImages;

private:
	QVector<DatagridCell *> Cells;
	
private slots:
	void CellChanged(DatagridCell *);
};
//
class DatagridDelegate : public QStyledItemDelegate
{
Q_OBJECT
public:
	DatagridDelegate(QObject * parent = 0);
	virtual ~DatagridDelegate();

	virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
};
//
class DatagridWidget : public QTableView
{
Q_OBJECT
public:
	DatagridWidget(const QString &GridID, QWidget *parent = 0);
	virtual ~DatagridWidget();

	const QString &getGridID() const;
	void Refresh();

signals:
	void cellClicked(const QString &GridID, int row, int column);

private slots:
	void CellActivated(const QModelIndex & index);

private:
	QString myGridID;
	DatagridDelegate itemDelegate;
public:
	DatagridData dataSource;
};
#endif
