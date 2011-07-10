#include "mainwindowimpl.h"
#include "AppData.h"
#include "OrbiterBackend.h"
#include "OrbiterScreen.h"
#include "DatagridWidget.h"

#include <QUrl>
#include <QPixmap>
#include <QNetworkRequest>
#include <QAuthenticator>
#include <QNetworkProxy>
#include <QSettings>
#include <QtXml>

//
MainWindowImpl::MainWindowImpl(QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
	setupUi(this);
	Settings = new QSettings(APP_ORG, APP_NAME);

	// menu bar actions - start
	connect(Connect, SIGNAL(clicked()), this, SLOT(ConnectPressed()));
	connect(Connect, SIGNAL(pressed()), this, SLOT(ConnectPressed()));
	// menu bar actions - end
	
	connect(UseProxy, SIGNAL(stateChanged(int)), this, SLOT(UseProxyStateChanged(int)));

	connect(OrbiterPixmap, SIGNAL(clicked(int, int)), this, SLOT(OrbiterTouched(int, int)));
	connect(OrbiterPixmap, SIGNAL(keypressed(int)), this, SLOT(OrbiterKeyPressed(int)));

	connect(Orbiter, SIGNAL(ScreenReady(OrbiterScreen *)), this, SLOT(ProcessScreen(OrbiterScreen *)));
	connect(Orbiter, SIGNAL(Error(const QString &)), this, SLOT(ProcessError(const QString &)));

	ApplyUISettings();
	StartFresh();
}

MainWindowImpl::~MainWindowImpl()
{
	delete Settings;
}

void MainWindowImpl::ConnectPressed()
{
	setConnectionFormEnabledState(false);
	Status->setText("Connecting...");

	SaveUISettings();

	Orbiter->Start();
}

void MainWindowImpl::ProcessError(const QString &Message)
{
	switch (CurrentStage)
	{
		case Stage::FirstPage:
			StartFresh(Message);
		break;

		case Stage::OrbiterPage:
			OrbiterPixmap->setText("Connection lost: " + Message);
		break;
	}
}

void MainWindowImpl::ProcessScreen(OrbiterScreen *Screen)
{
	CurrentStage = Stage::OrbiterPage;

	QPixmap *FlatImage = Screen->getFlatImage();
	if (!Settings->value("Device/FullScreen").toBool())
	{
		setMinimumSize(FlatImage->width(), FlatImage->height());
		setMaximumSize(FlatImage->width(), FlatImage->height());
	}
	else
	{
		showFullScreen();
	}

	OrbiterPixmap->setPixmap(*FlatImage);
	stackedWidget->setCurrentWidget(orbiter);
	
	if (Settings->value("Device/EnhancedUI").toBool())
	{
		ApplyEnhancedUI(Screen);
	}
}

void MainWindowImpl::ApplyEnhancedUI(OrbiterScreen *Screen)
{
	QMap<QString, DatagridWidget *>::iterator It;
	for (It = Datagrids.begin(); It != Datagrids.end(); It++)
		delete It.value();
	Datagrids.clear();
	
	QDomDocument *ScreenXml = Screen->getScreenXml();
	QDomNodeList ObjectList = ScreenXml->elementsByTagName("object");
	for (size_t i = 0; i < ObjectList.length(); i++)
	{
		QDomElement Object = ObjectList.item(i).toElement();
		QString ObjectType = Object.attribute("type");
		if (ObjectType != "Datagrid")
			continue;
			
		int x = Object.attribute("x").toInt();
		int y = Object.attribute("y").toInt();
		int width = Object.attribute("width").toInt();
		int height = Object.attribute("height").toInt();
		
		QString GridID;
		int nCols = 0, nRows = 0;
		QDomNodeList ParmList = Object.elementsByTagName("parameter");
		for (size_t j = 0; j < ParmList.length(); j++)
		{
			QDomElement Parm = ParmList.item(j).toElement();
			QString ParmId = Parm.attribute("id");
			if (ParmId == "Data grid ID")
				GridID = Parm.attribute("value");
			if (ParmId == "# of Columns")
				nCols = Parm.attribute("value").toInt();
			if (ParmId == "# of Rows")
				nRows = Parm.attribute("value").toInt();
		}
		
		if (GridID != "")
		{
			if (nRows == 0)
				nRows = 10;
			DatagridWidget *Table = new DatagridWidget(GridID, orbiter);
			Table->setGeometry(x, y, width, height);
			Table->horizontalHeader()->hide();
			Table->verticalHeader()->hide();
			Table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//			Table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		
			Datagrids[GridID] = Table;
			if (nCols > 0 && nRows > 0)
			{
				int RowHeight = round(1.0 * Table->height() / nRows);
				int ColWidth = round(1.0 * Table->width() / nCols);
				qDebug() << "Grid=" << GridID << " nRows=" << nRows << " nCols=" << nCols
					<< " Height=" << Table->height() << " Width=" << Table->width()
					<< " RowHeight=" << RowHeight << " ColWidth=" << ColWidth;
				Table->verticalHeader()->setDefaultSectionSize(RowHeight);
				Table->horizontalHeader()->setDefaultSectionSize(ColWidth);
			}
			
			connect(Table, SIGNAL(cellClicked(const QString &,int,int)),
				this, SLOT(DatagridCellClicked(const QString &,int,int)));
		}
	}
	
	QStringList DatagridNames = Screen->getDatagridNames();
	for (QStringList::iterator It = DatagridNames.begin(); It != DatagridNames.end(); It++)
	{
		QDomDocument *Xml = Screen->getDatagridXml(*It);
		QDomElement Datagrid = Xml->documentElement();
		
		DatagridWidget *Table = Datagrids[*It];
		
		int RowCount = 0;
		QDomNodeList Rows = Xml->elementsByTagName("row");
		for (size_t i = 0; i < Rows.length(); i++)
		{
			QDomNodeList Cols = Rows.item(i).toElement().elementsByTagName("column");
			for (size_t j = 0; j < Cols.length(); j++)
			{
				QString TextValue = "";
				QString ImagePath = "";

				QDomNodeList Text = Cols.item(j).toElement().elementsByTagName("text");
				if (Text.length() != 0)
					TextValue = Text.item(0).toElement().text();
				QDomNodeList Image = Cols.item(j).toElement().elementsByTagName("image");
				if (Image.length() != 0)
					ImagePath = Image.item(0).toElement().attribute("src");

				DatagridCell *Cell = new DatagridCell;
				Cell->Text = TextValue;
				Cell->ImagePath = ImagePath;
				Cell->Row = i;
				Cell->Column = j;

				QDomNodeList Parms = Cols.item(j).toElement().elementsByTagName("parameter");
				for (size_t k = 0; k < Parms.length(); k++)
				{
					QString ParmName = Parms.item(k).toElement().attribute("name");
					QString ParmValue = Parms.item(k).toElement().attribute("value");
					if (ParmName != "" && ParmValue != "")
						Cell->Parameter[ParmName] = ParmValue;
				}

				Table->dataSource.setCell(RowCount, Cell);
				RowCount++;
			}
		}
		
		Table->show();
		Table->Refresh();
	}
	
	DatagridWidget *MediaFileGrid = NULL, *_MediaFileGrid = NULL;
	for (QStringList::iterator It = DatagridNames.begin(); It != DatagridNames.end(); It++)
	{
		if ((*It).startsWith("MediaFile_"))
			MediaFileGrid = Datagrids[*It];
		else if ((*It).startsWith("_MediaFile_"))
			_MediaFileGrid = Datagrids[*It];
	}
	
	if (MediaFileGrid != NULL && _MediaFileGrid != NULL)
	{
		int PosX = 0;
		int PosY = 0;
		
		QRect Frame1 = MediaFileGrid->geometry();
		QRect Frame2 = _MediaFileGrid->geometry();
		
		QPoint Corner1 = Frame1.bottomRight();
		QPoint Corner2 = Frame2.bottomRight();
		int CornerX = std::max(Corner1.x(), Corner2.x());
		int CornerY = std::max(Corner1.y(), Corner2.y());
		
		int Width = CornerX - PosX;
		int Height = CornerY - PosY;
		
		_MediaFileGrid->setGeometry(PosX, PosY, Width, Height);
		int RowHeight = MediaFileGrid->verticalHeader()->defaultSectionSize();
		_MediaFileGrid->verticalHeader()->setDefaultSectionSize(RowHeight);
		_MediaFileGrid->dataSource.UseCellImages = true;
		_MediaFileGrid->Refresh();
		_MediaFileGrid->horizontalHeader()->resizeSection(0, RowHeight);
		_MediaFileGrid->horizontalHeader()->resizeSection(1, Width - RowHeight);
		
		MediaFileGrid->setParent(NULL);
	}
}

void MainWindowImpl::setConnectionFormEnabledState(bool Enabled)
{
	Connect->setEnabled(Enabled);
	ServerAddress->setEnabled(Enabled);
	DeviceNumber->setEnabled(Enabled);
}

void MainWindowImpl::OrbiterTouched(int X, int Y)
{
	X -= (OrbiterPixmap->width() - OrbiterPixmap->pixmap()->width()) / 2;
	Y -= (OrbiterPixmap->height() - OrbiterPixmap->pixmap()->height()) / 2;

	Orbiter->Touch(X, Y);
}

void MainWindowImpl::OrbiterKeyPressed(int key)
{
	if (key == Qt::Key_Escape)
		StartFresh();
}

void MainWindowImpl::StartFresh(const QString &Message)
{
	Orbiter->Stop();

	Status->setText(Message);
	setConnectionFormEnabledState(true);
	stackedWidget->setCurrentWidget(settings);
	showNormal();
	setMinimumSize(0, 0);
	setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

	CurrentStage = Stage::FirstPage;
}

void MainWindowImpl::UseProxyStateChanged(int NewState)
{
	bool Enabled = (NewState == Qt::Checked);

	ProxyAddress->setEnabled(Enabled);
	ProxyUsername->setEnabled(Enabled);
	ProxyPassword->setEnabled(Enabled);
}

void MainWindowImpl::ApplyUISettings()
{
	ServerAddress->setText(Settings->value("Server/Address").toString());
	ServerUsername->setText(Settings->value("Server/Username").toString());
	ServerPassword->setText(Settings->value("Server/Password").toString());
	ServerUseSSL->setCheckState(Settings->value("Server/UseSSL").toBool() ? Qt::Checked : Qt::Unchecked);

	UseProxy->setCheckState(Settings->value("Proxy/Use").toBool() ? Qt::Checked : Qt::Unchecked);

	ProxyAddress->setText(Settings->value("Proxy/Address").toString());
	ProxyUsername->setText(Settings->value("Proxy/Username").toString());
	ProxyPassword->setText(Settings->value("Proxy/Password").toString());

	DeviceNumber->setText(Settings->value("Device/ID").toString());
	FullScreen->setCheckState(Settings->value("Device/FullScreen").toBool() ? Qt::Checked : Qt::Unchecked);
	EnhancedUI->setCheckState(Settings->value("Device/EnhancedUI").toBool() ? Qt::Checked : Qt::Unchecked);
}

void MainWindowImpl::SaveUISettings()
{
	Settings->setValue("Device/ID", DeviceNumber->text());
	Settings->setValue("Device/FullScreen", FullScreen->checkState() == Qt::Checked);
	Settings->setValue("Device/EnhancedUI", EnhancedUI->checkState() == Qt::Checked);

	Settings->setValue("Server/Address", ServerAddress->text());
	Settings->setValue("Server/Username", ServerUsername->text());
	Settings->setValue("Server/Password", ServerPassword->text());
	Settings->setValue("Server/UseSSL", ServerUseSSL->checkState() == Qt::Checked);

	Settings->setValue("Proxy/Use", UseProxy->checkState() == Qt::Checked);

	Settings->setValue("Proxy/Address", ProxyAddress->text());
	Settings->setValue("Proxy/Username", ProxyUsername->text());
	Settings->setValue("Proxy/Password", ProxyPassword->text());
}

void MainWindowImpl::DatagridCellClicked(const QString &GridID, int row, int column)
{
	Orbiter->TouchDatagrid(GridID, row, column);
}
//
