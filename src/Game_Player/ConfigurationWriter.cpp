/**
 * ConfigurationWriter - A dirt simple engine for dealing with
 * text based configuration files common to emulator engines.
 * offers a simple method to replace placeholder items with
 * model state.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 */

#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "ConfigurationWriter.h"

namespace DCE
{
  ConfigurationWriter::ConfigurationWriter(string sTemplateFilename, string sTargetFilename, string sSystemConfiguration)
  {
    m_sTargetFilename = sTargetFilename;
    m_sTemplateFilename = sTemplateFilename;
    m_sSystemConfiguration = sSystemConfiguration;
    m_bStopIfItemMissing=false;
    m_mapConfigurationItems.clear();
  }
  
  ConfigurationWriter::~ConfigurationWriter()
  {
    m_sTargetFilename="";
    m_sTemplateFilename="";
    m_mapConfigurationItems.clear();
  }

  bool ConfigurationWriter::Process()
  {
    string sBuffer;
    if (m_sTemplateFilename.empty() && m_sTargetFilename.empty())
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ConfigurationWriter::process() Both Config File Template and Destination Config File names were not specified, bailing.");
	return false;
      }
    
    if (m_sTemplateFilename.empty())
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ConfigurationWriter::process() Config file template not specified, bailing.");
	return false;
      }

    if (m_sTargetFilename.empty())
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ConfigurationWriter::process() Config file destination not specified, bailing.");
	return false;
      }
    
    if (!FileUtils::ReadTextFile(m_sTemplateFilename, sBuffer))
      {
	// Could not read file.
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ConfigurationWriter::process() Could not read config template file %s",m_sTemplateFilename.c_str());
	return false;
      }
    else
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ConfigurationWriter::process() read config file template %s successfully.",m_sTemplateFilename.c_str());
      }

    // Merge in System Configuration Items
    if (!m_sSystemConfiguration.empty())
      {
	vector<string> vectLines;
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ConfigurationWriter::process() - System Configuration part of URL, parsing.");
	// Split into \n and then split each into values.
	StringUtils::Tokenize(m_sSystemConfiguration,"\n",vectLines);
	for (vector<string>::iterator it=vectLines.begin(); it!=vectLines.end(); ++it)
	  {
	    string sItem = *it;
	    string::size_type pos=0;
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"ConfigurationWriter::process() - Line %s",sItem.c_str());
	    string sParameter = StringUtils::Tokenize(sItem,"|",pos);
	    string sValue = StringUtils::Tokenize(sItem,"|",pos);
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"ConfigurationWriter::process() - sParameter %s sValue %s",
						sParameter.c_str(),
						sValue.c_str());
	    m_mapConfigurationItems[sParameter] = sValue;
	  }
      }

    // Substitute template variables, as needed.
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"ConfigurationWriter::process() - template file %s - %d config items found.",m_sTemplateFilename.c_str(),m_mapConfigurationItems.size());
    for (map<string, string>::iterator it=m_mapConfigurationItems.begin();
	 it != m_mapConfigurationItems.end();
	 ++it)
      {
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ConfigurationWriter::process() Replacing %s, with %s",it->first.c_str(), it->second.c_str());
	sBuffer = StringUtils::Replace(sBuffer,it->first, it->second);
      }

    // Finally, write out the file.
    if (!FileUtils::DirExists(FileUtils::BasePath(m_sTargetFilename)))
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ConfigurationWriter::process() - Configuration File Path %s does not exist, creating.",FileUtils::BasePath(m_sTargetFilename).c_str());
	string sCmd = "mkdir -p "+FileUtils::BasePath(m_sTargetFilename);
	system(sCmd.c_str());
      }
   
    if (!FileUtils::WriteTextFile(m_sTargetFilename, sBuffer))
      {
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ConfigurationWriter::process() - Could not write text file %s",m_sTargetFilename.c_str());
      }
    else
      {
	// Config file written successfully.
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"ConfigurationWriter::process() - Config file %s written successfully.",m_sTargetFilename.c_str());
      }

    return true; 

  }

  bool ConfigurationWriter::Write()
  {
    LoggerWrapper::GetInstance()->Write(LV_STATUS,"Attempting to write configuration file %s from template %s",
					m_sTargetFilename.c_str(),
					m_sTemplateFilename.c_str());
    return Process();
  }
  
  void ConfigurationWriter::StopIfItemMissing_set(bool bStopIfItemMissing)
  {
    m_bStopIfItemMissing=bStopIfItemMissing;
  }

}


