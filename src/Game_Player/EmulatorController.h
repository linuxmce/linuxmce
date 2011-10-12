/** 
 * EmulatorController - a base class for controlling an emulator
 *
 * @author Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef EMULATORCONTROLLER_H
#define EMULATORCONTROLLER_H

#include <string>

#include "EmulatorEngine.h"
#include "Game_Player.h"

using namespace std;

namespace DCE
{
  class EmulatorController
  {
  public:
    virtual bool setup() = 0;                      // setup routine
    virtual bool start() = 0;                      // start routine
    virtual bool stop() = 0;                       // stop routine
    virtual bool getVideoFrame(char *pData, int iData_Size) = 0; // return an emulator frame
    virtual bool gotoMediaMenu(int menuType) = 0;  // Go to media menu
    virtual bool pause() = 0;                      // pause the game
    virtual bool play() = 0;                       // return from pause
    virtual bool pressEnterGo() = 0;               // press enter/go
    virtual bool pressMoveUp() = 0;                // press move/up
    virtual bool pressMoveDown() = 0;              // press move/down
    virtual bool pressMoveLeft() = 0;              // press move/left
    virtual bool pressMoveRight() = 0;             // press move/right
    virtual bool press0() = 0;                     // press 0 key
    virtual bool press1() = 0;                     // press 1 key
    virtual bool press2() = 0;                     // press 2 key
    virtual bool press3() = 0;                     // press 3 key
    virtual bool press4() = 0;                     // press 4 key
    virtual bool press5() = 0;                     // press 5 key
    virtual bool press6() = 0;                     // press 6 key
    virtual bool press7() = 0;                     // press 7 key
    virtual bool press8() = 0;                     // press 8 key
    virtual bool press9() = 0;                     // press 9 key
    virtual bool saveState(string &sMediaPosition) = 0;                  // save state (for bookmarks, etc)
    virtual bool loadState(string &sMediaPosition) = 0;                  // load state

    EmulatorController(Game_Player *pGame_Player); // ctor
    ~EmulatorController();                         // dtor
  protected:
    EmulatorEngine *m_pEngine;                     // Emulator model.
    Game_Player *m_pGame_Player;                   // Game Player pointer.
    void setEngine(EmulatorEngine *pEngine) { m_pEngine = pEngine; }     // set engine accessor
    
  };
}
#endif /* EmulatorController.h */
