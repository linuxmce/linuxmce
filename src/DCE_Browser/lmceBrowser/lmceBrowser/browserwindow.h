#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QtGui/QMainWindow>
#include <QtWebKit/QtWebKit>
#include <QLineEdit>

#include <Gen_Devices/AllCommandsRequests.h>
#include "../../DCE_Browser.h"

namespace DCE
{
class DCE_Browser;
}

class browserWindow : public QMainWindow
{
    Q_OBJECT

public:
    browserWindow(int p_pkDevice, QWidget *parent = 0);
    ~browserWindow();

    DCE::DCE_Browser *m_pBrowser;
    void gotoUrl(QUrl lm_url);
    bool setupLMCE(int);


public slots:
    void gotoPage(QUrl iUrl);
    void goForward();
    void goBack();
    void fontUp();
    void fontDown();
    void bReload();

protected slots:
     void adjustTitle();
     void setProgress(int p);
     void finishLoading(bool);
     bool keepLmAlive();


 private:
     QString jQuery;
     QWebView *view;
     QLineEdit *locationEdit;

     int progress;
     bool bReconnect;
     //bool bConnected;
     int m_qsDeviceID;

     bool bConnected;


     void writeLog (QString);

signals:
     void loadComplete();
     void dceForward();
     void dceBack();
     void dceReload();
     void dceFontUp();
     void dceFontDown();
     void dceUrl (QUrl);
};

#endif // BROWSERWINDOW_H
