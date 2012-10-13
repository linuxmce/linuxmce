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
  }

  bool EmulatorModel::updateConfig()
  {

    updateTemplateVariables();
    
    string sBuffer;
    
    if (m_sConfigFileTemplate.empty() && m_sConfigFile.empty())
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorModel::updateConfig() Both Config File Template and Destination Config File names were not specified, bailing.");
	return false;
      }

    if (m_sConfigFileTemplate.empty())
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorModel::updateConfig() Config file template not specified, bailing.");
	return false;
      }

    if (m_sConfigFile.empty())
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorModel::updateConfig() Config file destination not specified, bailing.");
	return false;
      }
    
    if (!FileUtils::ReadTextFile(m_sConfigFileTemplate, sBuffer))
      {
	// Could not read file.
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorModel::updateConfig() Could not read config template file %s",m_sConfigFileTemplate.c_str());
	return false;
      }
    else
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"EmulatorModel::updateConfig() read config file template %s successfully.",m_sConfigFileTemplate.c_str());
      }
    
    // Merge in System Configuration Items
    if (!m_sSystemConfiguration.empty())
      {
	vector<string> vectLines;
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"EmulatorModel::updateConfig() - System Configuration part of URL, parsing.");
	// Split into \n and then split each into values.
	StringUtils::Tokenize(m_sSystemConfiguration,"\n",vectLines);
	for (vector<string>::iterator it=vectLines.begin(); it!=vectLines.end(); ++it)
	  {
	    string sItem = *it;
	    string::size_type pos=0;
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"EmulatorModel::updateConfig() - Line %s",sItem.c_str());
	    string sParameter = StringUtils::Tokenize(sItem,"|",pos);
	    string sValue = StringUtils::Tokenize(sItem,"|",pos);
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"EmulatorModel::updateConfig() - sParameter %s sValue %s",
						sParameter.c_str(),
						sValue.c_str());
	    m_mapConfigTemplateItems[sParameter] = sValue;
	  }
      }

    // Substitute template variables, as needed.
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorModel::updateConfig() - %d config items found.",m_mapConfigTemplateItems.size());
    for (map<string, string>::iterator it=m_mapConfigTemplateItems.begin();
	 it != m_mapConfigTemplateItems.end();
	 ++it)
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorModel::updateConfig() Replacing %s, with %s",it->first.c_str(), it->second.c_str());
	sBuffer = StringUtils::Replace(sBuffer,it->first, it->second);
      }

    // If destination target directory does not exist, create it.
    if (!FileUtils::DirExists(FileUtils::BasePath(m_sConfigFile)))
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"EmulatorModel::updateConfig() Directory %s does not exist, creating it.",FileUtils::BasePath(m_sConfigFile).c_str());
	FileUtils::MakeDir(FileUtils::BasePath(m_sConfigFile));
      }

    // Finally, write out the file.

    if (!FileUtils::WriteTextFile(m_sConfigFile, sBuffer))
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"EmulatorModel::updateConfig() - Could not write text file %s",m_sConfigFile.c_str());
      }
    else
      {
	// Config file written successfully.
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"EmulatorModel::updateConfig() - Config file %s written successfully.",m_sConfigFile.c_str());
      }

    return true; 

  }

}
