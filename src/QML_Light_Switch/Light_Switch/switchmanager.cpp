
#include "switchmanager.h"
#include <QML_Light_Switch/QML_Light_Switch.h>
#include <Gen_Devices/QML_Light_SwitchBase.h>


namespace DCE
{
class QML_Light_Switch ;
}


switchManager::switchManager(int pk_DeviceID, QWidget *parent) :
    QMainWindow(parent)
{

 //window dressing and placement
this->move(350,250);
setAttribute(Qt::WA_TranslucentBackground);         //sets translucent bg
setStyleSheet("background:transparent;");         //set a stylesheet. required still possible
/* turn off window decorations */
setWindowFlags(Qt::FramelessWindowHint);
m_qsDeviceID = pk_DeviceID;

}

void switchManager::loadUI()
 {
       writeLog("Loading Ui");
       QTlightSwitch switchInstance;
      // QDeclarativeEngine *engine = new QDeclarativeEngine(this);
      // QDeclarativeComponent interFace(engine, QUrl::fromLocalFile("qrc:../../qml/Light_Switch/main.qml"));

     //  qDebug() << interFace.errors();
     //  qml_view = interFace.create();

       ui = new QDeclarativeView;

       ui->rootContext()->setContextProperty("switch1" ,&switchInstance);
       //declarative view deals with Qgraphicsscene and qgraphics view functions.
       ui->setSource(QUrl("qrc:/QML/qml/Light_Switch/main.qml"));


       QObject *qml_view = dynamic_cast<QObject *>(ui->rootObject());

      QObject::connect(qml_view,SIGNAL(l_state(QString)), this, SLOT(writeLog(QString)));
       QObject::connect(qml_view,SIGNAL(close_app()), this, SLOT(close_app()));

       setCentralWidget(ui);
}


 bool switchManager::initialize_LMdevice(bool bRetryForever)
 {
     bConnected = false;
     bReconnect = true;

     std::string sHost = "127.0.0.1";

   //  class Router *pRouter = 0;
    writeLog("Connecting to router with Device ID:" + m_qsDeviceID );
     m_pLightSwitch = new DCE::QML_Light_Switch(m_qsDeviceID, sHost, true, false);

    if (m_pLightSwitch)
    {
        bConnected = m_pLightSwitch->GetConfig();

      QString * r_lightLevel = new QString(QString::fromStdString(m_pLightSwitch->GetStatus(m_qsDeviceID)));

        writeLog("connected state:" + bConnected);
        m_pLightSwitch->Register();
        m_pLightSwitch->dceLightSwitch=this;

        QTimer::singleShot(5000,this,SLOT(LMdeviceKeepAlive()));
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
 }

 void switchManager::initialize_Connections()
 {


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
     QString timestamp = QDateTime::currentDateTime().toString();
     std::cout << qPrintable(timestamp) << ": " << msg.toStdString() << endl;
     qDebug() << qPrintable(msg) << endl;
     //DCE::LoggerWrapper::GetInstance()->Write(1, &msg);

 }

 bool switchManager::check_log()
 {

     //local qt log
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
    //dce logging


 }

 void switchManager::light_on()
 {


 }

 void switchManager::light_off()
 {

    }

 void switchManager::set_level()
 {
    writeLog("Got DCE Command to set level to :");
 }

 void switchManager::LMdeviceKeepAlive()
 {  writeLog("Keeping Alive..");

     QString state = QString::fromStdString(m_pLightSwitch->GetStatus(m_qsDeviceID));
   //  qDebug() << "DCE State response:" <<  state;
    // qDebug() << "Running State: " <<  m_pLightSwitch->m_bRunning;
     if (m_pLightSwitch)
     {
             // if device is finishing
             if (m_pLightSwitch->m_bTerminate)
             {
                     writeLog("Connection to DCERouter terminated");
                     bool bReload = m_pLightSwitch->m_bReload;
                     deinitialize_LMdevice();
                     if (bReload)
                     {
                             writeLog("Reconnecting to DCERouter as requested");
                             if (!initialize_LMdevice())
                             {
                                     writeLog("Will retry later..");
                                     QTimer::singleShot(5000, this, SLOT(LMdeviceKeepAlive()));
                             }
                     }
             }
             else
             {
                 QTimer::singleShot(5000, this, SLOT(LMdeviceKeepAlive()));
            }
     }
 }

 void switchManager::deinitialize_LMdevice()
 {
     if (m_pLightSwitch)
     {
             m_pLightSwitch->OnQuit();
             delete m_pLightSwitch;
             m_pLightSwitch = NULL;

     }
        writeLog("DCE Lost connection at:" + QTime::currentTime().toString());
     if (this->window()->isActiveWindow())
     {
        this->initialize_LMdevice(TRUE);
     }

 }

 void switchManager::initialize_Start()
 {
     initialize_Connections();
     initialize_LMdevice(true);
 }

 void switchManager::r_Off()
{
 writeLog("Recieved OFF from DCE!");
 QVariant returnedValue;
  QVariant msg = "OFF";
  QVariant val = "0";
  writeLog("Invoking Method");
 QMetaObject::invokeMethod(ui->rootObject(), "dceCommand", Qt::DirectConnection, Q_RETURN_ARG(QVariant, returnedValue), Q_ARG(QVariant, msg), Q_ARG(QVariant, val));
qDebug() << returnedValue;
// this->switchInstance.dce_off();
 }

 void switchManager::r_On()
 {
     writeLog("Recieved ON from DCE!");
     QVariant returnedValue;
      QVariant msg = "ON";
      QVariant val = "100";
      writeLog("Invoking Method");
    QMetaObject::invokeMethod(ui->rootObject(), "dceCommand", Qt::DirectConnection, Q_RETURN_ARG(QVariant, returnedValue), Q_ARG(QVariant, msg), Q_ARG(QVariant, val));
  qDebug() << returnedValue;
    // this->switchInstance.dce_on();
 }

 void switchManager::r_setLevel(int rLevel)
 {
  writeLog("Recived message to set level to " + QString::number(rLevel) + " from DCE!");
  QVariant returnedValue;
   QVariant msg = "ON";
   QVariant val = rLevel;
   writeLog("Invoking Method");
 QMetaObject::invokeMethod(ui->rootObject(), "dceCommand", Qt::DirectConnection, Q_RETURN_ARG(QVariant, returnedValue), Q_ARG(QVariant, msg), Q_ARG(QVariant, val));
qDebug() << returnedValue;
 }
