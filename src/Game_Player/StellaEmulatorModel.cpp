/** 
 * StellaEmulatorModel - base configuration state for Stella based emulators
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version: 1.0
 */

#include "StellaEmulatorModel.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "INotifyCodes.h"
#include "DCE/Logger.h"

namespace DCE
{

  StellaEmulatorModel::StellaEmulatorModel()
    : INotifyEmulatorModel()
  {
    m_mapActionsToCommandBytes["PAUSE"] = make_pair(0x01,false);
    m_mapActionsToCommandBytes["UNPAUSE"] = make_pair(0x02,false);
    m_mapActionsToCommandBytes["P1_START"] = make_pair(0x03,true);
    m_mapActionsToCommandBytes["SELECT"] = make_pair(0x24,true);
    m_mapActionsToCommandBytes["KEYPAD1_0"] = make_pair(0x13,true);
    m_mapActionsToCommandBytes["KEYPAD1_1"] = make_pair(0x14,true);
    m_mapActionsToCommandBytes["KEYPAD1_2"] = make_pair(0x15,true);
    m_mapActionsToCommandBytes["KEYPAD1_3"] = make_pair(0x16,true);
    m_mapActionsToCommandBytes["KEYPAD1_4"] = make_pair(0x17,true);
    m_mapActionsToCommandBytes["KEYPAD1_5"] = make_pair(0x18,true);
    m_mapActionsToCommandBytes["KEYPAD1_6"] = make_pair(0x19,true);
    m_mapActionsToCommandBytes["KEYPAD1_7"] = make_pair(0x1A,true);
    m_mapActionsToCommandBytes["KEYPAD1_8"] = make_pair(0x1B,true);
    m_mapActionsToCommandBytes["KEYPAD1_9"] = make_pair(0x1C,true);
    m_mapActionsToCommandBytes["KEYPAD1_*"] = make_pair(0x1D,true);
    m_mapActionsToCommandBytes["KEYPAD1_#"] = make_pair(0x1E,true);
    m_mapActionsToCommandBytes["KEYPAD2_0"] = make_pair(0x2B,true);
    m_mapActionsToCommandBytes["KEYPAD2_1"] = make_pair(0x2C,true);
    m_mapActionsToCommandBytes["KEYPAD2_2"] = make_pair(0x2D,true);
    m_mapActionsToCommandBytes["KEYPAD2_3"] = make_pair(0x2E,true);
    m_mapActionsToCommandBytes["KEYPAD2_4"] = make_pair(0x2F,true);
    m_mapActionsToCommandBytes["KEYPAD2_5"] = make_pair(0x30,true);
    m_mapActionsToCommandBytes["KEYPAD2_6"] = make_pair(0x31,true);
    m_mapActionsToCommandBytes["KEYPAD2_7"] = make_pair(0x32,true);
    m_mapActionsToCommandBytes["KEYPAD2_8"] = make_pair(0x33,true);
    m_mapActionsToCommandBytes["KEYPAD2_9"] = make_pair(0x34,true);
    m_mapActionsToCommandBytes["KEYPAD2_*"] = make_pair(0x35,true);
    m_mapActionsToCommandBytes["KEYPAD2_#"] = make_pair(0x36,true);

    m_sStateDir="/home/mamedata/stella.sta";
    m_bCanSaveState=true;
  }

  StellaEmulatorModel::~StellaEmulatorModel()
  {
    m_mapActionsToCommandBytes.clear();
  }

  string StellaEmulatorModel::getVideoAccelleration()
  {
    // come back here later and actually flesh this out.
    return "opengl";
  }

  string StellaEmulatorModel::getResolution()
  {
    // string sRet = StringUtils::itos(m_iScreenWidth) + "x" +
    //   StringUtils::itos(m_iScreenHeight) + "@" +
    //   StringUtils::itos(m_iRefreshRate);
    // return sRet;
    return "";
  }

  /**
   * Update the /root/.mame/mame.ini file as needed.
   */
  bool StellaEmulatorModel::updateConfig()
  {
    ConfigurationWriter config(STELLA_CONFIG_FILE_TEMPLATE, STELLA_CONFIG_FILE, m_sSystemConfiguration);
    //    config.Add("###VIDEO###",getVideoAccelleration());
    //config.Add("###AVIWRITE###",(m_bIsRecording ? "recorded.avi" : ""));
    //config.Add("###STATE###",m_sState);
    //config.Add("###ROMPATH###",m_sRomPath);
    //config.Add("###RESOLUTION###",getResolution());
    return config.Write();
  }


}
