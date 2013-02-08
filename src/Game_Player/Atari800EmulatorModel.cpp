/** 
 * X11EmulatorModel - Creates a simple model that can be used 
 * to hold configuration and state information for the
 * an emulator controlled via X11 keys and events
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
    m_sConfigFile = ATARI800_CONFIG_FILE;
    m_sConfigFileTemplate = ATARI800_CONFIG_FILE_TEMPLATE;
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

  void Atari800EmulatorModel::updateTemplateVariables()
  {
    
    //    m_mapConfigTemplateItems["###ROMPATH###"] = FileUtils::BasePath(m_mapMedia_Find("default"));
    //m_mapConfigTemplateItems["###VIDEO###"] = getVideoAccelleration();
    //m_mapConfigTemplateItems["###AVIWRITE###"] = (m_bIsRecording ? "recorded.avi" : "");
    return;

  }

  /**
   * Update the /root/.mame/mame.ini file as needed.
   */
  bool Atari800EmulatorModel::updateConfig()
  {
    return X11EmulatorModel::updateConfig();
  }

  void Atari800EmulatorModel::initializeActionstoKeysyms()
  {
  
    // overridden by subclass.
    // todo: come back here and fill this in.

  }

}
