/**
 * EmulatorEngine.h - The Emulator Engine
 * 
 * Author: Thom Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */
#ifndef EMULATOR_ENGINE_H
#define EMULATOR_ENGINE_H

#include "DCE/Logger.h"

namespace DCE
{

  class EmulatorEngine
  {
  public:
    string m_sEnginePath;                                // path to the emulator engine. /usr/local/bin/mame
    string m_sMediaFileName;                             // path to media to place in system.
    string m_sWindowName;                                // Window name in WM_CLASS.WM_NAME format.
    long unsigned int m_iWindowID;                       // X11 Window ID.
    bool m_bIsRunning;                                   // is emulator running?
    int m_iPK_MediaType;                                 // PK_MediaType of the emulator engine.
  protected:
    bool m_bUseOpenGL;                                   // Whether OpenGL should be used
  private:

  public:
    EmulatorEngine();                                    // ctor
    ~EmulatorEngine();                                   // dtor
    virtual bool setMedia(string sMediaFilePath) = 0;    // File path to the media. 
    virtual bool setup() = 0;                            // set up all the key mapping mojo
    virtual bool updateConfig() = 0;                     // a helper to update the config for emulator.
    virtual bool doPlayerStart(int iPlayer) = 0;         // Do a player start.
    bool isRunning() { return m_bIsRunning; }            // accessor.
    void setRunning(bool bIsRunning) { m_bIsRunning = bIsRunning; }
  private:
  };

}
#endif /* EMULATOR_ENGINE_H */
