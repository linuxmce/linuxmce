/**
 * EmulatorFactory - given a media type, instantiate the needed controller
 * and model.
 * 
 * return the controller object.
 *
 */

#ifndef EMULATORFACTORY_H
#define EMULATORFACTORY_H

#include "EmulatorController.h"
#include "Game_Player.h"

namespace DCE
{
  class Game_Player;
  class EmulatorController;

  class EmulatorFactory
  {
  public:
 
    Game_Player *m_pGame_Player;
    
    EmulatorFactory(Game_Player *pGamePlayer);
    ~EmulatorFactory();

    bool init();
    void addEmulatorForMediaType(int iPK_MediaType, EmulatorController *pEmulatorController);
    EmulatorController* getEmulatorForMediaType(int iPK_MediaType);
  private:
    map<int, EmulatorController *> m_mapEmulatorControllerForMediaType;
    bool createEmulatorForMediaType(int iPK_MediaType);
  };
}

#endif
