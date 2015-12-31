/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648


     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/

/** @file App_Server.h
The main portion of app_server.
*/
//<-dceag-d-b->
#ifndef App_Server_h
#define App_Server_h

//	DCE Implemenation for #26 App Server

#include "Gen_Devices/App_ServerBase.h"
//<-dceag-d-e->

#ifndef WIN32
#include "amix.h"
#endif

    /** @class App_Server
    The main class of App_Server.
    */

//<-dceag-decl-b->
namespace DCE
{

	class App_Server : public App_Server_Command
	{
//<-dceag-decl-e->
protected:
		virtual void EnsureLogoIsDisplayed();

    /** Constructor.
    @param DeviceID is the device ID.
    @param ServerAddress is
    @param bConnectEventHandler is
    @param bLocalMode is
    @param pRouter is the router.

    The primary constructor - when the class is created as a stand-alone device
    */

        /** Standard Destructor.
        */

        /** Fill in the config information.
        */

        /** Register the device.
        This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.
        Otherwise Connect() will be called from the main().
        */

        /** ReceivedCommandForChild.

    When you receive commands that are destined to one of your children,
    then if that child implements DCE then there will already be a separate class
    created for the child that will get the message.
        If the child does not, then you will
    get all commands for your children in ReceivedCommandForChild, where
    pDeviceData_Base is the child device.
        If you handle the message, you should change the sCMD_Result to OK.
        */

        /** ReceivedUnknownCommand.

    When you received a valid command, but it wasn't for one of your children,
    then ReceivedUnknownCommand gets called.
        If you handle the message, you should change the sCMD_Result to OK.
        */

//<-dceag-const-b->
public:
		// Constructors/Destructor
		App_Server(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~App_Server();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
        virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

        // Private member variables
        typedef map<string, map<int, pair<string, string> > > vector_map;

        /**
         PIDs associated with an AppName - used for application mass killing (not massacres :P)
        */
        vector_map m_mapAppPids;

        pluto_pthread_mutex_t m_AppMutex; // Other classes may need this

        pthread_mutexattr_t m_MutexAttr;

        /** Stored mute flag for the instance. */
        bool m_bLastMute;

        /** Stored volume for the instance. */
        int m_iLastVolume;

        // Private methods
        /** Private method to start the application (declared publix).
        */
        bool StartApp(string CmdExecutable, string CmdParams, string AppName, string sCommandsOnSuccess, string sCommandsOnFailure);

        /** Private method called when exiting (declared public ).
        */
        void ProcessExited(int pid, int status);

        /** Private method to connect (declared public tho).
        */
		virtual bool Connect(int iPK_DeviceTemplate );

        /** KillSpawnedDevices.

        This will only be called when we are dying so we won't care
        about what happens to our spawned children.

        We had a problem that KillSpawnedDevices called KillPids.sh,
        which when exited, called the sh signal handler and hung at wait.
        */
		virtual void KillSpawnedDevices(); // We need to change the behavior

        /** Queue messages to the router.
        */
		void SendMessageList(string messageList);

        /** Display a message on the orbiter on screen display.
        */
		void DisplayMessageOnOrbVFD(string sMessage);
public:
    // Public member variables

    /** Flag - True if this is running on a hard-drive boot, not as a diskless LinuxMCE media director.
    */
		bool m_bHardDrive;

        /** Device number of the media director we are running on. */
		int m_dwPK_Device_MD;
#ifndef WIN32

        /** The master audio mixer??? */
		MasterMix m_MasterMix;
#endif

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	bool DATA_Get_Discrete_Volume();
	int DATA_Get_Volume_Level();
	void DATA_Set_Volume_Level(int Value);
	bool DATA_Get_Only_One_Per_PC();
	bool DATA_Get_Autoassign_to_parents_room();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** CMD_Simulate_Keypress
      COMMAND: #28 - Simulate Keypress

    Send a keypress event to an application
    @param sPK_Button #26 What key to simulate being pressed.
        If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified.
    @param sName #50 The application to send the keypress to.
        If not specified, it goes to the DCE device.
    @param sCMD_Result is the address of the area used to store the result.
    @param pMessage is a pointer to the message
    */
	virtual void CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,Message *pMessage);

    /** This is a short version that calls the above function.
    */
    virtual void CMD_Simulate_Keypress(string sPK_Button,string sName) {
        string sCMD_Result;
        CMD_Simulate_Keypress(sPK_Button.c_str(),sName.c_str(),sCMD_Result,NULL);
    };


	/** COMMAND: #67 - Spawn Application.
    Spawn an application.
    This version is called by the short version internally.
    @param sFilename #13 The name of the executable file to spawn.
    @param sName #50 A name that we'll remember the application by for future kill commands.
    @param sArguments #51 Command arguments, tab delimited.
    @param sSendOnFailure #94 Send this messages if the process exited with failure error code.
    @param sSendOnSuccess #95 Send this messages if the process exited with success error code.
    @param bShow_logo #115 If this is set then we will first select the logo  before spawning the application.
    @param bRetransmit #120 If false, and if Exclusive is true and another instance is killed,
        the 'send messages on termination' will not be sent.
    @param bExclusive #126 If true, then kill other apps with this same name.
    @param bDetach #241 Detach application after spawning / Don't kill this app on reload.
    @param sCMD_Result is the address of the area used to store the result.
    @param pMessage is a pointer to the message
    */
    virtual void CMD_Spawn_Application(string sFilename,string sName,string sArguments,string sSendOnFailure,string sSendOnSuccess,
                                       bool bShow_logo,bool bRetransmit,bool bExclusive,bool bDetach,string &sCMD_Result,Message *pMessage);

    /** This is a short version that calls the above function.
    */
	virtual void CMD_Spawn_Application(string sFilename,string sName,string sArguments,string sSendOnFailure,string sSendOnSuccess,
                                       bool bShow_logo,bool bRetransmit,bool bExclusive,bool bDetach) {
                                           string sCMD_Result; CMD_Spawn_Application(sFilename.c_str(),sName.c_str(),sArguments.c_str(),sSendOnFailure.c_str(),
                                                   sSendOnSuccess.c_str(),bShow_logo,bRetransmit,bExclusive,bDetach,sCMD_Result,NULL);
                                       };


	/** COMMAND: #69 - Kill Application.

    Kill an application.
    This version is called by the short version internally.
    @param sName #50 Application name given at spawn time.
    @param bRetransmit #120 If false, the 'send messages' won't be processed when the app terminates.
    */
	virtual void CMD_Kill_Application(string sName,bool bRetransmit,string &sCMD_Result,Message *pMessage);

    /** This is a short version that calls the above function.
    */
    virtual void CMD_Kill_Application(string sName,bool bRetransmit) { string sCMD_Result; CMD_Kill_Application(sName.c_str(),bRetransmit,sCMD_Result,NULL);};


	/** COMMAND: #70 - Hide Application.

    Hide an application.
    This version is called by the short version internally.
    @param sName #50 Application name given at spawn time.
    @param sCMD_Result is the address of the area used to store the result.
    @param pMessage is a pointer to the message
    */
	virtual void CMD_Hide_Application(string sName,string &sCMD_Result,Message *pMessage);

    /** This is a short version that calls the above function.
    */
    virtual void CMD_Hide_Application(string sName) {
        string sCMD_Result;
        CMD_Hide_Application(sName.c_str(),sCMD_Result,NULL);
    };


	/** COMMAND: #89 - Vol Up.

    Increase the volume 1%.
    This version is called by the short version internally.
    @param iRepeat_Command #72 If specified, repeat the volume up this many times.
    @param sCMD_Result is the address of the area used to store the result.
    @param pMessage is a pointer to the message
    */
    virtual void CMD_Vol_Up(int iRepeat_Command,string &sCMD_Result,Message *pMessage);

    /** This is a short version that calls the above function.
    */
	virtual void CMD_Vol_Up(int iRepeat_Command) {
        string sCMD_Result;
        CMD_Vol_Up(iRepeat_Command,sCMD_Result,NULL);
    };


	/** COMMAND: #90 - Vol Down.

    Decrease the volume 1%.
    This version is called by the short version internally.
    @param iRepeat_Command #72 If specified, repeat the volume down this many times.
    @param sCMD_Result is the address of the area used to store the result.
    @param pMessage is a pointer to the message
    */
    virtual void CMD_Vol_Down(int iRepeat_Command,string &sCMD_Result,Message *pMessage);

    /** This is a short version that calls the above function.
    */
	virtual void CMD_Vol_Down(int iRepeat_Command) {
        string sCMD_Result;
        CMD_Vol_Down(iRepeat_Command,sCMD_Result,NULL);
    };


	/** COMMAND: #97 - Mute.

    Toggle mute/unmute.
    This version is called by the short version internally.
    @param sCMD_Result is the address of the area used to store the result.
    @param pMessage is a pointer to the message
    */
    virtual void CMD_Mute(string &sCMD_Result,Message *pMessage);

    /** This is a short version that calls the above function.
    */
	virtual void CMD_Mute() {
        string sCMD_Result;
        CMD_Mute(sCMD_Result,NULL);
    };


	/** COMMAND: #313 - Set Volume.

    Set the volume to a specific level between 0-100.
    This version is called by the short version internally.
    @param sLevel #76 is the level to set.
    @param sCMD_Result is the address of the area used to store the result.
    @param pMessage is a pointer to the message

    A Level value is between 0 and 100 where 0 is mute and 100 is full volume.
    Numbers preceded with a - or + are relative.  +15 means up 15, -10 means down 10.
    */
	virtual void CMD_Set_Volume(string sLevel,string &sCMD_Result,Message *pMessage);

    /** This is a short version that calls the above function.
    */
    virtual void CMD_Set_Volume(string sLevel) { string sCMD_Result; CMD_Set_Volume(sLevel.c_str(),sCMD_Result,NULL);};


	/** COMMAND: #323 - Halt Device

    Halt or reboot this device.
    This version is called by the short version internally.
    @param iPK_Device #2 The device to halt.
    @param sForce #21 is the force string.
    @param sCMD_Result is the address of the area used to store the result.
    @param pMessage is a pointer to the message
    If Force is not specified this will do a suspend if the device supports suspend/resume,
    otherwise it will do a halt.
    Force:  "H"=halt, "S"=suspend, "D"=Display off, "R"=reboot, "N"=net boot, "V"=hard drive boot.
    */
    virtual void CMD_Halt_Device(int iPK_Device,string sForce,string &sCMD_Result,Message *pMessage);

    /** This is a short version that calls the above function.
    */
	virtual void CMD_Halt_Device(int iPK_Device,string sForce) {
         string sCMD_Result;
         CMD_Halt_Device(iPK_Device,sForce.c_str(),sCMD_Result,NULL);
    };


    /** COMMAND: #696 - Application is Running

    Checks if the given application is running.
    This version is called by the short version internally.
    @param sName #50 Application name given at spawn time.
    @param bTrueFalse #119 Indicates if it's running.
    @param sCMD_Result is the address of the area used to store the result.
    @param pMessage is a pointer to the message
    */
    virtual void CMD_Application_is_Running(string sName,bool *bTrueFalse,string &sCMD_Result,Message *pMessage);

    /** This is a short version that calls the above function.
    */
    virtual void CMD_Application_is_Running(string sName,bool *bTrueFalse) {
        string sCMD_Result;
        CMD_Application_is_Running(sName.c_str(),bTrueFalse,sCMD_Result,NULL);
    };


    /** COMMAND: #812 - Application Exited.

    Used by the appserver to tell itself that it received a SIGCHLD, in order to avoid a deadlock.
    This version is called by the short version internally.
    @param iPID #227 Process ID to be passed to the ApplicationExited function.
    @param iExit_Code #228 Exit Code to be passed to the ApplicationExited function.
    @param sCMD_Result is the address of the area used to store the result.
    @param pMessage is a pointer to the message
    */
    virtual void CMD_Application_Exited(int iPID,int iExit_Code,string &sCMD_Result,Message *pMessage);

    /** This is a short version that calls the above function.
    */
	virtual void CMD_Application_Exited(int iPID,int iExit_Code) {
        string sCMD_Result;
        CMD_Application_Exited(iPID,iExit_Code,sCMD_Result,NULL);
    };


//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
