/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
//<-dceag-d-b->
#ifndef Wii_Remote_Controller_h
#define Wii_Remote_Controller_h

//	DCE Implemenation for #1954 Wii Remote Controller

#include "Gen_Devices/Wii_Remote_ControllerBase.h"
//<-dceag-d-e->

#include "../LIRC_DCE/IRReceiverBase.h"
#include "IRBase/IRBase.h"
#include "AlarmManager.h"
#include <math.h>
#include <xwiimote.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

//<-dceag-decl-b->
namespace DCE
{
  /**
   * A single Wii Remote, holding state.  
   *
   */
  class Wiimote
  {
  private:
    string m_sCode;
    bool m_bLatched;
    bool m_bChanged;
    int ir_x, ir_y;
    struct timeval m_tvDownButton;

    double time_diff(struct timeval x , struct timeval y)
    {
      double x_ms , y_ms , diff;
      
      x_ms = (double)x.tv_sec*1000000 + (double)x.tv_usec;
      y_ms = (double)y.tv_sec*1000000 + (double)y.tv_usec;
      
      diff = (double)y_ms - (double)x_ms;
      
      return diff;
    }

  public:
    string m_sPath;
    bool m_bActive;
    struct xwii_iface *iface;
    struct xwii_event event;
    struct xwii_event event_prev;
    int uinput_fd;

    // IR fuzzy logic vectors
    struct timeval ir_last_valid_event;
    int ir_vec_x;
    int ir_vec_y;
    int ir_ref_x;
    int ir_ref_y;
    int ir_avg_x;
    int ir_avg_y;
    int ir_avg_count;
    int ir_avg_radius;
    int ir_avg_max_samples;
    int ir_avg_min_samples;
    int ir_avg_weight;
    int ir_keymap_expiry_secs;

    // old IR coordinates, used to prevent discontinuity (currently, check for 0)
    int old_ir_x;
    int old_ir_y;

    Wiimote() 
      {
	iface=NULL;
	// event=NULL;
	// event_prev=NULL;
	m_bActive=false;
	m_sPath="";
      }

    ~Wiimote() { }
    
    void setButton(string sCode)
    {
      m_bChanged=(sCode!=m_sCode);
      m_sCode=sCode;
      if (m_bChanged)
	gettimeofday(&m_tvDownButton,NULL);
    }

    bool isLatchedButton()
    {
      return m_bLatched;
    }

    bool isChangedButton()
    {
      return m_bChanged;
    }

    string getButton(bool alterChanged)
    {
      if (alterChanged)
	m_bChanged=false;
      return m_sCode;
    }

    bool timerButton(struct timeval tvNow)
    {
      return time_diff(m_tvDownButton,tvNow);
    }

    void retriggerButton()
    {
      gettimeofday(&m_tvDownButton,NULL);
      m_bLatched=true;
    }

  };

  class Wii_Remote_Controller : public Wii_Remote_Controller_Command, public AlarmEvent, IRReceiverBase, IRBase
	{
//<-dceag-decl-e->
		// Private member variables
	  int m_dwPK_Device_IRPlugin;
	  int m_iPK_Orbiter;
	  pluto_pthread_mutex_t m_WiimoteMutex;
	  pthread_t m_wiimoteCaptureThread;
	  class AlarmManager *m_pAlarmManager;
	  void AlarmCallback(int id, void *param);
	  string m_sAVWHost;
	  int m_iAVWPort;
	  pthread_t m_inputCaptureThread;
	  Wiimote *m_WiiMote1;
	  Wiimote *m_WiiMote2;
	  Wiimote *m_WiiMote3;
	  Wiimote *m_WiiMote4;

		// Private methods
	  string GetDevice(int num);
public:
		// Public member variables
	  map<string, pair<string, int> > m_mapCodesToButtons;
	  int m_DeviceID;

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Wii_Remote_Controller(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Wii_Remote_Controller();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->
		virtual void PrepareToDelete();
		virtual void CreateChildren();
		void FindWiimotes();
		virtual void SendIR(string Port, string IRCode, int iRepeat);
		int Wiimote_Capture(int deviceID);
		void ProcessWiiMote(Wiimote* pWiiMote);
		void ProcessWiiMote_Key(Wiimote* pWiiMote);
		void ProcessWiiMote_IR(Wiimote* pWiiMote);
		void HandleKeyEvent(string sCode);
		void SetupIRMouse(Wiimote *pWiiMote);
		pair<int, int> GetAveragedIRXY(Wiimote *pWiiMote);

//<-dceag-const2-b->!

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	bool DATA_Get_Ignore();
	bool DATA_Get_Only_One_Per_PC();
	bool DATA_Get_Autoassign_to_parents_room();
	bool DATA_Get_PNP_Create_Without_Prompting();
	bool DATA_Get_Immediate_Reload_Isnt_Necessar();

			*****EVENT***** accessors inherited from base class
	void EVENT_Device_Detected(string sMac_Address,string sText,string sIP_Address,int iPK_DeviceTemplate,string sVendorModelID,int iPK_CommMethod,int iPK_PnpProtocol,string sPNP_Serial_Number,string sDeviceData,string sCategory,string sSignature);
	void EVENT_Device_Removed(string sMac_Address,string sText,int iPK_Device,string sIP_Address,int iPK_DeviceTemplate,string sVendorModelID,int iPK_CommMethod,int iPK_PnpProtocol,string sPNP_Serial_Number,string sDeviceData,string sCategory);

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #191 - Send Code */
	/** The I/R code -- usually in Pronto format */
		/** @param #9 Text */
			/** The I/R code -- usually in Pronto format */

	virtual void CMD_Send_Code(string sText) { string sCMD_Result; CMD_Send_Code(sText.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Send_Code(string sText,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #245 - Learn IR */
	/** The next IR code received is to be learned in Pronto format and fire a Store IR Code command to the I/R Plugin when done */
		/** @param #2 PK_Device */
			/** You can specify the device to learn for here, or you can send the command to the device itself and leave this blank */
		/** @param #8 On/Off */
			/** Turn IR Learning mode on or off
0, 1 */
		/** @param #25 PK_Text */
			/** If specified, the text object  which should contain the result of the learn command */
		/** @param #154 PK_Command */
			/** Command ID for which the learning is done for */

	virtual void CMD_Learn_IR(int iPK_Device,string sOnOff,int iPK_Text,int iPK_Command) { string sCMD_Result; CMD_Learn_IR(iPK_Device,sOnOff.c_str(),iPK_Text,iPK_Command,sCMD_Result,NULL);};
	virtual void CMD_Learn_IR(int iPK_Device,string sOnOff,int iPK_Text,int iPK_Command,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #687 - Set Screen Type */
	/** Sent by Orbiter when the screen changes to tells the i/r receiver what type of screen is displayed so it can adjust mappings if necessary. */
		/** @param #48 Value */
			/** a character: M=Main Menu, m=other menu, R=Pluto Remote, r=Non-pluto remote, N=navigable OSD on media dev, f=full screen media app, F=File Listing, c=computing list, C=Computing full screen */

	virtual void CMD_Set_Screen_Type(int iValue) { string sCMD_Result; CMD_Set_Screen_Type(iValue,sCMD_Result,NULL);};
	virtual void CMD_Set_Screen_Type(int iValue,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
