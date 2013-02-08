/** 
 * lmce_game_utils - CRUD operations for ROM entries in lmce_game
 *
 * Author:        Thomas Cherryhomes
 * Version:       1.0
 *
 */

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include "DCE/Logger.h"

#include "lmce_game_utils.h"



void LMCE_Game_Utils::ClearVars()
{
  m_eOperation = LMCE_Game_Utils::OPERATION_NONE;
  m_pDatabase = NULL;
  m_sRomFilename = "";
  m_sRomMD5 = "";
  m_sRomDescription = "";
  m_sRomManufacturer = "";
  m_sRomYear = "";
  m_sRomGenre = "";
  m_sRomSystem = "";
  m_bVerbose=false;
}

/* ctor */
LMCE_Game_Utils::LMCE_Game_Utils()
{
  ClearVars();
}

/* dtor */
LMCE_Game_Utils::~LMCE_Game_Utils()
{
  ClearVars();
}

/* Open the Database connection */
bool LMCE_Game_Utils::OpenDatabase()
{
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Utils::OpenDatabase - Opening Database connection.");
  m_pDatabase = new Database_lmce_game(LoggerWrapper::GetInstance());
  if (!m_pDatabase->Connect("localhost","root","","lmce_game",3306))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"LMCE_Game_Utils::OpenDatabase - Could not open database connection.");
      return false;
    }
  else
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Utils::OpenDatabase - Connected to Database.");
    }
  return true;
}

/* Check if a ROM's MD5 is already in the database. */
Row_Rom* LMCE_Game_Utils::RomExists()
{
  //Row_Rom *pRow_Rom;
  vector <class Row_Rom *> v_Rom;
  Row_Rom *pResult = NULL;
  string sWhereQuery = "WHERE ";
  if (!m_sRomFilename.empty() && m_sRomMD5.empty())
    {
      sWhereQuery += "Romname = \""+m_sRomFilename+"\"";
    }
  else if (m_sRomFilename.empty() && !m_sRomMD5.empty())
    {
      sWhereQuery += "md5 = \""+m_sRomMD5+"\"";
    }
  else
    {
      sWhereQuery += "Romname = \""+m_sRomFilename+"\" OR md5 = \""+m_sRomMD5+"\"";
    }
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Utils::RomExists() - Query is %s",sWhereQuery.c_str());
  bool bExists = m_pDatabase->Rom_get()->GetRows(sWhereQuery,&v_Rom);
  if (!bExists)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Our query did not return a valid result! Please check the database.");
      return NULL;
    }
  else
    {
      if (v_Rom.empty())
	{
	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"ROM %s does not exist in database.",m_sRomFilename.c_str());
	  return NULL;
	}
      else
	{
	  if (m_bVerbose)
	    LoggerWrapper::GetInstance()->Write(LV_STATUS,"There are %u existing ROMs that matched.",v_Rom.size());
	  pResult = v_Rom[0];
	}
    }
  return pResult;
}

void LMCE_Game_Utils::DumpInstanceVars()
{
  switch (m_eOperation)
    {
    case OPERATION_NONE:
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"m_eOperation = NOT SET.");
      break;
    case OPERATION_UPDATE:
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"m_eOperation = UPDATE");
      break;
    case OPERATION_DELETE:
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"m_eOperation = DELETE");
      break;
    }

  LoggerWrapper::GetInstance()->Write(LV_STATUS,"m_sRomFilename = %s",m_sRomFilename.c_str());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"m_sRomMD5 = %s",m_sRomMD5.c_str());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"m_sRomDescription = %s",m_sRomDescription.c_str());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"m_sRomManufacturer = %s",m_sRomManufacturer.c_str());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"m_sRomYear = %s",m_sRomYear.c_str());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"m_sRomGenre = %s",m_sRomGenre.c_str());
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"m_sRomSystem = %s",m_sRomSystem.c_str());
}

bool LMCE_Game_Utils::checkNoption()
{
  return (!m_sRomMD5.empty());
}

bool LMCE_Game_Utils::checkFoption()
{
  return (!m_sRomMD5.empty());
}

bool LMCE_Game_Utils::Run(int &iRetValue)
{
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Utils::Run() - Start");

  if (m_bVerbose)
    DumpInstanceVars();

  OpenDatabase();

  switch (m_eOperation)
    {
    case OPERATION_NONE:
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"LMCE_Game_Utils::Run() - No Operation Set.");
      return false;
      break;
    case OPERATION_DELETE:
      iRetValue = deleteRom();
      break;
    case OPERATION_UPDATE:
      iRetValue = updateRom();
      break;
    }

  LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Utils::Run() - End");
  return iRetValue;
}

/**
 * Initial sanity check for update and delete. Simply make sure that either -n or -f are filled properly, and 
 * fill the other member, if needed.
 */
bool LMCE_Game_Utils::sanityCheck()
{
  // updateRom needs at the very least an -f or an -n to find the appropriate ROM entry.
  if (m_sRomFilename.empty() && m_sRomMD5.empty())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"LMCE_Game_Utils::updateRom() - neither a filename or md5 specified, aborting.");
      return false;
    }
  
  if (!m_sRomFilename.empty())
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Utils::updateRom() - Dealing with the filename.");
      m_sRomMD5 = FileUtils::FileChecksum(m_sRomFilename);

      if (m_bVerbose)
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Utils::updateRom() - File %s md5 is %s",
					    m_sRomFilename.c_str(),
					    m_sRomMD5.c_str());
    }
    else if (!m_sRomMD5.empty())
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Utils::updateRom() - Dealing with the md5.");
    }

  return true;
}

bool LMCE_Game_Utils::deleteRom()
{
  // Implement Me
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Utils::deleteRom() - Start");
  LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Utils::deleteRom() - End");
  return false;
}

bool LMCE_Game_Utils::updateRom()
{

  if (!sanityCheck())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Critical options are missing. At least -f or -n are needed. Will not perform Update.");
      return false;
    }

  if (m_sRomDescription.empty() && m_sRomManufacturer.empty() && m_sRomGenre.empty() && m_sRomYear.empty())
    {
      // No command line entries were added, let's try to parse for (Manufacturer) (19xx) tags.
      parseCoweringTags();
      if (m_bVerbose)
	{
	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Utils::updateRom() - No command line parameters found, Parsing Cowering tags.");
	  DumpInstanceVars();
	}
    }
  
  // Create/Update the entry in Rom table.
  Row_Rom *pRow_existingRom = RomExists();
  if (pRow_existingRom)
    {
      // ROM Exists, grab is pointer.
      string sRomFilename = pRow_existingRom->Romname_get();
      string sRomMD5 = pRow_existingRom->md5_get();
      string sRomDescription = GetRomDescription(pRow_existingRom);
      string sRomManufacturer = GetRomManufacturer(pRow_existingRom);
      string sRomGenre = GetRomGenre(pRow_existingRom);
      string sRomYear = GetRomYear(pRow_existingRom);
      string sRomSystem = GetRomSystem(pRow_existingRom);
      
      if (pRow_existingRom && m_bVerbose)
	{
	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"LMCE_Game_Utils::updateRom() - ROM Exists - Filename: %s - md5: %s - title - %s manufacturer - %s - genre - %s - year %s ",sRomFilename.c_str()
					      ,sRomMD5.c_str()
					      ,sRomDescription.c_str()
					      ,sRomManufacturer.c_str()
					      ,sRomGenre.c_str()
					      ,sRomYear.c_str());
	}
      else if (!pRow_existingRom && m_bVerbose) // Somehow RomExists failed to pass a pointer. Complain.
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"pRow_existingRom is (nil)");
	  return false;
	}
      
      // Merge command line options into existing rom, if available.
      if (m_sRomFilename.empty() && !sRomFilename.empty())
	m_sRomFilename = sRomFilename;
      if (m_sRomMD5.empty() && !sRomMD5.empty())
	m_sRomMD5 = sRomMD5;
      if (m_sRomDescription.empty() && !sRomDescription.empty())
	m_sRomDescription = sRomDescription;
      if (m_sRomManufacturer.empty() && !sRomManufacturer.empty())
	m_sRomManufacturer = sRomManufacturer;
      if (m_sRomGenre.empty() && !sRomGenre.empty())
	m_sRomGenre = sRomGenre;
      if (m_sRomYear.empty() && !sRomYear.empty())
	m_sRomYear = sRomYear;
      if (m_sRomSystem.empty() && !sRomSystem.empty())
	m_sRomSystem = sRomSystem;

      if (!m_sRomMD5.empty())
	{
	  // update the MD5 if needed.
	  pRow_existingRom->md5_set(m_sRomMD5);
	  pRow_existingRom->Table_Rom_get()->Commit();
	}
      
      if (!m_sRomSystem.empty())
	{
	  // Update the FK_GameSystem if needed.
	  pRow_existingRom->FK_GameSystem_set(GetPK_GameSystem(m_sRomSystem));
	  pRow_existingRom->Table_Rom_get()->Commit();
	}

    }
  else
    {
      // ROM Does not exist
      pRow_existingRom = m_pDatabase->Rom_get()->AddRow();
      pRow_existingRom->Romname_set(m_sRomFilename);
      pRow_existingRom->FK_GameSystem_set(GetPK_GameSystem(m_sRomSystem));
      pRow_existingRom->md5_set(m_sRomMD5);
      pRow_existingRom->Table_Rom_get()->Commit();
    }

  // At this point, we have an existing row, which we can attach attributes to. Do this.

  if (!m_sRomDescription.empty())
    UpdateRomAttribute(pRow_existingRom,
		       ROMATTRIBUTETYPE_title_CONST,
		       m_sRomDescription);

  if (!m_sRomManufacturer.empty())
    UpdateRomAttribute(pRow_existingRom,
		       ROMATTRIBUTETYPE_manufacturer_CONST,
		       m_sRomManufacturer);
  
  if (!m_sRomGenre.empty())
    UpdateRomAttribute(pRow_existingRom,
		       ROMATTRIBUTETYPE_genre_CONST,
		       m_sRomGenre);

  if (!m_sRomYear.empty())
    UpdateRomAttribute(pRow_existingRom,
		       ROMATTRIBUTETYPE_year_CONST,
		       m_sRomYear);

  if (!m_sRomSystem.empty())
    UpdateRomAttribute(pRow_existingRom,
		       ROMATTRIBUTETYPE_system_CONST,
		       m_sRomSystem);

  
  return true;
}

std::string LMCE_Game_Utils::GetRomAttribute(Row_Rom *pRow_Rom, int iFK_RomAttributeType)
{
  Row_RomAttribute *pRow_RomAttribute;
  // Row_Rom_RomAttribute *pRow_Rom_RomAttribute;
  vector<class Row_Rom_RomAttribute *> v_Rom_RomAttributes;
  
  pRow_Rom->Rom_RomAttribute_FK_Rom_getrows(&v_Rom_RomAttributes);

  for (unsigned int i=0;i<v_Rom_RomAttributes.size();++i)
    {
      pRow_RomAttribute = m_pDatabase->RomAttribute_get()
	->GetRow(v_Rom_RomAttributes[i]
		 ->FK_RomAttribute_get());

      if (pRow_RomAttribute->FK_RomAttributeType_get() == iFK_RomAttributeType)
	{
	  return pRow_RomAttribute->Name_get();
	}
    }
  return "";
}

int LMCE_Game_Utils::GetPK_GameSystem(string sPK_GameSystem)
{
  Row_GameSystem *pRow_GameSystem;
  vector<class Row_GameSystem *> v_GameSystem;
  string sWhereQuery = "WHERE Description = \""+sPK_GameSystem+"\"";
  if (!m_pDatabase->GameSystem_get()->GetRows(sWhereQuery,&v_GameSystem))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"No database handle while trying to get a Gamesystem value.");
      return 0;
    }
  else
    {
      if (v_GameSystem.size() < 1)
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"No Match for a gamesystem called %s, returning Unknown.",sPK_GameSystem.c_str());
	  return 0;
	}
      pRow_GameSystem = v_GameSystem[0];
    }

  return pRow_GameSystem->PK_GameSystem_get(); // int

}

std::string LMCE_Game_Utils::GetRomDescription(Row_Rom *pRow_Rom)
{
  return GetRomAttribute(pRow_Rom,ROMATTRIBUTETYPE_title_CONST);
}
;
std::string LMCE_Game_Utils::GetRomManufacturer(Row_Rom *pRow_Rom)
{
  return GetRomAttribute(pRow_Rom,ROMATTRIBUTETYPE_manufacturer_CONST);
}

std::string LMCE_Game_Utils::GetRomGenre(Row_Rom *pRow_Rom)
{
  return GetRomAttribute(pRow_Rom,ROMATTRIBUTETYPE_genre_CONST);
}

std::string LMCE_Game_Utils::GetRomYear(Row_Rom *pRow_Rom)
{
  return GetRomAttribute(pRow_Rom,ROMATTRIBUTETYPE_year_CONST);
}

std::string LMCE_Game_Utils::GetRomSystem(Row_Rom *pRow_Rom)
{
  if (!pRow_Rom)
    return "";

  if (pRow_Rom->FK_GameSystem_get() < 1)
    return "Unknown";

  vector<class Row_GameSystem *> v_GameSystem;
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"GameSystem is %d", pRow_Rom->FK_GameSystem_get());
  string sWhereQuery = "where PK_GameSystem = \"" + 
    StringUtils::itos(pRow_Rom->FK_GameSystem_get()) + "\"";

  if (m_pDatabase->GameSystem_get()->GetRows(sWhereQuery,&v_GameSystem))
    {
      if (v_GameSystem.size() > 0)
	return v_GameSystem[0]->Description_get();
    }
  else
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not get a proper database handle to the GameSystem table.");
    }
  return "";
}

void LMCE_Game_Utils::parseCoweringTags()
{
  // LoggerWrapper::GetInstance()->Write(LV_WARNING,"parseCoweringTags()");
  m_sRomFilename = FileUtils::FilenameWithoutPath(m_sRomFilename);
  // Assume that the first parenthesis ends the title.
  unsigned int iTitleEnd = m_sRomFilename.find("(");

  if (iTitleEnd == string::npos) 
    {
      m_sRomDescription = m_sRomFilename;
    } 
  else 
    {
      m_sRomDescription = m_sRomFilename.substr(0,iTitleEnd);
      m_sRomDescription = trim(m_sRomDescription);
    }

  string::size_type pos=0;
  string sTags = FileUtils::FileWithoutExtension(m_sRomFilename);
  sTags = sTags.substr(iTitleEnd);

  while (pos < sTags.length())
    {
      string sItem = StringUtils::Tokenize(sTags,"()",pos);
      sItem = trim(sItem);
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"%s",sItem.c_str());
      int iYearTest = atoi(sItem.c_str());
      if (sItem.length() > 3 && iYearTest == 0 && StringUtils::ToUpper(sItem) != "19XX" && sItem.find("[") == string::npos
	  )
	{
	  m_sRomManufacturer = sItem;
	}
      else if (iYearTest > 1900)
	{
	  m_sRomYear = sItem;
	}
    }
 
}

bool LMCE_Game_Utils::UpdateRomAttribute(Row_Rom *pRow_Rom, int iRomAttributeType, string sValue)
{
  // Find an existing Rom Attribute linkage if it's already there.
  Row_RomAttribute *pRow_RomAttribute;
  Row_Rom_RomAttribute *pRow_Rom_RomAttribute;
  vector<Row_Rom_RomAttribute *> v_Rom_RomAttributes;
  bool bNeedToAdd = true;

  if (!pRow_Rom)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"LMCE_Game_Utils::UpdateRomAttribute() - no pRow_Rom. Bailing!");
      return false;
    }
 
  // Get the matching attributes for a given ROM, if possible. 
  pRow_Rom->Rom_RomAttribute_FK_Rom_getrows(&v_Rom_RomAttributes);

  if (v_Rom_RomAttributes.size() > 0)
    {
      // We have attributes attached, let's find the one we're modifying.
      for (unsigned int i=0;i<v_Rom_RomAttributes.size();i++)
	{
	  pRow_RomAttribute = m_pDatabase->RomAttribute_get()->GetRow(v_Rom_RomAttributes[i]->FK_RomAttribute_get());
	  if (pRow_RomAttribute->FK_RomAttributeType_get() == iRomAttributeType)
	    {
	      // Existing attribute, simply change it. 
	      pRow_RomAttribute->Name_set(sValue);
	      pRow_RomAttribute->Table_RomAttribute_get()->Commit();
	      bNeedToAdd = false;
	      return true;
	    }
	}      
    }

  if (bNeedToAdd)
    {
      if (m_bVerbose)
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Adding Rom Attribute of type %u - value %s",
					    iRomAttributeType,
					    sValue.c_str());
      pRow_RomAttribute = m_pDatabase->RomAttribute_get()->AddRow();
      pRow_RomAttribute->FK_RomAttributeType_set(iRomAttributeType);
      pRow_RomAttribute->Name_set(sValue);
      pRow_RomAttribute->FK_GameSystem_set(GetPK_GameSystem(m_sRomSystem));
      pRow_RomAttribute->Table_RomAttribute_get()->Commit();
      
      if (m_bVerbose)
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Adding Table linkage of Rom %u to Attribute %u ",
					    pRow_Rom->PK_Rom_get(),
					    pRow_RomAttribute->PK_RomAttribute_get());
      
      pRow_Rom_RomAttribute = m_pDatabase->Rom_RomAttribute_get()->AddRow();
      pRow_Rom_RomAttribute->FK_Rom_set(pRow_Rom->PK_Rom_get());
      pRow_Rom_RomAttribute->FK_RomAttribute_set(pRow_RomAttribute->PK_RomAttribute_get());
      pRow_Rom_RomAttribute->Table_Rom_RomAttribute_get()->Commit();
    } 

  return true;

}
