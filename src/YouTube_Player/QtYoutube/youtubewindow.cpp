#include "youtubewindow.h"
#include <QtNetwork>
#include <QtWebKit>
#include <QDebug>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/Xmu/WinUtil.h>

YouTubeWindow::YouTubeWindow(int iPK_Device)
{
    QWebSettings *defaultSettings = QWebSettings::globalSettings(); 
    defaultSettings->setAttribute(QWebSettings::JavascriptEnabled, true); 
    defaultSettings->setAttribute(QWebSettings::PluginsEnabled, false);
    view = new QWebView(this);

    bool bLocalMode = false;
    string sRouter_IP = "dcerouter";
    m_pBrowser = new DCE::YouTube_Player(iPK_Device, sRouter_IP,true,bLocalMode);
    view->load(QUrl("http://www.youtube.com/tv"));
    setCentralWidget(view);

    Display *display = XOpenDisplay(getenv("DISPLAY"));
    Window window = winId();

    // Set WM_CLASS and WM_NAME for Orbiter
    XClassHint *classHint;
    classHint = XAllocClassHint();

    if (classHint)
      {
	classHint->res_name = "youtube";
	classHint->res_class = "Youtube";
      }

    XSetClassHint(display, window, classHint);
    XFree(classHint);

    XCloseDisplay(display);

}

YouTubeWindow::~YouTubeWindow()
{

}

bool YouTubeWindow::setupLMCE(int iPK_Device)
{
    bool bLocalMode = false;
    string sRouter_IP = "dcerouter";
    m_pBrowser = new DCE::YouTube_Player(iPK_Device, sRouter_IP,true,bLocalMode);

     if ( m_pBrowser)
         //->GetConfig() && m_pBrowser->Connect(m_pBrowser->PK_DeviceTemplate_get()) )
     {
         bConnected = m_pBrowser->GetConfig();

         m_pBrowser->Register();
         m_pBrowser->qYouTubeWindow = this;
         QTimer::singleShot(5000,this,SLOT(keepAlive()));

        return false;
     }
     else {return true;}

}

void YouTubeWindow::gotoPage(QUrl iUrl)
{

}

void YouTubeWindow::adjustTitle()
{

}

void YouTubeWindow::finishLoading(bool bFinish)
{

}

bool YouTubeWindow::keepAlive()
{
    m_pBrowser->GetConfig();
}

void YouTubeWindow::bReload()
{
    // emit dceReload();
}

void YouTubeWindow::moveUp()
{
  // TODO
}

void YouTubeWindow::moveDown()
{
  // TODO
}

void YouTubeWindow::moveLeft()
{
  // TODO
}

void YouTubeWindow::moveRight()
{
  // TODO
}

void YouTubeWindow::buttonOK()
{
  // TODO
}

void YouTubeWindow::buttonHome()
{
  // TODO
}

void YouTubeWindow::buttonBack()
{
  // TODO
}
