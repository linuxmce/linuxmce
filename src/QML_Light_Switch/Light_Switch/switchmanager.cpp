
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
m_qsMySQLPort = "3306";

initialize_Connections();
initialize_LMdevice(true);

}

void switchManager::loadUI()
 {
       ui = new QDeclarativeView;                        //declarative view deals with Qgraphicsscene and qgraphics view functions.
       ui->setSource(QUrl("qrc:/QML/qml/Light_Switch/main.qml"));
       setCentralWidget(ui);                           //it also has been subclassed from qwidget, allowing it to be the central widget for this window

       QObject *qml_view = dynamic_cast<QObject *>(ui->rootObject());
       QObject::connect(qml_view,SIGNAL(close_app()), this, SLOT(close_app()));
}


 bool switchManager::checkMySQL(bool showMessageBox)
 {
    openDB();
    closeDB();
 }

 bool switchManager::closeDB()
 {
     pPlutoDatabase.close();
 }

 bool switchManager::openDB()
 {
     if (pPlutoDatabase.isOpen())
     {
            // writeLog("Attempted to reopen already opened DB, closing current connection first", false, LV_WARNING);
             closeDB();
     }

    // writeLog("Opening DB connection..", true);
     pPlutoDatabase =QSqlDatabase::addDatabase("QMYSQL");
     qDebug() << "Availible Drivers" << pPlutoDatabase.drivers();
     //pPlutoDatabase.addDatabase( "QMYSQL" );
     qDebug() << "Current Driver" << pPlutoDatabase.driver() << endl;
     pPlutoDatabase.setDatabaseName( "pluto_main" );
     pPlutoDatabase.setUserName( "root" );
     pPlutoDatabase.setPassword( "" );
     pPlutoDatabase.setHostName( "dcerouter" );
   //  pPlutoDatabase->setPort(3306);

     if (pPlutoDatabase.open())
     {
             //writeLog("Successful", true);
         qDebug() << "db connect success"<< endl;

             return true;
     }
     else
     {
          //   writeLog("Error: " + pPlutoDatabase->lastError().text(), true, LV_WARNING);
            // QSqlDatabase::removeDatabase(pPlutoDatabase);
             qDebug() << "db connect fail"<< endl;
             pPlutoDatabase.close();
             return false;
     }
 }

 bool switchManager::initialize_LMdevice(bool bRetryForever)
 {
     bConnected = false;

     std::string sHost = "127.0.0.1";
     int m_TdeviceID = 0;
   //  class Router *pRouter = 0;


    /* problem block which returns "error: undefined reference to
    `DCE::QML_Light_Switch::QML_Light_Switch(int, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool, bool, DCE::Router*)"

    qDebug() << "Connecting to router" << endl;
     m_pLightSwitch = new DCE::QML_Light_Switch(m_TdeviceID, sHost, true, false);
    //bool bConnected = m_pLightSwitch->GetConfig();
    m_pLightSwitch->dceLightSwitch=this;

     if(bConnected == true)
     {qDebug() << "Connected to router!" << endl;}
     else
     {
         qDebug() << "Couldnt Connect!" << endl;
     }
    */
     qDebug() << "Skipped DCERouter Device initilization" << endl;

 }

 void switchManager::initialize_Connections()
 {
     checkMySQL(false);
     loadUI();

 }

 void switchManager::close_app()
 {
     closeDB();
     this->close();
 }
