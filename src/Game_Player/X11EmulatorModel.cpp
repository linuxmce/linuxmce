/** 
 * X11EmulatorModel - Creates a simple model that can be used 
 * to hold configuration and state information for the
 * an emulator controlled via X11 keys and events
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version: 1.0
 */

#include "X11EmulatorModel.h"
#include <X11/Xlib.h>
#include "pluto_main/Define_Button.h"
#include <X11/keysym.h>

namespace DCE 
{
  X11EmulatorModel::X11EmulatorModel()
    : EmulatorModel()
  {
    m_pDisplay = NULL;
    m_iWindowId = 0;
    m_iEventSerialNum = 0;
    m_iCurrentKeyModifier=0;
    m_iScreenWidth=m_iScreenHeight=m_iRefreshRate=0;
    initializeButtontoKeysyms();
  }

  X11EmulatorModel::~X11EmulatorModel()
  {
    m_pDisplay = NULL;
    m_iWindowId = 0;
  }

  bool X11EmulatorModel::updateConfig()
  {
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"X11EmulatorModel::updateConfig - do not call this method. It is abstract.");
    return false;
  }

  /**
   * Update a VideoFrameGeometry for a particular PK_Device
   * Used primarily for Get Video Frame, and Simulate Mouse Click
   *
   * Create a VideoFrameGeometry for a PK_Device if it does not exist,
   * update its width and height if it does.
   */
  void X11EmulatorModel::updateVideoFrameGeometry(long int iPK_Device, int iWidth, int iHeight)
  {
    if (!m_mapVideoFrameGeometry_Exists(iPK_Device))
      {
	// make a new videoframegeometry for a device.
	m_mapVideoFrameGeometry[iPK_Device] = new VideoFrameGeometry(iWidth, iHeight);
      }
    else
      {
	// update existing videoframegeometry
	map<long int, VideoFrameGeometry *>::iterator it = m_mapVideoFrameGeometry.find(iPK_Device);
	(*it).second->m_iWidth=iWidth;
	(*it).second->m_iHeight=iHeight;
      }
  }

  /**
   * Returns a pair of width and height for a video frame geometry.
   */
  pair<int, int> X11EmulatorModel::getVideoFrameGeometry(long int iPK_Device)
  {
    if (!m_mapVideoFrameGeometry_Exists(iPK_Device))
      {
	return make_pair(-1,-1);
      }

    VideoFrameGeometry* vfg = m_mapVideoFrameGeometry_Find(iPK_Device);
    int iWidth = vfg->m_iWidth;
    int iHeight = vfg->m_iHeight;
    return make_pair(iWidth, iHeight);
  }

  /** 
   * Initialize the map of PK_Buttons to XKeySyms
   *
   * This is ultimately used by functions requiring a PK_Button
   * value, such as CMD_Simulate_Keypress()
   *
   * This can be overriden by subclasses to subtly change mappings
   * as needed.
   */
  void X11EmulatorModel::initializeButtontoKeysyms()
  {
    // Common keys, this should take care of most keys shared between all of the
    // computer emulations.
    m_mapButtonToKeysyms[BUTTON_Up_Arrow_CONST] = XK_Up;
    m_mapButtonToKeysyms[BUTTON_Down_Arrow_CONST] = XK_Down;
    m_mapButtonToKeysyms[BUTTON_Left_Arrow_CONST] = XK_Left;
    m_mapButtonToKeysyms[BUTTON_Right_Arrow_CONST] = XK_Right;
    m_mapButtonToKeysyms[BUTTON_Enter_CONST] = XK_Return;
    m_mapButtonToKeysyms[BUTTON_F1_CONST] = XK_F1;
    m_mapButtonToKeysyms[BUTTON_F2_CONST] = XK_F2;
    m_mapButtonToKeysyms[BUTTON_F3_CONST] = XK_F3;
    m_mapButtonToKeysyms[BUTTON_F4_CONST] = XK_F4;
    m_mapButtonToKeysyms[BUTTON_F5_CONST] = XK_F5;
    m_mapButtonToKeysyms[BUTTON_F6_CONST] = XK_F6;
    m_mapButtonToKeysyms[BUTTON_F7_CONST] = XK_F7;
    m_mapButtonToKeysyms[BUTTON_F8_CONST] = XK_F8;
    m_mapButtonToKeysyms[BUTTON_F9_CONST] = XK_F9;
    m_mapButtonToKeysyms[BUTTON_F10_CONST] = XK_F10;
    m_mapButtonToKeysyms[BUTTON_1_CONST] = XK_1;
    m_mapButtonToKeysyms[BUTTON_2_CONST] = XK_2;
    m_mapButtonToKeysyms[BUTTON_3_CONST] = XK_3;
    m_mapButtonToKeysyms[BUTTON_4_CONST] = XK_4;
    m_mapButtonToKeysyms[BUTTON_5_CONST] = XK_5;
    m_mapButtonToKeysyms[BUTTON_6_CONST] = XK_6;
    m_mapButtonToKeysyms[BUTTON_7_CONST] = XK_7;
    m_mapButtonToKeysyms[BUTTON_8_CONST] = XK_8;
    m_mapButtonToKeysyms[BUTTON_9_CONST] = XK_9;
    m_mapButtonToKeysyms[BUTTON_0_CONST] = XK_0;
    m_mapButtonToKeysyms[BUTTON_Asterisk_CONST] = XK_asterisk;
    m_mapButtonToKeysyms[BUTTON_Pound_CONST] = XK_numbersign;
    m_mapButtonToKeysyms[BUTTON_a_CONST] = XK_a;
    m_mapButtonToKeysyms[BUTTON_b_CONST] = XK_b;
    m_mapButtonToKeysyms[BUTTON_c_CONST] = XK_c;
    m_mapButtonToKeysyms[BUTTON_d_CONST] = XK_d;
    m_mapButtonToKeysyms[BUTTON_e_CONST] = XK_e;
    m_mapButtonToKeysyms[BUTTON_f_CONST] = XK_f;
    m_mapButtonToKeysyms[BUTTON_g_CONST] = XK_g;
    m_mapButtonToKeysyms[BUTTON_h_CONST] = XK_h;
    m_mapButtonToKeysyms[BUTTON_i_CONST] = XK_i;
    m_mapButtonToKeysyms[BUTTON_j_CONST] = XK_j;
    m_mapButtonToKeysyms[BUTTON_k_CONST] = XK_k;
    m_mapButtonToKeysyms[BUTTON_l_CONST] = XK_l;
    m_mapButtonToKeysyms[BUTTON_m_CONST] = XK_m;
    m_mapButtonToKeysyms[BUTTON_n_CONST] = XK_n;
    m_mapButtonToKeysyms[BUTTON_o_CONST] = XK_o;
    m_mapButtonToKeysyms[BUTTON_p_CONST] = XK_p;
    m_mapButtonToKeysyms[BUTTON_q_CONST] = XK_q;
    m_mapButtonToKeysyms[BUTTON_r_CONST] = XK_r;
    m_mapButtonToKeysyms[BUTTON_s_CONST] = XK_s;
    m_mapButtonToKeysyms[BUTTON_t_CONST] = XK_t;
    m_mapButtonToKeysyms[BUTTON_u_CONST] = XK_u;
    m_mapButtonToKeysyms[BUTTON_v_CONST] = XK_v;
    m_mapButtonToKeysyms[BUTTON_w_CONST] = XK_w;
    m_mapButtonToKeysyms[BUTTON_x_CONST] = XK_x;
    m_mapButtonToKeysyms[BUTTON_y_CONST] = XK_y;
    m_mapButtonToKeysyms[BUTTON_z_CONST] = XK_z;
    m_mapButtonToKeysyms[BUTTON_A_CONST] = XK_A;
    m_mapButtonToKeysyms[BUTTON_B_CONST] = XK_B;
    m_mapButtonToKeysyms[BUTTON_C_CONST] = XK_C;
    m_mapButtonToKeysyms[BUTTON_D_CONST] = XK_D;
    m_mapButtonToKeysyms[BUTTON_E_CONST] = XK_E;
    m_mapButtonToKeysyms[BUTTON_F_CONST] = XK_F;
    m_mapButtonToKeysyms[BUTTON_G_CONST] = XK_G;
    m_mapButtonToKeysyms[BUTTON_H_CONST] = XK_H;
    m_mapButtonToKeysyms[BUTTON_I_CONST] = XK_I;
    m_mapButtonToKeysyms[BUTTON_J_CONST] = XK_J;
    m_mapButtonToKeysyms[BUTTON_K_CONST] = XK_K;
    m_mapButtonToKeysyms[BUTTON_L_CONST] = XK_L;
    m_mapButtonToKeysyms[BUTTON_M_CONST] = XK_M;
    m_mapButtonToKeysyms[BUTTON_N_CONST] = XK_N;
    m_mapButtonToKeysyms[BUTTON_O_CONST] = XK_O;
    m_mapButtonToKeysyms[BUTTON_P_CONST] = XK_P;
    m_mapButtonToKeysyms[BUTTON_Q_CONST] = XK_Q;
    m_mapButtonToKeysyms[BUTTON_R_CONST] = XK_R;
    m_mapButtonToKeysyms[BUTTON_S_CONST] = XK_S;
    m_mapButtonToKeysyms[BUTTON_T_CONST] = XK_T;
    m_mapButtonToKeysyms[BUTTON_U_CONST] = XK_U;
    m_mapButtonToKeysyms[BUTTON_V_CONST] = XK_V;
    m_mapButtonToKeysyms[BUTTON_W_CONST] = XK_W;
    m_mapButtonToKeysyms[BUTTON_X_CONST] = XK_X;
    m_mapButtonToKeysyms[BUTTON_Y_CONST] = XK_Y;
    m_mapButtonToKeysyms[BUTTON_Z_CONST] = XK_Z;
    m_mapButtonToKeysyms[BUTTON_space_CONST] = XK_space;
    m_mapButtonToKeysyms[BUTTON_comma_CONST] = XK_comma;
    m_mapButtonToKeysyms[BUTTON_period_CONST] = XK_period;
    m_mapButtonToKeysyms[BUTTON_slash_CONST] = XK_slash;
    m_mapButtonToKeysyms[BUTTON_dash_CONST] = XK_minus;
    m_mapButtonToKeysyms[BUTTON_plus_CONST] = XK_plus;
    m_mapButtonToKeysyms[BUTTON_semicolumn_CONST] = XK_semicolon;
    m_mapButtonToKeysyms[BUTTON_single_quote_CONST] = XK_apostrophe;
    m_mapButtonToKeysyms[BUTTON_dot_CONST] = XK_period;
    m_mapButtonToKeysyms[BUTTON_escape_CONST] = XK_Escape;
    m_mapButtonToKeysyms[BUTTON_tab_CONST] = XK_Tab;
    m_mapButtonToKeysyms[BUTTON_exclamation_point_CONST] = XK_exclam;
    m_mapButtonToKeysyms[BUTTON_at_sign_CONST] = XK_at;
    m_mapButtonToKeysyms[BUTTON_dollar_CONST] = XK_dollar;
    //m_mapButtonToKeysyms[BUTTON_caret_CONST] = XK_caret;
    m_mapButtonToKeysyms[BUTTON_ampersand_CONST] = XK_ampersand;
    m_mapButtonToKeysyms[BUTTON_left_parenthesis_CONST] = XK_parenleft;
    m_mapButtonToKeysyms[BUTTON_right_parenthesis_CONST] = XK_parenright;
    m_mapButtonToKeysyms[BUTTON_underscore_CONST] = XK_underscore;
    m_mapButtonToKeysyms[BUTTON_equals_sign_CONST] = XK_equal;
    m_mapButtonToKeysyms[BUTTON_colon_CONST] = XK_colon;
    m_mapButtonToKeysyms[BUTTON_double_quote_CONST] = XK_quotedbl;
    m_mapButtonToKeysyms[BUTTON_percent_CONST] = XK_percent;
    m_mapButtonToKeysyms[BUTTON_Backslash_CONST] = XK_backslash;
    m_mapButtonToKeysyms[BUTTON_backtick_CONST] = XK_grave;
  }
 
}
