/**
 * StellaEmulatorModel - Configuration Model for Stella (Atari 2600)
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version: 1.0
 */

#ifndef STELLAEMULATORMODEL_H
#define STELLAEMULATORMODEL_H

#include "INotifyEmulatorModel.h"
#include "EmulatorModel.h"

#define STELLA_CONFIG_FILE_TEMPLATE "/usr/pluto/templates/stellarc.template"
#define STELLA_CONFIG_FILE "/root/.stella/stellarc"

namespace DCE
{
  class StellaEmulatorModel : public INotifyEmulatorModel
  {
  private:
  public:
    StellaEmulatorModel();
    virtual ~StellaEmulatorModel();

    string m_sSystemName;   // Emulator System name
    string m_sState;
    string m_sRomPath;

  protected:

    virtual bool updateConfig();
    virtual string getVideoAccelleration();
    virtual string getResolution();
    
  };
}

#endif
