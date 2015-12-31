/**
 * VICEEmulatorModel - Store information needed by VICE based emulators 
 *     (e.g. Commodore 64 and VIC-20)
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#include "VICEEmulatorModel.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"
#include "pluto_main/Define_Button.h"

namespace DCE
{
  VICEEmulatorModel::VICEEmulatorModel() : INotifyEmulatorModel()
  {
    m_sConfigFile = VICE_CONFIG_FILE;
    m_sConfigFileTemplate = VICE_CONFIG_FILE_TEMPLATE;
    initializeActionstoKeysyms();
    initializeButtontoKeysyms();
    m_sEmulatorBinary = "/usr/bin/"; // also set by the subclass.
    m_sProcessName = ""; // set by the subclass.
    m_bChangeRequiresRestart=true;
    m_bRunning=false;
    m_bHasArgs=true;
    m_bCanSaveState=true;
    m_sArgs="";
  }

  VICEEmulatorModel::~VICEEmulatorModel()
  {
  }

  string VICEEmulatorModel::getVideoAccelleration()
  {
    return "opengl";   // not used yet.
  }

  /**
   * Update VICE configuration file
   */
  bool VICEEmulatorModel::updateConfig()
  {
    ConfigurationWriter config(VICE_CONFIG_FILE_TEMPLATE, VICE_CONFIG_FILE, m_sSystemConfiguration);
    return config.Write();
  }

  void VICEEmulatorModel::initializeActionstoKeysyms()
  {
    m_mapActionsToCommandBytes["PAUSE"] = make_pair(0x01,false);
    m_mapActionsToCommandBytes["UNPAUSE"] = make_pair(0x02,false);
    m_mapActionsToCommandBytes["RUN_STOP"] = make_pair(0x03,true);
    m_mapActionsToCommandBytes["1"] = make_pair(0x04,true);
    m_mapActionsToCommandBytes["2"] = make_pair(0x05,true);
    m_mapActionsToCommandBytes["3"] = make_pair(0x06,true);
    m_mapActionsToCommandBytes["4"] = make_pair(0x07,true);
    m_mapActionsToCommandBytes["5"] = make_pair(0x08,true);
    m_mapActionsToCommandBytes["6"] = make_pair(0x09,true);
    m_mapActionsToCommandBytes["7"] = make_pair(0x0A,true);
    m_mapActionsToCommandBytes["8"] = make_pair(0x0B,true);
    m_mapActionsToCommandBytes["9"] = make_pair(0x0C,true);
    m_mapActionsToCommandBytes["0"] = make_pair(0x0D,true);
    m_mapActionsToCommandBytes["+"] = make_pair(0x0E,true);
    m_mapActionsToCommandBytes["-"] = make_pair(0x0F,true);
    m_mapActionsToCommandBytes["DELETE"] = make_pair(0x10,true);
    m_mapActionsToCommandBytes["CTRL"] = make_pair(0x11,true);
    m_mapActionsToCommandBytes["Q"] = make_pair(0x12,true);
    m_mapActionsToCommandBytes["W"] = make_pair(0x13,true);
    m_mapActionsToCommandBytes["E"] = make_pair(0x14,true);
    m_mapActionsToCommandBytes["R"] = make_pair(0x15,true);
    m_mapActionsToCommandBytes["T"] = make_pair(0x16,true);
    m_mapActionsToCommandBytes["Y"] = make_pair(0x17,true);
    m_mapActionsToCommandBytes["U"] = make_pair(0x18,true);
    m_mapActionsToCommandBytes["I"] = make_pair(0x19,true);
    m_mapActionsToCommandBytes["O"] = make_pair(0x1A,true);
    m_mapActionsToCommandBytes["P"] = make_pair(0x1B,true);
    m_mapActionsToCommandBytes["@"] = make_pair(0x1C,true);
    m_mapActionsToCommandBytes["*"] = make_pair(0x1D,true);
    m_mapActionsToCommandBytes["RETURN"] = make_pair(0x1E,true);
    m_mapActionsToCommandBytes["COMMODORE"] = make_pair(0x1F,true);
    m_mapActionsToCommandBytes["A"] = make_pair(0x20,true);
    m_mapActionsToCommandBytes["S"] = make_pair(0x21,true);
    m_mapActionsToCommandBytes["D"] = make_pair(0x22,true);
    m_mapActionsToCommandBytes["F"] = make_pair(0x23,true);
    m_mapActionsToCommandBytes["G"] = make_pair(0x24,true);
    m_mapActionsToCommandBytes["H"] = make_pair(0x25,true);
    m_mapActionsToCommandBytes["J"] = make_pair(0x26,true);
    m_mapActionsToCommandBytes["K"] = make_pair(0x27,true);
    m_mapActionsToCommandBytes["L"] = make_pair(0x28,true);
    m_mapActionsToCommandBytes[":"] = make_pair(0x29,true);
    m_mapActionsToCommandBytes[";"] = make_pair(0x2A,true);
    m_mapActionsToCommandBytes["LEFT_ARROW"] = make_pair(0x2B,true);
    m_mapActionsToCommandBytes["="] = make_pair(0x2C,true);
    m_mapActionsToCommandBytes["LEFT_SHIFT"] = make_pair(0x2D,false);
    m_mapActionsToCommandBytes["CAPS_LOCK"] = make_pair(0x2E,true);
    m_mapActionsToCommandBytes["Z"] = make_pair(0x2F,true);
    m_mapActionsToCommandBytes["X"] = make_pair(0x30,true);
    m_mapActionsToCommandBytes["C"] = make_pair(0x31,true);
    m_mapActionsToCommandBytes["V"] = make_pair(0x32,true);
    m_mapActionsToCommandBytes["B"] = make_pair(0x33,true);
    m_mapActionsToCommandBytes["N"] = make_pair(0x34,true);
    m_mapActionsToCommandBytes["M"] = make_pair(0x35,true);
    m_mapActionsToCommandBytes[","] = make_pair(0x36,true);
    m_mapActionsToCommandBytes["."] = make_pair(0x37,true);
    m_mapActionsToCommandBytes["/"] = make_pair(0x38,true);
    m_mapActionsToCommandBytes["RIGHT_SHIFT"] = make_pair(0x39,false);
    m_mapActionsToCommandBytes["SPACE"] = make_pair(0x3A,true);
    m_mapActionsToCommandBytes["F1"] = make_pair(0x3B,true);
    m_mapActionsToCommandBytes["F2"] = make_pair(0x3C,true);
    m_mapActionsToCommandBytes["F3"] = make_pair(0x3D,true);
    m_mapActionsToCommandBytes["F4"] = make_pair(0x3E,true);
    m_mapActionsToCommandBytes["F5"] = make_pair(0x3F,true);
    m_mapActionsToCommandBytes["F6"] = make_pair(0x40,true);
    m_mapActionsToCommandBytes["F7"] = make_pair(0x41,true);
    m_mapActionsToCommandBytes["F8"] = make_pair(0x42,true);
    m_mapActionsToCommandBytes["CLR_HOME"] = make_pair(0x43,true);
    m_mapActionsToCommandBytes["UP"] = make_pair(0x44,true);
    m_mapActionsToCommandBytes["LEFT"] = make_pair(0x45,true);
    m_mapActionsToCommandBytes["RIGHT"] = make_pair(0x46,true);
    m_mapActionsToCommandBytes["DOWN"] = make_pair(0x47,true);
    m_mapActionsToCommandBytes["STERLING"] = make_pair(0x48,true);
    m_mapActionsToCommandBytes["UP_ARROW"] = make_pair(0x49,true);
    m_mapActionsToCommandBytes["RESTORE"] = make_pair(0x4A,true);
    m_mapActionsToCommandBytes["GET_SNAPSHOT"] = make_pair(0x4B,false);
    m_mapActionsToCommandBytes["SAVE_STATE"] = make_pair(0x4C,false);
    m_mapActionsToCommandBytes["LOAD_STATE"] = make_pair(0x4D,false);
    m_mapActionsToCommandBytes["GOTO_MENU"] = make_pair(0x4E,false);
    m_mapActionsToCommandBytes["LEAVE_MENU"] = make_pair(0x4F,false);
    m_mapActionsToCommandBytes["RESET"] = make_pair(0x50,false);
  }

  void VICEEmulatorModel::initializeButtontoKeysyms()
  {
    m_mapButtonsToActions[BUTTON_1_CONST] = "1";
    m_mapButtonsToActions[BUTTON_2_CONST] = "2";
    m_mapButtonsToActions[BUTTON_3_CONST] = "3";
    m_mapButtonsToActions[BUTTON_4_CONST] = "4";
    m_mapButtonsToActions[BUTTON_5_CONST] = "5";
    m_mapButtonsToActions[BUTTON_6_CONST] = "6";
    m_mapButtonsToActions[BUTTON_7_CONST] = "7";
    m_mapButtonsToActions[BUTTON_8_CONST] = "8";
    m_mapButtonsToActions[BUTTON_9_CONST] = "9";
    m_mapButtonsToActions[BUTTON_0_CONST] = "0";
    m_mapButtonsToActions[BUTTON_plus_CONST] = "+";
    m_mapButtonsToActions[BUTTON_dash_CONST] = "-";
    m_mapButtonsToActions[BUTTON_commoClrHome_CONST] = "CLR_HOME";
    m_mapButtonsToActions[BUTTON_commoInsDel_CONST] = "DELETE";
    m_mapButtonsToActions[BUTTON_Control_CONST] = "CTRL";
    m_mapButtonsToActions[BUTTON_q_CONST] = "Q";
    m_mapButtonsToActions[BUTTON_w_CONST] = "W";
    m_mapButtonsToActions[BUTTON_e_CONST] = "E";
    m_mapButtonsToActions[BUTTON_r_CONST] = "R";
    m_mapButtonsToActions[BUTTON_t_CONST] = "T";
    m_mapButtonsToActions[BUTTON_y_CONST] = "Y";
    m_mapButtonsToActions[BUTTON_u_CONST] = "U";
    m_mapButtonsToActions[BUTTON_i_CONST] = "I";
    m_mapButtonsToActions[BUTTON_o_CONST] = "O";
    m_mapButtonsToActions[BUTTON_p_CONST] = "P";
    m_mapButtonsToActions[BUTTON_at_CONST] = "@";
    m_mapButtonsToActions[BUTTON_asterisk_CONST] = "*";
    m_mapButtonsToActions[BUTTON_Enter_CONST] = "RETURN";
    m_mapButtonsToActions[BUTTON_commoKey_CONST] = "COMMODORE";
    m_mapButtonsToActions[BUTTON_a_CONST] = "A";
    m_mapButtonsToActions[BUTTON_s_CONST] = "S";
    m_mapButtonsToActions[BUTTON_d_CONST] = "D";
    m_mapButtonsToActions[BUTTON_f_CONST] = "F";
    m_mapButtonsToActions[BUTTON_g_CONST] = "G";
    m_mapButtonsToActions[BUTTON_h_CONST] = "H";
    m_mapButtonsToActions[BUTTON_j_CONST] = "J";
    m_mapButtonsToActions[BUTTON_k_CONST] = "K";
    m_mapButtonsToActions[BUTTON_l_CONST] = "L";
    m_mapButtonsToActions[BUTTON_colon_CONST] = ":";
    m_mapButtonsToActions[BUTTON_semicolumn_CONST] = ";";
    m_mapButtonsToActions[BUTTON_commoLeft_CONST] = "LEFT_ARROW";
    m_mapButtonsToActions[BUTTON_equals_sign_CONST] = "=";
    m_mapButtonsToActions[BUTTON_Shift_Left_CONST] = "LEFT_SHIFT";
    m_mapButtonsToActions[BUTTON_caps_lock_CONST] = "CAPS_LOCK";
    m_mapButtonsToActions[BUTTON_z_CONST] = "Z";
    m_mapButtonsToActions[BUTTON_x_CONST] = "X";
    m_mapButtonsToActions[BUTTON_c_CONST] = "C";
    m_mapButtonsToActions[BUTTON_v_CONST] = "V";
    m_mapButtonsToActions[BUTTON_b_CONST] = "B";
    m_mapButtonsToActions[BUTTON_n_CONST] = "N";
    m_mapButtonsToActions[BUTTON_m_CONST] = "M";
    m_mapButtonsToActions[BUTTON_comma_CONST] = ",";
    m_mapButtonsToActions[BUTTON_period_CONST] = ".";
    m_mapButtonsToActions[BUTTON_slash_CONST] = "/";
    m_mapButtonsToActions[BUTTON_Shift_Right_CONST] = "RIGHT_SHIFT";
    m_mapButtonsToActions[BUTTON_space_CONST] = "SPACE";
    m_mapButtonsToActions[BUTTON_F1_CONST] = "F1";
    m_mapButtonsToActions[BUTTON_F2_CONST] = "F2";
    m_mapButtonsToActions[BUTTON_F3_CONST] = "F3";
    m_mapButtonsToActions[BUTTON_F4_CONST] = "F4";
    m_mapButtonsToActions[BUTTON_F5_CONST] = "F5";
    m_mapButtonsToActions[BUTTON_F6_CONST] = "F6";
    m_mapButtonsToActions[BUTTON_F7_CONST] = "F7";
    m_mapButtonsToActions[BUTTON_F8_CONST] = "F8";
    m_mapButtonsToActions[BUTTON_commoUp_CONST] = "UP_ARROW";
    m_mapButtonsToActions[BUTTON_sterling_CONST] = "STERLING";
    m_mapButtonsToActions[BUTTON_Restore_CONST] = "RESTORE";
    m_mapButtonsToActions[BUTTON_Run_Stop_CONST] = "RUN_STOP";
  }

}
