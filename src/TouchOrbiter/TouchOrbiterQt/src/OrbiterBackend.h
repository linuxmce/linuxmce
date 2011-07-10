#ifndef __ORBITERBACKEND_H__
#define __ORBITERBACKEND_H__

#include <QObject>

class QString;
class QNetworkRequest;
class QNetworkReply;
class QNetworkAccessManager;
class QAuthenticator;
class QSettings;
class QTimer;
class QSslError;
class QPixmap;

class OrbiterScreen;

#define Orbiter OrbiterBackend::GetInstance()

class OrbiterBackend : public QObject
{
Q_OBJECT
private:
	OrbiterBackend(QObject* parent = 0);
	static class OrbiterBackend *Instance;
public:
	static class OrbiterBackend *GetInstance();
	virtual ~OrbiterBackend();

signals:
	void ScreenReady(OrbiterScreen *Screen);
	void Error(const QString &Message);
	void ImageDownloadComplete(QPixmap *Image);

public slots:
	void Start();
	void Stop();
	void Touch(int X, int Y);
	void TouchDatagrid(const QString &GridID, int Row, int Col);

public:
	void GetImage(const QString &FilePath, QObject *Receiver);

private slots:
	void TimerExpired();
	void AuthenticateHTTP(QNetworkReply *Reply, QAuthenticator *Authenticator);
	void RequestFinished(QNetworkReply *Reply);
	void SslErrors(QNetworkReply *Reply, const QList<QSslError> &SslErrors);

private:
	QSettings *Settings;
	QTimer *RefreshTimer;
	QNetworkAccessManager *NetworkManager;
	OrbiterScreen *Screen;

	QString Protocol;

	bool bAuthenticationAttempted;
	bool bConnectionError;

	void GetImage();
	void DoCmd(const QString &Cmd);
	void GetScreen();
	void GetDatagrid(const QString &GridID);

	void GetRequest(const QNetworkRequest &Request);
	void ProcessCommandReply(QNetworkReply *Reply);
	void ProcessImageReply(QNetworkReply *Reply);
	void ProcessXML(const QByteArray &XMLData);
};

#endif // __ORBITERBACKEND_H__
