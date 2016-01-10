/**
 * ConfigurationWriter - A dirt simple engine for dealing with
 * text based configuration files common to emulator engines.
 * offers a simple method to replace placeholder items with
 * model state.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#ifndef CONFIGURATIONWRITER_H
#define CONFIGURATIONWRITER_H

#include <vector>
#include <string>

using namespace std;

namespace DCE
{
  class ConfigurationWriter
  {
  private:
    string m_sSystemConfiguration;
    map<string, string> m_mapConfigurationItems;
    string m_sTargetFilename;
    string m_sTemplateFilename;
    bool m_bStopIfItemMissing;
    bool Process();
  public:
    ConfigurationWriter(string sTemplateFilename, string sTargetFilename, string sSystemConfiguration = "");
    ~ConfigurationWriter();
    void Add(string sKey, string sVal) { m_mapConfigurationItems[sKey] = sVal; };
    void Delete(string sKey) { m_mapConfigurationItems.erase(sKey); }
    bool Write();
    void StopIfItemMissing_set(bool bStopIfItemMissing);
  };
}

#endif
