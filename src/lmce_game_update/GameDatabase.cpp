/**
 * GameDatabase - A simple class to abstract sql2cpp calls to update the
 * game database.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#include "GameDatabase.h"

using namespace std;

GameDatabase::GameDatabase()
{
  m_pDatabase=NULL;
}

GameDatabase::~GameDatabase()
{

  if (m_pDatabase)
    delete m_pDatabase;
  
  m_pDatabase=NULL;

}

bool GameDatabase::Init()
{
  m_pDatabase = new Database_lmce_game(LoggerWrapper::GetInstance());
  if (!m_pDatabase->Connect("localhost","root","","lmce_game",3306))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"GameDatabase::Init() - Unable to connect to database.");
      return false;
    }
  else
    {
      return true;
    }
}

bool GameDatabase::RomExists(string sRomName)
{
    vector<class Row_Rom *> v_RowRom;
  string sRomFile = sRomName + ".zip";
  string sWhereQuery = "WHERE Romname = '"+sRomName+"'";

  if (!m_pDatabase->Rom_get()->GetRows(sWhereQuery,&v_RowRom))
    {
      return false;
    }
  else
    {
      if (v_RowRom.empty())
	{
	  return false;  // does not exist.
	}
      else
	{
	  LoggerWrapper::GetInstance()->Write(LV_WARNING,"ROM EXISTS: %s",sRomName.c_str());
	  return true;   // does exist.
	}
    }
}

long int GameDatabase::AddRom(string sRomName)
{
  Row_Rom *pRow_Rom = m_pDatabase->Rom_get()->AddRow();
  string sRomFile = sRomName + ".zip";
  if (pRow_Rom)
    {
      pRow_Rom->Romname_set(sRomName);
      pRow_Rom->FK_GameSystem_set(GAMESYSTEM_Arcade_CONST);
      pRow_Rom->Table_Rom_get()->Commit();
      long iRom = pRow_Rom->PK_Rom_get();
      return iRom;
    }
  else
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"GameDatabase::AddRom(%s) - could not add row.",sRomName.c_str());
      return 0;
    }
}

long int GameDatabase::GetPK_Rom(string sRomName)
{
  string sWhereQuery = "Romname = '"+StringUtils::SQLEscape(sRomName)+"'";
  vector<class Row_Rom *> vectRows;
  if (!m_pDatabase->Rom_get()->GetRows(sWhereQuery, &vectRows))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"GameDatabase::GetRom(%s) - Could not fetch rows from database!",sRomName.c_str());
      return -1;
    }
  if (vectRows.size()==0)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"GameDatabase::GetRom(%s) - Returned 0 rows! Should not have happened due to Exists check!",sRomName.c_str());
      return -1;
    }
  if (vectRows.size()>1)
    {
      LoggerWrapper::GetInstance()->Write(LV_WARNING,"GameDatabase::GetRom(%s) - Returned %d rows. Returning the first match.",sRomName.c_str(),vectRows.size());
    }
  Row_Rom *pRow_Rom = vectRows[0];
  if (!pRow_Rom)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"GameDatabase::GetRom(%s) - somehow we got a NULL row!",sRomName.c_str());
      return -1;
    }
  return pRow_Rom->PK_Rom_get();
}

bool GameDatabase::AddTitleHash(string sTitle, string sTitleHash)
{
  Row_RomTitles *pRow_RomTitles = m_pDatabase->RomTitles_get()->AddRow();
  if (pRow_RomTitles)
    {
      pRow_RomTitles->Title_set(sTitle);
      pRow_RomTitles->TitleHash_set(sTitleHash);
      pRow_RomTitles->Table_RomTitles_get()->Commit();
    }
  else
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"GameDatabase::AddTitleHash(%s, %s) - could not add row.",sTitle.c_str(),sTitleHash.c_str());
      return false;
    }
  return true;
}

bool GameDatabase::GetTitleForHash(string sTitleHash, string &sTitle)
{
  vector<class Row_RomTitles *> vectRowRomTitles;
  string sWhereQuery = "TitleHash = '"+sTitleHash+"'";
  
  if (!m_pDatabase->RomTitles_get()->GetRows(sWhereQuery,&vectRowRomTitles))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"GameDatabase::GetTitleHash(%s) - Could not perform query.",sTitleHash.c_str());
      return false;
    }
  else
    {
      if (vectRowRomTitles.empty())
	{
	  return false; // Titlehash does not exist.
	}
      else
	{
	  Row_RomTitles *pRow_RomTitles = vectRowRomTitles[0];
	  sTitle = pRow_RomTitles->Title_get();
	  return true;
	}
    }
  
  return false;
}

bool GameDatabase::UpdateRomAttribute(long int iPK_Rom, int iRomAttributeType, string sName)
{
  Row_Rom *pRow_Rom = m_pDatabase->Rom_get()->GetRow(iPK_Rom);
  Row_RomAttribute *pRow_RomAttribute;
  Row_Rom_RomAttribute *pRow_Rom_RomAttribute;
  vector<Row_RomAttribute *> vectRow_RomAttributes;
  vector<Row_Rom_RomAttribute *> vectRow_Rom_RomAttributes;
  if (!pRow_Rom)
    {
      return false;
    }

  string sRomFile = pRow_Rom->Romname_get();

  m_pDatabase->RomAttribute_get()->GetRows("Name = '"+StringUtils::SQLEscape(sName)+"' AND FK_RomAttributeType = '"+StringUtils::itos(iRomAttributeType)+"'",&vectRow_RomAttributes);

  // If Attribute Row does not exist.
  if (vectRow_RomAttributes.size() == 0)
    {
      LoggerWrapper::GetInstance()->Write(LV_STATUS,"GameDatabase::UpdateRom(%s) - Adding new attribute of type %d with value %s",sRomFile.c_str(),iRomAttributeType,sName.c_str());
      pRow_RomAttribute = m_pDatabase->RomAttribute_get()->AddRow();
      pRow_RomAttribute->FK_GameSystem_set(GAMESYSTEM_Arcade_CONST);
      pRow_RomAttribute->FK_RomAttributeType_set(iRomAttributeType);
      pRow_RomAttribute->Name_set(sName);
      pRow_RomAttribute->Table_RomAttribute_get()->Commit();      
    }

  // If Attribute row already exists.
  if (vectRow_RomAttributes.size() > 0)
    {
      bool bDoCommit=false;
      if (vectRow_RomAttributes.size() > 1)
	{
	  LoggerWrapper::GetInstance()->Write(LV_WARNING,"GameDatabase::UpdateRom(%s) - %d rows returned, returning first one.",sRomFile.c_str(),vectRow_RomAttributes.size());
	}
      pRow_RomAttribute=vectRow_RomAttributes[0];
      if (pRow_RomAttribute->FK_GameSystem_get() != GAMESYSTEM_Arcade_CONST)
	{
	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"GameDatabase::UpdateRom(%s) - Updating GameSystem to %d",sRomFile.c_str(),GAMESYSTEM_Arcade_CONST);
	  pRow_RomAttribute->FK_GameSystem_set(GAMESYSTEM_Arcade_CONST);
	  bDoCommit=true;
	}
      if (pRow_RomAttribute->FK_RomAttributeType_get() != iRomAttributeType)
	{
	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"GameDatabase::UpdateRom(%s) - Updating Rom Attribute Type to %d",sRomFile.c_str(),iRomAttributeType);
	  pRow_RomAttribute->FK_RomAttributeType_set(iRomAttributeType);
	  bDoCommit=true;
	}
      if (pRow_RomAttribute->Name_get() != sName)
	{
	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"GameDatabase::UpdateRom(%s) - Updating value to %s",sRomFile.c_str(),sName.c_str());
	  pRow_RomAttribute->Name_set(sName);
	  bDoCommit=true;
	}
      if (bDoCommit)
	{
	  LoggerWrapper::GetInstance()->Write(LV_STATUS,"GameDatabase::UpdateRom(%s) - Changes made, Committing.",sRomFile.c_str());
	  pRow_RomAttribute->Table_RomAttribute_get()->Commit();
	}
     
      m_pDatabase->Rom_RomAttribute_get()->GetRows("WHERE FK_Rom = "+StringUtils::itos(pRow_Rom->PK_Rom_get())+" AND FK_RomAttribute = "+StringUtils::itos(pRow_RomAttribute->PK_RomAttribute_get()),&vectRow_Rom_RomAttributes);
      
      if (vectRow_Rom_RomAttributes.size() == 0)
	{
	  pRow_Rom_RomAttribute = m_pDatabase->Rom_RomAttribute_get()->AddRow();
	  pRow_Rom_RomAttribute->FK_Rom_set(pRow_Rom->PK_Rom_get());
	  pRow_Rom_RomAttribute->FK_RomAttribute_set(pRow_RomAttribute->PK_RomAttribute_get());
	  pRow_Rom_RomAttribute->Table_Rom_RomAttribute_get()->Commit();
	}
      else
	{
	  // Do nothing.
	}
    }
  return true;
}

bool GameDatabase::AddRomAttribute(long int iPK_Rom, int iRomAttributeType, string sName)
{
  Row_Rom *pRow_Rom = m_pDatabase->Rom_get()->GetRow(iPK_Rom);
  Row_RomAttribute *pRow_RomAttribute;
  Row_Rom_RomAttribute *pRow_Rom_RomAttribute;
  vector<Row_RomAttribute *> vectRow_RomAttributes;
  vector<Row_Rom_RomAttribute *> vectRow_Rom_RomAttributes;
  if (!pRow_Rom)
    {
      return false;
    }

  m_pDatabase->RomAttribute_get()->GetRows("WHERE Name = '"+StringUtils::SQLEscape(sName)+"' AND FK_RomAttributeType = '"+StringUtils::itos(iRomAttributeType)+"'",&vectRow_RomAttributes);

  if (vectRow_RomAttributes.size() == 0)
    {
      pRow_RomAttribute = m_pDatabase->RomAttribute_get()->AddRow();
      pRow_RomAttribute->FK_GameSystem_set(GAMESYSTEM_Arcade_CONST);
      pRow_RomAttribute->FK_RomAttributeType_set(iRomAttributeType);
      pRow_RomAttribute->Name_set(sName);
      pRow_RomAttribute->Table_RomAttribute_get()->Commit();
      // LoggerWrapper::GetInstance()->Write(LV_WARNING,"Adding new attribute %s of type %d",sName.c_str(),iRomAttributeType);
    }
  else
    {
      // LoggerWrapper::GetInstance()->Write(LV_WARNING,"Using existing attribute %s of type %d",sName.c_str(),iRomAttributeType);
      pRow_RomAttribute = vectRow_RomAttributes[0];
    }

  m_pDatabase->Rom_RomAttribute_get()->GetRows("WHERE FK_Rom = "+StringUtils::itos(pRow_Rom->PK_Rom_get())+" AND FK_RomAttribute = "+StringUtils::itos(pRow_RomAttribute->PK_RomAttribute_get()),&vectRow_Rom_RomAttributes);

  if (vectRow_Rom_RomAttributes.size() == 0)
    {
      pRow_Rom_RomAttribute = m_pDatabase->Rom_RomAttribute_get()->AddRow();
      pRow_Rom_RomAttribute->FK_Rom_set(pRow_Rom->PK_Rom_get());
      pRow_Rom_RomAttribute->FK_RomAttribute_set(pRow_RomAttribute->PK_RomAttribute_get());
      pRow_Rom_RomAttribute->Table_Rom_RomAttribute_get()->Commit();
    }
  else
    {
      // Do nothing.
    }

  return true;

}
