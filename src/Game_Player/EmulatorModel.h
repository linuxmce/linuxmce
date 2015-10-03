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
#include "ConfigurationWriter.h"
#include "DCE/Logger.h"

namespace DCE
{

  class EmulatorModel
  {

    friend class EmulatorController; // for updateConfig()
    friend class ConfigurationWriter; // also for updateConfig()

  private:
  protected:
    virtual bool updateConfig() = 0;
    virtual string getVideoAccelleration() = 0;
  public:
    string m_sConfigFileTemplate;
    string m_sConfigFile;
    string m_sVideoAccelleration;
    string m_sEmulatorBinary;
    string m_sProcessName;
    string m_sHostName;
    string m_sMediaPosition; // Used for Play Media commands to have a preloaded bookmark
    int m_iStreamID;
    int m_iSpeed;
    useconds_t m_tStreamingClientLaunchDelay; // usleep time for # of seconds before launching.
    bool m_bChangeRequiresRestart;
    bool m_bRunning;
    bool m_bIsPaused;
    bool m_bHasArgs;
    bool m_bQuitting;
    bool m_bIsStreaming; // Are we streaming game to multiple media directors?
    bool m_bIsStreamingSource; // Are we the master machine?
    bool m_bCanSaveState; // Can this emulation save state?
    string m_sArgs;
    bool m_bRunning_get() { return m_bRunning; }
    void m_bRunning_set(bool bRunning) { m_bRunning = bRunning; }
    bool m_bQuitting_get() { return m_bQuitting; }
    void m_bQuitting_set(bool bQuitting) { m_bQuitting = bQuitting; }
    int m_iActiveMenu;
    bool m_bIsRecording;
    string m_sRecordingFilename;
    long int m_dwPK_Device_Orbiter;
    string m_sSystemConfiguration;
    int m_iExit_Code;
    bool m_bEmitsMenuChanges;
    bool emitsMenuChanges() { return m_bEmitsMenuChanges; }
    bool emulatorHasCrashed() { return (m_iExit_Code > 0); }

    map<string, string> m_mapMedia;
    string m_mapMedia_Find(string sSlot) 
    {
      map<string, string>::iterator it = m_mapMedia.find(sSlot);
      return it == m_mapMedia.end() ? "" : (*it).second;
    }
    bool m_mapMedia_Exists(string sSlot)
    {
      map<string, string>::iterator it = m_mapMedia.find(sSlot);
      return it != m_mapMedia.end();
    }

    map<string,string> m_mapOptions;
    string m_mapOptions_Find(string sPath) 
    {
      map<string, string>::iterator it = m_mapOptions.find(sPath);
      return it == m_mapOptions.end() ? "" : (*it).second;
    }    

    EmulatorModel();
    virtual ~EmulatorModel();

    void coldReset(); // Wipe everything before a run();

  };

}

#endif
