/*
     Copyright (C) 2013 LinuxMCE

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#include "Hue_Controller.h"
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
#include <qjson/parser.h>
#include <qjson/serializer.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qeventloop.h>
#include <QVariantMap>

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Hue_Controller::Hue_Controller(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter, QObject*parent)
    : DCE::Hue_Controller_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
    //<-dceag-const-e->
{
    
    qDebug() << "Created Controller.";
    mb_isNew = true;
    communicator = new QNetworkAccessManager(this);
    QObject::connect(this, SIGNAL(initiateConfigDownload()), this, SLOT(getHueDataStore()));
    QObject::connect(this, SIGNAL(initiateConfigDownload()), this, SLOT(dummySlot()));
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
Hue_Controller::Hue_Controller(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
    : DCE::Hue_Controller_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
    //<-dceag-const2-e->
{
}

//<-dceag-dest-b->
Hue_Controller::~Hue_Controller()
        //<-dceag-dest-e->
{

}


//<-dceag-getconfig-b->
bool Hue_Controller::GetConfig()
{
    if( !Hue_Controller_Command::GetConfig() )
        return false;
    //<-dceag-getconfig-e->

    // Put your code here to initialize the data in this class
    // The configuration parameters DATA_ are now populated
    QString m_id= QString::fromStdString(this->DATA_Get_Device());
    targetIpAddress= QString::fromStdString(this->DATA_Get_Server_IP());
    // authUser = QString::fromStdString(this->DATA_Get_Username());
    authUser="newdeveloper";
    if(m_id.isEmpty()){
        qDebug() << "Hue ID is not set, Adding 1st missing hue controller." << m_id;
        qDebug() << "Looking up ::"<< targetIpAddress;
        qDebug() << "Defult username"<<authUser;
        initBridgeConnection();

    }
    return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Hue_Controller::Register()
        //<-dceag-reg-e->
{
    return Connect(PK_DeviceTemplate_get());
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
Hue_Controller_Command *Create_Hue_Controller(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
    return new Hue_Controller(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
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
void Hue_Controller::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
        //<-dceag-cmdch-e->
{
    sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Hue_Controller::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
        //<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN COMMAND";
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void Hue_Controller::SomeFunction()
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

void Hue_Controller::CMD_Set_Level(string sLevel,string &sCMD_Result,Message *pMessage)
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

void Hue_Controller::CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage)
        //<-dceag-c192-e->
{
    cout << "Need to implement command #192 - On" << endl;
    cout << "Parm #97 - PK_Pipe=" << iPK_Pipe << endl;
    cout << "Parm #98 - PK_Device_Pipes=" << sPK_Device_Pipes << endl;
}

//<-dceag-c193-b->

/** @brief COMMAND: #193 - Off */
/** Turn off the device */
/** @param #97 PK_Pipe */
/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

void Hue_Controller::CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage)
        //<-dceag-c193-e->
{
    cout << "Need to implement command #193 - Off" << endl;
    cout << "Parm #97 - PK_Pipe=" << iPK_Pipe << endl;
}

//<-dceag-c756-b->

/** @brief COMMAND: #756 - Report Child Devices */
/** Report all the child sensors this has by firing an event 'Reporting Child Devices' */

void Hue_Controller::CMD_Report_Child_Devices(string &sCMD_Result,Message *pMessage)
        //<-dceag-c756-e->
{
    cout << "Need to implement command #756 - Report Child Devices" << endl;
}

//<-dceag-c757-b->

/** @brief COMMAND: #757 - Download Configuration */
/** Download new configuration data for this device */
/** @param #9 Text */
/** Any information the device may want to do the download */

void Hue_Controller::CMD_Download_Configuration(string sText,string &sCMD_Result,Message *pMessage)
        //<-dceag-c757-e->
{
    cout << "Need to implement command #757 - Download Configuration" << endl;
    cout << "Parm #9 - Text=" << sText << endl;
   emit initiateConfigDownload();

}

void Hue_Controller::getHueDataStore(){
    QNetworkAccessManager* http = new QNetworkAccessManager(this);
    QNetworkRequest tx;
    tx.setUrl(QUrl("http://"+targetIpAddress+"/api/"+authUser));
    QNetworkReply *tr = http->get(tx);
    QObject::connect(tr,SIGNAL(finished()),this, SLOT(downloadConfigResponse()));

    qDebug() << "Sent Data Store request";
    QEventLoop respWait;
    QObject::connect(http, SIGNAL(finished(QNetworkReply*)), &respWait, SLOT(quit()));
    respWait.exec();
}

void Hue_Controller::downloadConfigResponse(){
    qDebug() << " Got DataStore Response " ;
    QNetworkReply* r = qobject_cast<QNetworkReply*>(sender());
    QJson::Parser parser;
    bool ok;
    QVariant p = parser.parse(r->readAll(), &ok);
    qDebug() << p;

}

void Hue_Controller::dummySlot(){
    qDebug()<< "dummy executed";
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

void Hue_Controller::CMD_Send_Command_To_Child(string sID,int iPK_Command,string sParameters,string &sCMD_Result,Message *pMessage)
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

void Hue_Controller::CMD_Reset(string sArguments,string &sCMD_Result,Message *pMessage)
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

void Hue_Controller::CMD_StatusReport(string sArguments,string &sCMD_Result,Message *pMessage)
        //<-dceag-c788-e->
{
    cout << "Need to implement command #788 - StatusReport" << endl;
    cout << "Parm #51 - Arguments=" << sArguments << endl;
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

void Hue_Controller::CMD_Set_Color_RGB(int iRed_Level,int iGreen_Level,int iBlue_Level,string &sCMD_Result,Message *pMessage)
        //<-dceag-c980-e->
{

    cout << "Need to implement command #980 - Set Color RGB" << endl;
    cout << "Parm #279 - Red_Level=" << iRed_Level << endl;
    cout << "Parm #280 - Green_Level=" << iGreen_Level << endl;
    cout << "Parm #281 - Blue_Level=" << iBlue_Level << endl;
}

bool Hue_Controller::findControllers(){

}

void Hue_Controller::initBridgeConnection(){
    qDebug()<<"Init() Connection...";
    QUrl initUrl = "http://"+targetIpAddress+"/api/"+authUser+"/"+"config";

    QNetworkRequest init(initUrl);
    QNetworkReply * rt = communicator->get(QNetworkRequest(init));
    //this->connect(rt, SIGNAL(aboutToClose()), this, SLOT(processResponse()));
    QObject::connect(rt, SIGNAL(finished()), this, SLOT(initResponse()));
    qDebug() << init.url();
    qDebug()<<"Request Sent.";

    QEventLoop respWait;
    QObject::connect(communicator, SIGNAL(finished(QNetworkReply*)), &respWait, SLOT(quit()));
    cout << "Waiting for Response" << endl;
    respWait.exec();
    

}

void Hue_Controller::initResponse(){
    qDebug() << " Got Reponse " ;
    QNetworkReply* r = qobject_cast<QNetworkReply*>(sender());
    QJson::Parser parser;
    bool ok;

    QVariant p = parser.parse(r->readAll(), &ok);

   // qDebug() << p.typeName();
    QString tipe = p.typeName();

    if(tipe=="QVariantList"){
        qDebug()<<"Error";

    }else if(tipe=="QVariantMap"){
       qDebug() << "Success";      \
       QVariantMap successType= p.toMap();
       if(successType.contains("config")){
           qDebug() << "config call";
       }
       //qDebug() << configItems["lights"];
    }
    //qDebug() << p;
r->deleteLater();

}





