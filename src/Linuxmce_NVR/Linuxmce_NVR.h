/*
     Copyright (C) 2013 LinuxMCE

     www.linuxmce.org


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef Linuxmce_NVR_h
#define Linuxmce_NVR_h

//	DCE Implemenation for #2330 Linuxmce NVR

#include "Gen_Devices/Linuxmce_NVRBase.h"
//<-dceag-d-e->
#include "qobject.h"
#include "cameraClasses/abstractpicamera.h"
#include "qvector.h"
//<-dceag-decl-b->
namespace DCE
{
class Linuxmce_NVR : public QObject, public Linuxmce_NVR_Command
{
    Q_OBJECT
    //<-dceag-decl-e->
    // Private member variables

    // Private methods
public:
    // Public member variables

    //<-dceag-const-b->
public:
    // Constructors/Destructor
    Linuxmce_NVR(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
    virtual ~Linuxmce_NVR();
    virtual bool GetConfig();
    virtual bool Register();
    virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
    virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);

    //<-dceag-const-e->
    bool OnReplaceHandler(string r);
    void CreateChildren();
    void OnReload();
    //<-dceag-const2-b->
    // The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
    // You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
    Linuxmce_NVR(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
    //<-dceag-const2-e->

    //<-dceag-h-b->
    /*
                                AUTO-GENERATED SECTION
                                Do not change the declarations
        */

    /*
                        *****DATA***** accessors inherited from base class
        string DATA_Get_Configuration();
        void DATA_Set_Configuration(string Value,bool bUpdateDatabase=false);
        int DATA_Get_Listen_Port();
        void DATA_Set_Listen_Port(int Value,bool bUpdateDatabase=false);
        string DATA_Get_EmailSettings();
        void DATA_Set_EmailSettings(string Value,bool bUpdateDatabase=false);

                        *****EVENT***** accessors inherited from base class

                        *****COMMANDS***** we need to implement
        */


    /** @brief COMMAND: #277 - Get Capture Video Frame */
    /** Get a video frame from a specified camera number */
    /** @param #19 Data */
    /** The pointer to the video frame */
    /** @param #112 CameraID */
    /** The number (ID) of the camera where to capture from */

    virtual void CMD_Get_Capture_Video_Frame(int iCameraID,char **pData,int *iData_Size) { string sCMD_Result; CMD_Get_Capture_Video_Frame(iCameraID,pData,iData_Size,sCMD_Result,NULL);};
    virtual void CMD_Get_Capture_Video_Frame(int iCameraID,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage);


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

    //<-dceag-h-e->
signals:
    void addCamera(NvrCameraBase *c);
    void removeCamera();
    void commandForCamera();
    void newListenPort(int p);
    void requestCameraImage(int camId);
    void setDetectionState(bool enabled, int camId);

public slots:

   void handleMotionEvent(int device, bool motionDetected);
   void handleManagerMessage(QString msg);

private:
    bool createManager();

private:
    QList<QObject*> cameraDevices;
     NvrManager *mp_manager;
     QVector<NvrCameraBase*> cam_list;
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
