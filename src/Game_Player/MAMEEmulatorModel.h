/** 
 * MAMEEmulatorModel - Store information needed by the MAME emulation
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version: 1.0
 */

#ifndef MAMEEMULATORMODEL_H
#define MAMEEMULATORMODEL_H

#include "X11EmulatorModel.h"
#include "EmulatorModel.h"

#define MAME_CONFIG_FILE_TEMPLATE "/usr/pluto/templates/mame.ini.template"
#define MAME_CONFIG_FILE "/root/.mame/mame.ini"

namespace DCE
{
  class MAMEEmulatorModel : public X11EmulatorModel
  {
  private:
  public:
    MAMEEmulatorModel();
    ~MAMEEmulatorModel();

    string m_sState; 

  protected:

    virtual void initializeActionstoKeysyms(); // set key mappings for MAME.
    virtual bool updateConfig();
    virtual string getVideoAccelleration();
    virtual string getResolution();

  };
}

#endif
