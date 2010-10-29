#include "OrbiterBackend.h"
#include "AppData.h"

#include <QPixmap>
#include <QSettings>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QAuthenticator>
#include <QTimer>

OrbiterBackend::OrbiterBackend(QObject* parent)
	: QObject(parent)
{
	Settings = new QSettings(APP_ORG, APP_NAME);
	RefreshTimer = new QTimer;
	RefreshTimer->setInterval(1000);
	NetworkManager = NULL;
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
	/* _AUTH_CACHE_WORKAROUND_ */

	bAuthenticationAttempted = false;
	bConnectionError = false;

	QNetworkProxy Proxy;

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

	GetImage();
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
}

void OrbiterBackend::Touch(int X, int Y)
{
	DoCmd(QString("TOUCH %1x%2").arg(X).arg(Y));
}

void OrbiterBackend::DoCmd(const QString &Cmd)
{
	QUrl Url;
	Url.setUrl(QString("http://") + Settings->value("Server/Address").toString() + "/lmce-admin/weborbiter_command.php");
	Url.addQueryItem("device_id", Settings->value("Device/ID").toString());
	Url.addQueryItem("cmd", Cmd);
	QNetworkRequest Request(Url);

	GetRequest(Request);
}

void OrbiterBackend::GetImage()
{
	QUrl Url;
	Url.setUrl(QString("http://") + Settings->value("Server/Address").toString() + "/lmce-admin/weborbiter_image.php");
	Url.addQueryItem("device_id", Settings->value("Device/ID").toString());
	QNetworkRequest Request(Url);

	GetRequest(Request);
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
		GetImage();
		return;
	}

	if (FirstLine == "ERROR\n")
		return;

	int SpacePos = FirstLine.indexOf(' ');
	if (SpacePos == -1)
		return;

	int Length = FirstLine.mid(SpacePos + 1).trimmed().toInt(); 
	if (FirstLine.startsWith("NEWS "))
	{
		QByteArray AnyNews = Reply->read(Length);
		if (AnyNews.startsWith("yes") || bConnectionError)
			GetImage();
	}
}

void OrbiterBackend::ProcessImageReply(QNetworkReply *Reply)
{
	QPixmap OrbiterImage;
	OrbiterImage.loadFromData(Reply->readAll());
	emit ImageReady(OrbiterImage);
}
