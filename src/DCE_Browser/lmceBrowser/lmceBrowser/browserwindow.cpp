
#include "browserwindow.h"

browserWindow::browserWindow(int p_pkDevice, QWidget *parent)
    : QMainWindow(parent)
{
    progress = 0;

         QFile file;
         file.setFileName(":/jquery.min.js");
         file.open(QIODevice::ReadOnly);
         jQuery = file.readAll();
         file.close();
        QWebSettings *defaultSettings = QWebSettings::globalSettings(); defaultSettings->setAttribute(QWebSettings::JavascriptEnabled, true); defaultSettings->setAttribute(QWebSettings::PluginsEnabled, true);
         view = new QWebView(this);

         bool bLocalMode = false;
         string sRouter_IP = "dcerouter";
         int PK_Device = p_pkDevice ;
         QWebSettings::PluginsEnabled;
         m_pBrowser = new DCE::DCE_Browser(PK_Device, sRouter_IP,true,bLocalMode);

         view->load(QUrl("http://www.linuxmce.org"));
         connect(view, SIGNAL(loadFinished(bool)), SLOT(adjustLocation()));
         connect(view, SIGNAL(titleChanged(const QString&)), SLOT(adjustTitle()));
         connect(view, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
         connect(view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));
         connect(this,SIGNAL(dceForward()), view, SLOT(forward()));
         connect(this, SIGNAL(dceUrl(QUrl)), this, SLOT(gotoPage(QUrl)));
         connect(this,SIGNAL(dceBack()), view, SLOT(back()));
         connect(this,SIGNAL(dceReload()), view,SLOT(reload()));

         setCentralWidget(view);
}

void browserWindow::adjustTitle()
 {
     if (progress <= 0 || progress >= 100)
         setWindowTitle(view->title());
     else
         setWindowTitle(QString("%1 (%2%)").arg(view->title()).arg(progress));
 }

 void browserWindow::setProgress(int p)
 {
     progress = p;
     adjustTitle();
 }

 void browserWindow::finishLoading(bool)
 {
     progress = 100;
     adjustTitle();
     view->page()->mainFrame()->evaluateJavaScript(jQuery);
 }


browserWindow::~browserWindow()
{

}

bool browserWindow::setupLMCE(int p_pkDevice)
{
    bool bLocalMode = false;
    string sRouter_IP = "dcerouter";
    int PK_Device = p_pkDevice ;
    m_pBrowser = new DCE::DCE_Browser(PK_Device, sRouter_IP,true,bLocalMode);

     if ( m_pBrowser)
         //->GetConfig() && m_pBrowser->Connect(m_pBrowser->PK_DeviceTemplate_get()) )
     {
         bConnected = m_pBrowser->GetConfig();

         qDebug()<< "Connecting";

         m_pBrowser->Register();
         m_pBrowser->qWebBrowser = this;
         QTimer::singleShot(5000,this,SLOT(keepLmAlive()));

        return false;
     }
     else {return true;}


    /*  bConnected = false;
    bReconnect = true;

    std::string sHost = "127.0.0.1";
    m_pBrowser = new DCE::DCE_Browser(m_qsDeviceID, sHost, true, false);

    if (m_pBrowser)
    {
        bConnected = m_pBrowser->GetConfig();

        writeLog("connected state:" + bConnected);
        m_pBrowser->Register();
        m_pBrowser->dceWebBrowser=this;


     if(bConnected == true)
         {
             writeLog("Connected to router with Device ID:" + QString::number(m_qsDeviceID));
             writeLog("at " + QTime::currentTime().toString());
         }
     else
         {
         writeLog("Couldnt Connect! Intialization failed!");
         }
     }
     */

}

void browserWindow::writeLog(QString text){
    cout << text.toStdString() << endl;
}

void browserWindow::gotoUrl(QUrl lm_url)
{
      emit dceUrl( lm_url);
}

bool browserWindow::keepLmAlive()
{
    m_pBrowser->GetConfig();
}

void browserWindow::gotoPage(QUrl iUrl)
{
    qDebug() << "Setting url!" << iUrl.toString() ;
    view->setUrl(iUrl);

}

void browserWindow::goForward()
{
    emit dceForward();

}

void browserWindow::goBack()
{
    emit dceBack();
}

void browserWindow::fontUp()
{
}

void browserWindow::fontDown()
{
}

void browserWindow::bReload()
{
    emit dceReload();
}
