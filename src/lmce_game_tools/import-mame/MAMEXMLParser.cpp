/**
 * MAMEXMLParser - A pugixml based parser to extract relevant data
 * from mame's -listxml output.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Start Date: 01/25/2016
 */

#include <iostream>

#include "MAMEXMLParser.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "DCE/Logger.h"

MAMERom::MAMERom(string sRomTitle,
		 string sTitleHash,
		 string sRomSubtitle,
		 string sRomManufacturer,
		 string sRomYear,
		 string sRomStatus,
		 string sRomCloneOf)
{
  m_sRomTitle=sRomTitle;
  m_sTitleHash=sTitleHash;
  m_sRomSubtitle=sRomSubtitle;
  m_sRomManufacturer=sRomManufacturer;
  m_sRomYear=sRomYear;
  m_sRomStatus=sRomStatus;
  m_sRomCloneOf=sRomCloneOf;
}

MAMERom::~MAMERom()
{
}

MAMEXMLParser::MAMEXMLParser(string sMAMEPath)
{
  m_sMAMEPath = sMAMEPath;
}

MAMEXMLParser::~MAMEXMLParser()
{

}

bool MAMEXMLParser::mameOutputIsSane()
{
  string sSanityOutput;
  // Sanity checks.
  if (!FileUtils::FileExists("/tmp/mame.tmp"))
    {
      cout << "MAME output at path: /tmp/mame.tmp does not exist!" << endl;
      return false;
    }

  if (FileUtils::FileSize("/tmp/mame.tmp") < 16384)
    {
      cout << "MAME output at path: /tmp/mame.tmp is too small!" << endl << endl;
      cout << "Output:" << endl;
      cout << "-------" << endl;
      FileUtils::ReadTextFile("/tmp/mame.tmp",sSanityOutput);
      cout << sSanityOutput << endl;
      return false;
    }
  return true;
}

bool MAMEXMLParser::getMAMEOutput()
{
  string sMameOutput;
  string sErr;
  const char *args[] ={m_sMAMEPath.c_str(),"-listxml",NULL};
  cout << "Grabbing XML output from MAME, please wait..." << flush;
  
  if (!FileUtils::FileExists(m_sMAMEPath))
    {
      cout << "MAME at path: " << m_sMAMEPath << " not found!" << endl;
      return false;
    }
  
  ProcessUtils::GetCommandOutput(args[0],args,sMameOutput,sErr);
  FileUtils::WriteTextFile("/tmp/mame.tmp",sMameOutput);

  if (!mameOutputIsSane())
    return false;

  cout << "Done!";
  cout << endl;
  return true;
}

bool MAMEXMLParser::parseMAMEOutput()
{
  pugi::xml_document doc;
  pugi::xml_parse_result result; 
  int i=0;

  if (!mameOutputIsSane())
    return false;

  cout << "Parsing MAME output. Please wait..." << flush;
  result = doc.load_file("/tmp/mame.tmp"); 

  if (!result)
    {
      cout << "Could not parse MAME output. Error description: " << result.description() << " at offset: " << result.offset << endl;
      return false;
    }

  pugi::xpath_node_set machines = doc.select_nodes("/mame/machine[not(@runnable)]");

  for (pugi::xpath_node_set::const_iterator it = machines.begin(); it != machines.end(); ++it)
    {
      pugi::xpath_node node = *it;
      cout << node.node().attribute("name").value() << "\n";
      i++;
    }
  
  cout << i << endl;

  return true;
}

bool MAMEXMLParser::run()
{
  if (!getMAMEOutput())
    return false;

  if (!parseMAMEOutput())
    return false;

  return true;

}
