/**
 * lmce_game_mame_prune - A little utility to get rid of unwanted roms
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version 1.0
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
#include "lmce_game_mame_prune.h"

LMCE_Game_Mame_Prune::LMCE_Game_Mame_Prune(bool bDryRun, string sMamePath, string sRomPath, string sCategoryPath)
{
  m_sMamePath=sMamePath;
  m_sRomPath=sRomPath;
  m_sCategoryPath=sCategoryPath;
  m_iDeletedRoms=0;
  m_iNotDeletedRoms=0;
  m_bDryRun=bDryRun;
  m_pMAMECategory=new MAMECategory(sCategoryPath);
  m_pGameDatabase=new GameDatabase();
}

LMCE_Game_Mame_Prune::~LMCE_Game_Mame_Prune()
{
  if (m_pMAMECategory)
    {
      delete m_pMAMECategory;
      m_pMAMECategory=NULL;
    }

  if (m_pGameDatabase)
    {
      delete m_pGameDatabase;
      m_pGameDatabase=NULL;
    }
}

int LMCE_Game_Mame_Prune::Run()
{
  if (!FileUtils::DirExists(m_sRomPath))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Directory %s does not exist!",m_sRomPath.c_str());
      return 1;
    }
  if (!m_pGameDatabase)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not initialize GameDatabase object.");
      return 1;
    }

  if (!m_pGameDatabase->Init())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not initialize GameDatabase connection.");
      return 1;
    }

  cout << "Parsing category list, Please Wait...";
  if (!m_pMAMECategory->Parse())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not parse category file. See above error messages.");
      return 1;
    }
  cout << endl;

  string sMameOutput;
  string sErr;

  cout << "Grabbing XML output from MAME, Please Wait...";
  const char *args[]={m_sMamePath.c_str(),"-listxml",NULL};
  ProcessUtils::GetCommandOutput(args[0],args,sMameOutput,sErr);
  FileUtils::WriteTextFile("/tmp/mame.tmp",sMameOutput);
  cout << endl;

  MAMEParser p;

  cout << "Parsing MAME XML output, Please Wait...";
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

  cout << endl;
  cout << "Processing MAME Game Entries into Database, Please Wait...";
  for (map<string, MAMERom *>::iterator it=p.m_mapRomToMAMERom.begin();
       it != p.m_mapRomToMAMERom.end();
       ++it)
    {
      string sRomEntry = it->first;
      MAMERom *pCurrentRom = it->second;
      ProcessEntry(sRomEntry, pCurrentRom);
    }

  cout << endl << endl;
  
  LoggerWrapper::GetInstance()->Write(LV_WARNING,"Number of Deleted Roms: %d",m_iDeletedRoms);
  LoggerWrapper::GetInstance()->Write(LV_WARNING,"Number of kept Roms: %d",m_iNotDeletedRoms);

  cout << "Done." << endl << endl;
  return 0;
}

bool LMCE_Game_Mame_Prune::GetMAMEOutput(string &sMameOutput)
{
  return true;
}

bool LMCE_Game_Mame_Prune::IsUnwantedCategory(string sCategory)
{
  return (sCategory == "Adult" ||
	  sCategory == "Games / Gambling" ||
	  sCategory == "Logic / Flower Matching" ||
	  sCategory == "Logic / Mahjong" ||
	  sCategory == "Logic / Quiz" ||
	  sCategory == "Logic / Quiz (Japan)" ||
	  sCategory == "Logic / Shanghai" ||
	  sCategory == "Logic / Shanghai 2" ||
	  sCategory == "Logic / Shougi" ||
	  sCategory == "Xtra Unknown" ||
	  sCategory == "Xtras AGEMAME - PinMAME");
}

void LMCE_Game_Mame_Prune::ProcessEntry(string sRomName, MAMERom *pCurrentRom)
{
  string sRomFile=sRomName+".zip";
  bool bDelete=false;
  if (!pCurrentRom)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ProcessEntry(%s) - MAMERom = NULL.",sRomName.c_str());
      return;
    }
  
  if (pCurrentRom->RomStatus_get() == "preliminary")
    {
      // We do not want preliminary ROMs, at all!
      bDelete=true;
    }

  string sTmpCategory;
  if (m_pMAMECategory->m_mapRomToCategory_Find(sRomName).empty())
    {
      // LoggerWrapper::GetInstance()->Write(LV_WARNING,"No Genre found for Game %s, trying clone %s",sRomName.c_str(), pCurrentRom->RomCloneOf_get().c_str());
      if (m_pMAMECategory->m_mapRomToCategory_Find(pCurrentRom->RomCloneOf_get()).empty())
	{
	  // LoggerWrapper::GetInstance()->Write(LV_WARNING,"No Genre found for Clone of Game %s, Setting to Unknown.",pCurrentRom->RomCloneOf_get().c_str());
	  sTmpCategory="Unknown";
	}
      else
	{
	  sTmpCategory=m_pMAMECategory->m_mapRomToCategory_Find(pCurrentRom->RomCloneOf_get());
	}
    }
  else
    {
      sTmpCategory=m_pMAMECategory->m_mapRomToCategory_Find(sRomName);
    }

  if (IsUnwantedCategory(sTmpCategory))
    {
      bDelete=true;
    }

  // Delete the rom and associated CHD if anything above flagged.
  if (bDelete)
    {
      if (!m_bDryRun)
	{
	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Deleting ROM %s",sRomName.c_str());
	  string sCmd = "rm -rf "+m_sRomPath+"/"+sRomName+".zip";
	  system(sCmd.c_str());
	  string sCmd2 = "rm -rf "+m_sRomPath+"/"+sRomName;
	  system(sCmd2.c_str());
	  m_iDeletedRoms++;
	}
      else
	{
	  cout << "rm -rf " << m_sRomPath << "/" << sRomName << ".zip" << endl;
	  cout << "rm -rf " << m_sRomPath << "/" << sRomName << endl;
 	  // LoggerWrapper::GetInstance()->Write(LV_STATUS,"Would have deleted ROM %s",sRomName.c_str());
	  m_iDeletedRoms++;
	}
    }
  else
    {
      m_iNotDeletedRoms++;
    }

}


int main(int argc, char* argv[])
{
  bool bError = true;
  bool bDryRun=false;
  string sMamePath;
  string sRomPath;
  string sCategoryPath;
  char c;
  int iRetCode;

  cout << "lmce_game_mame_prune, v. 2.1" << endl;

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
	  if (argc > 6)
	    {
	      sMamePath = argv[++optnum];
	      bError=false;
	    }
	  break;
	case 'r':
	  if (argc > 6)
	    {
	      sRomPath = argv[++optnum];
	      bError=false;
	    }
	  break;
	case 'c':
	  if (argc > 6)
	    {
	      sCategoryPath = argv[++optnum];
	      bError=false;
	    }
	  break;
	case 'd':
	  if (argc > 6)
	    {
	      bDryRun=true;
	      LoggerWrapper::GetInstance()->Write(LV_STATUS,"This will be a dry run. No files will be deleted.");
	    }
	}
    }

  if (bError) 
    {
      cout << "Prunes unwanted ROMs from a MAME rom set." << endl
	   << endl 
	   << "Usage: lmce_game_update -p /path/to/mame" << endl
	   << endl
	   << "\t -p -- The Path to the MAME binary. (Required)" << endl
	   << "\t -r -- The Path to the directory containing ROMs. (Required)" << endl
           << "\t -c -- The Path to the Category.ini file (Required)" << endl
	   << "\t -d -- Do a dry run (do not delete anything)" << endl;
      exit(1);
    }

  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"bDryRun %d sMamePath %s sRomPath %s sCategoryPath %s",bDryRun,sMamePath.c_str(),sRomPath.c_str(),sCategoryPath.c_str());
  LMCE_Game_Mame_Prune *pLMCE_Game_Mame_Prune = new LMCE_Game_Mame_Prune(bDryRun,sMamePath,sRomPath,sCategoryPath);
  iRetCode = pLMCE_Game_Mame_Prune->Run();
  delete pLMCE_Game_Mame_Prune;
  pLMCE_Game_Mame_Prune=NULL;

  return iRetCode;

}
