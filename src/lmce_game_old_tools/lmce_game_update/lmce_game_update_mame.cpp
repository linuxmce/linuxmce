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
  m_pGameDatabase=new GameDatabase();
}

LMCE_Game_Update_MAME::~LMCE_Game_Update_MAME()
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

bool LMCE_Game_Update_MAME::GetMAMEOutput(string &sMAMEOutput)
{
  return true;
}

int LMCE_Game_Update_MAME::Run()
{

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

  cout << "Done." << endl << endl;
  return 0;
}

bool LMCE_Game_Update_MAME::UpdateRom(MAMERom *pCurrentRom, string sRomFile, string sRomName)
{
  long int iPK_Rom = m_pGameDatabase->GetPK_Rom(sRomFile);
  string sTitle;

  if (iPK_Rom < 0)
    {
      LoggerWrapper::GetInstance()->Write(LV_WARNING,"LMCE_Game_Update_MAME::UpdateRom(%s) - Could not fetch PK_Rom entry for Rom File.",sRomFile.c_str());
      return false;
    }

  if (!m_pGameDatabase->GetTitleForHash(pCurrentRom->TitleHash_get(), sTitle))
    {
      m_pGameDatabase->AddTitleHash(pCurrentRom->RomTitle_get(),pCurrentRom->TitleHash_get());
      m_pGameDatabase->UpdateRomAttribute(iPK_Rom,ROMATTRIBUTETYPE_title_CONST,pCurrentRom->RomTitle_get());
    }
  else
    {
      m_pGameDatabase->UpdateRomAttribute(iPK_Rom,ROMATTRIBUTETYPE_title_CONST,sTitle);
    }
  m_pGameDatabase->UpdateRomAttribute(iPK_Rom,ROMATTRIBUTETYPE_subtitle_CONST,pCurrentRom->RomSubtitle_get());
  m_pGameDatabase->UpdateRomAttribute(iPK_Rom,ROMATTRIBUTETYPE_manufacturer_CONST,pCurrentRom->RomManufacturer_get());
  
  string sTmpCategory;
  if (m_pMAMECategory->m_mapRomToCategory_Find(sRomName).empty())
    {
      LoggerWrapper::GetInstance()->Write(LV_WARNING,"No Genre found for Game %s, trying clone %s",sRomName.c_str(), pCurrentRom->RomCloneOf_get().c_str());
      if (m_pMAMECategory->m_mapRomToCategory_Find(pCurrentRom->RomCloneOf_get()).empty())
	{
	  LoggerWrapper::GetInstance()->Write(LV_WARNING,"No Genre found for Clone of Game %s, Setting to Unknown.",pCurrentRom->RomCloneOf_get().c_str());
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

  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Genre is %s",sTmpCategory.c_str());
  m_pGameDatabase->UpdateRomAttribute(iPK_Rom,ROMATTRIBUTETYPE_genre_CONST,sTmpCategory);
  m_pGameDatabase->UpdateRomAttribute(iPK_Rom,ROMATTRIBUTETYPE_year_CONST,pCurrentRom->RomYear_get());
  return true;
}

bool LMCE_Game_Update_MAME::AddRom(MAMERom *pCurrentRom, string sRomFile, string sRomName)
{
  long iPK_Rom = m_pGameDatabase->AddRom(sRomFile);

  if (iPK_Rom == 0)
    {
      // Ignore
    }
  else
    {
      string sTitle;
      if (!m_pGameDatabase->GetTitleForHash(pCurrentRom->TitleHash_get(), sTitle))
	{
	  m_pGameDatabase->AddTitleHash(pCurrentRom->RomTitle_get(),pCurrentRom->TitleHash_get());
	  m_pGameDatabase->AddRomAttribute(iPK_Rom,ROMATTRIBUTETYPE_title_CONST,pCurrentRom->RomTitle_get());
	}
      else
	{
	  m_pGameDatabase->AddRomAttribute(iPK_Rom,ROMATTRIBUTETYPE_title_CONST,sTitle);
	}
      m_pGameDatabase->AddRomAttribute(iPK_Rom,ROMATTRIBUTETYPE_subtitle_CONST,pCurrentRom->RomSubtitle_get());
      m_pGameDatabase->AddRomAttribute(iPK_Rom,ROMATTRIBUTETYPE_manufacturer_CONST,pCurrentRom->RomManufacturer_get());

      string sTmpCategory;
      if (m_pMAMECategory->m_mapRomToCategory_Find(sRomName).empty())
	{
	  LoggerWrapper::GetInstance()->Write(LV_WARNING,"No Genre found for Game %s, trying clone %s",sRomName.c_str(), pCurrentRom->RomCloneOf_get().c_str());
	  if (m_pMAMECategory->m_mapRomToCategory_Find(pCurrentRom->RomCloneOf_get()).empty())
	    {
	      LoggerWrapper::GetInstance()->Write(LV_WARNING,"No Genre found for Clone of Game %s, Setting to Unknown.",pCurrentRom->RomCloneOf_get().c_str());
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

      m_pGameDatabase->AddRomAttribute(iPK_Rom,ROMATTRIBUTETYPE_genre_CONST,sTmpCategory);
      m_pGameDatabase->AddRomAttribute(iPK_Rom,ROMATTRIBUTETYPE_year_CONST,pCurrentRom->RomYear_get());
    }
  return true;
}

void LMCE_Game_Update_MAME::ProcessEntry(string sRomName, MAMERom* pCurrentRom)
{
  string sRomFile=sRomName+".zip";
  if (!pCurrentRom)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ProcessEntry(%s) - MAMERom = NULL.",sRomName.c_str());
      return;
    }

  if (m_pGameDatabase->RomExists(sRomFile))
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"ProcessEntry(%s) - ROM Exists. Attempting to update.",sRomName.c_str());
      if (!UpdateRom(pCurrentRom, sRomFile, sRomName))
	{
	  LoggerWrapper::GetInstance()->Write(LV_WARNING,"ProcessEntry(%s) - Could not update ROM entry.",sRomFile.c_str());
	  return;
	}
      else
	{
	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"ProcessEntry(%s) - Processed existing ROM",sRomFile.c_str());
	  return;
	}
    }
  else
    {
      if (!AddRom(pCurrentRom, sRomFile, sRomName))
	{
	  LoggerWrapper::GetInstance()->Write(LV_WARNING,"ProcessEntry(%s) - Could not process ROM entry.",sRomFile.c_str());
	  return;
	}
      else
	{
	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"ProcessEntry(%s) - Processed new ROM.",sRomFile.c_str());
	}
    }

}

int main(int argc, char* argv[])
{

  bool bError = true;  // Set when there was an error parsing command line.
  string sMamePath;    // Path to MAME binary.
  string sCategoryPath; // Path to Category.ini file.
  char c;              // Current Option.
  int iRetCode;        // Return code

  cout << "lmce_game_update_mame, v. 2.0" << endl;

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
