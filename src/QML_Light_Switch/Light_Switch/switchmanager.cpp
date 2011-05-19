
#include "switchmanager.h"
#include <QML_Light_Switch/QML_Light_Switch.h>

switchManager::switchManager(QWidget *parent) :
    QMainWindow(parent)
{

                                    //setting fixed size for now
    this->move(350,250);
    setAttribute(Qt::WA_TranslucentBackground);         //sets translucent bg
      setStyleSheet("background:transparent;");         //set a stylesheet. required still possible
      /* turn off window decorations */
      setWindowFlags(Qt::FramelessWindowHint);
loadUI();
openDB();

m_qsMySQLPort = "3306";
}

void switchManager::loadUI()
 {
     ui = new QDeclarativeView;                        //declarative view deals with Qgraphicsscene and qgraphics view functions.
       ui->setSource(QUrl("qrc:/QML/qml/Light_Switch/main.qml"));
       setCentralWidget(ui);                           //it also has been subclassed from qwidget, allowing it to be the central widget for this window
 }

 int switchManager::initializeDCE()
 {
   //  DCE::QML_Light_Switch *dcePanel = new DCE::QML_Light_Switch();
     return 0;
 }

 bool switchManager::checkMySQL(bool showMessageBox)
 {

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
          //   writeLog("Successful", true);
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
     std::string sHost = "127.0.0.1";
     int m_TdeviceID = 0;
     class router *pRouter = 0;

     m_pLightSwitch = new DCE::QML_Light_Switch(m_TdeviceID, sHost, true, false, pRouter);

 }
