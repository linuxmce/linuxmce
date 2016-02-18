/**
 * Database - the class to talk to the sql2cpp stuff.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#include "Database.h"
#include "NameHash.h"
#include <vector>

Database::Database()
{
  m_pDatabase=NULL;
  m_bInitialized=false;
}

Database::~Database()
{
  if (m_pDatabase)
    delete m_pDatabase;

  m_pDatabase=NULL;
}

bool Database::Init()
{
  m_pDatabase = new Database_lmce_game(LoggerWrapper::GetInstance());
  if (!m_pDatabase->Connect("localhost","root","","lmce_game",3306))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::Init() - Unable to connect to database.");
      return false;
    }
  else
    {
      m_bInitialized=true;
      return true;
    }
}

bool Database::NameHashExists(MAMEMachine* m)
{
  if (!m_bInitialized)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::NameHashExists() - Database not initialized, aborting.");
      return false;
    }

  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::NameHashExists() - m is NULL. Aborting.");
      return false;
    }

  vector<class Row_NameHash *> v_RowNameHash;
  string sNameHash = NameHash::FromString(m->MachineDescription_get());
  if (!m_pDatabase->NameHash_get()->GetRows("where NameHash=\""+sNameHash+"\"",&v_RowNameHash))
    {
      return false;
    }
  else
    {
      if (v_RowNameHash.empty())
	{
	  return false;  // does not exist.
	}
      else
	{
	  // FIXME: POSSIBLE LEAK!!!
	  return true;   // does exist.
	}
    }
  
  return false;
}

long int Database::AddNameHash(MAMEMachine *m)
{
  Row_NameHash *pRow_NameHash = m_pDatabase->NameHash_get()->AddRow();
  long int PK_NameHash=0;
  if (pRow_NameHash)
    {
      pRow_NameHash->NameHash_set(NameHash::FromString(m->MachineDescription_get()));
      pRow_NameHash->Original_set(m->MachineDescription_get());
      pRow_NameHash->Normalized_set(NameHash::NormalizeString(m->MachineDescription_get()));
      pRow_NameHash->Table_NameHash_get()->Commit();
      PK_NameHash=pRow_NameHash->PK_NameHash_get();
      // FIXME: ANOTHER POSSIBLE LEAK!!!
      return PK_NameHash;
    }
  return 0;
}

long int Database::GetPKNameHash(MAMEMachine *m)
{
  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::ProcessMachine(m) - m is NULL. Aborting.");
      return false;
    }

  string sWhereQuery = "NameHash = '"+NameHash::FromString(m->MachineDescription_get())+"'";
  vector<class Row_NameHash *> v_RowNameHash;
  if (!m_pDatabase->NameHash_get()->GetRows(sWhereQuery,&v_RowNameHash))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetPKNameHash() - could not fetch rows. Aborting.");
      return 0;
    }
  if (v_RowNameHash.size()==0)
    {
      return 0;
    }
  else
    {
      Row_NameHash* pRow_NameHash = v_RowNameHash[0];
      if (!pRow_NameHash)
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetPKNameHash() - How in the hell did we get a NULL?");
	  return 0;
	}
      else
	{
	  return pRow_NameHash->PK_NameHash_get();
	}
    }
  return 0;
  
}

bool Database::GameExists(MAMEMachine* m)
{
  if (!m_bInitialized)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GameExists() - Database not initialized, aborting.");
      return false;
    }

  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GameExists() - m is NULL. Aborting.");
      return false;
    }

  vector<class Row_Game *> v_RowGame;
  if (!m_pDatabase->Game_get()->GetRows("where FK_NameHash=\""+StringUtils::itos(m->liPK_NameHash_get())+"\" AND Subtitle=\""+m->MachineSubDescription_get()+"\"",&v_RowGame))
    {
      return false;
    }
  else
    {
      if (v_RowGame.empty())
	{
	  return false;  // does not exist.
	}
      else
	{
	  // FIXME: POSSIBLE LEAK!!!
	  return true;   // does exist.
	}
    }
  
  return false;
  
}

long int Database::AddGame(MAMEMachine* m)
{
  Row_Game *pRow_Game = m_pDatabase->Game_get()->AddRow();
  long int PK_Game=0;
  if (pRow_Game)
    {
      pRow_Game->FK_NameHash_set(m->liPK_NameHash_get());
      pRow_Game->Subtitle_set(m->MachineSubDescription_get());
      pRow_Game->Table_Game_get()->Commit();
      PK_Game=pRow_Game->PK_Game_get();
      // FIXME: ANOTHER POSSIBLE LEAK!!!
      return PK_Game;
    }
  return 0;
}

long int Database::GetPKGame(MAMEMachine* m)
{
  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::ProcessMachine(m) - m is NULL. Aborting.");
      return false;
    }

  string sWhereQuery = "FK_NameHash = '"+StringUtils::itos(m->liPK_NameHash_get())+"' AND Subtitle=\""+m->MachineSubDescription_get()+"\"";
  vector<class Row_Game *> v_RowGame;
  if (!m_pDatabase->Game_get()->GetRows(sWhereQuery,&v_RowGame))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetPKGame() - could not fetch rows. Aborting.");
      return 0;
    }
  if (v_RowGame.size()==0)
    {
      return 0;
    }
  else
    {
      Row_Game* pRow_Game = v_RowGame[0];
      if (!pRow_Game)
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetPKGame() - How in the hell did we get a NULL?");
	  return 0;
	}
      else
	{
	  return pRow_Game->PK_Game_get();
	}
    }
  return 0;
}

bool Database::ProcessMachine(MAMEMachine* m)
{
  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::ProcessMachine(m) - m is NULL. Aborting.");
      return false;
    }

  // Add NameHash if needed, otherwise query, come out with a PK_NameHash
  long int liPK_NameHash;

  if (!NameHashExists(m))
    {
      // Add Name Hash
      liPK_NameHash=AddNameHash(m);
    }
  else
    {
      liPK_NameHash=GetPKNameHash(m);
    }

  m->liPK_NameHash_set(liPK_NameHash);

  // Add Game entry if needed, otherwise query, come out with a PK_Game

  long int liPK_Game;
  if (!GameExists(m))
    {
      // Add Game Entry
      liPK_Game=AddGame(m);
    }
  else
    {
      liPK_Game=GetPKGame(m);
    }

  m->liPK_Game_set(liPK_Game);

  // Add Manufacturer entry if needed, otherwise query, come out with a PK_Manufacturer

  long int liPK_Manufacturer;
  if (!ManufacturerExists(m))
    {
      // Add Game Entry
      liPK_Manufacturer=AddManufacturer(m);
    }
  else
    {
      liPK_Manufacturer=GetPKManufacturer(m);
    }

  m->liPK_Manufacturer_set(liPK_Manufacturer);

  return false;
}
