/** 
 * EmulatorModel - Creates a simple model that can be used 
 * to hold configuration and state information for the
 * emulator in question
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version: 1.0
 */

#ifndef EMULATORMODEL_H
#define EMULATORMODEL_H

namespace DCE
{

  class EmulatorModel
  {
  private:
    bool m_bRunning;
  protected:
    virtual bool updateConfig() = 0;
    virtual bool setup() = 0;
  public:
    bool m_bRunning_get() { return m_bRunning; }
    void m_bRunning_set(bool bRunning) { m_bRunning = bRunning; }
    EmulatorModel();
    ~EmulatorModel();
  };

}

#endif
