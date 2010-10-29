#include "mainwindowimpl.h"
#include "AppData.h"
#include "OrbiterBackend.h"

#include <QUrl>
#include <QPixmap>
#include <QNetworkRequest>
#include <QAuthenticator>
#include <QNetworkProxy>
#include <QSettings>

//
MainWindowImpl::MainWindowImpl(QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
	setupUi(this);
	Settings = new QSettings(APP_ORG, APP_NAME);
	Orbiter = new OrbiterBackend;

	// menu bar actions - start
	connect(Connect, SIGNAL(clicked()), this, SLOT(ConnectPressed()));
	connect(Connect, SIGNAL(pressed()), this, SLOT(ConnectPressed()));
	// menu bar actions - end
	
	connect(UseProxy, SIGNAL(stateChanged(int)), this, SLOT(UseProxyStateChanged(int)));

	connect(OrbiterPixmap, SIGNAL(clicked(int, int)), this, SLOT(OrbiterTouched(int, int)));
	connect(OrbiterPixmap, SIGNAL(keypressed(int)), this, SLOT(OrbiterKeyPressed(int)));

	connect(Orbiter, SIGNAL(ImageReady(const QPixmap &)), this, SLOT(ProcessImage(const QPixmap &)));
	connect(Orbiter, SIGNAL(Error(const QString &)), this, SLOT(ProcessError(const QString &)));

	ApplyUISettings();
	StartFresh();
}

MainWindowImpl::~MainWindowImpl()
{
	delete Orbiter;
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

		case Stage::Orbiter:
			OrbiterPixmap->setText("Connection lost: " + Message);
		break;
	}
}

void MainWindowImpl::ProcessImage(const QPixmap &Image)
{
	CurrentStage = Stage::Orbiter;

	if (!Settings->value("Device/FullScreen").toBool())
	{
		setMinimumSize(Image.width(), Image.height());
		setMaximumSize(Image.width(), Image.height());
	}
	else
	{
		showFullScreen();
	}

	OrbiterPixmap->setPixmap(Image);
	stackedWidget->setCurrentWidget(orbiter);

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

	UseProxy->setCheckState(Settings->value("Proxy/Use").toBool() ? Qt::Checked : Qt::Unchecked);

	ProxyAddress->setText(Settings->value("Proxy/Address").toString());
	ProxyUsername->setText(Settings->value("Proxy/Username").toString());
	ProxyPassword->setText(Settings->value("Proxy/Password").toString());

	DeviceNumber->setText(Settings->value("Device/ID").toString());
	FullScreen->setCheckState(Settings->value("Device/FullScreen").toBool() ? Qt::Checked : Qt::Unchecked);
}

void MainWindowImpl::SaveUISettings()
{
	Settings->setValue("Device/ID", DeviceNumber->text());
	Settings->setValue("Device/FullScreen", FullScreen->checkState() == Qt::Checked);

	Settings->setValue("Server/Address", ServerAddress->text());
	Settings->setValue("Server/Username", ServerUsername->text());
	Settings->setValue("Server/Password", ServerPassword->text());

	Settings->setValue("Proxy/Use", UseProxy->checkState() == Qt::Checked);

	Settings->setValue("Proxy/Address", ProxyAddress->text());
	Settings->setValue("Proxy/Username", ProxyUsername->text());
	Settings->setValue("Proxy/Password", ProxyPassword->text());
}
//
