/** 
 * MAMEEmulatorEngine - a model class supporting MAME.
 * 
 * This one is special, because no other emulations are decended from it.
 * It handles only one nebulous system type, an arcade game of some sort.
 * And all the arcade games are known within the system.
 * 
 * Author: Thom Cherryhomes <thom.cherryhomes@gmail.com>
 */

/** 
 * Shit we know we need to take care of:
 * Passing in special controller override a la TankStick.
 */

#include "MAMEEmulatorEngine.h"
#include "DCE/Logger.h"
#include <X11/keysym.h> // for XK mapping.

#define MAME_DIR "/root/.mame"
#define MAME_INI "/root/.mame/mame.ini"
#define MAME_INI_TEMPLATE "/usr/pluto/templates/mame.ini.template"

namespace DCE
{
  
  /** 
   * ctor 
   */
  MAMEEmulatorEngine::MAMEEmulatorEngine() : EmulatorEngine()
  {
    m_sEnginePath = "/usr/local/bin/mame";
    setMedia("");
    setRunning(false);
  }
  
  /** 
   * dtor
   */
  MAMEEmulatorEngine::~MAMEEmulatorEngine()
  {
    setMedia("");
    setRunning(false);
  }
  
  /**
   * updateConfig() - write out a mess ini from template.
   * inherited from EmulatorEngine.
   */
  bool MAMEEmulatorEngine::updateConfig()
  {
    if (!updateINI())
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Unable to update /root/.mame/mame.ini");
      }
    return true;
  }

  string MAMEEmulatorEngine::getVideoDriver()
  {
    return (m_bUseOpenGL ? "opengl" : "soft");
  }
  
  bool MAMEEmulatorEngine::updateINI()
  {
    string sConfFile; 

    if (!FileUtils::FileExists(MAME_INI_TEMPLATE))
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MAMEEmulatorEngine::updateINI() - Unable to read template file MAME_INI_TEMPLATE");
	return false;
      }

    sConfFile = FileUtils::ReadTextFile(MAME_INI_TEMPLATE, sConfFile);
    sConfFile = StringUtils::Replace(sConfFile,"###ROMPATH###",FileUtils::BasePath(m_sMediaFileName));
    sConfFile = StringUtils::Replace(sConfFile,"###VIDEO###",getVideoDriver());

    if (!FileUtils::DirExists (MAME_DIR))
      {
	// hopefully try to create it.
	LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,
					      "MAME_INI_DIR didn't exist, attempting to create.");
	FileUtils::MakeDir (MAME_DIR);
	if (!FileUtils::DirExists (MAME_DIR))
	  {
	    LoggerWrapper::GetInstance ()->Write (LV_CRITICAL,
						  "Couldn't create MAME_INI_DIR, bailing...");
	    return false;
	  }
	else
	  {
	    LoggerWrapper::GetInstance ()->Write (LV_STATUS,
						  "Created MAME_INI_DIR Successfully.");
	  }
      }
    
    if (!FileUtils::WriteTextFile(MAME_INI, sConfFile))
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not write MAME_INI.");
	return false;
      }
    else
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"MAME_INI written.");
      }
    
    return true;
  }
  
  bool MAMEEmulatorEngine::setup()
  {
    if (!updateConfig())
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"MAMEEmulatorEngine::setup() updateConfig - unable to update configuration.");
      }
    
    setupDefaultControls();

    return true;
  }

  void MAMEEmulatorEngine::setupDefaultControls()
  {
    m_mapKeysToPorts["START1"] = make_pair<int, int>(XK_1,0);
    m_mapKeysToPorts["START2"] = make_pair<int, int>(XK_2,0);
    m_mapKeysToPorts["START3"] = make_pair<int, int>(XK_3,0);
    m_mapKeysToPorts["START4"] = make_pair<int, int>(XK_4,0);
    m_mapKeysToPorts["COIN1"] = make_pair<int, int>(XK_5,0);
    m_mapKeysToPorts["COIN2"] = make_pair<int, int>(XK_6,0);
    m_mapKeysToPorts["COIN3"] = make_pair<int, int>(XK_7,0);
    m_mapKeysToPorts["COIN4"] = make_pair<int, int>(XK_8,0);
    m_mapKeysToPorts["START"] = make_pair<int, int>(XK_1,0);
    m_mapKeysToPorts["SELECT"] = make_pair<int, int>(XK_5,0);
    m_mapKeysToPorts["SERVICE1"] = make_pair<int, int>(XK_9,0);
    m_mapKeysToPorts["SERVICE2"] = make_pair<int, int>(XK_0,0);
    m_mapKeysToPorts["SERVICE3"] = make_pair<int, int>(XK_minus,0);
    m_mapKeysToPorts["SERVICE4"] = make_pair<int, int>(XK_plus,0);
    m_mapKeysToPorts["SERVICE"] = make_pair<int, int>(XK_F2,0);
    m_mapKeysToPorts["UI_CONFIGURE"] = make_pair<int, int>(XK_Tab,0);
    m_mapKeysToPorts["UI_PAUSE"] = make_pair<int, int>(XK_P,0);
    m_mapKeysToPorts["UI_RESET_MACHINE"] = make_pair<int, int>(XK_F3,XK_Shift_L);
    m_mapKeysToPorts["UI_SNAPSHOT"] = make_pair<int, int>(XK_F12,0);
    m_mapKeysToPorts["UI_RECORD_MOVIE"] = make_pair<int, int>(XK_F12,XK_Shift_L);
    m_mapKeysToPorts["UI_UP"] = make_pair<int, int>(XK_Up,0);
    m_mapKeysToPorts["UI_DOWN"] = make_pair<int, int>(XK_Down,0);
    m_mapKeysToPorts["UI_LEFT"] = make_pair<int, int>(XK_Left,0);
    m_mapKeysToPorts["UI_RIGHT"] = make_pair<int, int>(XK_Right,0);
    m_mapKeysToPorts["UI_SELECT"] = make_pair<int, int>(XK_Return,0);
    m_mapKeysToPorts["UI_CANCEL"] = make_pair<int, int>(XK_Escape,0);
    m_mapKeysToPorts["UI_TOGGLE_UI"] = make_pair<int, int>(XK_Scroll_Lock,0);
    m_mapKeysToPorts["UI_SAVE_STATE"] = make_pair<int, int>(XK_F7,XK_Shift_L);
    m_mapKeysToPorts["UI_LOAD_STATE"] = make_pair<int, int>(XK_F7,0);
    
  }

}
