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
    m_sArgs="";
    m_sConfigFile="";
    m_sConfigFileTemplate="";
    m_sVideoAccelleration="";
    m_bChangeRequiresRestart=true;
  }

  EmulatorModel::~EmulatorModel()
  {
    m_bRunning_set(false);
    m_sConfigFile.clear();
    m_sConfigFileTemplate.clear();
    m_mapMedia.clear();
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
    
    // Substitute template variables, as needed.

    for (map<string, string>::iterator it=m_mapConfigTemplateItems.begin();
	 it != m_mapConfigTemplateItems.end();
	 ++it)
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"EmulatorModel::updateConfig() Replacing %s, with %s",it->first.c_str(), it->second.c_str());
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
