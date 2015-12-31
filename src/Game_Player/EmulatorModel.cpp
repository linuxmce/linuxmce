/** 
 * EmulatorModel - Creates a simple model that can be used 
 * to hold configuration and state information for the
 * emulator in question
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version: 1.0
 */

#include "EmulatorModel.h"

#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"

using namespace std;

namespace DCE
{
  
  EmulatorModel::EmulatorModel()
  {
    m_bRunning_set(false);
    m_bHasArgs=true;
    m_bIsPaused=false;
    m_sArgs="";
    m_sConfigFile="";
    m_sConfigFileTemplate="";
    m_sVideoAccelleration="";
    m_bChangeRequiresRestart=true;
    m_iSpeed=1000; // 1x speed when initialized.
    m_iActiveMenu=0;
    m_iStreamID=0;
    m_sMediaPosition="";
    m_bIsStreaming=false;
    m_bIsStreamingSource=false;
    m_tStreamingClientLaunchDelay=3000000; // 3 sec delay by default.
    m_bIsRecording=false;
    m_bCanSaveState=false; // assume that emulator can't save state unless overriden.
    m_iExit_Code=-1; // -1 = hasn't started yet.
    m_bEmitsMenuChanges=false; // assume emulator can't emit menu changes unless patched.
    m_iCurrentDisk=0;
  }

  EmulatorModel::~EmulatorModel()
  {
    m_bRunning_set(false);
    m_bIsPaused=false;
    m_sConfigFile.clear();
    m_sConfigFileTemplate.clear();
    m_mapMedia.clear();
    m_iActiveMenu=0;
    m_iStreamID=0;
    m_iSpeed=0;
    m_sMediaPosition="";
    m_bIsStreaming=false;
    m_bIsStreamingSource=false;
    m_sHostName="";
    m_bIsRecording=false;
    m_iExit_Code=-1;
    m_dequeDisks.clear();
    m_iCurrentDisk=0;
  }

  void EmulatorModel::coldReset()
  {
    m_bRunning_set(false);
    m_bIsPaused=false;
    m_iSpeed=1000; // 1x speed when initialized.
    m_iActiveMenu=0;
    m_iStreamID=0;
    m_bIsStreaming=false;
    m_bIsStreamingSource=false;
    m_bIsRecording=false;
  }

}
