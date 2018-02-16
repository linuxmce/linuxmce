/*
     Copyright (C) 2013 LinuxMCE

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "HueController.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include <QDebug>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qeventloop.h>
#include <QVariantMap>
#include <QJsonObject>
#include <QJsonDocument>
#include <QStringList>
#include <huecontrollerhardware.h>
#include <pthread.h>
#include <QtGui/QColor>
#include <math.h>
#include <QDate>

const char HueController::getGroups[] ="http://%s/api/%s/groups,GET";
const char HueController::getNewLights[] = "http://%s/api/%s/lights/new,GET";
const char HueController::searchNewLights[]= "http://%s/api/%s/lights,POST";
const char HueController::getLightInfo[]="http://%s/api/%s/lights/%s, GET";
const char HueController::renameLight[]="http://%s/api/%s/lights/%s,PUT";
const char HueController::setLightState[]="http://%s/api/%s/lights/%s/state,PUT";

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
HueController::HueController(int DeviceID, string ServerAddress, bool bConnectEventHandler, bool bLocalMode, class Router *pRouter,QObject*parent)
    : DCE::HueController_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter),
      m_updateStatus(true),
      mb_isNew(true),
      validated(false),
      linkButton(false),
      triesLeft(6),
      targetIpAddress(""),
      authUser(""),
      mp_linkButtonTimer(new QTimer()),
      mp_lightRefreshTimer(new QTimer()),
      mp_sensorRefreshTimer(new QTimer()),
      mp_queueCommandTimer(new QTimer()),
      linkButtonManager(new QNetworkAccessManager()),
      commandManager(new QNetworkAccessManager()),
      lightRequestManager(new QNetworkAccessManager()),
      sensorRequestManager(new QNetworkAccessManager()),
      m_dayNightSensor(new HueDayNightSensor() )
    //<-dceag-const-e->
{

    LoggerWrapper::GetInstance()->Write(LV_WARNING, "HueController, Device %d is alive!", this->m_dwPK_Device);

    //DEVICETEMPLATE_Hue_Lightbulb_CONST or DEVICETEMPLATE_Hue_Light_Strips_CONST
    m_extendedColorLightModels = QStringList() << "LCT001"<< "LCT007"<< "LCT010"<< "LCT014"<< "LCT015"
                                               << "LCT016"<< "LCT002" << "LCT003"<< "LCT011" << "LLM001"
                                               << "LLC020" << "LST002" << "LCT012" ;

    //DEVICETEMPLATE_Hue_Lightbulb_CONST or DEVICETEMPLATE_Hue_Light_Strips_CONST
    m_colorLightModels =        QStringList()  << "LST001" << "LLC010" << "LLC011" << "LLC012" << "LLC006"
                                               << "LLC005" << "LLC007" << "LLC014" << "LLC013" ;
    //DEVICETEMPLATE_Hue_White_Ambiance_CONST
    m_colorTemperatureModels =  QStringList()  << "LTW011" << "LLM010" <<  "LLM011" << "LLM012" << "LTW001"
                                               << "LTW004" << "LTW010" << "LTW015" << "LTW013" << "LTW014"
                                               << "LTW012" << "LTP001" << "LTP002" << "LTP003" << "LTP004"
                                               << "LTP005" << "LTD003" << "LTT001" <<  "LDT001" ;

    //DEVICETEMPLATE_Hue_Lux_Bulb_CONST
    m_dimmableModels =          QStringList()  << "LWB004" << "LWB006" << "LWB007" << "LWB010" << "LWB014"
                                               << "LDF002" << "LDF001" << "LDD002" << "LDD001" << "MWM001";



    QObject::connect(this, SIGNAL(initiateConfigDownload(QUrl)), this, SLOT(downloadControllerConfig(QUrl)));

    QObject::connect(mp_lightRefreshTimer, SIGNAL(timeout()), this, SLOT(checkLightInformation()));
    QObject::connect(lightRequestManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleCheckLightInformation(QNetworkReply*)));
    mp_lightRefreshTimer->setInterval(5000);
    mp_lightRefreshTimer->start();

    QObject::connect(mp_sensorRefreshTimer, SIGNAL(timeout()), this, SLOT(checkSensorInformation()));
    QObject::connect(sensorRequestManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleCheckSensorInformation(QNetworkReply*)));
    mp_sensorRefreshTimer->setInterval(1500);
    mp_sensorRefreshTimer->start();


    mp_queueCommandTimer->setInterval(150);
    mp_queueCommandTimer->setSingleShot(false);
    connect(mp_queueCommandTimer, SIGNAL(timeout()) ,this, SLOT(sendCommandMessage()));
    connect(this, SIGNAL(startQueue()), mp_queueCommandTimer, SLOT(start()));
    connect(this, SIGNAL(lastCommand()), mp_queueCommandTimer, SLOT(stop()));

    //    connect(this, SIGNAL(lastCommand()), mp_lightRefreshTimer, SLOT(start()));
    //    connect(this, SIGNAL(startQueue()), mp_lightRefreshTimer, SLOT(stop()));


    mp_linkButtonTimer->setInterval(3000);
    QObject::connect(mp_linkButtonTimer, SIGNAL(timeout()), this, SLOT(checkLinkButton()));
    mp_linkButtonTimer->setSingleShot(false);

}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
//HueController::HueController(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
//    : DCE::HueController_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
//{
//}

//<-dceag-dest-b->
HueController::~HueController()
//<-dceag-dest-e->
{

}
bool HueController::checkLinkButton(){
    //  LoggerWrapper::GetInstance()->Write(LV_STATUS, "Checking Link Button Status");
    QString lclUser=authUser;
    if(authUser==""||authUser=="linuxmceSetup"){
        lclUser="newdeveloper";
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Device not setup, using system username :: newdeveloper");
    }


    QUrl setupUrl = "http://"+targetIpAddress+"/api/"+lclUser;
    //  qDebug()<< setupUrl.toString();

    QNetworkRequest setupReq(setupUrl);
    QNetworkReply * setupReply = linkButtonManager->get(setupReq);
    QEventLoop respWait;
    QObject::connect(linkButtonManager, SIGNAL(finished(QNetworkReply*)), &respWait, SLOT(quit()));
    respWait.exec();

    QByteArray setupResponse = setupReply->readAll();

    QVariantMap res = QJsonDocument::fromBinaryData(setupResponse).object().toVariantMap();


    if(res.keys().contains("error")){
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "Link Button watcher::unknown");
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Could not connect to %s ! Response was invalid. Please check the device ip and that it is setup.", targetIpAddress.toStdString().c_str());
        this->setLinkButton(false);
        return false;
    } else if (res.keys().contains("config")){
        QVariantMap  configBlock = res["config"].toMap();
        if(configBlock["linkbutton"].toBool()==true){

            this->setLinkButton(true);
            mp_linkButtonTimer->stop();
            return true;
        }else {
            this->setLinkButton(false);
            return false;
        }
    }

    return false;

}

bool HueController::setupController(int controllerIndex){

    QString installationTag = QString::number(m_pData->m_dwPK_Installation);
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Setting up Hue Hub with LinuxMCE-%s ", installationTag.toStdString().c_str());

    QUrl setupUrl = "http://"+targetIpAddress+"/api/";
    QVariantMap setupParams;
    installationTag.prepend("LinuxMCE-");
    setupParams.insert("devicetype",installationTag);

    QByteArray dat = QJsonDocument::fromVariant(setupParams).toBinaryData();
    QNetworkAccessManager *setupManager = new QNetworkAccessManager;
    QNetworkRequest setupReq(setupUrl);
    QNetworkReply * setupReply = setupManager->post(setupReq,dat);
    QEventLoop respWait;
    QObject::connect(setupManager, SIGNAL(finished(QNetworkReply*)), &respWait, SLOT(quit()));
    respWait.exec();

    QByteArray setupResponse = setupReply->readAll();
    setupResponse.remove(0,1);
    setupResponse.remove(setupResponse.length()-1, 1);

    QVariantMap res = QJsonDocument::fromBinaryData(setupResponse).object().toVariantMap();

    if(res.isEmpty()){
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Could not connect to %s ! Response was invalid. Please check the device ip and that it is setup.", targetIpAddress.toStdString().c_str());
        return false;
    }

    if(res.keys().contains("error")){
        QVariantMap errorDetails = res["error"].toMap();
        QString error_msg= errorDetails["description"].toString();
        switch(errorDetails["type"].toInt()){
        case 101:
            LoggerWrapper::GetInstance()->Write(LV_WARNING, "Setup command failed. Reason to follow::%s",error_msg.toStdString().c_str() );

            /* Send Message to Screen somehow */

            if(linkButton==true){
                setupController(controllerIndex);
                //  qDebug()<< linkButton;
            } else {
                /* delay  and wait until button is pressed */

                return false;
            }
            break;

        default:
            LoggerWrapper::GetInstance()->Write(LV_WARNING, "Setup command failed. Reason to follow::%s",error_msg.toStdString().c_str() );
            return false;

        }
    } else {

        QVariantMap sucessDetail = res["success"].toMap();     ;
        authUser = sucessDetail["username"].toString();
        mp_linkButtonTimer->stop();
        this->DATA_Set_Username(authUser.toStdString(), true);
        // pthread_yield();
        return true;
    }
    return false;
}


//<-dceag-getconfig-b->
bool HueController::GetConfig()
{
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Hue Controller Device getting configuration");

    if( !HueController_Command::GetConfig() )
        return false;
    //<-dceag-getconfig-e->

    mb_isNew = true;

    // Put your code here to initialize the data in this class
    // The configuration parameters DATA_ are now populated

    QStringList db_controllers;
    db_controllers.append(QString::fromStdString(m_pData->GetIPAddress().c_str()));

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Controllers found: %s", db_controllers.join("||").toStdString().c_str());

    if(db_controllers.isEmpty()){
        //  pthread_yield();
        return false;
    }

    authUser = QString::fromStdString(DATA_Get_Username().c_str());

    for (int i = 0; i < db_controllers.length(); i++){
        HueControllerHardware *controllerDevice = new HueControllerHardware();
        targetIpAddress = db_controllers.at(i);
        controllerDevice->m_ipAddress = db_controllers.at(i);
        hueControllers.append(controllerDevice);

        if(authUser=="linuxmceSetup" || authUser.isEmpty()){
            /* send screen message to press button here */

            LoggerWrapper::GetInstance()->Write(LV_STATUS, "Default Username Found. Will now attempt to setup device");
            this->mb_isNew = true;
            if(setupController(hueControllers.count()-1)){

                // DCE::CMD_Reload reloadRouter(this->m_dwPK_Device, 1);
                // SendCommand(reloadRouter);

            }else{
                LoggerWrapper::GetInstance()->Write(LV_STATUS, "Failed to setup device!");
                // pthread_yield();
                return false;
            }

        } else {
            this->mb_isNew = true;
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "Existing Username %s Found. Delete and re-run setup to change.", authUser.toStdString().c_str());
        }

        if(downloadControllerConfig(QUrl(controllerDevice->m_ipAddress))){
            qDebug() << "Finished processing controller " << i;
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "Finished processing controller #%d", i);
            mp_linkButtonTimer->start();

            if( (int)m_pData->m_vectDeviceData_Impl_Children.size() != hueBulbs.length()+hueMotionSensors.length() ){
                this->CMD_Report_Child_Devices();
            }
            //  pthread_yield();
            return true;
        }
    }


    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Finished with all Hue Controllers. Total Count: %d", db_controllers.size());

    //        if(mb_isNew==true){
    //
    //            qDebug()<< "Reporting new devices";
    //                this->CMD_Report_Child_Devices();
    //        }
    //        else{
    //            qDebug() << "not reporting new devices.";
    //        }
    return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool HueController::Register()
//<-dceag-reg-e->
{
    return Connect(PK_DeviceTemplate_get());
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
    cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You
    can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
//HueController_Command *Create_HueController(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
//{
//    return new HueController(pPrimaryDeviceCommand, pData, pEvent, pRouter);
//}
//<-dceag-createinst-e->

/*
    When you receive commands that are destined to one of your children,
    then if that child implements DCE then there will already be a separate class
    created for the child that will get the message.  If the child does not, then you will
    get all	commands for your children in ReceivedCommandForChild, where
    pDeviceData_Base is the child device.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void HueController::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{

    Q_UNUSED(pDeviceData_Impl)
    mp_linkButtonTimer->stop();
    // qDebug() << Q_FUNC_INFO << pMessage->ToString().c_str();
    QStringList command = QString::fromStdString(pMessage->ToString(false)).remove("-o ").remove("\"").split(" ");
    int cmd = command.at(3).toInt();
    int device = pMessage->m_dwPK_Device_To;

    int ID=-1;
    AbstractWirelessBulb *bulb;
    for (int f = 0; f < hueBulbs.size(); ++f){
        if(hueBulbs.at(f)->linuxmceId()==device){
            bulb=hueBulbs.at(f);
            ID = bulb->id();
        }
    }

    QVariantMap params;
    QUrl target;

    if(targetIpAddress.isEmpty() || ID == -1){
        //        qDebug() << "Invalid command for ID "<<ID << " linuxmce id " << device;
        //        qDebug() << "target ip"<<targetIpAddress;
        sCMD_Result="Failed";
        return;
    }
    int conversion_var = ceil(65280 / 360);

    QString setLevelVal;

    if(command.length() > 5 && cmd ==184){
        // qDebug()<<"CMD_SET_LEVEL::"<< setLevelVal;
        setLevelVal = command.at(5);
        setLevelVal.remove("\"");
        bulb->setCurrentLevel(setLevelVal.toDouble());
        bulb->setPowerOn(true);
    }

    QColor q;
    if(command.length() > 10){
        int redLevel = command.at(5).toInt();
        int greenLevel = command.at(7).toInt();
        int blueLevel = command.at(9).toInt();
        q.setRgb(redLevel,greenLevel,blueLevel);
        bulb->setRgb(redLevel, greenLevel, blueLevel);
        //  qDebug() << Q_FUNC_INFO <<"CMD_SET_COLOR+RGB::"<<redLevel<<"::" <<greenLevel << "::"<< blueLevel;
    }

    switch (cmd) {
    case 193:
        target.setUrl("http://"+targetIpAddress+"/api/"+authUser+"/lights/"+QString::number(ID)+"/state");
        sCMD_Result = "OFF - OK";
        params.insert("on", false);
        bulb->setPowerOn(false);
        if(addMessageToQueue(target, params)){
            sCMD_Result = "OFF - OK";

        }
        else{
            sCMD_Result = "Failed";
        }

        break;
    case 192:
        target.setUrl("http://"+targetIpAddress+"/api/"+authUser+"/lights/"+QString::number(ID)+"/state");
        params.insert("on", true);
        bulb->setPowerOn(true);
        if(addMessageToQueue(target, params))
            sCMD_Result = "ON - OK";

        break;
    case 184:
        qDebug() << command.at(5); //.replace("","\"");

        target.setUrl("http://"+targetIpAddress+"/api/"+authUser+"/lights/"+QString::number(ID)+"/state");
        params.insert("on", true);
        params.insert("bri",int(setLevelVal.toInt()*2.55));
        bulb->setPowerOn(true);
        bulb->setBrightness(setLevelVal.toUInt()*2.55);
        if(addMessageToQueue(target, params))
            sCMD_Result = "SET LEVEL - OK";

        break;
    case 980:
        target.setUrl("http://"+targetIpAddress+"/api/"+authUser+"/lights/"+QString::number(ID)+"/state");
        params.insert("on", true);
        params.insert("bri",q.lightness());
        params.insert("hue",( q.hsvHue()*conversion_var) );
        params.insert("sat",q.hsvSaturation());
        addMessageToQueue(target, params);
        sCMD_Result = "SET RBG - OK";
    case 641:
        target.setUrl("http://"+targetIpAddress+"/api/"+authUser+"/lights/"+QString::number(ID)+"/state");
        params.insert("on", true);
        params.insert("alert",command.at(5).toLower()=="on" ? "lselect":"none");
        if(addMessageToQueue(target, params)){
            sCMD_Result = "SET STROBE - OK";
        }
        break;
    case 550:
        target.setUrl("http://"+targetIpAddress+"/api/"+authUser+"/lights/"+QString::number(ID)+"/state");
        params.insert("on", true);
        params.insert("effect",command.at(5).toLower());
        if(addMessageToQueue(target, params)) {sCMD_Result = "SET MODE OK"; }

        break;
    case 1160:
        setLevelVal = command.at(5);
        setLevelVal.remove("\"");
        target.setUrl("http://"+targetIpAddress+"/api/"+authUser+"/lights/"+QString::number(ID)+"/state");
        params.insert("on", true);
        params.insert("ct",setLevelVal.toInt()*.10);
        qDebug() << setLevelVal.toInt()*.10;
        if(addMessageToQueue(target, params)) {sCMD_Result = "SET CT OK"; }

        break;

    default:
        sCMD_Result = "NOT IMPLEMENTED";
        break;
    }
    //  qDebug() << "Command " << cmd;

    // updateDevice(bulb, ID);
}

/*
    When you received a valid command, but it wasn't for one of your children,
    then ReceivedUnknownCommand gets called.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void HueController::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN COMMAND";
}

void HueController::CreateChildren()
{
    qDebug() << "creating children";
    for( int i=0; i < (int)m_pData->m_vectDeviceData_Impl_Children.size(); i++ )
    {
        DeviceData_Impl *pDeviceData_Impl_Child = m_pData->m_vectDeviceData_Impl_Children[i];
        qDebug() << "Checking child device #"<<pDeviceData_Impl_Child->m_dwPK_Device;

        // This device was marked as disabled
        if (pDeviceData_Impl_Child->m_bDisabled)
        {
            LoggerWrapper::GetInstance()->Write(LV_WARNING, "Child device %d is disabled", pDeviceData_Impl_Child->m_dwPK_Device);
            continue;
        }
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Creating child %d", pDeviceData_Impl_Child->m_dwPK_Device);

        // This device has it's own executible. Try to spawn it. If that fails, we will try to create it ourselves
        if( pDeviceData_Impl_Child->m_bImplementsDCE && !pDeviceData_Impl_Child->m_bIsEmbedded )
        {
            if( SpawnChildDevice(pDeviceData_Impl_Child->m_dwPK_Device,pDeviceData_Impl_Child->m_sCommandLine) )
                continue;
        }
        Event_Impl *pEvent = m_pEvent->CreateEvent( pDeviceData_Impl_Child->m_dwPK_DeviceTemplate, m_pPrimaryDeviceCommand->m_pEvent->m_pClientSocket, pDeviceData_Impl_Child->m_dwPK_Device );
        if ( !pEvent )
        {
            pEvent = new Event_Impl( m_pPrimaryDeviceCommand->m_pEvent->m_pClientSocket, pDeviceData_Impl_Child->m_dwPK_Device );
            LoggerWrapper::GetInstance()->Write( LV_WARNING, "Note: Device manager has attached a device of type %d that this has no custom event handler for.  It will not fire events.",
                                                 pDeviceData_Impl_Child->m_dwPK_DeviceTemplate);
        }
        Command_Impl *pCommand = m_pPrimaryDeviceCommand->CreateCommand( pDeviceData_Impl_Child->m_dwPK_DeviceTemplate, m_pPrimaryDeviceCommand, pDeviceData_Impl_Child, pEvent );
        if ( !pCommand )
        {
            pCommand = new Command_Impl(m_pPrimaryDeviceCommand, pDeviceData_Impl_Child, pEvent, m_pRouter);
            pCommand->m_bGeneric=true;
            LoggerWrapper::GetInstance()->Write(LV_WARNING, "Note: Device manager has attached a device of type %d that this has no custom handler for.  This is normal for IR.", pDeviceData_Impl_Child->m_dwPK_DeviceTemplate);
            int linuxmceID = pDeviceData_Impl_Child->m_dwPK_Device;

            QStringList p = QString::fromStdString(pDeviceData_Impl_Child->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST)).split(":");
            int deviceID = p.last().toInt();

            if(deviceID < 1 || linuxmceID < 1){
                qDebug() << "Child device "<< linuxmceID << " is misconfigured. please reload configuration.";
            }
            else{


                for (int n = 0; n < hueBulbs.size(); n++){
                    if(hueBulbs.at(n)->getController()->getIpAddress() == p.at(0)&& hueBulbs.at(n)->id()==deviceID ){

                        QVariantMap mapping =QJsonDocument::fromJson( QString::fromStdString(pDeviceData_Impl_Child->mapParameters_Find(DEVICEDATA_Mapping_CONST).c_str()).toLocal8Bit()).object().toVariantMap();
                        qDebug() << mapping;
                        hueBulbs.at(n)->setColorMap(mapping.value("color").toMap());
                        hueBulbs.at(n)->setLinuxmceId(linuxmceID);
                        hueBulbs.at(n)->setBrightness(0);
                        hueBulbs.at(n)->setPowerOn(false);
                        hueBulbs.at(n)->setDeviceTemplate(pDeviceData_Impl_Child->m_dwPK_DeviceTemplate );
                        qDebug() << "Linked existing light with linuxmce db. " << hueBulbs.at(n)->displayName();
                    }
                }

                for (int n = 0; n < hueMotionSensors.size(); n++){
                    if( pDeviceData_Impl_Child->m_dwPK_DeviceTemplate == DEVICETEMPLATE_SML001_CONST
                            && hueMotionSensors.at(n)->controller()->getIpAddress() == p.at(0)
                            && hueMotionSensors.at(n)->hueId()== deviceID){


                        qDebug() << "Linked existing sensor with linuxmce db. " << hueMotionSensors.at(n)->name();
                        hueMotionSensors.at(n)->setLinuxmceId(linuxmceID);
                        CMD_Set_Device_Data batt(m_dwPK_Device, 4, linuxmceID, StringUtils::itos(hueMotionSensors.at(n)->batteryLevel()), DEVICEDATA_Battery_state_CONST);
                        if(SendCommand(batt)){ qDebug()<<"Set battery level " ;}

                        for( int i=0; i < (int)pDeviceData_Impl_Child->m_vectDeviceData_Impl_Children.size(); i++ )
                        {
                            DeviceData_Impl *innerchild = pDeviceData_Impl_Child->m_vectDeviceData_Impl_Children[i];
                            qDebug() << innerchild->m_dwPK_DeviceTemplate;
                            switch (innerchild->m_dwPK_DeviceTemplate) {
                            case DEVICETEMPLATE_Ambient_Light_Sensor_CONST:
                                hueMotionSensors.at(n)->lightsensor()->setLinuxmceId(innerchild->m_dwPK_Device);
                                qDebug() << "linked existing light sensor " << innerchild->m_dwPK_Device ;
                                break;
                            case DEVICETEMPLATE_Hue_Temperature_Sensor_CONST:
                                hueMotionSensors.at(n)->tempsensor()->setLinuxmceId(innerchild->m_dwPK_Device);
                                qDebug() << "linked existing temp sensor " << innerchild->m_dwPK_Device;
                            default:
                                break;
                            }
                        }
                        connect(hueMotionSensors.at(n), SIGNAL(notifyEvent(DCE::Message*)), this, SLOT(handleMotionSensorEvent(DCE::Message*)));
                        hueMotionSensors.at(n)->initValues();
                    } else {

                    }
                }
            }

        }

        pCommand->m_pParent = this;
        pCommand->CreateChildren();
        m_mapCommandImpl_Children[pDeviceData_Impl_Child->m_dwPK_Device] = pCommand;
    }

    qDebug() << "Finished with Children Devices";

    //    if(m_pData->!=hueBulbs.size()){
    //        qDebug() << hueBulbs.size();
    //        qDebug() << m_pData->m_vectDeviceData_Impl_Children.size();
    //      //  qDebug() << hueBulbs.count()- m_pData->m_vectDeviceData_Impl_Children.size() << " device(s) not added to linuxmce.";
    //        this->CMD_Report_Child_Devices();
    //    }
    // pthread_yield();
}


//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void HueController::SomeFunction()
{
    // If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
    // to do all your registration, such as creating message interceptors

    // If you use an IDE with auto-complete, after you type DCE:: it should give you a list of all
    // commands and requests, including the parameters.  See "AllCommandsRequests.h"

    // Examples:

    // Send a specific the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.
    DCE::CMD_Simulate_Mouse_Click CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);
    SendCommand(CMD_Simulate_Mouse_Click);

    // Send the message to orbiters 32898 and 27283 (ie a device list, hence the _DL)
    // And we want a response, which will be "OK" if the command was successfull
    string sResponse;
    DCE::CMD_Simulate_Mouse_Click_DL CMD_Simulate_Mouse_Click_DL(m_dwPK_Device,"32898,27283",55,77)
    SendCommand(CMD_Simulate_Mouse_Click_DL,&sResponse);

    // Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
    // Note the _Cat for category
    DCE::CMD_Simulate_Mouse_Click_Cat CMD_Simulate_Mouse_Click_Cat(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77)
    SendCommand(CMD_Simulate_Mouse_Click_Cat);

    // Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
    // Note the _DT.
    DCE::CMD_Simulate_Mouse_Click_DT CMD_Simulate_Mouse_Click_DT(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);
    SendCommand(CMD_Simulate_Mouse_Click_DT);

    // This command has a normal string parameter, but also an int as an out parameter
    int iValue;
    DCE::CMD_Get_Signal_Strength CMD_Get_Signal_Strength(m_dwDeviceID, DestDevice, sMac_address,&iValue);
    // This send command will wait for the destination device to respond since there is
    // an out parameter
    SendCommand(CMD_Get_Signal_Strength);

    // This time we don't care about the out parameter.  We just want the command to
    // get through, and don't want to wait for the round trip.  The out parameter, iValue,
    // will not get set
    SendCommandNoResponse(CMD_Get_Signal_Strength);

    // This command has an out parameter of a data block.  Any parameter that is a binary
    // data block is a pair of int and char *
    // We'll also want to see the response, so we'll pass a string for that too

    int iFileSize;
    char *pFileContents
    string sResponse;
    DCE::CMD_Request_File CMD_Request_File(m_dwDeviceID, DestDevice, "filename",&pFileContents,&iFileSize,&sResponse);
    SendCommand(CMD_Request_File);

    // If the device processed the command (in this case retrieved the file),
    // sResponse will be "OK", and iFileSize will be the size of the file
    // and pFileContents will be the file contents.  **NOTE**  We are responsible
    // free deleting pFileContents.


    // To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

    // Get our IP address from our data
    string sIP = DATA_Get_IP_Address();

    // Set our data "Filename" to "myfile"
    DATA_Set_Filename("myfile");

    // Fire the "Finished with file" event, which takes no parameters
    EVENT_Finished_with_file();
    // Fire the "Touch or click" which takes an X and Y parameter
    EVENT_Touch_or_click(10,150);
}
*/
//<-dceag-sample-e->

/*

    COMMANDS TO IMPLEMENT

*/

//<-dceag-c184-b->

/** @brief COMMAND: #184 - Set Level */
/** Sets the brightness of the light.  The value is 0 for off, up to 100 for full brightness. Several devices such as Drapes Switch can accept relative levels (+1 to step forward/-1 to step backword). */
/** @param #76 Level */
/** The level to set, as a value between 0 (off) and 100 (full).  It can be preceeded with a - or + indicating a relative value.  +20 means up 20%. */

void HueController::CMD_Set_Level(string sLevel,string &sCMD_Result,Message *pMessage)
//<-dceag-c184-e->
{
    cout << "Need to implement command #184 - Set Level" << endl;
    cout << "Parm #76 - Level=" << sLevel << endl;
}

//<-dceag-c192-b->

/** @brief COMMAND: #192 - On */
/** Turn on the device */
/** @param #97 PK_Pipe */
/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */
/** @param #98 PK_Device_Pipes */
/** Normally when a device is turned on the corresponding "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic */

void HueController::CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage)
//<-dceag-c192-e->
{
    cout << "Need to implement command #192 - On" << endl;
    cout << "Parm #97 - PK_Pipe=" << iPK_Pipe << endl;
    cout << "Parm #98 - PK_Device_Pipes=" << sPK_Device_Pipes << endl;
    // pthread_yield(); //this->m_pThread);
    emit testSignal();

    //  pthread_yield();
}

//<-dceag-c193-b->

/** @brief COMMAND: #193 - Off */
/** Turn off the device */
/** @param #97 PK_Pipe */
/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

void HueController::CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage)
//<-dceag-c193-e->
{
    cout << "Need to implement command #193 - Off" << endl;
    cout << "Parm #97 - PK_Pipe=" << iPK_Pipe << endl;
}

//<-dceag-c756-b->

/** @brief COMMAND: #756 - Report Child Devices */
/** Report all the child sensors this has by firing an event 'Reporting Child Devices' */

void HueController::CMD_Report_Child_Devices(string &sCMD_Result,Message *pMessage)
//<-dceag-c756-e->
{
    // pthread_yield();
    // cout << "Need to implement command #756 - Report Child Devices" << endl;
    qDebug() << Q_FUNC_INFO;
    QList<int>added;

    for(int n=0; n < hueBulbs.size(); n++ ){
        qDebug()<< "Current Light: \t" << hueBulbs.at(n)->displayName();
        qDebug()<< "Current Light LMCE id \t" << hueBulbs.at(n)->linuxmceId();
        if(hueBulbs.at(n)->linuxmceId()==0){
            qDebug() <<  hueBulbs.at(n)->displayName() << " has no linuxmce id. it should be added.";
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "%s has no LinuxMCE Device number, it should be added.", hueBulbs.at(n)->displayName().toStdString().c_str());

            for( int i=0; i < (int)m_pData->m_vectDeviceData_Impl_Children.size(); i++ )
            {
                DeviceData_Impl *existingBulb = m_pData->m_vectDeviceData_Impl_Children.at(i);



                if (existingBulb->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST)=="" && added.indexOf(existingBulb->m_dwPK_Device)==-1 ){

                    qDebug () << "Setting ID for existing light with unknown configuration with " << hueBulbs.at(n)->displayName();
                    CMD_Set_Device_Data setUnit(this->m_dwPK_Device, 4, existingBulb->m_dwPK_Device,StringUtils::itos(hueBulbs.at(n)->id()),DEVICEDATA_UnitNo_CONST);
                    string pResonseA = "";
                    if(SendCommand(setUnit, &pResonseA)){
                        qDebug() << "Set internal id";
                    }
                    QString chanaddress = hueBulbs.at(n)->getController()->getIpAddress()+":"+QString::number(hueBulbs.at(n)->id());
                    qDebug()<< chanaddress;
                    CMD_Set_Device_Data setController(this->m_dwPK_Device,
                                                      4,
                                                      existingBulb->m_dwPK_Device,
                                                      chanaddress.toStdString(),
                                                      DEVICEDATA_PortChannel_Number_CONST
                                                      );
                    if(SendCommand(setController)){
                        qDebug() << "Set port / channel";
                    }

                    CMD_Set_Device_Data setName(m_dwPK_Device, 4, existingBulb->m_dwPK_Device, hueBulbs.at(n)->displayName().toStdString(), DEVICEDATA_Name_CONST);
                    if(SendCommand(setName)){
                        qDebug()<<"Set Device Name";
                    }
                    added.append(existingBulb->m_dwPK_Device);
                }
                else{
                    qDebug() << "LiuxmceID for " << hueBulbs.at(n)->displayName() << "::" << hueBulbs.at(n)->linuxmceId();
                }
                // qDebug() << "There are no more bulbs in the linuxmce system, but the controller has more." <<hueBulbs.at(n)->lightName << "is being added them now. ";
            }

            if(hueBulbs.at(n)->linuxmceId()==0){
                qDebug() << "Not found in system, need to add "<< hueBulbs.at(n)->displayName() << hueBulbs.at(n)->lightModel();
                qDebug() << "Creating...";
                int newDevice;
                string cResponse="";
                QString modelNo = hueBulbs.at(n)->lightModel();
                int dtNumber = -1;

                if( m_extendedColorLightModels.contains(modelNo)  ){
                    if( modelNo == "LST002" ||  modelNo == "LST002"  ){
                        dtNumber = DEVICETEMPLATE_Hue_Light_Strips_CONST;
                    } else {
                        dtNumber = DEVICETEMPLATE_Hue_Extended_Color_Bulb_CONST;
                    }
                } else if(m_colorLightModels.contains(modelNo)){
                    if( modelNo == "LST002" ||  modelNo == "LST002"  ){
                        dtNumber = DEVICETEMPLATE_Hue_Light_Strips_CONST;
                    } else {
                        dtNumber = DEVICETEMPLATE_Hue_Color_Light_Bulb_CONST;
                    }
                } else if(m_colorTemperatureModels.contains(modelNo)){
                    dtNumber = DEVICETEMPLATE_Hue_White_Ambiance_CONST;
                } else if(m_dimmableModels.contains(modelNo)){
                    dtNumber = DEVICETEMPLATE_Hue_Lux_Bulb_CONST;
                } else {
                    qDebug() << "Did not match model no: " << modelNo << " using generic on / off ";
                    dtNumber = DEVICETEMPLATE_Connected_A19_60w_Equivalent_CONST;
                }

                qDebug() << " creating device template " << dtNumber;

                DCE::CMD_Create_Device createChild(
                            this->m_dwPK_Device,
                            4,
                            dtNumber,           //COMMANDPARAMETER_PK_DeviceTemplate_CONST
                            "",                 //COMMANDPARAMETER_Mac_address_CONST
                            0,                  //COMMANDPARAMETER_PK_Room_CONST
                            "",                 //COMMANDPARAMETER_IP_Address_CONST
                            "",                 //COMMANDPARAMETER_Data_String_CONST
                            0,                  //COMMANDPARAMETER_PK_DHCPDevice_CONST
                            this->m_dwPK_Device,    //COMMANDPARAMETER_PK_Device_ControlledVia_CONST
                            hueBulbs.at(n)->displayName().toStdString(),    //COMMANDPARAMETER_Description_CONST
                            0,                  //COMMANDPARAMETER_PK_Orbiter_CONST
                            0,                  //COMMANDPARAMETER_PK_Device_Related_CONST
                            &newDevice);        //COMMANDPARAMETER_PK_Device_CONST

                if(SendCommand(createChild, &cResponse)){
                    qDebug() << cResponse.c_str();
                    qDebug() << "New Device id " << newDevice;


                    AbstractWirelessBulb *b = hueBulbs.at(n);

                    QString chanaddress = hueBulbs.at(n)->getController()->getIpAddress()+":"+QString::number(b->id());
                    qDebug()<< chanaddress;
                    CMD_Set_Device_Data setController(this->m_dwPK_Device, 4, newDevice,chanaddress.toStdString(),DEVICEDATA_PortChannel_Number_CONST);
                    if(SendCommand(setController)){
                        qDebug() << "Set port / channel";
                    }

                    CMD_Set_Device_Data setName(m_dwPK_Device, 4, newDevice, b->displayName().toStdString(), DEVICEDATA_Name_CONST);
                    if(SendCommand(setName)){
                        qDebug()<<"Set Device Name";
                    }

                    CMD_Set_Device_Data serialNumber(m_dwPK_Device, 4, newDevice, b->uniqueId().toStdString(), DEVICEDATA_Serial_Number_CONST);
                    if(SendCommand(serialNumber)){
                        qDebug()<<"Set uniqueId " << b->uniqueId();
                    }

                    CMD_Set_Device_Data lighttype(m_dwPK_Device, 4, newDevice, b->manufacturerName().toStdString(), DEVICEDATA_Type_CONST);
                    if(SendCommand(lighttype)){
                        qDebug()<<"set manufacturer " << b->manufacturerName();
                    }

                    CMD_Set_Device_Data model(m_dwPK_Device, 4, newDevice, b->lightModel().toStdString(), DEVICEDATA_Model_CONST);
                    if(SendCommand(model)){
                        qDebug()<<"Set light model " << b->lightModel();
                    }

                    added.append(newDevice);
                }
            }
        }
    }

    //sensors

    for(int n=0; n < hueMotionSensors.size(); n++ ){
        qDebug()<< "Current Sensor: \t" << hueMotionSensors.at(n)->name();
        qDebug()<< "Current Sensor LMCE id \t" << hueMotionSensors.at(n)->linuxmceId();
        if(hueMotionSensors.at(n)->linuxmceId()==0){
            qDebug() <<  hueMotionSensors.at(n)->name() << " has no linuxmce id. it should be added.";
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "%s has no LinuxMCE Device number, it should be added.", hueMotionSensors.at(n)->name().toStdString().c_str());


            if(hueMotionSensors.at(n)->linuxmceId()==0){
                qDebug() << " sensor Not found in system, need to add "<< hueMotionSensors.at(n)->name();
                qDebug() << "Creating...";
                int newDevice;
                string cResponse="";
                QString modelNo = hueMotionSensors.at(n)->modelId();
                int dtNumber = -1;
                if( modelNo == "SML001" ){
                    dtNumber = DEVICETEMPLATE_SML001_CONST ;
                } else if (modelNo == "PHDL00" ){

                }

                if(dtNumber == -1 ) return;
                qDebug() << " Creating motion sensor ";
                DCE::CMD_Create_Device createChild(
                            this->m_dwPK_Device,
                            4,
                            dtNumber,           //COMMANDPARAMETER_PK_DeviceTemplate_CONST
                            "",                 //COMMANDPARAMETER_Mac_address_CONST
                            0,                  //COMMANDPARAMETER_PK_Room_CONST
                            "",                 //COMMANDPARAMETER_IP_Address_CONST
                            "",                 //COMMANDPARAMETER_Data_String_CONST
                            0,                  //COMMANDPARAMETER_PK_DHCPDevice_CONST
                            this->m_dwPK_Device,    //COMMANDPARAMETER_PK_Device_ControlledVia_CONST
                            hueMotionSensors.at(n)->name().toStdString(),    //COMMANDPARAMETER_Description_CONST
                            0,                  //COMMANDPARAMETER_PK_Orbiter_CONST
                            0,                  //COMMANDPARAMETER_PK_Device_Related_CONST
                            &newDevice);        //COMMANDPARAMETER_PK_Device_CONST

                if(SendCommand(createChild, &cResponse)){
                    qDebug() << cResponse.c_str();
                    qDebug() << "New Sensor Device id " << newDevice;



                    if(dtNumber == DEVICETEMPLATE_SML001_CONST){
                        HueMotionSensor * s = hueMotionSensors.at(n);
                        int tempDevice;
                        int brightnessDevice;

                        DCE::CMD_Create_Device createTemp(
                                    this->m_dwPK_Device,
                                    4,
                                    DEVICETEMPLATE_Hue_Temperature_Sensor_CONST,           //COMMANDPARAMETER_PK_DeviceTemplate_CONST
                                    "",                 //COMMANDPARAMETER_Mac_address_CONST
                                    0,                  //COMMANDPARAMETER_PK_Room_CONST
                                    "",                 //COMMANDPARAMETER_IP_Address_CONST
                                    "",                 //COMMANDPARAMETER_Data_String_CONST
                                    0,                  //COMMANDPARAMETER_PK_DHCPDevice_CONST
                                    newDevice,    //COMMANDPARAMETER_PK_Device_ControlledVia_CONST
                                    hueMotionSensors.at(n)->name().toStdString(),    //COMMANDPARAMETER_Description_CONST
                                    0,                  //COMMANDPARAMETER_PK_Orbiter_CONST
                                    0,                  //COMMANDPARAMETER_PK_Device_Related_CONST
                                    &tempDevice);       //COMMANDPARAMETER_PK_Device_CONST


                        string cmdResponse = "";
                        if(SendCommand(createTemp, &cmdResponse)){

                            if(cmdResponse=="OK"){
                                QString chanaddress = QString(hueMotionSensors.at(n)->controller()->getIpAddress()+":"+QString::number(s->tempsensor()->id()));
                                qDebug() << "New sub deivice temp Sensor Device id " << tempDevice;
                                CMD_Set_Device_Data tportChannel( this->m_dwPK_Device, 4, tempDevice, chanaddress.toStdString(), DEVICEDATA_PortChannel_Number_CONST);
                                if(SendCommand(tportChannel, &cmdResponse)){   qDebug() << " Set temp sensor port / channel  " << s->hueId();  }

                                DCE::Message *tstatus = new Message(
                                            tempDevice,
                                            DEVICEID_EVENTMANAGER,
                                            DCE::PRIORITY_NORMAL,
                                            DCE::MESSAGETYPE_EVENT,
                                            EVENT_State_Changed_CONST,
                                            1 /* number of parameter's pairs (id, value) */,
                                            EVENTPARAMETER_State_CONST,
                                            QString(s->tempsensor()->temp()).toStdString().c_str()
                                            );
                                m_pEvent->SendMessage(tstatus);
                            }
                        }
                        int ambientDevice;
                        DCE::CMD_Create_Device ambient(
                                    this->m_dwPK_Device,
                                    4,
                                    DEVICETEMPLATE_Ambient_Light_Sensor_CONST,           //COMMANDPARAMETER_PK_DeviceTemplate_CONST
                                    "",                 //COMMANDPARAMETER_Mac_address_CONST
                                    0,                  //COMMANDPARAMETER_PK_Room_CONST
                                    "",                 //COMMANDPARAMETER_IP_Address_CONST
                                    "",                 //COMMANDPARAMETER_Data_String_CONST
                                    0,                  //COMMANDPARAMETER_PK_DHCPDevice_CONST
                                    newDevice,    //COMMANDPARAMETER_PK_Device_ControlledVia_CONST
                                    hueMotionSensors.at(n)->name().toStdString(),    //COMMANDPARAMETER_Description_CONST
                                    0,                  //COMMANDPARAMETER_PK_Orbiter_CONST
                                    0,                  //COMMANDPARAMETER_PK_Device_Related_CONST
                                    &ambientDevice);       //COMMANDPARAMETER_PK_Device_CONST


                        cmdResponse = "";
                        if(SendCommand(ambient, &cmdResponse)){

                            if(cmdResponse=="OK"){
                                QString chanaddress = QString(hueMotionSensors.at(n)->controller()->getIpAddress()+":"+QString::number(s->lightsensor()->id()));
                                qDebug() << "New sub deivice abient light sensor Sensor Device id " << ambientDevice;
                                CMD_Set_Device_Data tportChannel( this->m_dwPK_Device, 4, ambientDevice, chanaddress.toStdString(), DEVICEDATA_PortChannel_Number_CONST);
                                if(SendCommand(tportChannel, &cmdResponse)){   qDebug() << " DEVICEDATA_PortChannel_Number_CONST " << chanaddress;  }
                                cmdResponse = "";

                                DCE::Message *status = new Message(
                                            ambientDevice,
                                            DEVICEID_EVENTMANAGER,
                                            DCE::PRIORITY_NORMAL,
                                            DCE::MESSAGETYPE_EVENT,
                                            EVENT_State_Changed_CONST,
                                            1 /* number of parameter's pairs (id, value) */,
                                            EVENTPARAMETER_State_CONST,
                                            QString(s->lightsensor()->lightLevel()).toStdString().c_str()
                                            );
                                m_pEvent->SendMessage(status);
                            }
                        }




                        QString chanaddress = QString(hueMotionSensors.at(n)->controller()->getIpAddress()+":"+QString::number(s->hueId()));
                        cmdResponse = "";
                        CMD_Set_Device_Data portchannel( this->m_dwPK_Device, 4, newDevice, chanaddress.toStdString(), DEVICEDATA_PortChannel_Number_CONST);
                        if(SendCommand(portchannel, &cmdResponse)){   qDebug() << " Set controller and hue id " << s->hueId();  }
                        cmdResponse = "";
                        CMD_Set_Device_Data setUnit(this->m_dwPK_Device, 4,newDevice,StringUtils::itos(s->hueId()),DEVICEDATA_UnitNo_CONST);
                        if(SendCommand(setUnit, &cmdResponse)){ qDebug() << "Set hue id";  }

                        cmdResponse = "";
                        CMD_Set_Device_Data setName(m_dwPK_Device, 4, newDevice,s->name().toStdString(), DEVICEDATA_Name_CONST);
                        if(SendCommand(setName, &cmdResponse)){ qDebug()<<"Set Device Name";  }

                        cmdResponse = "";
                        CMD_Set_Device_Data setSerialNumber(m_dwPK_Device, 4, newDevice, s->uniqueId().toStdString(), DEVICEDATA_Serial_Number_CONST);
                        if(SendCommand(setSerialNumber)){ qDebug()<<"Set uniqueId " << s->uniqueId();  }

                        cmdResponse = "";
                        CMD_Set_Device_Data batt(m_dwPK_Device, 4, newDevice, StringUtils::itos(s->batteryLevel()), DEVICEDATA_Battery_state_CONST);
                        if(SendCommand(batt)){ qDebug()<<"Set battery level " << s->batteryLevel();  }
                    }

                    added.append(newDevice);

                }
            }
        }
    }






    mb_isNew=false;
    //  pthread_yield();
    sCMD_Result= "Done";
    this->EVENT_Reporting_Child_Devices("OK", "New Devices");
}

//<-dceag-c757-b->

/** @brief COMMAND: #757 - Download Configuration */
/** Download new configuration data for this device */
/** @param #9 Text */
/** Any information the device may want to do the download */

void HueController::CMD_Download_Configuration(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c757-e->
{

    cout << "Need to implement command #757 - Download Configuration" << endl;
    cout << "Parm #9 - Text=" << sText << endl;
    // pthread_yield();
    emit initiateConfigDownload(QUrl("http://"+targetIpAddress+"/api/"+authUser));/*"+/config/"*/
}



void HueController::downloadConfigResponse(QNetworkReply*){
    qDebug() << " Got DataStore Response " ;
    QNetworkReply* r = qobject_cast<QNetworkReply*>(sender());
    QVariant p = QJsonDocument::fromBinaryData(r->readAll()).toVariant();
    qDebug() << p;

}

void HueController::dummySlot(){
    qDebug()<< "dummy executed";
}

void HueController::processDataStore(const QByteArray data)
{


}




//<-dceag-c760-b->

/** @brief COMMAND: #760 - Send Command To Child */
/** After reporting new child devices, there may be children we want to test, but we haven't done a quick reload router and can't send them messages directly.  This way we can send 'live' messages to children */
/** @param #10 ID */
/** The internal ID used for this device--not the Pluto device ID. */
/** @param #154 PK_Command */
/** The command to send */
/** @param #202 Parameters */
/** Parameters for the command in the format:
PK_CommandParameter|Value|... */

void HueController::CMD_Send_Command_To_Child(string sID,int iPK_Command,string sParameters,string &sCMD_Result,Message *pMessage)
//<-dceag-c760-e->
{
    cout << "Need to implement command #760 - Send Command To Child" << endl;
    cout << "Parm #10 - ID=" << sID << endl;
    cout << "Parm #154 - PK_Command=" << iPK_Command << endl;
    cout << "Parm #202 - Parameters=" << sParameters << endl;
}

//<-dceag-c776-b->

/** @brief COMMAND: #776 - Reset */
/** Reset device. */
/** @param #51 Arguments */
/** Argument string
NOEMON or CANBUS */

void HueController::CMD_Reset(string sArguments,string &sCMD_Result,Message *pMessage)
//<-dceag-c776-e->
{
    cout << "Need to implement command #776 - Reset" << endl;
    cout << "Parm #51 - Arguments=" << sArguments << endl;
}

//<-dceag-c788-b->

/** @brief COMMAND: #788 - StatusReport */
/** Test comand. Asq a report */
/** @param #51 Arguments */
/** Argument string */

void HueController::CMD_StatusReport(string sArguments,string &sCMD_Result,Message *pMessage)
//<-dceag-c788-e->
{
    cout << "Need to implement command #788 - StatusReport" << endl;
    cout << "Parm #51 - Arguments=" << sArguments << endl;
    m_updateStatus=true;
    sCMD_Result="OK";
}

//<-dceag-c980-b->

/** @brief COMMAND: #980 - Set Color RGB */
/** set the rgb color of a device */
/** @param #279 Red Level */
/** The red level for the device */
/** @param #280 Green Level */
/** The green level for the device */
/** @param #281 Blue Level */
/** The blue level for the device */

void HueController::CMD_Set_Color_RGB(int iRed_Level,int iGreen_Level,int iBlue_Level,string &sCMD_Result,Message *pMessage)
//<-dceag-c980-e->
{

    cout << "Need to implement command #980 - Set Color RGB" << endl;
    cout << "Parm #279 - Red_Level=" << iRed_Level << endl;
    cout << "Parm #280 - Green_Level=" << iGreen_Level << endl;
    cout << "Parm #281 - Blue_Level=" << iBlue_Level << endl;
}

bool HueController::downloadControllerConfig(QUrl deviceIp)
{
    int index=0;
    qDebug() << Q_FUNC_INFO <<"Connecting to "<< deviceIp;
    qDebug() << "Controller #" << (index+1);

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Connecting to %s , controller # %d ",deviceIp.toString().toStdString().c_str(),index);

    QUrl initUrl = "http://"+targetIpAddress+"/api/"+authUser;
    QNetworkAccessManager *initManager = new QNetworkAccessManager;
    QNetworkRequest init(initUrl);
    QNetworkReply * rt = initManager->get(QNetworkRequest(init));
    qDebug() << init.url();
    QEventLoop respWait;
    QObject::connect(initManager, SIGNAL(finished(QNetworkReply*)), &respWait, SLOT(quit()));
    respWait.exec();

    qDebug() << Q_FUNC_INFO << "Got Response";

    int deviceIndex = index;
    QByteArray rep = rt->readAll();


    QJsonObject obj =QJsonDocument::fromJson(rep).object();
    QVariantMap p = obj.toVariantMap();

    if(p.isEmpty())
    {
        qDebug() << " config download error! \n" << p;
        LoggerWrapper::GetInstance()->Write(LV_WARNING, "Could not connect to %s ! Response was invalid. Please check the device ip and that it is setup.", deviceIp.toString().toStdString().c_str());
        return false;
    }

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Config Response from %s , controller # %d is valid.",deviceIp.toString().toStdString().c_str(),index);

    QVariantMap lightNest = p["lights"].toMap();
    QVariantMap sensorNest = p["sensors"].toMap();

    QVariantMap configuration=p["config"].toMap();
    qDebug() << "Light Count"<< lightNest.count();
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Total lights for controller %d ==>  %d ",index, lightNest.count());
    if(deviceIndex > -1){
        hueControllers.at(deviceIndex)->m_macAddress= configuration["mac"].toString();
        hueControllers.at(deviceIndex)->m_ipAddress= configuration["ipaddress"].toString();
    }
    else{
        qDebug() << "Fail Notify::" << configuration["ipaddress"].toString()+"::"+hueControllers.at(index)->m_ipAddress;
        qDebug() << deviceIndex;
        return false;
    }

    QMap<QString, QVariant>::const_iterator i;
    for(i=lightNest.begin(); i!= lightNest.constEnd(); i++ )
    {
        AbstractWirelessBulb *b = new AbstractWirelessBulb(hueControllers.at(index));
        connect(b, SIGNAL(dceMessage(int)), this, SLOT(handleLightEvent(int)), Qt::QueuedConnection);
        connect(b, SIGNAL(dataEvent(DCE::PreformedCommand)), this, SLOT(handleLightMessage(DCE::PreformedCommand)));
        // << " :: " << i.value();
        QVariantMap light = i.value().toMap();
        QVariantMap state = light["state"].toMap();
        b->setPowerOn(state["on"].toBool());
        b->setId(i.key().toInt());

        if(light["manufacturername"].isValid()){ b->setManufacturerName(light["manufacturername"].toString()); } //hue software 1.4
        if(light["uniqueid"].isValid()) { b->setUniqueId(light["uniqueid"].toString());  }                       //hue software 1.7
        if(light["type"].isValid()) {b->setLightType(light["type"].toString());}

        b->setDisplayName(light["name"].toString());
        b->setLightModel(light["modelid"].toString());
        b->setLinuxmceId(0);
        b->setController( hueControllers.at(deviceIndex));

        //  qDebug() << i.key() << "::" << light["name"].toString() ;

        for( int l=0; l < (int)m_pData->m_vectDeviceData_Impl_Children.size(); l++ ){
            DeviceData_Impl *existingBulb = m_pData->m_vectDeviceData_Impl_Children.at(l);
            //qDebug() << "Existing light UnitNo :: " << existingBulb->mapParameters_Find(DEVICEDATA_UnitNo_CONST).c_str();

             QStringList p = QString::fromStdString(existingBulb->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST)).split(":");

            if(p.last().toInt() == b->id()){
                qDebug() << "Matched existing light" << b->displayName();
                b->setLinuxmceId(existingBulb->m_dwPK_Device);
                b->setRoom(existingBulb->m_dwPK_Room);
            } else {
                //  qDebug() << "no match  existing light" << existingBulb->mapParameters_Find(DEVICEDATA_UnitNo_CONST).c_str() << "::" << b->id();
            }

        }
        hueBulbs.append(b);
    }

    for(i=sensorNest.begin(); i!= sensorNest.constEnd(); i++ )
    {

        QVariantMap sensor = i.value().toMap();
        QString sensorType =  sensor["type"].toString();
        QString matchId= sensor["uniqueid"].toString().mid(0,20);

        if(sensor["modelid"].toString()== "PHDL00"){
            //daylight sensor
        } else if ( sensor["modelid"]=="SML001" ) {
            qDebug() << Q_FUNC_INFO << sensor["name"];
            HueMotionSensor * s = motionSensorHash.value(matchId);

            if(s==0  ){
                s = new HueMotionSensor( hueControllers.at(index));


                s->setHueId(i.key().toInt());
                motionSensorHash.insert(matchId, s);

                for( int l=0; l < (int)m_pData->m_vectDeviceData_Impl_Children.size(); l++ ){
                    DeviceData_Impl *existingSensors= m_pData->m_vectDeviceData_Impl_Children.at(l);

                    if(existingSensors->m_dwPK_DeviceTemplate == DEVICETEMPLATE_SML001_CONST){

                         QStringList p = QString::fromStdString(existingSensors->mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST)).split(":");
                         int unit = p.last().toInt();
                         qDebug() << "Existing Top level Sensor UnitNo :: " << unit << " for " << s->hueId();

                        if(unit == s->hueId()) {
                            qDebug() << "Matched existing sensor" << s->name();
                            s->setLinuxmceId(existingSensors->m_dwPK_Device);

                            DeviceData_Base *tsensor = existingSensors->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_Hue_Temperature_Sensor_CONST );
                            if(tsensor){
                                s->tempsensor()->setLinuxmceId(tsensor->m_dwPK_Device);
                                qDebug() << "Matched existing  temp sensor" << s->name();
                            }

                            DeviceData_Base *lsensor = existingSensors->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_Ambient_Light_Sensor_CONST );
                            if(lsensor){
                                s->lightsensor()->setLinuxmceId(lsensor->m_dwPK_Device);
                                qDebug() << "Matched existing light sensor" << s->name();
                            }

                        }
                    }

                }
                hueMotionSensors.append(s);
            }

            if(sensorType == "ZLLTemperature"){
                s->setTempSensor(i.key().toInt(), sensor);
            } else if(sensorType == "ZLLPresence"){
                s->setHueId(i.key().toInt());
                s->setName(sensor["name"].toString());
                s->setModelId(sensor["modelid"].toString());
                s->setSensitivity(sensor.value("sensitivity").toInt());
                s->setEnabled(sensor.value("config").toMap().value("on").toBool());
                s->setManufacturer(sensor.value("manufacturername").toString());
                s->setPresenceDetected(sensor.value("state").toMap().value("presence").toBool());
                s->setBatteryLevel(sensor.value("config").toMap().value("battery").toInt());
                s->setUniqueId(sensor["uniqueid"].toString());
                s->setLinuxmceId(0);
            } else if (sensorType == "ZLLLightLevel"){
                s->setLightSensor(i.key().toInt(), sensor);
            }

        }
    }

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Controller Lights:: %d ",hueBulbs.count());
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Controller Sensors:: %d ",hueMotionSensors.count());

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Lights setup in LinuxMCE :: %d ",m_pData->m_vectDeviceData_Impl_Children.size());


    //  pthread_yield();
    return true;
}

void HueController::updateDevice(AbstractWirelessBulb *b, int d)
{
    for( int l=0; l < (int)m_pData->m_vectDeviceData_Impl_Children.size(); l++ ){
        DeviceData_Impl *existingBulb = m_pData->m_vectDeviceData_Impl_Children.at(l);
        if(existingBulb->m_dwPK_Device==b->linuxmceId()){


            //            CMD_Set_Device_Data setUnit(this->m_dwPK_Device, 4,d,StringUtils::itos(b->id()),DEVICEDATA_UnitNo_CONST);
            //            string pResonseA = "";
            //            if(SendCommand(setUnit, &pResonseA)){
            //                qDebug() << "Set internal id";
            //            }
            //            QString chanaddress = b->getController()->getIpAddress()+":"+QString::number(b->id());
            //            qDebug()<< chanaddress;
            //            CMD_Set_Device_Data setController(this->m_dwPK_Device, 4, d,chanaddress.toStdString(),DEVICEDATA_PortChannel_Number_CONST);
            //            if(SendCommand(setController)){
            //                qDebug() << "Set port / channel";
            //            }

            CMD_Set_Device_Data setName(m_dwPK_Device, 4, d, b->displayName().toStdString(), DEVICEDATA_Name_CONST);
            if(SendCommand(setName)){
                qDebug()<<"Set Device Name";
            }

            CMD_Set_Device_Data setId(m_dwPK_Device, 4, d, b->uniqueId().toStdString(), DEVICEDATA_Serial_Number_CONST);
            if(SendCommand(setId)){
                qDebug()<<"Set uniqueId " << b->uniqueId();
            }
        }
    }
}

void HueController::checkLightInformation()
{
    // qDebug() << Q_FUNC_INFO <<  QDateTime::currentDateTime() << "enter " ;
    QString urlBulder=QString("http://%1/api/%2/lights/").arg(hueControllers.first()->getIpAddress()).arg(authUser);
    QUrl initUrl(urlBulder);

    QNetworkRequest init(initUrl);
    lightRequestManager->get(QNetworkRequest(init));
    // qDebug() << Q_FUNC_INFO <<  QDateTime::currentDateTime() << " exit" ;
}

void HueController::handleCheckLightInformation(QNetworkReply *reply)
{

    QByteArray rep = reply->readAll();
    QVariantMap p = QJsonDocument::fromJson(rep).object().toVariantMap();
    if(p.isEmpty()){
        qDebug() << " Empty Reply !" ;
        return;
    }
    foreach(AbstractWirelessBulb*b, hueBulbs){

        b->proccessStateInformation(p.value(QString::number(b->id())).toMap());


        // qDebug() << Q_FUNC_INFO << " light " << b->displayName();
        if(m_updateStatus){

            QString chanaddress = b->getController()->getIpAddress()+":"+QString::number(b->id());
            // qDebug()<< chanaddress;
            CMD_Set_Device_Data setController(this->m_dwPK_Device, 4, b->linuxmceId(),chanaddress.toStdString(),DEVICEDATA_PortChannel_Number_CONST);
            if(SendCommand(setController)){
                //  qDebug() << "Set port / channel";
            }

            /* needs fix to check name diff */

            CMD_Set_Device_Data setName(m_dwPK_Device, 4, b->linuxmceId(), b->displayName().toStdString(), DEVICEDATA_Name_CONST);
            if(SendCommand(setName)){
                //   qDebug()<<"Set Device Name";
            }

            CMD_Set_Device_Data setDesc(m_dwPK_Device, 4, b->linuxmceId(), b->displayName().toStdString(), DEVICEDATA_Description_CONST);
            if(SendCommand(setDesc)){
                // qDebug() << " Updated display name " << b->displayName();
            }


            CMD_Set_Device_Data setId(m_dwPK_Device, 4, b->linuxmceId(), b->uniqueId().toStdString(), DEVICEDATA_Serial_Number_CONST);
            if(SendCommand(setId)){
                //  qDebug()<<"Set uniqueId " << b->uniqueId();
            }

            CMD_Set_Device_Data setType(m_dwPK_Device, 4, b->linuxmceId(), b->getLightType().toStdString(), DEVICEDATA_Type_CONST );
            if(SendCommand(setType)){
                //  qDebug() <<"Set type";
            }
            m_updateStatus = false;
        }

    }

    reply->deleteLater();
}

void HueController::checkSensorInformation()
{

    QString urlBulder=QString("http://%1/api/%2/sensors/").arg(hueControllers.first()->getIpAddress()).arg(authUser);
    QUrl initUrl(urlBulder);

    QNetworkRequest init(initUrl);
    sensorRequestManager->get(QNetworkRequest(init));

}

void HueController::handleCheckSensorInformation(QNetworkReply *reply)
{


    QMap<QString, QVariant>::const_iterator i;
    QVariantMap data = QJsonDocument::fromJson(reply->readAll()).object().toVariantMap();

    for(i=data.begin(); i!= data.constEnd(); i++ )
    {
        QVariantMap currentSensor = i.value().toMap();

        if(currentSensor["modelid"].toString() =="SML001") {

            QString matchId= currentSensor["uniqueid"].toString().mid(0,20);
            HueMotionSensor * s = motionSensorHash.value(matchId);

            if(s!= 0 ){

                QString cmp = currentSensor["type"].toString();

                if(cmp=="ZLLPresence"){
                    s->setPresenceData(currentSensor);

                } else if (cmp == "ZLLLightLevel") {
                    s->setLightLevelData(currentSensor);
                } else if(cmp == "ZLLTemperature") {
                    s->setTempData(currentSensor);
                }
            }

            m_updateStatus = false;


        }
    }

    reply->deleteLater();
}

void HueController::handleLightEvent(int whichEvent)
{
    AbstractWirelessBulb *b = (AbstractWirelessBulb*)QObject::sender();
    if(!b){
        return;
    }

    if(whichEvent==EVENT_Device_OnOff_CONST ){
        if(b->linuxmceId()==0)return;
        qDebug() << Q_FUNC_INFO;

        DCE::Message *m = new DCE::Message (
                    b->linuxmceId(),
                    DEVICEID_EVENTMANAGER,
                    DCE::PRIORITY_NORMAL,
                    DCE::MESSAGETYPE_EVENT,
                    EVENT_Device_OnOff_CONST,
                    1, /* number of parameter's pairs (id, value) */
                    EVENTPARAMETER_OnOff_CONST,
                    b->powerOn() ?"1" : "0"
                                  );

        this->m_pEvent->SendMessage(m);
    } else if (whichEvent==EVENT_Brightness_Changed_CONST){
        QString val = QString::number(b->CurrentLevel()) ;
        DCE::Message *a = new Message(
                    b->linuxmceId(),
                    DEVICEID_EVENTMANAGER,
                    DCE::PRIORITY_NORMAL,
                    DCE::MESSAGETYPE_EVENT,
                    EVENT_Brightness_Changed_CONST,
                    1 /* number of parameter's pairs (id, value) */,
                    EVENTPARAMETER_Value_CONST,
                    val.toStdString().c_str()
                    );

        this->m_pEvent->SendMessage(a);
    } else if (whichEvent==EVENT_State_Changed_CONST){
        DCE::Message *st = new Message(
                    b->linuxmceId(),
                    DEVICEID_EVENTMANAGER,
                    DCE::PRIORITY_NORMAL,
                    DCE::MESSAGETYPE_EVENT,
                    EVENT_State_Changed_CONST,
                    1 /* number of parameter's pairs (id, value) */,
                    EVENTPARAMETER_State_CONST,
                    StringUtils::itos(  ceil(b->CurrentLevel()) ).c_str()
                    );

        this->m_pEvent->SendMessage(st);
    }
}

void HueController::handleMotionSensorEvent(Message *m)
{    
    this->m_pEvent->SendMessage(m);
}

void HueController::handleLightMessage(DCE::PreformedCommand m)
{
    string resp;
    if(SendCommand(m, &resp)){
        qDebug() << Q_FUNC_INFO << resp.c_str();
    }

}


void HueController::getScreenSaverColor()
{
    //    char *pData="";
    //    int pData_size;

}

void HueController::initBridgeConnection(){
    qDebug()<<"Init() Connection...";
    QUrl initUrl = "http://"+targetIpAddress+"/api/"+authUser;
    QNetworkAccessManager *initManager = new QNetworkAccessManager;
    QNetworkRequest init(initUrl);
    QNetworkReply * rt = initManager->get(QNetworkRequest(init));
    qDebug() << init.url();
    qDebug()<<"Request Sent.";

    QEventLoop respWait;
    QObject::connect(initManager, SIGNAL(finished(QNetworkReply*)), &respWait, SLOT(quit()));
    cout << "Waiting for Response" << endl;
    respWait.exec();
    qDebug()<< "Got Response";
    qDebug() << rt->readAll();
}

void HueController::initResponse(){

    QNetworkReply* r = qobject_cast<QNetworkReply*>(sender());
    qDebug() << r->size();
    processDataStore(r->readAll());
    r->deleteLater();
    return;
}

bool HueController::addMessageToQueue(QUrl msg, QVariant params)
{
    HueCommand *t = new HueCommand(msg, params);
    bool wasEmpty = cmdQueue.isEmpty();

    cmdQueue.append(t);
    // LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending Message:: %s ",message.toString(), params.toString());

    if(wasEmpty){
        emit startQueue();
    }

    return true;
}


void HueController::sendCommandMessage()
{

    if(cmdQueue.count()==0){
        emit lastCommand();
        return;
    }

    QNetworkRequest pr(cmdQueue.at(0)->target);
    // QJson::Serializer serializer;

    // QByteArray serialized = serializer.serialize(cmdQueue.at(0)->parameters);
    QJsonDocument doc = QJsonDocument::fromVariant(cmdQueue.at(0)->parameters);
    QByteArray outJson = doc.toJson();

    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending Message:: %s ",cmdQueue.at(0)->target.toString().toStdString().c_str(), cmdQueue.at(0)->parameters.toString().toStdString().c_str());
    QEventLoop respWait;
    QObject::connect(commandManager, SIGNAL(finished(QNetworkReply*)), &respWait, SLOT(quit()));
    QNetworkReply * ptx =  commandManager->put(pr, outJson);

    respWait.exec();
    handleCommandResponse(ptx);
}

void HueController::handleCommandResponse(QNetworkReply *r)
{
    if(cmdQueue.length()!=0){
        cmdQueue.removeFirst();
    }

    r->deleteLater();
}

void HueController::OnReload(){
    Disconnect();
    exit(0);
}

void HueController::setDeviceStatus(int device, QString status)
{

}


