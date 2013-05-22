/** 
 * Atari800EmulatorModel - Hold configuration state for emulations based on Atari800
 * (http://atari800.sf.net/)
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version: 1.0
 */

#include "Atari800EmulatorModel.h"
#include <X11/keysym.h>
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"

namespace DCE
{

  Atari800EmulatorModel::Atari800EmulatorModel()
    : X11EmulatorModel()
  {
    m_sWindowName = "atari800.atari800";
    initializeActionstoKeysyms();
  }

  Atari800EmulatorModel::~Atari800EmulatorModel()
  {
  }

  string Atari800EmulatorModel::getVideoAccelleration()
  {
    // come back here later and actually flesh this out.
    return "opengl";
  }

  /**
   * Update the /root/.mame/mame.ini file as needed.
   */
  bool Atari800EmulatorModel::updateConfig()
  {
    ConfigurationWriter config(ATARI800_CONFIG_FILE_TEMPLATE, ATARI800_CONFIG_FILE, m_sSystemConfiguration);
    return config.Write();
  }

  void Atari800EmulatorModel::initializeActionstoKeysyms()
  {
  
    // overridden by subclass.
    // todo: come back here and fill this in.

  }

}
