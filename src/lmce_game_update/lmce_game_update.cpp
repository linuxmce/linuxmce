/**
 * lmce_game_update - Update the lmce_game database with the output from a running sdlmame.
 * 
 * This will skip any existing ROM entries, only putting in ROM entries that do not already
 * exist.
 *
 * Author:     Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version:    1.0
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
#include "lmce_game_update.h"

LMCE_Game_Update::LMCE_Game_Update(string sMamePath)
{
  m_pMyDatabase = NULL;
  m_sMamePath = sMamePath;
}

LMCE_Game_Update::~LMCE_Game_Update()
{
  delete m_pMyDatabase;
  m_pMyDatabase = NULL;
}

bool LMCE_Game_Update::InitDatabase()
{
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Update::InitDatabase() - Begin");
  m_pMyDatabase = new Database_lmce_game(LoggerWrapper::GetInstance());
  if (!m_pMyDatabase->Connect("localhost","root","","lmce_game",3306))
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Update::InitDatabase() - Unable to connect to DB.");
      return false;
    }
  else
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Update::InitDatabase() - Connected to DB.");
    }
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Update::InitDatabase() - End");
  return true;
}

bool LMCE_Game_Update::GetMetadataForRom(string sRomName, string& sMetaData)
{
  string sOutput, sStdErr;
  char csRomName[100];
  strcpy(csRomName,sRomName.c_str());
  char * const args[] = {"./get_mame_info.pl",csRomName,NULL};
  if (ProcessUtils::GetCommandOutput(args[0], args, sOutput, sStdErr) == 0)
    {
      vector<string> vectTmp;  // used for a basic sanity check.
      StringUtils::Tokenize(sOutput,(string)"|",vectTmp);
      if (vectTmp.size() == 4)
	{
	  sMetaData = sOutput;
	}
      else 
	{
	  sMetaData = "ERROR";
	}
      return true;
    }
  else
    {
      sMetaData = "ERROR";
      return false;
    }
}

bool LMCE_Game_Update::ParseGameListFromMAME() 
{
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Update::ParseGameListFromMAME() - Begin");
  string sOutput, sStdErr;
  char csMAMEPath[100];
  strcpy(csMAMEPath,m_sMamePath.c_str());
  char * const args[] = {csMAMEPath, "-listfull", NULL};
  if (ProcessUtils::GetCommandOutput(args[0], args, sOutput, sStdErr) == 0)
    {
      vector<string> v_sOutput_Rows;
      vector<string>::iterator it;
      StringUtils::Tokenize(sOutput, "\n", v_sOutput_Rows);
      for (it = v_sOutput_Rows.begin()+1; it != v_sOutput_Rows.end(); ++it)
	{
	  string sCurrentLine, sRomName, sDescription;
	  sCurrentLine = *it;
	  sRomName = sCurrentLine.substr(0, sCurrentLine.find(" "));  // Chop away the leading whitespace.
	  sDescription = sCurrentLine.substr(sCurrentLine.find_first_of("\"")+1, sCurrentLine.find_last_of("\"")-4);
	  sDescription.erase(sDescription.length()-1);
  
	  LoggerWrapper::GetInstance()->Write(42,"LMCE_Game_Update::ParseGameFromMAME() - Romname: %s, Description: %s",sRomName.c_str(),sDescription.c_str());

	  m_vRomNames.push_back(sRomName);
	  m_vRomDescriptions.push_back(sDescription);

	}
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Update::ParseGameListFromMAME() - End");
      return true;
    } 
  else 
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Update::ParseGameListFromMAME() - End");
      return false;
    }
}

bool LMCE_Game_Update::RomExists(string sRomName)
{
  Row_Rom *pRow_Rom;
  vector<class Row_Rom *> v_Rom;
  string sRomFile = sRomName + ".zip";
  string sWhereQuery = "WHERE Romname = '"+sRomFile+"'";
  //  LoggerWrapper::GetInstance()->Write(LV_STATUS,"sWhereQuery is %s",sWhereQuery.c_str());
  if (!m_pMyDatabase->Rom_get()->GetRows(sWhereQuery,&v_Rom)) 
    {
      return false;
    } 
  else 
    {
      // Rom DOES exist in DB already.
      //      LoggerWrapper::GetInstance()->Write(LV_STATUS,"Size is %d",v_Rom.size());

      // LoggerWrapper::GetInstance()->Write(LV_STATUS,"Rom %s does exist.",sRomName.c_str());
      if (v_Rom.empty()) 
	{
	  //	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Rom %s does not exist",sRomName.c_str());
	  return false;
	}
      else
	{
	  return true;
	}
    }
}

int LMCE_Game_Update::AddRomm(string sRomName)
{
  Row_Rom *pRow_Rom = m_pMyDatabase->Rom_get()->AddRow();
  string sRomFile = sRomName + ".zip";
  pRow_Rom->Romname_set(sRomFile);
  pRow_Rom->FK_GameSystem_set(GAMESYSTEM_Arcade_CONST);
  pRow_Rom->Table_Rom_get()->Commit();
  return pRow_Rom->PK_Rom_get();
}

bool LMCE_Game_Update::AddRommAttribute(int iPK_Rom, int iRomAttributeType, string sName)
{
  Row_Rom *pRow_Rom = m_pMyDatabase->Rom_get()->GetRow(iPK_Rom);
  vector<Row_RomAttribute *> vectRow_RomAttributes;
  vector<Row_Rom_RomAttribute *> vectRow_Rom_RomAttributes;
  if (!pRow_Rom) 
    {
      return false;
    }

  Row_RomAttribute *pRow_RomAttribute;
  Row_Rom_RomAttribute *pRow_Rom_RomAttribute;
  
  m_pMyDatabase->RomAttribute_get()->GetRows("WHERE Name = '"+StringUtils::SQLEscape(sName)+"' AND FK_RomAttributeType = '"+StringUtils::itos(iRomAttributeType)+"'",&vectRow_RomAttributes);

  if (vectRow_RomAttributes.size() == 0)
    {
      // Add the attribute to the DB
      pRow_RomAttribute = m_pMyDatabase->RomAttribute_get()->AddRow();
      pRow_RomAttribute->FK_GameSystem_set(GAMESYSTEM_MAME_CONST);
      pRow_RomAttribute->FK_RomAttributeType_set(iRomAttributeType);
      pRow_RomAttribute->Name_set(sName);
      pRow_RomAttribute->Table_RomAttribute_get()->Commit();      
    }
  else
    {
      // Use the Attribute from the DB
      pRow_RomAttribute = vectRow_RomAttributes[0];
    }

  m_pMyDatabase->Rom_RomAttribute_get()->GetRows("WHERE FK_Rom = "+StringUtils::itos(pRow_Rom->PK_Rom_get())+" AND FK_RomAttribute = "+StringUtils::itos(pRow_RomAttribute->PK_RomAttribute_get()),&vectRow_Rom_RomAttributes);  

  if (vectRow_Rom_RomAttributes.size() == 0) 
    {
      // Add the linkage to the db
      pRow_Rom_RomAttribute = m_pMyDatabase->Rom_RomAttribute_get()->AddRow();
      pRow_Rom_RomAttribute->FK_Rom_set(pRow_Rom->PK_Rom_get());
      pRow_Rom_RomAttribute->FK_RomAttribute_set(pRow_RomAttribute->PK_RomAttribute_get());
      pRow_Rom_RomAttribute->Table_Rom_RomAttribute_get()->Commit();
    }
  else
    {
      // Don't do a damn thing.
    }


  return true;

}

int LMCE_Game_Update::Run()
{
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Update::Run() - Start");

  if (!InitDatabase())
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Update::Run() - Unable to Initialize Database Connection.");
      return false;
    }

  if (!ParseGameListFromMAME())
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Update::Run() - Unable to Parse Game list from MAME.");
      return false;
    }

  LoggerWrapper::GetInstance()->Write(LV_STATUS,"Currently checking for ROMS not already in database...");

  vector<string>::iterator it;
  for (it=m_vRomNames.begin();it != m_vRomNames.end();++it)
    {
      string sRomName = *it;
      string sMetaData;
      if (!RomExists(sRomName)) 
	{
	  LoggerWrapper::GetInstance()->Write(LV_WARNING,"Rom %s does not exist in database. Grabbing info.",sRomName.c_str());
	  if (!GetMetadataForRom(sRomName,sMetaData) || (sMetaData == "ERROR")) 
	    {
	      // Could not grab metadata.
	      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not get metadata for Rom %s from MAWS.",sRomName.c_str());
	    }
	  else 
	    {
	      // Grab metadata, parse it, bring it back, and slam it into the database.
	      // LoggerWrapper::GetInstance()->Write(LV_STATUS,"Rom Untokenised is: %s",sMetaData.c_str());
	      int iPK_Rom = AddRomm(sRomName);
	      if (!iPK_Rom)
		{
		  // Ignore
		}
	      else 
		{
		  vector<string> vectMetadata;
		  StringUtils::Tokenize(sMetaData,"|",vectMetadata);
		  
		  string sTitle = vectMetadata[0];
		  string sManufacturer = vectMetadata[1];
		  string sYear = vectMetadata[2];
		  string sGenre = vectMetadata[3];
		  AddRommAttribute(iPK_Rom,ROMATTRIBUTETYPE_title_CONST,sTitle);
		  AddRommAttribute(iPK_Rom,ROMATTRIBUTETYPE_genre_CONST,sGenre);
		  AddRommAttribute(iPK_Rom,ROMATTRIBUTETYPE_year_CONST,sYear);
		  AddRommAttribute(iPK_Rom,ROMATTRIBUTETYPE_manufacturer_CONST,sManufacturer);
		}
	    }
	}
    }
  

  LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Update::Run() - End");
  return 0;
}

int main(int argc, char* argv[])
{

  bool bError = true;  // Set when there was an error parsing command line.
  string sMamePath;    // Path to MAME binary.
  char c;              // Current Option.
  int iRetCode;        // Return code

  cout << "lmce_game_update, v. 1.0" << endl;

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
	  if (argc > 2)
	    {
	      sMamePath = argv[++optnum];
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
	   << "\t -p -- The Path to the MAME binary. (Required)" << endl;
      exit(1);
    }

  LMCE_Game_Update *pLMCE_Game_Update = new LMCE_Game_Update(sMamePath);
  iRetCode = pLMCE_Game_Update->Run();

  return iRetCode;

}
