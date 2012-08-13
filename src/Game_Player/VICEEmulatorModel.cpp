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

  void VICEEmulatorModel::updateTemplateVariables()
  {
    // implement this.
  }

  /**
   * Update VICE configuration file
   */
  bool VICEEmulatorModel::updateConfig()
  {
    return X11EmulatorModel::updateConfig();
  }

  void VICEEmulatorModel::initializeActionstoKeysyms()
  {
    // Overridden subtly by subclass.
    
  }

}
