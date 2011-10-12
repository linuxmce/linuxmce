/**
 * EmulatorEngine.cpp - The emulator engine.
 * This is a base class, should be subclassed for each emulator type.
 *
 * Author: Thom Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "EmulatorEngine.h"

namespace DCE
{
  
  /** 
   * Ctor
   */
  EmulatorEngine::EmulatorEngine()
  {
    m_bIsRunning = false;
    m_bUseOpenGL = false;
    m_sMediaFileName = "";
    m_sEnginePath = "";
  }
  
  /** 
   * dtor
   */
  EmulatorEngine::~EmulatorEngine()
  {
  }
  
}
