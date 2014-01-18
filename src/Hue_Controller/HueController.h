/*
     Copyright (C) 2013 LinuxMCE 

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef HueController_h
#define HueController_h

//	DCE Implemenation for #2256 Hue Controller

#include "Gen_Devices/HueControllerBase.h"
//<-dceag-d-e->
#include <QObject>
#include <qjson/serializer.h>
#include <qjson/parser.h>
#include <QNetworkAccessManager>
#include <QVariant>
#include <QTimer>
#include <QUrl>
#include <huecontrollerhardware.h>
#include <huebulb.h>
//<-dceag-decl-b->
namespace DCE
{
    class HueController : public HueController_Command
    {
        Q_OBJECT
        //<-dceag-decl-e->
        // Private member variables
        bool mb_isNew;
        bool validated;
        bool linkButton;
        int triesLeft;
        QString targetIpAddress;
        QString authUser;

        QList<HueControllerHardware*> hueControllers;
        QList<HueBulb*> hueBulbs;

        QTimer rTime;


        // Private methods
    public:
        // Public member variables


        //<-dceag-const-b->
    public:
        // Constructors/Destructor
        HueController(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL, QObject*parent=0);
        virtual ~HueController();
        virtual bool GetConfig();
        virtual bool Register();
        virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
        virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
        virtual void CreateChildren();
       
       
        //<-dceag-const-e->
        void initBridgeConnection();
        bool findControllers();
        bool downloadControllerConfig(QUrl deviceIp, int index);
        void getScreenSaverColor();
        bool setupController(int controllerIndex);
	virtual void OnReload();




        //<-dceag-const2-b->
        // The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
        // You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
        //HueController(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
        //<-dceag-const2-e->

        //<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Device();
	string DATA_Get_Username();
	void DATA_Set_Username(string Value,bool bUpdateDatabase=false);
	string DATA_Get_Server_IP();

			*****EVENT***** accessors inherited from base class
	void EVENT_State_Changed(int iValue);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #184 - Set Level */
	/** Sets the brightness of the light.  The value is 0 for off, up to 100 for full brightness. Several devices such as Drapes Switch can accept relative levels (+1 to step forward/-1 to step backword). */
        /** @param #76 Level */
        /** The level to set, as a value between 0 (off) and 100 (full).  It can be preceeded with a - or + indicating a relative value.  +20 means up 20%. */

	virtual void CMD_Set_Level(string sLevel) { string sCMD_Result; CMD_Set_Level(sLevel.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Set_Level(string sLevel,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #192 - On */
	/** Turn on the device */
        /** @param #97 PK_Pipe */
        /** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */
        /** @param #98 PK_Device_Pipes */
        /** Normally when a device is turned on the corresponding "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic */

	virtual void CMD_On(int iPK_Pipe,string sPK_Device_Pipes) { string sCMD_Result; CMD_On(iPK_Pipe,sPK_Device_Pipes.c_str(),sCMD_Result,NULL);};
	virtual void CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #193 - Off */
	/** Turn off the device */
        /** @param #97 PK_Pipe */
        /** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

	virtual void CMD_Off(int iPK_Pipe) { string sCMD_Result; CMD_Off(iPK_Pipe,sCMD_Result,NULL);};
	virtual void CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #756 - Report Child Devices */
	/** Report all the child sensors this has by firing an event 'Reporting Child Devices' */

	virtual void CMD_Report_Child_Devices() { string sCMD_Result; CMD_Report_Child_Devices(sCMD_Result,NULL);};
	virtual void CMD_Report_Child_Devices(string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #757 - Download Configuration */
	/** Download new configuration data for this device */
        /** @param #9 Text */
        /** Any information the device may want to do the download */

	virtual void CMD_Download_Configuration(string sText) { string sCMD_Result; CMD_Download_Configuration(sText.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Download_Configuration(string sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #760 - Send Command To Child */
	/** After reporting new child devices, there may be children we want to test, but we haven't done a quick reload router and can't send them messages directly.  This way we can send 'live' messages to children */
        /** @param #10 ID */
        /** The internal ID used for this device--not the Pluto device ID. */
        /** @param #154 PK_Command */
        /** The command to send */
        /** @param #202 Parameters */
        /** Parameters for the command in the format:
PK_CommandParameter|Value|... */

	virtual void CMD_Send_Command_To_Child(string sID,int iPK_Command,string sParameters) { string sCMD_Result; CMD_Send_Command_To_Child(sID.c_str(),iPK_Command,sParameters.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Send_Command_To_Child(string sID,int iPK_Command,string sParameters,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #776 - Reset */
	/** Reset device. */
        /** @param #51 Arguments */
        /** Argument string
NOEMON or CANBUS */

	virtual void CMD_Reset(string sArguments) { string sCMD_Result; CMD_Reset(sArguments.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Reset(string sArguments,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #788 - StatusReport */
	/** Test comand. Asq a report */
        /** @param #51 Arguments */
        /** Argument string */

	virtual void CMD_StatusReport(string sArguments) { string sCMD_Result; CMD_StatusReport(sArguments.c_str(),sCMD_Result,NULL);};
	virtual void CMD_StatusReport(string sArguments,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #980 - Set Color RGB */
	/** set the rgb color of a device */
        /** @param #279 Red Level */
        /** The red level for the device */
        /** @param #280 Green Level */
        /** The green level for the device */
        /** @param #281 Blue Level */
        /** The blue level for the device */

	virtual void CMD_Set_Color_RGB(int iRed_Level,int iGreen_Level,int iBlue_Level) { string sCMD_Result; CMD_Set_Color_RGB(iRed_Level,iGreen_Level,iBlue_Level,sCMD_Result,NULL);};
	virtual void CMD_Set_Color_RGB(int iRed_Level,int iGreen_Level,int iBlue_Level,string &sCMD_Result,Message *pMessage);

        //<-dceag-h-e->
    signals:
        void responseRecieved();
        void initiateConfigDownload(QUrl target);
        void testSignal();
        void linkButtonChanged();


    public slots:
        void initResponse();      
        void downloadConfigResponse(QNetworkReply*);
        void dummySlot();
        void setLinkButton(bool link){linkButton = link; emit linkButtonChanged();}
        bool getLinkButton(){return linkButton;}
        bool checkLinkButton();

        void processDataStore(const QByteArray data);

        HueControllerHardware* getController(int index) {return hueControllers.at(index); }


    protected:
        bool sendPowerMessage(QUrl message, QVariant params);

    };

    //<-dceag-end-b->
}
#endif
//<-dceag-end-e->
