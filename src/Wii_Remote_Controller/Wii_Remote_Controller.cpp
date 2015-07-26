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
#include "Wii_Remote_Controller.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#define XWIIMOTE_IR_AVG_RADIUS 10
#define XWIIMOTE_IR_AVG_MAX_SAMPLES 8
#define XWIIMOTE_IR_AVG_MIN_SAMPLES 4
#define XWIIMOTE_IR_AVG_WEIGHT 3

#define XWIIMOTE_IR_KEYMAP_EXPIRY_SECS 1

#define XWIIMOTE_DISTSQ(ax, ay, bx, by) \
	((ax - bx) * (ax - bx) + (ay - by) * (ay - by))

#define FIND_WIIMOTES 0

void * StartInputThread(void * Arg)
{
  Wii_Remote_Controller *pWii_Remote_Controller = (Wii_Remote_Controller *) Arg;
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
  pWii_Remote_Controller->Wiimote_Capture(pWii_Remote_Controller->m_DeviceID);
  return NULL;
}

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Wii_Remote_Controller::Wii_Remote_Controller(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Wii_Remote_Controller_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	,IRReceiverBase(this)
	,m_WiimoteMutex("wii_remote_controller")
	,m_iAVWPort(0)
{
  m_WiimoteMutex.Init(NULL);
  m_pAlarmManager=NULL;
  m_inputCaptureThread=0;
  m_DeviceID=0;
  m_WiiMote1 = new Wiimote();
  m_WiiMote2 = new Wiimote();
  m_WiiMote3 = new Wiimote();
  m_WiiMote4 = new Wiimote();
}

//<-dceag-const2-b->!

void Wii_Remote_Controller::PrepareToDelete()
{
  Command_Impl::PrepareToDelete();

  if (m_WiiMote1->m_bActive)
    {
      ioctl(m_WiiMote1->uinput_fd,UI_DEV_DESTROY);
      xwii_iface_close(m_WiiMote1->iface,XWII_IFACE_ALL);
      xwii_iface_unref(m_WiiMote1->iface);
    }

  if (m_WiiMote2->m_bActive)
    {
      xwii_iface_close(m_WiiMote2->iface,XWII_IFACE_ALL);
      xwii_iface_unref(m_WiiMote2->iface);
    }

  if (m_WiiMote3->m_bActive)
    {
      xwii_iface_close(m_WiiMote3->iface,XWII_IFACE_ALL);
      xwii_iface_unref(m_WiiMote3->iface);
    }

  if (m_WiiMote4->m_bActive)
    {
      xwii_iface_close(m_WiiMote4->iface,XWII_IFACE_ALL);
      xwii_iface_unref(m_WiiMote4->iface);
    }

  delete m_pAlarmManager;
  delete m_WiiMote1;
  delete m_WiiMote2;
  delete m_WiiMote3;
  delete m_WiiMote4;
  m_pAlarmManager=NULL;
}

//<-dceag-dest-b->
Wii_Remote_Controller::~Wii_Remote_Controller()
//<-dceag-dest-e->
{
  
}

//<-dceag-getconfig-b->
bool Wii_Remote_Controller::GetConfig()
{
	if( !Wii_Remote_Controller_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	if ( m_dwPK_Device != DEVICEID_MESSAGESEND && !m_bLocalMode )
	  {
	    if ( !m_Virtual_Device_Translator.GetConfig(m_pData) )
	      return false;
	    
	    // Put your code here to initialize the data in this class
	    // The configuration parameters DATA_ are now populated
	    	    
	    IRBase::setCommandImpl(this);
	    IRBase::setAllDevices(&(GetData()->m_AllDevices));
	    IRReceiverBase::GetConfig(m_pData);
	  }

	m_pAlarmManager = new AlarmManager();
	m_pAlarmManager->Start(2);

	if ( !m_bLocalMode )
	  {
	    DeviceData_Base *pDevice = m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Infrared_Plugins_CONST);
	    
	    if ( pDevice )
	      m_dwPK_Device_IRPlugin = pDevice->m_dwPK_Device;
	    else
	      m_dwPK_Device_IRPlugin = 0;
	    
	    string sResult;
	    DCE::CMD_Get_Sibling_Remotes CMD_Get_Sibling_Remotes(m_dwPK_Device,m_dwPK_Device_IRPlugin, DEVICECATEGORY_Wii_Remote_Controls_CONST, &sResult);
	    SendCommand(CMD_Get_Sibling_Remotes);
	    
	    vector<string> vectRemotes;
	    StringUtils::Tokenize(sResult, "`",vectRemotes);
	    size_t i;
	    for (i=0;i<vectRemotes.size();i++)
	      {
		vector<string> vectRemoteConfigs;
		StringUtils::Tokenize(vectRemotes[i],"~",vectRemoteConfigs);
		if (vectRemoteConfigs.size() == 3)
		  {
		    vector<string> vectCodes;
		    int PK_DeviceRemote = atoi(vectRemoteConfigs[0].c_str());
		    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Adding remote ID %d, layout %s\r\n",PK_DeviceRemote,vectRemoteConfigs[1].c_str());
		    StringUtils::Tokenize(vectRemoteConfigs[2],"\r\n",vectCodes);
		    for (size_t s=0;s<vectCodes.size();++s)
		      {
			string::size_type pos=0;
			string sButton = StringUtils::Tokenize(vectCodes[s]," ",pos);
			while(pos<vectCodes[s].size())
			  {
			    string sCode = StringUtils::Tokenize(vectCodes[s]," ",pos);
			    m_mapCodesToButtons[sCode] = make_pair<string,int> (sButton,PK_DeviceRemote);
			    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Code: %s will fire button %s",sCode.c_str(),sButton.c_str());
			  }
		      }
		  }
	      }
	  }
	else
	  {
	    // Local mode, hard code values to lookup.
	    // Button 1 = OK
	    // Buttons 2-10 = Mode buttons to blindly set video modes.
	    m_mapCodesToButtons["USB-GAMEPAD-UP"] = make_pair<string,int>("up",0);
	    m_mapCodesToButtons["USB-GAMEPAD-DOWN"] = make_pair<string,int>("down",0);
	    m_mapCodesToButtons["USB-GAMEPAD-LEFT"] = make_pair<string,int>("left",0);
	    m_mapCodesToButtons["USB-GAMEPAD-RIGHT"] = make_pair<string,int>("right",0);
	    m_mapCodesToButtons["USB-GAMEPAD-B1"] = make_pair<string,int>("ok",0);
	    m_mapCodesToButtons["USB-GAMEPAD-B2"] = make_pair<string,int>("1",0);
	    m_mapCodesToButtons["USB-GAMEPAD-B3"] = make_pair<string,int>("q",0);
	    m_mapCodesToButtons["USB-GAMEPAD-B4"] = make_pair<string,int>("a",0);
	    m_mapCodesToButtons["USB-GAMEPAD-B5"] = make_pair<string,int>("2",0);
	    m_mapCodesToButtons["USB-GAMEPAD-B6"] = make_pair<string,int>("w",0);
	    m_mapCodesToButtons["USB-GAMEPAD-B7"] = make_pair<string,int>("3",0);
	    m_mapCodesToButtons["USB-GAMEPAD-B8"] = make_pair<string,int>("4",0);
	    m_mapCodesToButtons["USB-GAMEPAD-B9"] = make_pair<string,int>("-",0);
	    m_mapCodesToButtons["USB-GAMEPAD-B10"] = make_pair<string,int>("+",0);
	  }
	
	// Create the input thread.
	if (pthread_create(&m_inputCaptureThread, NULL, StartInputThread, (void *) this))
	  {
	    // failed, bail.
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Failed to create Input thread.");
	    m_bQuit_set(true);
	    return false;
	  }

	return true;

}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Wii_Remote_Controller::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Wii_Remote_Controller::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
  if (IRBase::ProcessMessage(pMessage))
    {
      printf("Message Processed by IRBase");
      sCMD_Result = "OK";
      return;
    }
  
  sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Wii_Remote_Controller::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

void Wii_Remote_Controller::ProcessWiiMote(Wiimote* pWiiMote)
{
  int ret;
  if (!pWiiMote)
    return;

  pWiiMote->event_prev = pWiiMote->event;
  ret = xwii_iface_poll(pWiiMote->iface,&pWiiMote->event);

  if (ret == -EAGAIN)
    {
      // Nothing here. Let's go back and get the next one, and/or sleep.
      return;
    }
  else if (ret)
    {
      LoggerWrapper::GetInstance()->Write(LV_WARNING,"Error reading Wiimote event, error #%d",ret);
      return;
    }

  switch (pWiiMote->event.type)
    {
    case XWII_EVENT_KEY:
      ProcessWiiMote_Key(pWiiMote);
      break;
    case XWII_EVENT_IR:
      ProcessWiiMote_IR(pWiiMote);
      break;
      // Accellerometer events are ignored.
    }

}

void Wii_Remote_Controller::ProcessWiiMote_Key(Wiimote* pWiiMote)
{
  unsigned int code = pWiiMote->event.v.key.code;
  bool pressed = pWiiMote->event.v.key.state;
  string sCode = "";
  struct timeval tvNow;

  gettimeofday(&tvNow,NULL);

  if (pressed)
    {
      switch(code)
	{
	case XWII_KEY_LEFT:
	  pWiiMote->setButton("LEFT");
	  break;
	case XWII_KEY_RIGHT:
	  pWiiMote->setButton("RIGHT");
	  break;
	case XWII_KEY_UP:
	  pWiiMote->setButton("UP");
	  break;
	case XWII_KEY_DOWN:
	  pWiiMote->setButton("DOWN");
	  break;
	case XWII_KEY_A:
	  pWiiMote->setButton("A");
	  break;
	case XWII_KEY_B:
	  pWiiMote->setButton("B");
	  break;
	case XWII_KEY_HOME:
	  pWiiMote->setButton("HOME");
	  break;
	case XWII_KEY_MINUS:
	  pWiiMote->setButton("MINUS");
	  break;
	case XWII_KEY_PLUS:
	  pWiiMote->setButton("PLUS");
	  break;
	case XWII_KEY_ONE:
	  pWiiMote->setButton("ONE");
	  break;
	case XWII_KEY_TWO:
	  pWiiMote->setButton("TWO");
	  break;
	}
    }
  else
    {
      if (pWiiMote->getButton(false) == "B")
	{
	  struct input_event ev[2];
	  memset(&ev,0,sizeof(ev));
	      ev[0].type = EV_KEY;
	      ev[0].code = BTN_LEFT;
	      ev[0].value = 0;
	      ev[1].type = EV_SYN;
	      ev[1].code = SYN_REPORT;
	      ev[1].value = 0;
	      write(pWiiMote->uinput_fd,ev,sizeof(ev));
	}
      pWiiMote->setButton("");
    }
    
  if (pWiiMote->getButton(false)!="")
    {
      if (pWiiMote->isChangedButton() && pWiiMote->getButton(false)!="B")
	{
	  if (pWiiMote->timerButton(tvNow) < 1000000)
	    {
	      HandleKeyEvent(pWiiMote->getButton(true));
	      pWiiMote->retriggerButton();
	    }
	}
      if (pWiiMote->isLatchedButton() && pWiiMote->getButton(false)!="B")
	{
	  if (pWiiMote->timerButton(tvNow) > 250000)
	    {
	      HandleKeyEvent(pWiiMote->getButton(true));
	      pWiiMote->retriggerButton();
	    }
	}
      if (pWiiMote->isChangedButton() && pWiiMote->getButton(false)=="B")
	{
	  // Handle B is left mouse button.
	  if (m_cCurrentScreen!='G')
	    {
	      struct input_event ev[2];
	      memset(&ev,0,sizeof(ev));
	      ev[0].type = EV_KEY;
	      ev[0].code = BTN_LEFT;
	      ev[0].value = 1;
	      ev[1].type = EV_SYN;
	      ev[1].code = SYN_REPORT;
	      ev[1].value = 0;
	      write(pWiiMote->uinput_fd,ev,sizeof(ev));
	    }
	}

    }

}

void Wii_Remote_Controller::HandleKeyEvent(string sCode)
{
  string sPrefix="WII-REMOTE-";
  string sFinalCode=sPrefix+sCode;
  bool bOverride=false;

  map <string,pair<string,int> >::iterator it=m_mapCodesToButtons.find(sFinalCode);

  if (m_cCurrentScreen == 'G')
    {
      if (it->second.first == "start")
	{
	  // We're good.
	}
      else if (it->second.first == "Home")
	{
	  // Also good.
	}
      else
	{
	  return;
	}
    }
  
  if (m_cCurrentScreen == 'B') // File browser
    {
      if (it->second.first == "up")
	{
	  bOverride=true;
	  ReceivedCode(0,"chup");
	}
      else if (it->second.first == "down")
	{
	  bOverride=true;
	  ReceivedCode(0,"chdown");
	}
    }


  if (it==m_mapCodesToButtons.end())
    {
      LoggerWrapper::GetInstance()->Write(LV_WARNING,"Can't find a mapping for button %s",sFinalCode.c_str());
    }
  else
    {
      if (!bOverride)
	{
	  ReceivedCode(it->second.second,it->second.first.c_str());
	  if (m_dwPK_Device==DEVICEID_MESSAGESEND)
	    {
	      ForceKeystroke(it->second.first, m_sAVWHost, m_iAVWPort);
	    }
	}
    }

  return;

}

pair<int, int> Wii_Remote_Controller::GetAveragedIRXY(Wiimote* pWiiMote)
{
  struct xwii_event_abs *a, *b, *c, d;
  int i, dists[6];
  
  /* Grab first two valid points */
  a = b = NULL;
  for (i = 0; i < 4; ++i) {
    c = &pWiiMote->event.v.abs[i];
    if (xwii_event_ir_is_valid(c) && (c->x || c->y)) {
      if (!a) {
	a = c;
      } else if (!b) {
	b = c;
      } else {
	/* This may be a noisy point. Keep the two points that are
	 * closest to the reference points. */
	d.x = pWiiMote->ir_ref_x + pWiiMote->ir_vec_x;
	d.y = pWiiMote->ir_ref_y + pWiiMote->ir_vec_y;
	dists[0] = XWIIMOTE_DISTSQ(c->x, c->y, pWiiMote->ir_ref_x, pWiiMote->ir_ref_y);
	dists[1] = XWIIMOTE_DISTSQ(c->x, c->y, d.x, d.y);
	dists[2] = XWIIMOTE_DISTSQ(a->x, a->y, pWiiMote->ir_ref_x, pWiiMote->ir_ref_y);
	dists[3] = XWIIMOTE_DISTSQ(a->x, a->y, d.x, d.y);
	dists[4] = XWIIMOTE_DISTSQ(b->x, b->y, pWiiMote->ir_ref_x, pWiiMote->ir_ref_y);
	dists[5] = XWIIMOTE_DISTSQ(b->x, b->y, d.x, d.y);
	if (dists[1] < dists[0]) dists[0] = dists[1];
	if (dists[3] < dists[2]) dists[2] = dists[3];
	if (dists[5] < dists[4]) dists[4] = dists[5];
	if (dists[0] < dists[2]) {
	  if (dists[4] < dists[2]) {
	    a = c;
	  } else {
	    b = c;
	  }
	} else if (dists[0] < dists[4]) {
	  b = c;
	}
      }
    }
  }
  if (!a)
    return make_pair(0,0);

  if (!b) {
    /* Generate the second point based on historical data */
    b = &d;
    b->x = a->x - pWiiMote->ir_vec_x;
    b->y = a->y - pWiiMote->ir_vec_y;
    if (XWIIMOTE_DISTSQ(a->x, a->y, pWiiMote->ir_ref_x, pWiiMote->ir_ref_y)
	< XWIIMOTE_DISTSQ(b->x, b->y, pWiiMote->ir_ref_x, pWiiMote->ir_ref_y)) {
      b->x = a->x + pWiiMote->ir_vec_x;
      b->y = a->y + pWiiMote->ir_vec_y;
      pWiiMote->ir_ref_x = a->x;
      pWiiMote->ir_ref_y = a->y;
    } else {
      pWiiMote->ir_ref_x = b->x;
      pWiiMote->ir_ref_y = b->y;
    }
  } else {
    /* Record some data in case one of the points disappears */
    pWiiMote->ir_vec_x = b->x - a->x;
    pWiiMote->ir_vec_y = b->y - a->y;
    pWiiMote->ir_ref_x = a->x;
    pWiiMote->ir_ref_y = a->y;
  }
  
  /* Final point is the average of both points */
  a->x = (a->x + b->x) / 2;
  a->y = (a->y + b->y) / 2;
  
  /* Start averaging if the location is consistant */
  pWiiMote->ir_avg_x = (pWiiMote->ir_avg_x * pWiiMote->ir_avg_count + a->x) / (pWiiMote->ir_avg_count+1);
  pWiiMote->ir_avg_y = (pWiiMote->ir_avg_y * pWiiMote->ir_avg_count + a->y) / (pWiiMote->ir_avg_count+1);
  if (++pWiiMote->ir_avg_count > pWiiMote->ir_avg_max_samples)
    pWiiMote->ir_avg_count = pWiiMote->ir_avg_max_samples;
  if (XWIIMOTE_DISTSQ(a->x, a->y, pWiiMote->ir_avg_x, pWiiMote->ir_avg_y)
      < pWiiMote->ir_avg_radius * pWiiMote->ir_avg_radius) {
    if (pWiiMote->ir_avg_count >= pWiiMote->ir_avg_min_samples) {
      a->x = (a->x + pWiiMote->ir_avg_x * pWiiMote->ir_avg_weight) / (pWiiMote->ir_avg_weight+1);
      a->y = (a->y + pWiiMote->ir_avg_y * pWiiMote->ir_avg_weight) / (pWiiMote->ir_avg_weight+1);
    }
  } else {
    pWiiMote->ir_avg_count = 0;
  }
  
  pWiiMote->ir_last_valid_event = pWiiMote->event.time;

  return make_pair(floor(a->x),floor(a->y));
}

void Wii_Remote_Controller::ProcessWiiMote_IR(Wiimote* pWiiMote)
{

  pair<int, int> avgd = GetAveragedIRXY(pWiiMote);
  // uinput_mouse_absolute_movement(pWiiMote,avgd.first,avgd.second);

  struct input_event ev[3]; // X, Y, and SYN
  int ir_x=avgd.first;
  int ir_y=avgd.second;

  memset(&ev,0,sizeof(ev));
  
  if (ir_x==0)
    {
      ir_x=pWiiMote->old_ir_x;
    }
  
  if (ir_y==0)
    {
      ir_y=pWiiMote->old_ir_y;
    }

  cout << "\tx:\t" << ir_x << "\ty:\t" << ir_y << endl;

  ev[0].type = EV_ABS;
  ev[0].code = ABS_X;
  ev[0].value = abs(ir_x-1023);
  ev[1].type = EV_ABS;
  ev[1].code = ABS_Y;
  ev[1].value = ir_y;
  ev[2].type = EV_SYN;
  ev[2].code = SYN_REPORT;
  ev[2].value = 0;
  write(pWiiMote->uinput_fd,ev,sizeof(ev));

  pWiiMote->old_ir_x=avgd.first;
  pWiiMote->old_ir_y=avgd.second;

}

int Wii_Remote_Controller::Wiimote_Capture(int deviceID)
{
  LoggerWrapper::GetInstance()->Write(LV_WARNING,"Starting Wiimote Capture Thread.");
  // Schedule an initial device probe.
  m_pAlarmManager->AddRelativeAlarm(1, this, FIND_WIIMOTES, NULL);
  Sleep(2000);

  while (!m_bQuit_get())
    {
      if (m_WiiMote1->m_bActive)
	ProcessWiiMote(m_WiiMote1);
      if (m_WiiMote2->m_bActive)
	ProcessWiiMote(m_WiiMote2);
      if (m_WiiMote3->m_bActive)
	ProcessWiiMote(m_WiiMote3);
      if (m_WiiMote4->m_bActive)
	ProcessWiiMote(m_WiiMote4);

      usleep(10000);
    }

  return 0;
}

void Wii_Remote_Controller::AlarmCallback(int id, void *param)
{
  switch (id)
    {
    case FIND_WIIMOTES:
      FindWiimotes();
      break;
    }
}

void Wii_Remote_Controller::SetupIRMouse(Wiimote *pWiiMote)
{
  struct uinput_user_dev uidev;
  memset(&uidev, 0, sizeof(uidev));
  snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "WiiMote Mouse");
  uidev.id.bustype = BUS_USB;
  uidev.id.vendor  = 0x1234;
  uidev.id.product = 0xfedc;
  uidev.id.version = 1;
  uidev.absmin[ABS_X] = 0;
  uidev.absmax[ABS_X] = 1023;
  uidev.absmin[ABS_Y] = 0;
  uidev.absmax[ABS_Y] = 767;

  pWiiMote->uinput_fd = open("/dev/uinput",O_WRONLY|O_NONBLOCK);
  ioctl(pWiiMote->uinput_fd, UI_SET_EVBIT, EV_ABS);
  ioctl(pWiiMote->uinput_fd, UI_SET_ABSBIT, ABS_X);
  ioctl(pWiiMote->uinput_fd, UI_SET_ABSBIT, ABS_Y);
  ioctl(pWiiMote->uinput_fd, UI_SET_EVBIT, EV_KEY);
  ioctl(pWiiMote->uinput_fd, UI_SET_KEYBIT, BTN_LEFT);

  if (pWiiMote->uinput_fd < 0)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not open uinput.");
      return;
    }

  write(pWiiMote->uinput_fd,&uidev,sizeof(uidev));

  ioctl(pWiiMote->uinput_fd,UI_DEV_CREATE);
}

void Wii_Remote_Controller::FindWiimotes()
{

  string sPath1=GetDevice(1);
  string sPath2=GetDevice(2);
  string sPath3=GetDevice(3);
  string sPath4=GetDevice(4);
  int ret;

  if (!sPath1.empty() && !m_WiiMote1->m_bActive)
    {
      m_WiiMote1->m_sPath=sPath1;

      ret = xwii_iface_new(&m_WiiMote1->iface,m_WiiMote1->m_sPath.c_str());

      if (ret)
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not create new interface for Remote 1");
	  m_WiiMote1->m_bActive=false;
	  m_WiiMote1->m_sPath="";
	  return;
	}
      
      ret = xwii_iface_open(m_WiiMote1->iface,XWII_IFACE_CORE | XWII_IFACE_IR);

      if (ret)
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not open interface for Remote 1");
	  m_WiiMote1->m_bActive=false;
	  m_WiiMote1->m_sPath="";
	  return;
	}
      m_WiiMote1->m_bActive=true;
      LoggerWrapper::GetInstance()->Write(LV_WARNING,"Remote 1 active. %s",sPath1.c_str());
      SetupIRMouse(m_WiiMote1);
    }
  else if (sPath1.empty() && m_WiiMote1->m_bActive)
    {
      m_WiiMote1->m_sPath="";
      m_WiiMote1->m_bActive=false;
      ioctl(m_WiiMote1->uinput_fd,UI_DEV_DESTROY);
      xwii_iface_close(m_WiiMote1->iface,XWII_IFACE_CORE|XWII_IFACE_IR);
      xwii_iface_unref(m_WiiMote1->iface);
      LoggerWrapper::GetInstance()->Write(LV_WARNING,"Remote 1 inactive.",m_WiiMote1->m_sPath.c_str());
    }

  // Reschedule a check in 3 seconds.
  m_pAlarmManager->CancelAlarmByType(FIND_WIIMOTES);
  m_pAlarmManager->AddRelativeAlarm(3, this, FIND_WIIMOTES, NULL);

}

void Wii_Remote_Controller::CreateChildren()
{
  Wii_Remote_Controller_Command::CreateChildren();
  Start();
}

void Wii_Remote_Controller::SendIR(string Port, string sIRCode, int iRepeat)
{
  // not used. but must be implemented.
}

string Wii_Remote_Controller::GetDevice(int num)
{
  struct xwii_monitor *mon;
  char *ent;
  int i=0;

  mon = xwii_monitor_new(false, false);

  if (!mon)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Can't create Wiimote Monitor. Quitting");
      m_bQuit_set(true);
    }

  while ((ent = xwii_monitor_poll(mon)))
    {
      if (++i == num)
	break;
      free(ent);
    }

  xwii_monitor_unref(mon);

  if (!ent)
    {
      // Can't find device #'d num, fill this in.
      return "";
    }

  // Found it, return the entity.
  string sRet = string(ent);
  free(ent);

  return sRet;

}

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c191-b->

	/** @brief COMMAND: #191 - Send Code */
	/** The I/R code -- usually in Pronto format */
		/** @param #9 Text */
			/** The I/R code -- usually in Pronto format */

void Wii_Remote_Controller::CMD_Send_Code(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c191-e->
{
	cout << "Need to implement command #191 - Send Code" << endl;
	cout << "Parm #9 - Text=" << sText << endl;
}

//<-dceag-c245-b->

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

void Wii_Remote_Controller::CMD_Learn_IR(int iPK_Device,string sOnOff,int iPK_Text,int iPK_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c245-e->
{
	cout << "Need to implement command #245 - Learn IR" << endl;
	cout << "Parm #2 - PK_Device=" << iPK_Device << endl;
	cout << "Parm #8 - OnOff=" << sOnOff << endl;
	cout << "Parm #25 - PK_Text=" << iPK_Text << endl;
	cout << "Parm #154 - PK_Command=" << iPK_Command << endl;
}

//<-dceag-c687-b->

	/** @brief COMMAND: #687 - Set Screen Type */
	/** Sent by Orbiter when the screen changes to tells the i/r receiver what type of screen is displayed so it can adjust mappings if necessary. */
		/** @param #48 Value */
			/** a character: M=Main Menu, m=other menu, R=Pluto Remote, r=Non-pluto remote, N=navigable OSD on media dev, f=full screen media app, F=File Listing, c=computing list, C=Computing full screen */

void Wii_Remote_Controller::CMD_Set_Screen_Type(int iValue,string &sCMD_Result,Message *pMessage)
//<-dceag-c687-e->
{
	cout << "Need to implement command #687 - Set Screen Type" << endl;
	cout << "Parm #48 - Value=" << iValue << endl;
	m_cCurrentScreen=(char) iValue;
}

