/**
 * Atari800EmulatorController - Support to control the Atari800 emulator.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "Atari800EmulatorController.h"
#include "PlutoUtils/FileUtils.h"
#include "pluto_main/Define_Button.h"
#include <X11/keysym.h>
#include "WindowUtils/WindowUtils.h"

#define ATARI800_STATE_FILE "/run/Game_Player/atari800.state"

namespace DCE
{
  Atari800EmulatorController::Atari800EmulatorController(Game_Player *pGame_Player, Atari800EmulatorModel *pEmulatorModel)
    : X11EmulatorController(pGame_Player, pEmulatorModel)
  {
    m_pGame_Player = pGame_Player;
    m_pEmulatorModel = pEmulatorModel;
  }

  Atari800EmulatorController::~Atari800EmulatorController()
  {
   
  }

  bool Atari800EmulatorController::init()
  {
    return X11EmulatorController::init();
  }

  void Atari800EmulatorController::insertMediaNamed(string sMediaFile, string sSlot)
  {
    string sMediaUpper = StringUtils::ToUpper(sMediaFile);
    if ((sMediaUpper.find(".ATR") != string::npos) ||
	(sMediaUpper.find(".XFD") != string::npos) ||
	(sMediaUpper.find(".DCM") != string::npos))
      {
	sSlot=""; // Atari800 doesn't use a switch for this one.
      }
    else if ((sMediaUpper.find(".CAR") != string::npos) ||
	     (sMediaUpper.find(".ROM") != string::npos))
      {
	sSlot="-cart";
      }
    else if ((sMediaUpper.find(".XEX") != string::npos))
      {
	sSlot="-run";
      }

    EmulatorController::insertMediaNamed(sMediaFile, sSlot);
  }

  string Atari800EmulatorController::getSlotsAndRoms()
  {
    string sRet = "";
    
    if (!m_pEmulatorModel)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MESSEmulatorController::getSlotsAndRoms() - no m_pEmulatorModel, bailing!");
	return "";
      }
    
    for (map<string,string>::iterator it=m_pEmulatorModel->m_mapMedia.begin(); it!=m_pEmulatorModel->m_mapMedia.end(); ++it)
      {
	string sSlot = it->first;
	string sMediaFile = it->second;
		
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Inserting %s into slot %s",sSlot.c_str(),sMediaFile.c_str());
	
	if (sMediaFile.find("(OSB)") != string::npos)
	  {
	    sRet += "-atari\t";
	  }
	else
	  {
	    sRet += "-320xe\t";
	  }
	
	if (sMediaFile.find("(SIO)") != string::npos)
	  {
	    sRet += "-nopatchall\t";
	  }

	if (sMediaFile.find("(BASIC)") != string::npos)
	  {
	    sRet += "-basic\t";
	  }

	if (sMediaFile.find("(PAL)") != string::npos)
	  {
	    sRet += "-pal\t";
	  }
	else 
	  {
	    sRet += "-ntsc\t";
	  }

	sRet += "-" + sSlot + "\t" +
	  sMediaFile;
      }

    return sRet;
  }

  bool Atari800EmulatorController::run()
  {
    if (!m_pEmulatorModel)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Atari800EmulatorController::run() - pEmulatorModel is null. Bailing.");
	return false;
      }

    m_pEmulatorModel->m_sArgs = getSlotsAndRoms();

    if (!m_pEmulatorModel->m_sMediaPosition.empty() && !m_pEmulatorModel->m_bIsStreamingSource)
      {
	// Implement State loading if possible.
	string sPath = "/home/mamedata/sta/atari800";
	string sSource = sPath + "/" + m_pEmulatorModel->m_sMediaPosition;
	string sDest = ATARI800_STATE_FILE;
	
	FileUtils::DelFile(sDest); // some basic house cleaning.
	
	if (m_pEmulatorModel->m_sMediaPosition.find("AutoSave") != string::npos)
	  {
	    // Autosave specified, just load the autosave file for this game.
	    sSource = sPath + "/AutoSave";
	  }
	
	if (!FileUtils::FileExists(sSource))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Atari800EmulatorController::loadState() - Could not load state file %s as it does not exist.",sSource.c_str());
	  }
	
	// move file into place.
	string sCmd = "cp "+sSource+" "+sDest;
	system(sCmd.c_str());
	
	if (!FileUtils::FileExists(sDest))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Atari800EmulatorController::loadState() - Could not load copied state file %s as it does not exist. Bailing!",sDest.c_str());
	  }
	else
	  {
	    // tell emulator to start with state file. 
	    m_pEmulatorModel->m_sArgs += "\t-state\t" ATARI800_STATE_FILE;
	  }
      }

    if (m_pEmulatorModel->m_bIsStreaming)
      {
	// Implement Streaming if possible.
	if (m_pEmulatorModel->m_bIsStreamingSource)
	  {
	    // This is the server, set args as needed.
	  }
	else
	  {
	    // This is the client, set appropriately.
	  }
      }
    else
      {
	// Regular Non-streaming instance
      }

    if (X11EmulatorController::run())
      {
        usleep(2000);
        m_pGame_Player->EVENT_Menu_Onscreen(m_pEmulatorModel->m_iStreamID,false);
        usleep(2000);
      	return true;
      }
    
    return false;
  }

  bool Atari800EmulatorController::stop()
  {
      X11EmulatorController::stop();

      // TODO: remember why the hell I removed this from the base class!
      m_pEmulatorModel->m_mapMedia.clear();
      return true;
  }

  bool Atari800EmulatorController::pause()
  {
    doAction("PAUSE");
    return true;
  }

  bool Atari800EmulatorController::unpause()
  {
    doAction("UNPAUSE");
    return true;
  }

  bool Atari800EmulatorController::setSpeed(int iSpeed)
  {
    switch (iSpeed)
      {
      case 0:
	pause();
	break;
      case 1000:
	unpause();
	break;
      default:
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Atari800EmulatorController:setSpeed(%d) not implemented. Ignoring.");
	break;
      }
    return EmulatorController::setSpeed(iSpeed);
  }

  bool Atari800EmulatorController::gotoMenu(int iMenu)
  {
    switch(iMenu)
      {
      case 0:
	if (m_pEmulatorModel->m_iActiveMenu == 2) // 2 and above is a UI of some sort.
	  doAction("UI_EXIT");
	break;
      case 1:
	doAction("MONITOR_MODE");
	break;
      case 2:
	doAction("UI_ENTER");
	break;
      }
    
    return X11EmulatorController::gotoMenu(iMenu); // and up the chain...
  }

  bool Atari800EmulatorController::saveState(string& sPosition, string& sText, bool bAutoSave, string sAutoSaveName)
  {
    if (!m_pEmulatorModel->m_bIsStreaming && 
	m_pEmulatorModel->m_bCanSaveState)
      {
	doAction("SAVE_STATE");
	
	// the save game is now in ATARI800_STATE_FILE
	// make a copy of this to reference later.
	
	if (!FileUtils::FileExists(ATARI800_STATE_FILE))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Atari800EmulatorController::saveState() - State file " ATARI800_STATE_FILE " does not exist. Bailing.");
	    sPosition="";
	    sText="";
	    return false;
	  }
	
	// Autosave bookmark is always saved as AutoSave if bAutoSave is true
	// otherwise, generate an md5 from the state file and use that as the
	// saved filename on disk. 
	string sBookmarkName;
	if (!bAutoSave)
	  {
	    sBookmarkName = FileUtils::FileChecksum(ATARI800_STATE_FILE);
	  }
	else
	  {
	    sBookmarkName = "AutoSave";
	  }
	string sCmd0 = "mkdir -p /home/mamedata/sta/atari800";
	system(sCmd0.c_str());
	string sCmd = "mv " ATARI800_STATE_FILE " /home/mamedata/sta/atari800/"+sBookmarkName;
	system(sCmd.c_str());
	
	if (!FileUtils::FileExists("/home/mamedata/sta/atari800/"+sBookmarkName))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MAMEEmulatorController::saveState() - Moving " ATARI800_STATE_FILE " to /home/mamedata/sta/atari800/%s failed, Bailing!",sBookmarkName.c_str());
	    sPosition="";
	    sText="";
	    return false;
	  }
	
	// all is successsful, set sPosition and sText for return.
	sPosition = sBookmarkName;
	sText = "Saved bookmark on "+StringUtils::SQLDateTime(0)+" at "+StringUtils::HourMinute(0,false);
	
	return true;
      }
    else
      {
	// State ignored in streaming mode.
	return true;
      }
    
  }
  
  void Atari800EmulatorController::setMediaPosition(string sMediaPosition)
  {
    EmulatorController::setMediaPosition(sMediaPosition);
    return;
  }
  
  bool Atari800EmulatorController::loadState(string sPosition)
  {
    
    if (!m_pEmulatorModel->m_bIsStreaming && m_pEmulatorModel->m_bCanSaveState)
      {
	string sPath = "/home/mamedata/sta/atari800";
	string sSource = sPath + "/" + sPosition;
	string sDest = ATARI800_STATE_FILE;
	
	FileUtils::DelFile(sDest); // some basic house cleaning.
	
	if (sPosition.find("AutoSave") != string::npos)
	  {
	    // Autosave specified, just load the autosave file for this game.
	    sSource = sPath + "/AutoSave";
	  }
	
	if (!FileUtils::FileExists(sSource))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Atari800EmulatorController::loadState() - Could not load state file %s as it does not exist.",sSource.c_str());
	    return false;
	  }
	
	// move file into place.
	string sCmd = "cp "+sSource+" "+sDest;
	system(sCmd.c_str());
	
	if (!FileUtils::FileExists(sDest))
	  {
	    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Atari800EmulatorController::loadState() - Could not load copied state file %s as it does not exist. Bailing!",sDest.c_str());
	  }
	
	// File moved into place, save state.
	doAction("LOAD_STATE");
	
	return true;
      }
    else
      {
	// No state pushed during streaming. csmame freaks.
	return true;
      }
    return false;
  }
  
  bool Atari800EmulatorController::record()
  {
    // implement record.
    return false;
  }

  bool Atari800EmulatorController::getSnap(long int iPK_Device, int iWidth, int iHeight, char **pData, int& iData_Size)
  {
    if (m_pEmulatorModel->m_iActiveMenu == 2)
      {
	return X11EmulatorController::getSnap(iPK_Device,iWidth,iHeight,pData,iData_Size);
      }
    else
      {
	size_t size;
	m_pEmulatorModel->updateVideoFrameGeometry(iPK_Device,iWidth,iHeight);
	doAction("GET_SNAPSHOT");
	// Currently, it winds up in /usr/pluto/bin, let's patch that later -tschak
	string sCmd = "convert /run/Game_Player/atari_screenshot.png -trim -resize " + StringUtils::itos(iWidth) + "x" + StringUtils::itos(iHeight) + " -background black -gravity center -extent " + StringUtils::itos(iWidth) + "x" + StringUtils::itos(iHeight) + " +repage /run/Game_Player/atari_screenshot.jpg";
	system(sCmd.c_str());
	*pData = FileUtils::ReadFileIntoBuffer("/run/Game_Player/atari_screenshot.jpg",size);
	iData_Size = size;
	
	FileUtils::DelFile("/run/Game_Player/atari_screenshot.png");
	FileUtils::DelFile("/run/Game_Player/atari_screenshot.jpg");
      }
    return true;
    
  }

  bool Atari800EmulatorController::pressButton(int iPK_Button, Message *pMessage)
  {
    PLUTO_SAFETY_LOCK(gm, m_pGame_Player->m_X11ControllerMutex);

    int iKey=0;
    bool bDoNotPressModifier=false;

    if (!m_pEmulatorModel->m_pDisplay)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"X11EmulatorModel::pressButton(%d) - No valid display pointer, bailing!",iPK_Button);
	return false;
      }
    
    if (!m_pEmulatorModel->m_iWindowId)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"X11EmulatorModel::pressButton(%d) - No valid window, bailing!",iPK_Button);
	return false;
      }

    switch (iPK_Button)
      {
      case BUTTON_left_shift_CONST:
	m_pEmulatorModel->m_iCurrentKeyModifier = XK_Shift_L;
	break;
      case BUTTON_right_shift_CONST:
	m_pEmulatorModel->m_iCurrentKeyModifier = XK_Shift_R;
	break;
      case BUTTON_Control_CONST:
	m_pEmulatorModel->m_iCurrentKeyModifier = XK_Control_L;
	break;
      case BUTTON_Shift_Left_CONST:
	m_pEmulatorModel->m_iCurrentKeyModifier = XK_Shift_L;
	break;
      case BUTTON_Shift_Right_CONST:
	m_pEmulatorModel->m_iCurrentKeyModifier = XK_Shift_R;
	break;
      case BUTTON_caps_lock_CONST:
	m_pEmulatorModel->m_bCapsLockPressed=!m_pEmulatorModel->m_bCapsLockPressed;
	break;
      }

    // Simulate Caps lock.
    if (m_pEmulatorModel->m_bCapsLockPressed)
      {
	m_pEmulatorModel->m_iCurrentKeyModifier = XK_Shift_L;
      }

    switch (iPK_Button)
      {
      case BUTTON_escape_CONST:
	iKey=XK_Escape;
	break;
      case BUTTON_1_CONST:
	if ((m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_L) || (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_R))
	  {
	    iKey = XK_exclam;
	    bDoNotPressModifier=true;
	  }
	else
	  {
	    iKey = XK_1;
	  }
	break;
      case BUTTON_2_CONST:
	if ((m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_L) || (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_R))
	  {
	    iKey = XK_quotedbl;
	    bDoNotPressModifier=true;
	  }
	else
	  {
	    iKey = XK_2;
	  }
	break;
      case BUTTON_3_CONST:
	if ((m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_L) || (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_R))
	  {
	    iKey = XK_numbersign;
	    bDoNotPressModifier=true;
	  }
	else
	  {
	    iKey = XK_3;
	  }
	break;
      case BUTTON_4_CONST:
	if ((m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_L) || (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_R))
	  {
	    iKey = XK_dollar;
	    bDoNotPressModifier=true;
	  }
	else
	  {
	    iKey = XK_4;
	  }
	break;
      case BUTTON_5_CONST:
	if ((m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_L) || (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_R))
	  {
	    iKey = XK_percent;
	    bDoNotPressModifier=true;
	  }
	else
	  {
	    iKey = XK_5;
	  }
	break;
      case BUTTON_6_CONST:
	if ((m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_L) || (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_R))
	  {
	    iKey = XK_ampersand;
	    bDoNotPressModifier=true;
	  }
	else
	  {
	    iKey = XK_6;
	  }
	break;
      case BUTTON_7_CONST:
	if ((m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_L) || (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_R))
	  {
	    iKey = XK_apostrophe;
	    bDoNotPressModifier=true;
	  }
	else
	  {
	    iKey = XK_7;
	  }
	break;
      case BUTTON_8_CONST:
	if ((m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_L) || (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_R))
	  {
	    iKey = XK_at;
	    bDoNotPressModifier=true;
	  }
	else
	  {
	    iKey = XK_8;
	  }
	break;
      case BUTTON_9_CONST:
	if ((m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_L) || (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_R))
	  {
	    iKey = XK_parenleft;
	    bDoNotPressModifier=true;
	  }
	else
	  {
	    iKey = XK_9;
	  }
	break;
      case BUTTON_0_CONST:
	if ((m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_L) || (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_R))
	  {
	    iKey = XK_parenright;
	    bDoNotPressModifier=true;
	  }
	else
	  {
	    iKey = XK_0;
	  }
	break;
      case BUTTON_atariltclr_CONST:
	if ((m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_L) || (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_R))
	  {
	    iKey = XK_Delete;
	    bDoNotPressModifier=true;
	  }
	else
	  {
	    iKey = XK_less;
	  }
	break;
      case BUTTON_atarigtins_CONST:
	if ((m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_L) || (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_R))
	  {
	    iKey = XK_Insert;
	    bDoNotPressModifier=true;
	  }
	else
	  {
	    iKey = XK_greater;
	  }
	break;
      case BUTTON_Back_CONST:
	iKey = XK_BackSpace;
	break;
      case BUTTON_break_CONST:
	iKey = XK_Break; // Shit, after I broke this key for pause...
	break;
      case BUTTON_tab_CONST:
	iKey = XK_Tab;
	break;
      case BUTTON_q_CONST:
	iKey = XK_q;
	break;
      case BUTTON_w_CONST:
	iKey = XK_w;
	break;
      case BUTTON_e_CONST:
	iKey = XK_e;
	break;
      case BUTTON_r_CONST:
	iKey = XK_r;
	break;
      case BUTTON_t_CONST:
	iKey = XK_t;
	break;
      case BUTTON_y_CONST:
	iKey = XK_y;
	break;
      case BUTTON_u_CONST:
	iKey = XK_u;
	break;
      case BUTTON_i_CONST:
	iKey = XK_i;
	break;
      case BUTTON_o_CONST:
	iKey = XK_o;
	break;
      case BUTTON_p_CONST:
	iKey = XK_p;
	break;
      case BUTTON_a8dash_CONST:
	if (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_L || m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_R)
	  {
	    iKey = XK_underscore;
	  }
	else if (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Control_L)
	  {
	    iKey = XK_Up;
	  }
	else
	  {
	    iKey = XK_minus;
	  }
	break;
      case BUTTON_a8equals_CONST:
	if (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_L || m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_R)
	  {
	    iKey = XK_bar;
	  }
	else if (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Control_L)
	  {
	    iKey = XK_Down;
	  }
	else
	  {
	    iKey = XK_equal;
	  }
	break;	
      case BUTTON_Enter_CONST:
	iKey = XK_Return;
	break;
      case BUTTON_a_CONST:
	iKey = XK_a;
	break;
      case BUTTON_s_CONST:
	iKey = XK_s;
	break;
      case BUTTON_d_CONST:
	iKey = XK_d;
	break;
      case BUTTON_f_CONST:
	iKey = XK_f;
	break;
      case BUTTON_g_CONST:
	iKey = XK_g;
	break;
      case BUTTON_h_CONST:
	iKey = XK_h;
	break;
      case BUTTON_j_CONST:
	iKey = XK_j;
	break;
      case BUTTON_k_CONST:
	iKey = XK_k;
	break;
      case BUTTON_l_CONST:
	iKey = XK_l;
	break;
      case BUTTON_semicolumn_CONST:
	if (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_L || m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_R)
	  {
	    iKey = XK_colon;
	  }
	else
	  {
	    iKey = XK_semicolon;
	  }
	break;
      case BUTTON_a8plus_CONST:
	if (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_L || m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_R)
	  {
	    iKey = XK_backslash;
	  }
	else if (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Control_L)
	  {
	    iKey = XK_Left;
	  }
	else
	  {
	    iKey = XK_plus;
	  }
	break;
      case BUTTON_a8ast_CONST:
	if (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_L || m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_R)
	  {
	    iKey = XK_asciicircum; // Seriously? You fucks couldn't simply have called it XK_caret?
	  }
	else if (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Control_L)
	  {
	    iKey = XK_Right;
	  }
	else
	  {
	    iKey = XK_asterisk;
	  }
	break;
      case BUTTON_z_CONST:
	iKey = XK_z;
	break;
      case BUTTON_x_CONST:
	iKey = XK_x;
	break;
      case BUTTON_c_CONST:
	iKey = XK_c;
	break;
      case BUTTON_v_CONST:
	iKey = XK_v;
	break;
      case BUTTON_b_CONST:
	iKey = XK_b;
	break;
      case BUTTON_n_CONST:
	iKey = XK_n;
	break;
      case BUTTON_m_CONST:
	iKey = XK_m;
	break;
      case BUTTON_comma_CONST:
	if (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_L || m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_R)
	  {
	    iKey = XK_bracketleft;
	  }
	else
	  {
	    iKey = XK_comma;
	  }
	break;
      case BUTTON_period_CONST:
	if (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_L || m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_R)
	  {
	    iKey = XK_bracketright;
	  }
	else
	  {
	    iKey = XK_period;
	  }
	break;
      case BUTTON_slash_CONST:
	if (m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_L || m_pEmulatorModel->m_iCurrentKeyModifier == XK_Shift_R)
	  {
	    iKey = XK_question;
	  }
	else
	  {
	    iKey = XK_slash;
	  }
	break;
      case BUTTON_a8logo_CONST:
	iKey = XK_grave;
	break;
      case BUTTON_a8option_CONST:
	iKey = XK_F2;
	break;
      case BUTTON_a8select_CONST:
	iKey = XK_F3;
	break;
      case BUTTON_a8start_CONST:
	iKey = XK_F4;
	break;
      }

    // Will collapse this with a ternary, later. -tschak
    if (bDoNotPressModifier)
      {
	WindowUtils::SendKeyToWindow(m_pEmulatorModel->m_pDisplay,
				     m_pEmulatorModel->m_iWindowId,
				     iKey,
				     0);
      }
    else
      {
	XTestFakeKeyEvent( m_pEmulatorModel->m_pDisplay, XKeysymToKeycode( m_pEmulatorModel->m_pDisplay, m_pEmulatorModel->m_iCurrentKeyModifier),True, CurrentTime );
	WindowUtils::SendKeyToWindow(m_pEmulatorModel->m_pDisplay,
				     m_pEmulatorModel->m_iWindowId,
				     iKey,
				     m_pEmulatorModel->m_iCurrentKeyModifier);
	XTestFakeKeyEvent( m_pEmulatorModel->m_pDisplay, XKeysymToKeycode( m_pEmulatorModel->m_pDisplay, m_pEmulatorModel->m_iCurrentKeyModifier),False, CurrentTime );
      }

    if (m_pEmulatorModel->m_iCurrentKeyModifier>0)
      {
	m_pEmulatorModel->m_iCurrentKeyModifier=0; // Unpress modifier after use. 
      }

    return true;

  }

}
