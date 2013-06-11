/**
 * lmce_game_update_mame - Update the lmce_game database with the output from a running sdlmame.
 * 
 * This will skip any existing ROM entries, only putting in ROM entries that do not already
 * exist.
 *
 * Author:     Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version:    2.0
 *
 */

// PlutoUtils Defines ///////////////
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

// DCE Defines //////////////////////
#include "DCE/Logger.h"

// Other Defines
#include <string>
#include <vector>
#include <map>
#include "lmce_game_update_mame.h"

LMCE_Game_Update_MAME::LMCE_Game_Update_MAME(string sMamePath, string sCategoryPath)
{
  m_sMamePath=sMamePath;
  m_sCategoryPath=sCategoryPath;
  m_pMAMECategory=new MAMECategory(sCategoryPath);
}

LMCE_Game_Update_MAME::~LMCE_Game_Update_MAME()
{
  delete m_pMAMECategory;
}

bool LMCE_Game_Update_MAME::GetMAMEOutput(string &sMAMEOutput)
{
  return true;
}

int LMCE_Game_Update_MAME::Run()
{
  if (!m_pMAMECategory->Parse())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not parse category file. See above error messages.");
      return 1;
    }

  string sMameOutput;
  string sErr;

  const char *args[]={m_sMamePath.c_str(),"-listxml",NULL};
  ProcessUtils::GetCommandOutput(args[0],args,sMameOutput,sErr);
  FileUtils::WriteTextFile("/tmp/mame.tmp",sMameOutput);

  MAMEParser p;

  try
    {
      p.set_substitute_entities(true);
      p.parse_file("/tmp/mame.tmp");
    }
  catch (const xmlpp::exception& ex)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Parsing error: %s",ex.what());
      return false;
    }

  FileUtils::DelFile("/tmp/mame.tmp");

  for (map<string, MAMERom *>::iterator it=p.m_mapRomToMAMERom.begin();
       it != p.m_mapRomToMAMERom.end();
       ++it)
    {
      string sRomEntry = it->first;
      MAMERom *pCurrentRom = it->second;
      ProcessEntry(sRomEntry, pCurrentRom);
    }

  return 0;
}

void LMCE_Game_Update::ProcessEntry(string sRomName, MAMERom* pCurrentRom)
{
  // Come back here and implement the sql2cpp calls
  // after the schema changes have been made.
}

int main(int argc, char* argv[])
{

  bool bError = true;  // Set when there was an error parsing command line.
  string sMamePath;    // Path to MAME binary.
  string sCategoryPath; // Path to Category.ini file.
  char c;              // Current Option.
  int iRetCode;        // Return code

  cout << "lmce_game_update, v. 2.0" << endl;

  for(int optnum=1;optnum<argc;++optnum)
    {
      if ( argv[optnum][0]!='-' )
	{
	  cerr << "Unknown option: " << argv[optnum] << endl;
	  bError=true;
	}

      c=argv[optnum][1];
      switch (c)
	{
	case 'p':
	  if (argc > 4)
	    {
	      sMamePath = argv[++optnum];
	      bError=false;
	    }
	  break;
	case 'c':
	  if (argc > 4)
	    {
	      sCategoryPath = argv[++optnum];
	      bError=false;
	    }
	}
    }

  if (bError) 
    {
      cout << "Imports new MAME ROMs into lmce_game for inclusion via sqlCVS." << endl
	   << endl 
	   << "Usage: lmce_game_update -p /path/to/mame" << endl
	   << endl
	   << "\t -p -- The Path to the MAME binary. (Required)" << endl
	   << "\t -c -- The Path to the Category.ini file." << endl;
      exit(1);
    }

  LMCE_Game_Update_MAME *pLMCE_Game_Update_MAME = new LMCE_Game_Update_MAME(sMamePath,sCategoryPath);
  iRetCode = pLMCE_Game_Update_MAME->Run();

  return iRetCode;

}
