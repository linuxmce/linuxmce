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
  string sWhereQuery = "WHERE NameHash = '"+NameHash::FromString(m->MachineDescription_get())+"'";
  if (!m_pDatabase->NameHash_get()->GetRows(sWhereQuery,&v_RowNameHash))
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
	  return true;   // does exist.
	}
    }
}

long int Database::AddNameHash(MAMEMachine *m)
{
  Row_NameHash *pRow_NameHash = m_pDatabase->NameHash_get()->AddRow();
  if (pRow_NameHash)
    {
      pRow_NameHash->NameHash_set(NameHash::FromString(m->MachineDescription_get()));
      pRow_NameHash->Original_set(m->MachineDescription_get());
      pRow_NameHash->Normalized_set(NameHash::NormalizeString(m->MachineDescription_get()));
      pRow_NameHash->Table_NameHash_get()->Commit();
      return pRow_NameHash->PK_NameHash_get();
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

  return false;
}
