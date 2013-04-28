#ifndef YOUTUBEWINDOW_H
#define YOUTUBEWINDOW_H

#include <QtGui>
#include <QMainWindow>
#include <QtWebKitWidgets>

#include "../../Gen_Devices/AllCommandsRequests.h"
#include "../YouTube_Player.h"

namespace DCE 
{
  class YouTube_Player;
}

class YouTubeWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit YouTubeWindow(int iPK_Device);
    ~YouTubeWindow();

    DCE::YouTube_Player *m_pBrowser;
    void gotoUrl(QUrl lm_url);
    bool setupLMCE(int iPK_Device);

    public slots:
    void gotoPage(QUrl iUrl);
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void buttonOK();
    void buttonHome();
    void buttonBack();
    void bReload();

    protected slots:
    void adjustTitle();
    void finishLoading(bool);
    bool keepAlive();
    
private:
    QWebView *view;
    bool bReconnect;
    int m_qsDeviceID;
    bool bConnected;
};

#endif // YOUTUBEWINDOW_H
