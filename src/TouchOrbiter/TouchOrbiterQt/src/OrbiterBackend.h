#ifndef __ORBITERBACKEND_H__
#define __ORBITERBACKEND_H__

#include <QObject>

class QPixmap;
class QString;
class QNetworkRequest;
class QNetworkReply;
class QNetworkAccessManager;
class QAuthenticator;
class QSettings;
class QTimer;

class OrbiterBackend : public QObject
{
Q_OBJECT
public:
	OrbiterBackend(QObject* parent = 0);
	virtual ~OrbiterBackend();

signals:
	void ImageReady(const QPixmap &Image);
	void Error(const QString &Message);

public slots:
	void Start();
	void Stop();
	void Touch(int X, int Y);

private slots:
	void TimerExpired();
	void AuthenticateHTTP(QNetworkReply *Reply, QAuthenticator *Authenticator);
	void RequestFinished(QNetworkReply *Reply);

private:
	QSettings *Settings;
	QTimer *RefreshTimer;
	QNetworkAccessManager *NetworkManager;

	bool bAuthenticationAttempted;
	bool bConnectionError;

	void DoCmd(const QString &Cmd);
	void GetImage();
	void GetRequest(const QNetworkRequest &Request);
	void ProcessCommandReply(QNetworkReply *Reply);
	void ProcessImageReply(QNetworkReply *Reply);
};

#endif // __ORBITERBACKEND_H__
