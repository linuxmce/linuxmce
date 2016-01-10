#include "OrbiterBackend.h"
#include "AppData.h"
#include "OrbiterScreen.h"

#include <QPixmap>
#include <QSettings>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QAuthenticator>
#include <QTimer>
#include <QSslError>
#include <QtXml>
#include <QMetaObject>

OrbiterBackend *OrbiterBackend::Instance = NULL;

OrbiterBackend::OrbiterBackend(QObject* parent)
	: QObject(parent)
{
	Settings = new QSettings(APP_ORG, APP_NAME);
	RefreshTimer = new QTimer;
	RefreshTimer->setInterval(1000);
	NetworkManager = NULL;
	Screen = NULL;
}

OrbiterBackend *OrbiterBackend::GetInstance()
{
	if (Instance == NULL)
		Instance = new OrbiterBackend;
	return Instance;
}

OrbiterBackend::~OrbiterBackend()
{
	Stop();
	delete RefreshTimer;
	delete Settings;
}

void OrbiterBackend::Start()
{
	/* _AUTH_CACHE_WORKAROUND_
	 * Initialize our QNetworkAccessManager here rather than the constructor.
	 * QNetworkAccessManager doesn't have any means to forget the
	 * authentication credentials. It forced to forget by deleting it on Stop()
	 */
	NetworkManager = new QNetworkAccessManager;

	connect(RefreshTimer, SIGNAL(timeout()), this, SLOT(TimerExpired()));

	connect(NetworkManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(RequestFinished(QNetworkReply *)));
	connect(NetworkManager, SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)),
		this, SLOT(AuthenticateHTTP(QNetworkReply *, QAuthenticator *)));
	connect(NetworkManager, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)),
		this, SLOT(SslErrors(QNetworkReply *, const QList<QSslError> &)));
	/* _AUTH_CACHE_WORKAROUND_ */

	bAuthenticationAttempted = false;
	bConnectionError = false;

	QNetworkProxy Proxy;

	Protocol = "http://";
	if (Settings->value("Server/UseSSL").toBool())
		Protocol = "https://";

	if (Settings->value("Proxy/Use").toBool())
	{
		QString Address = Settings->value("Proxy/Address").toString();
		QString Hostname;
		int Port;
		if (Address.contains(':'))
		{
			int Pos = Address.indexOf(':');
			Hostname = Address.left(Pos);
			Port = Address.mid(Pos + 1).toInt();
		}
		else
		{
			Hostname = Address;;
			Port = 3128;
		}

		Proxy.setType(QNetworkProxy::HttpProxy);
		Proxy.setHostName(Hostname);
		Proxy.setPort(Port);
		Proxy.setUser(Settings->value("Proxy/Username").toString());
		Proxy.setPassword(Settings->value("Proxy/Password").toString());
	}
	else
	{
		Proxy.setType(QNetworkProxy::NoProxy);
	}

	NetworkManager->setProxy(Proxy);

	Screen = new OrbiterScreen;
	GetScreen();
	RefreshTimer->start();
}

void OrbiterBackend::Stop()
{
	/* _AUTH_CACHE_WORKAROUND_
	 * Force the NetworkManager to forget all authentication credentials.
	 */
	if (NetworkManager != NULL)
	{
		NetworkManager->deleteLater();
		NetworkManager = NULL;
	}
	/* _AUTH_CACHE_WORKAROUND_ */

	RefreshTimer->stop();
	delete Screen;
	Screen = NULL;
}

void OrbiterBackend::Touch(int X, int Y)
{
	DoCmd(QString("TOUCH %1x%2").arg(X).arg(Y));
}

void OrbiterBackend::TouchDatagrid(const QString &GridID, int Row, int Col)
{
	DoCmd(QString("DATAGRID_TOUCH %1 %2 %3").arg(GridID).arg(Row).arg(Col));
}

void OrbiterBackend::GetImage()
{
	QUrl Url;
	Url.setUrl(Protocol + Settings->value("Server/Address").toString() + "/lmce-admin/weborbiter_image.php");
	Url.addQueryItem("device_id", Settings->value("Device/ID").toString());
	QNetworkRequest Request(Url);

	GetRequest(Request);
}

void OrbiterBackend::GetImage(const QString &FilePath, QObject *Receiver)
{
	QUrl Url;
	Url.setUrl(Protocol + Settings->value("Server/Address").toString() + "/lmce-admin/weborbiter_image.php");
	Url.addQueryItem("device_id", Settings->value("Device/ID").toString());
	Url.addQueryItem("file", FilePath);

	QNetworkRequest Request(Url);
	Request.setOriginatingObject(Receiver);

	GetRequest(Request);
}

void OrbiterBackend::DoCmd(const QString &Cmd)
{
	QUrl Url;
	Url.setUrl(Protocol + Settings->value("Server/Address").toString() + "/lmce-admin/weborbiter_command.php");
	Url.addQueryItem("device_id", Settings->value("Device/ID").toString());
	Url.addQueryItem("cmd", Cmd);
	QNetworkRequest Request(Url);

	GetRequest(Request);
}

void OrbiterBackend::GetScreen()
{
	if (Screen->getInProgress())
		return;
	Screen->setInProgress(true);
	GetImage();
	if (Settings->value("Device/EnhancedUI").toBool())
	{
		DoCmd("SCREEN_XML");
	}
}

void OrbiterBackend::GetDatagrid(const QString &GridID)
{
	DoCmd(QString("DATAGRID_XML %1").arg(GridID));
}

void OrbiterBackend::GetRequest(const QNetworkRequest &Request)
{
	if (NetworkManager != NULL)
		NetworkManager->get(Request);
}

void OrbiterBackend::AuthenticateHTTP(QNetworkReply *Reply, QAuthenticator *Authenticator)
{
	if (bAuthenticationAttempted)
	{
		Reply->abort();
		emit Error("Bad username and/or password");
		return;
	}

	bAuthenticationAttempted = true;
	Authenticator->setUser(Settings->value("Server/Username").toString());
	Authenticator->setPassword(Settings->value("Server/Password").toString());
}

void OrbiterBackend::TimerExpired()
{
	DoCmd("ANYNEWS?");
}

void OrbiterBackend::RequestFinished(QNetworkReply *Reply)
{
	if (Reply->error() == QNetworkReply::NoError)
	{
		QVariant ContentTypeHeader = Reply->header(QNetworkRequest::ContentTypeHeader);
		if (ContentTypeHeader.type() == QVariant::Invalid)
			return;

		QString ContentType = ContentTypeHeader.toString();
		if (ContentType != "text/plain" && !ContentType.startsWith("image/"))
			return;

		if (ContentType == "text/plain")
			ProcessCommandReply(Reply);
		else if (ContentType.startsWith("image/"))
			ProcessImageReply(Reply);
		else
			return;

		bool bComplete = Screen->getFlatImage() != NULL;
		if (Settings->value("Device/EnhancedUI").toBool())
		{
			bComplete = bComplete && Screen->getScreenXml() != NULL;
			QList<QString> DatagridList = Screen->getDatagridNames();
			for (QList<QString>::iterator It = DatagridList.begin(); It != DatagridList.end(); It++)
				bComplete = bComplete && Screen->getDatagridXml(*It) != NULL;
		}

		if (bComplete)
		{
			emit ScreenReady(Screen);
			delete Screen;
			Screen = new OrbiterScreen;
			
			RefreshTimer->start();
		}

		bConnectionError = false;
	}
	else
	{
		emit Error(Reply->errorString());
		bConnectionError = true;
	}

	Reply->deleteLater();
}

void OrbiterBackend::ProcessCommandReply(QNetworkReply *Reply)
{
	QByteArray FirstLine = Reply->readLine();
	if (FirstLine == "OK\n")
	{
		//GetScreen();
		return;
	}

	if (FirstLine == "ERROR\n")
		return;

	int SpacePos = FirstLine.indexOf(' ');
	if (SpacePos == -1)
		return;

	QList<QByteArray> Words = FirstLine.split(' ');
	QString ReplyType = Words[0];
	int Length = Words[1].trimmed().toInt();

	if (ReplyType == "NEWS")
	{
		QByteArray AnyNews = Reply->read(Length);
		if (AnyNews.startsWith("yes") || bConnectionError)
		{
			RefreshTimer->stop();
			GetScreen();
		}
	}
	else if (ReplyType == "DATAGRID_XML" || ReplyType == "SCREEN_XML")
	{
		QByteArray XMLData = Reply->read(Length);
		ProcessXML(XMLData);
	}
}

void OrbiterBackend::ProcessImageReply(QNetworkReply *Reply)
{
	QPixmap *Image = new QPixmap;
	Image->loadFromData(Reply->readAll());
	
	QObject * Receiver = Reply->request().originatingObject();
	if (Receiver == NULL)
		Screen->setFlatImage(Image);
	else
		QMetaObject::invokeMethod(Receiver, "ImageDownloadComplete", Q_ARG(QPixmap *, Image)); 
}

void OrbiterBackend::ProcessXML(const QByteArray &XMLData)
{
	QDomDocument *XmlDoc = new QDomDocument;
	XmlDoc->setContent(XMLData);
	QDomElement XmlRoot = XmlDoc->documentElement();
	if (XmlRoot.tagName() == "screen")
	{
		QList<QString> ListOfGrids;
		QDomNodeList ObjectList = XmlRoot.elementsByTagName("object");
		for (size_t i = 0; i < ObjectList.length(); i++)
		{
			QDomElement Object = ObjectList.item(i).toElement();
			QString ObjectType = Object.attribute("type");
			if (ObjectType != "Datagrid")
				continue;
			QDomNodeList ParameterList = Object.elementsByTagName("parameter");
			for (size_t j = 0; j < ParameterList.length(); j++)
			{
				QDomElement Parameter = ParameterList.item(j).toElement();
				QString AttrId = Parameter.attribute("id");
				if (AttrId != "Data grid ID")
					continue;
				QString AttrValue = Parameter.attribute("value");
				if (AttrValue == "")
					continue;
				ListOfGrids.append(AttrValue);
			}
		}
		
		Screen->setScreenXml(XmlDoc);
		for (QList<QString>::iterator It = ListOfGrids.begin(); It != ListOfGrids.end(); It++)
		{
			Screen->setDatagridXml(*It, NULL);
			GetDatagrid(*It);
		}
	}
	else if (XmlRoot.tagName() == "datagrid")
	{
		QString GridID = XmlRoot.attribute("id");
		Screen->setDatagridXml(GridID, XmlDoc);
	}
	else
	{
		delete XmlDoc;
	}
}

void OrbiterBackend::SslErrors(QNetworkReply *Reply, const QList<QSslError> &SslErrors)
{
	(void) SslErrors;
	Reply->ignoreSslErrors();
}
