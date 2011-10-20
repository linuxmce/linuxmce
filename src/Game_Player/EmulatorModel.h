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

using namespace std;

#include <string>
#include <map>

namespace DCE
{

  class EmulatorModel
  {
  private:
  protected:
    virtual bool updateConfig();
    virtual void updateTemplateVariables() = 0;
  public:
    string m_sConfigFileTemplate;
    string m_sConfigFile;
    string m_sRomPath;
    string m_sVideoAccelleration;
    bool m_bRunning;
    bool m_bRunning_get() { return m_bRunning; }
    void m_bRunning_set(bool bRunning) { m_bRunning = bRunning; }
    map<string, string> m_mapConfigTemplateItems;

    EmulatorModel();
    ~EmulatorModel();
  };

}

#endif
