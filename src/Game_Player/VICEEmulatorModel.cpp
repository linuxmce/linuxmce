/**
 * VICEEmulatorModel - Store information needed by VICE based emulators 
 *     (e.g. Commodore 64 and VIC-20)
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#include "VICEEmulatorModel.h"
#include <X11/keysym.h>
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"

namespace DCE
{
  VICEEmulatorModel::VICEEmulatorModel() : X11EmulatorModel()
  {
    m_sConfigFile = VICE_CONFIG_FILE;
    m_sConfigFileTemplate = VICE_CONFIG_FILE_TEMPLATE;
    m_sWindowName = ""; // Set by the subclass, as they are all different.
    initializeActionstoKeysyms();
    initializeButtontoKeysyms();
    m_sEmulatorBinary = "/usr/bin/"; // also set by the subclass.
    m_sProcessName = ""; // set by the subclass.
    m_bChangeRequiresRestart=true;
    m_bRunning=false;
    m_bHasArgs=true;
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
    m_mapActionsToKeysyms["0"] = make_pair(XK_0,0);
    m_mapActionsToKeysyms["1"] = make_pair(XK_1,0);
    m_mapActionsToKeysyms["2"] = make_pair(XK_2,0);
    m_mapActionsToKeysyms["3"] = make_pair(XK_3,0);
    m_mapActionsToKeysyms["4"] = make_pair(XK_4,0);
    m_mapActionsToKeysyms["5"] = make_pair(XK_5,0);
    m_mapActionsToKeysyms["6"] = make_pair(XK_6,0);
    m_mapActionsToKeysyms["7"] = make_pair(XK_7,0);
    m_mapActionsToKeysyms["8"] = make_pair(XK_8,0);
    m_mapActionsToKeysyms["9"] = make_pair(XK_9,0);
    m_mapActionsToKeysyms["LOAD_STATE"] = make_pair(XK_l,XK_Alt_L);
    m_mapActionsToKeysyms["SAVE_STATE"] = make_pair(XK_s,XK_Alt_L);
    m_mapActionsToKeysyms["PAUSE"] = make_pair(XK_p,XK_Alt_L);
    m_mapActionsToKeysyms["RESET"] = make_pair(XK_r,XK_Alt_L);
    m_mapActionsToKeysyms["UI_ENTER"] = make_pair(XK_F12,0);
    m_mapActionsToKeysyms["UI_EXIT"] = make_pair(XK_F12,0);
  }

  void VICEEmulatorModel::initializeButtontoKeysyms()
  {
  }

}
