#include "DatagridWidget.h"
#include "OrbiterBackend.h"

#include <QDebug>
//
DatagridWidget::DatagridWidget(const QString &GridID, QWidget *parent) 
	: QTableView(parent), myGridID(GridID), itemDelegate(NULL), dataSource(NULL)
{
	setItemDelegate(&itemDelegate);
	dataSource.Table = this;
	connect(this, SIGNAL(activated(const QModelIndex &)), this, SLOT(CellActivated(const QModelIndex &)));
	connect(this, SIGNAL(clicked(const QModelIndex &)), this, SLOT(CellActivated(const QModelIndex &)));
}
//
DatagridWidget::~DatagridWidget()
{
}
//
const QString &DatagridWidget::getGridID() const
{
	return myGridID;
}
//
void DatagridWidget::Refresh()
{
	setModel(NULL);
	setModel(&dataSource);
}
//
void DatagridWidget::CellActivated(const QModelIndex & index)
{
	const DatagridCell *Cell = dataSource.getCell(index.row());
	if (Cell == NULL)
		return;
	qDebug() << "CellActivated: grid:" << myGridID << "row:" << Cell->Row << "col:" << Cell->Column;
	if (myGridID.startsWith("_MediaFile_"))
		emit cellClicked(myGridID.mid(1), index.row(), 0);
	else
		emit cellClicked(myGridID, Cell->Row, Cell->Column);
}
//
//
//
DatagridDelegate::DatagridDelegate(QObject * parent)
	: QStyledItemDelegate(parent)
{
}
//
DatagridDelegate::~DatagridDelegate()
{
}
//
void DatagridDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	const DatagridData *Data = dynamic_cast<const DatagridData *>(index.model());
	if (Data == NULL || !Data->UseCellImages || index.column() == 1)
	{
		QStyledItemDelegate::paint(painter, option, index);
	}
	else if (index.column() == 0) 
	{
		painter->save();
		
		QStyleOptionViewItem option_copy = option;
		initStyleOption(&option_copy, index);
		
		DatagridCell *Cell = Data->getCell(index.row());
		if (Cell == NULL || Cell->Image.isNull())
		{
			QPixmap emptyImage(256, 256);
			emptyImage.fill(Qt::white);
			painter->drawPixmap(option_copy.rect, emptyImage);
			Cell->DownloadImage();
		}
		else
		{
			painter->drawPixmap(option_copy.rect, Cell->Image);
		}
		
		painter->restore();
	}
}
//
//
//
DatagridCell::DatagridCell(QObject * parent)
	: QObject(parent)
{
	downloadStarted = false;
}
//
DatagridCell::~DatagridCell()
{
}
//
void DatagridCell::DownloadImage()
{
	if (downloadStarted || ImagePath == "")
		return;
	
	downloadStarted = true;
	
	Orbiter->GetImage(ImagePath, this);
}
//
void DatagridCell::ImageDownloadComplete(QPixmap * theImage)
{
	Image = *theImage;
	delete theImage;
	emit CellChanged(this);
}
//
//
//
DatagridData::DatagridData(QObject * parent)
	: QAbstractTableModel(parent), Table(NULL), UseCellImages(false)
{
}
//
DatagridData::~DatagridData()
{
	foreach (DatagridCell *Cell, Cells)
		delete Cell;
	Cells.clear();
}
//
void DatagridData::setCell(int Row, DatagridCell *Cell)
{
	if (Cells.size() <= Row)
		Cells.resize(Row + 1);
	delete Cells[Row];

	Cell->parentRow = Row;
	connect(Cell, SIGNAL(CellChanged(DatagridCell *)), this, SLOT(CellChanged(DatagridCell *)));

	Cells[Row] = Cell;
}
//
DatagridCell *DatagridData::getCell(int Row) const
{
	if (Cells.size() <= Row)
		return NULL;
	return Cells[Row];
}
//
int DatagridData::rowCount(const QModelIndex & parent) const
{
	Q_UNUSED(parent);
	int RowCount = Cells.size();
	return RowCount;
}
//
int DatagridData::columnCount(const QModelIndex & parent) const
{
	Q_UNUSED(parent);
	int ColCount = UseCellImages ? 2 : 1;
	return ColCount;
}
//
QVariant DatagridData::data(const QModelIndex & index, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	
	const DatagridCell *Cell = getCell(index.row());
	if (Cell == NULL)
		return QVariant();
		
	QString CellText;
	if (Table->getGridID().startsWith("mattrfile_"))
		CellText = "* " + Cell->Parameter["Title"] + ":\n" + Cell->Parameter["Name"];
	else
		CellText = Cell->Text;
	return QVariant(CellText);
}
//
void DatagridData::CellChanged(DatagridCell *Cell)
{
	emit dataChanged(createIndex(Cell->Row, 0), createIndex(Cell->Row, columnCount(QModelIndex())));
}
//
