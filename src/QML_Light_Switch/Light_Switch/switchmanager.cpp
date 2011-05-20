
#include "switchmanager.h"
#include <QML_Light_Switch/QML_Light_Switch.h>
#include <Gen_Devices/QML_Light_SwitchBase.h>


namespace DCE
{
class QML_Light_Switch ;
}


switchManager::switchManager(QWidget *parent) :
    QMainWindow(parent)
{

 //window dressing and placement
this->move(350,250);
setAttribute(Qt::WA_TranslucentBackground);         //sets translucent bg
setStyleSheet("background:transparent;");         //set a stylesheet. required still possible
/* turn off window decorations */
setWindowFlags(Qt::FramelessWindowHint);

initialize_Connections();
initialize_LMdevice(true);

}

void switchManager::loadUI()
 {
    writeLog("Loading Ui");
       ui = new QDeclarativeView;                        //declarative view deals with Qgraphicsscene and qgraphics view functions.
       ui->setSource(QUrl("qrc:/QML/qml/Light_Switch/main.qml"));
       setCentralWidget(ui);                           //it also has been subclassed from qwidget, allowing it to be the central widget for this window

       QObject *qml_view = dynamic_cast<QObject *>(ui->rootObject());

       QObject::connect(qml_view,SIGNAL(close_app()), this, SLOT(close_app()));
       QObject::connect(qml_view,SIGNAL(writeLog(QString)), this, SLOT(writeLog(QString)));
}


 bool switchManager::initialize_LMdevice(bool bRetryForever)
 {
     bConnected = false;

     std::string sHost = "127.0.0.1";
     int m_TdeviceID = 0;
   //  class Router *pRouter = 0;


    /* problem block which returns "error: undefined reference to
    DCE::QML_Light_Switch::QML_Light_Switch(int, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool, bool, DCE::Router*)"*/

    writeLog("Connecting to router");
     m_pLightSwitch = new DCE::QML_Light_Switch(m_TdeviceID, sHost, true, false);

    if (m_pLightSwitch)
    {

    m_pLightSwitch->dceLightSwitch=this;
   //  bConnected = m_pLightSwitch->GetConfig();

     if(bConnected == true)
         {writeLog("Connected to router!");}
     else
         {
         writeLog("Couldnt Connect! Intialization failed!");
         }

     }
 }

 void switchManager::initialize_Connections()
 {
     loadUI();

     if (check_log() == false)
     {
         writeLog("Logging unavailible, file error");
     }
     else
     {
         writeLog("Logfile Check!");
     }

 }

 void switchManager::close_app()
 {

     writeLog("Closing DB, exiting app");
     fclose (stdout);
     this->close();
 }

 void switchManager::writeLog(QString msg)
 {

     std::cout << msg.toStdString() << endl;
 }

 bool switchManager::check_log()
 {
    QFile logFile("/var/log/QML_Light_Switch.log");

    if (logFile.open(QFile::WriteOnly) == true)
    {
    logFile.close();
    freopen ("/var/log/QML_Light_Switch.log","a+",stdout); //open the log
    return true;
    }
    else
    {
        return false;
    }
 }
