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

bool Database::ManufacturerNameHashExists(MAMEMachine* m)
{
  if (!m_bInitialized)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::ManufacturerNameHashExists() - Database not initialized, aborting.");
      return false;
    }

  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::ManufacturerNameHashExists() - m is NULL. Aborting.");
      return false;
    }

  vector<class Row_NameHash *> v_RowNameHash;
  string sNameHash = NameHash::FromString(m->MachineManufacturer_get());
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

long int Database::AddManufacturerNameHash(MAMEMachine *m)
{
  Row_NameHash *pRow_NameHash = m_pDatabase->NameHash_get()->AddRow();
  long int PK_NameHash=0;
  if (pRow_NameHash)
    {
      pRow_NameHash->NameHash_set(NameHash::FromString(m->MachineManufacturer_get()));
      pRow_NameHash->Original_set(m->MachineManufacturer_get());
      pRow_NameHash->Normalized_set(NameHash::NormalizeString(m->MachineManufacturer_get()));
      pRow_NameHash->Table_NameHash_get()->Commit();
      PK_NameHash=pRow_NameHash->PK_NameHash_get();
      // FIXME: ANOTHER POSSIBLE LEAK!!!
      return PK_NameHash;
    }
  return 0;
}

long int Database::GetManufacturerPKNameHash(MAMEMachine *m)
{
  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::ProcessMachine(m) - m is NULL. Aborting.");
      return false;
    }

  string sWhereQuery = "NameHash = '"+NameHash::FromString(m->MachineManufacturer_get())+"'";
  vector<class Row_NameHash *> v_RowNameHash;
  if (!m_pDatabase->NameHash_get()->GetRows(sWhereQuery,&v_RowNameHash))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetManufacturerPKNameHash() - could not fetch rows. Aborting.");
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
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetManufacturerPKNameHash() - How in the hell did we get a NULL?");
	  return 0;
	}
      else
	{
	  return pRow_NameHash->PK_NameHash_get();
	}
    }
  return 0;
}

bool Database::ManufacturerExists(MAMEMachine* m)
{
  if (!m_bInitialized)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::ManufacturerExists() - Database not initialized, aborting.");
      return false;
    }

  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::ManufacturerExists() - m is NULL. Aborting.");
      return false;
    }

  vector<class Row_Manufacturer *> v_RowManufacturer;
  if (!m_pDatabase->Manufacturer_get()->GetRows("FK_NameHash="+StringUtils::itos(m->liPK_Manufacturer_NameHash_get()),&v_RowManufacturer))
    {
      return false;
    }
  else
    {
      if (v_RowManufacturer.empty())
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

long int Database::AddManufacturer(MAMEMachine* m)
{
  Row_Manufacturer *pRow_Manufacturer = m_pDatabase->Manufacturer_get()->AddRow();
  long int PK_Manufacturer=0;
  if (pRow_Manufacturer)
    {
      pRow_Manufacturer->FK_NameHash_set(m->liPK_Manufacturer_NameHash_get());
      pRow_Manufacturer->Table_Manufacturer_get()->Commit();
      PK_Manufacturer=pRow_Manufacturer->PK_Manufacturer_get();
      // FIXME: ANOTHER POSSIBLE LEAK!!!
      return PK_Manufacturer;
    }
  return 0;
}

long int Database::GetPKManufacturer(MAMEMachine* m)
{
  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::ProcessMachine(m) - m is NULL. Aborting.");
      return false;
    }

  string sWhereQuery = "FK_NameHash = "+StringUtils::itos(m->liPK_Manufacturer_NameHash_get());
  vector<class Row_Manufacturer *> v_RowManufacturer;
  if (!m_pDatabase->Manufacturer_get()->GetRows(sWhereQuery,&v_RowManufacturer))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetPKManufacturer() - could not fetch rows. Aborting.");
      return 0;
    }
  if (v_RowManufacturer.size()==0)
    {
      return 0;
    }
  else
    {
      Row_Manufacturer* pRow_Manufacturer = v_RowManufacturer[0];
      if (!pRow_Manufacturer)
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetPKManufacturer() - How in the hell did we get a NULL?");
	  return 0;
	}
      else
	{
	  return pRow_Manufacturer->PK_Manufacturer_get();
	}
    }
  return 0;
}

bool Database::GenreNameHashExists(MAMEMachine* m)
{
  if (!m_bInitialized)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GenreNameHashExists() - Database not initialized, aborting.");
      return false;
    }

  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GenreNameHashExists() - m is NULL. Aborting.");
      return false;
    }

  vector<class Row_NameHash *> v_RowNameHash;
  string sNameHash = NameHash::FromString(m->MachineGenre_get());
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

long int Database::AddGenreNameHash(MAMEMachine *m)
{
  Row_NameHash *pRow_NameHash = m_pDatabase->NameHash_get()->AddRow();
  long int PK_NameHash=0;
  if (pRow_NameHash)
    {
      pRow_NameHash->NameHash_set(NameHash::FromString(m->MachineGenre_get()));
      pRow_NameHash->Original_set(m->MachineGenre_get());
      pRow_NameHash->Normalized_set(NameHash::NormalizeString(m->MachineGenre_get()));
      pRow_NameHash->Table_NameHash_get()->Commit();
      PK_NameHash=pRow_NameHash->PK_NameHash_get();
      // FIXME: ANOTHER POSSIBLE LEAK!!!
      return PK_NameHash;
    }
  return 0;
}

long int Database::GetGenrePKNameHash(MAMEMachine *m)
{
  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::ProcessMachine(m) - m is NULL. Aborting.");
      return false;
    }

  string sWhereQuery = "NameHash = '"+NameHash::FromString(m->MachineGenre_get())+"'";
  vector<class Row_NameHash *> v_RowNameHash;
  if (!m_pDatabase->NameHash_get()->GetRows(sWhereQuery,&v_RowNameHash))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetGenrePKNameHash() - could not fetch rows. Aborting.");
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
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetGenrePKNameHash() - How in the hell did we get a NULL?");
	  return 0;
	}
      else
	{
	  return pRow_NameHash->PK_NameHash_get();
	}
    }
  return 0;
}

bool Database::GenreExists(MAMEMachine* m)
{
  if (!m_bInitialized)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GenreExists() - Database not initialized, aborting.");
      return false;
    }

  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GenreExists() - m is NULL. Aborting.");
      return false;
    }

  vector<class Row_Genre *> v_RowGenre;
  if (!m_pDatabase->Genre_get()->GetRows("FK_NameHash="+StringUtils::itos(m->liPK_Genre_NameHash_get()),&v_RowGenre))
    {
      return false;
    }
  else
    {
      if (v_RowGenre.empty())
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

long int Database::AddGenre(MAMEMachine* m)
{
  Row_Genre *pRow_Genre = m_pDatabase->Genre_get()->AddRow();
  long int PK_Genre=0;
  if (pRow_Genre)
    {
      pRow_Genre->FK_NameHash_set(m->liPK_Genre_NameHash_get());
      pRow_Genre->Table_Genre_get()->Commit();
      PK_Genre=pRow_Genre->PK_Genre_get();
      // FIXME: ANOTHER POSSIBLE LEAK!!!
      return PK_Genre;
    }
  return 0;
}

long int Database::GetPKGenre(MAMEMachine* m)
{
  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::ProcessMachine(m) - m is NULL. Aborting.");
      return false;
    }

  string sWhereQuery = "FK_NameHash = "+StringUtils::itos(m->liPK_Genre_NameHash_get());
  vector<class Row_Genre *> v_RowGenre;
  if (!m_pDatabase->Genre_get()->GetRows(sWhereQuery,&v_RowGenre))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetPKGenre() - could not fetch rows. Aborting.");
      return 0;
    }
  if (v_RowGenre.size()==0)
    {
      return 0;
    }
  else
    {
      Row_Genre* pRow_Genre = v_RowGenre[0];
      if (!pRow_Genre)
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetPKGenre() - How in the hell did we get a NULL?");
	  return 0;
	}
      else
	{
	  return pRow_Genre->PK_Genre_get();
	}
    }
  return 0;
}

bool Database::GameGameSystemExists(MAMEMachine* m)
{
  if (!m_bInitialized)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GameGameSystemExists() - Database not initialized, aborting.");
      return false;
    }

  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GameGameSystemExists() - m is NULL. Aborting.");
      return false;
    }

  vector<class Row_Game_GameSystem *> v_RowGameGameSystem;
  if (!m_pDatabase->Game_GameSystem_get()->GetRows("FK_Game="+StringUtils::itos(m->liPK_Game_get())+" AND FK_GameSystem="+StringUtils::itos(GAMESYSTEM_MAME_CONST)+" AND FK_Manufacturer="+StringUtils::itos(m->liPK_Manufacturer_get())+" AND FK_Genre="+StringUtils::itos(m->liPK_Genre_get()),&v_RowGameGameSystem))
    {
      return false;
    }
  else
    {
      if (v_RowGameGameSystem.empty())
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

long int Database::AddGameGameSystem(MAMEMachine* m)
{
  Row_Game_GameSystem *pRow_Game_GameSystem = m_pDatabase->Game_GameSystem_get()->AddRow();
  long int PK_Game_GameSystem=0;
  if (pRow_Game_GameSystem)
    {
      pRow_Game_GameSystem->FK_Game_set(m->liPK_Game_get());
      pRow_Game_GameSystem->FK_GameSystem_set(GAMESYSTEM_MAME_CONST);
      pRow_Game_GameSystem->FK_Manufacturer_set(m->liPK_Manufacturer_get());
      pRow_Game_GameSystem->FK_Genre_set(m->liPK_Genre_get());
      pRow_Game_GameSystem->Year_set(atoi(m->MachineYear_get().c_str()));
      pRow_Game_GameSystem->Table_Game_GameSystem_get()->Commit();
      PK_Game_GameSystem=pRow_Game_GameSystem->PK_Game_GameSystem_get();
      // FIXME: ANOTHER POSSIBLE LEAK!!!
      return PK_Game_GameSystem;
    }
  return 0;
}

long int Database::GetPKGameGameSystem(MAMEMachine* m)
{
  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::ProcessMachine(m) - m is NULL. Aborting.");
      return false;
    }

  string sWhereQuery = "FK_Game="+StringUtils::itos(m->liPK_Game_get())+" AND FK_GameSystem="+StringUtils::itos(GAMESYSTEM_MAME_CONST)+" AND FK_Manufacturer="+StringUtils::itos(m->liPK_Manufacturer_get())+" AND FK_Genre="+StringUtils::itos(m->liPK_Genre_get());
  vector<class Row_Game_GameSystem *> v_RowGame_GameSystem;
  if (!m_pDatabase->Game_GameSystem_get()->GetRows(sWhereQuery,&v_RowGame_GameSystem))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetPKGame_GameSystem() - could not fetch rows. Aborting.");
      return 0;
    }
  if (v_RowGame_GameSystem.size()==0)
    {
      return 0;
    }
  else
    {
      Row_Game_GameSystem* pRow_Game_GameSystem = v_RowGame_GameSystem[0];
      if (!pRow_Game_GameSystem)
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetPKGame_GameSystem() - How in the hell did we get a NULL?");
	  return 0;
	}
      else
	{
	  return pRow_Game_GameSystem->PK_Game_GameSystem_get();
	}
    }
  return 0;
}

bool Database::RomExists(MAMEMachine* m)
{
  if (!m_bInitialized)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::RomExists() - Database not initialized, aborting.");
      return false;
    }

  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::RomExists() - m is NULL. Aborting.");
      return false;
    }

  if (m->MachineRomSHA1_get().empty())
    return true; // Pretend rom exists, if there is no hash, as it is not needed.

  vector<class Row_Rom *> v_RowRom;
  if (!m_pDatabase->Rom_get()->GetRows("where IK_Hash=\""+m->MachineRomSHA1_get()+"\"",&v_RowRom))
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
	  // FIXME: POSSIBLE LEAK!!!
	  return true;   // does exist.
	}
    }
  
  return false;
  
}

long int Database::AddRom(MAMEMachine* m)
{
  if (m->MachineRomSHA1_get().empty())
    return 0;

  Row_Rom *pRow_Rom = m_pDatabase->Rom_get()->AddRow();
  long int PK_Rom=0;
  if (pRow_Rom)
    {
      pRow_Rom->IK_Hash_set(m->MachineRomSHA1_get());
      pRow_Rom->Table_Rom_get()->Commit();
      PK_Rom=pRow_Rom->PK_Rom_get();
      // FIXME: ANOTHER POSSIBLE LEAK!!!
      return PK_Rom;
    }
  return 0;
}

long int Database::GetPKRom(MAMEMachine* m)
{
  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::ProcessMachine(m) - m is NULL. Aborting.");
      return false;
    }

  if (m->MachineRomSHA1_get().empty())
    return 0;

  string sWhereQuery = "IK_Hash=\""+m->MachineRomSHA1_get()+"\"";
  vector<class Row_Rom *> v_RowRom;
  if (!m_pDatabase->Rom_get()->GetRows(sWhereQuery,&v_RowRom))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetPKRom() - could not fetch rows. Aborting.");
      return 0;
    }
  if (v_RowRom.size()==0)
    {
      return 0;
    }
  else
    {
      Row_Rom* pRow_Rom = v_RowRom[0];
      if (!pRow_Rom)
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetPKRom() - How in the hell did we get a NULL?");
	  return 0;
	}
      else
	{
	  return pRow_Rom->PK_Rom_get();
	}
    }
  return 0;
}

bool Database::GameGameSystemRomExists(MAMEMachine* m)
{
  if (!m_bInitialized)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GameGameSystemRomExists() - Database not initialized, aborting.");
      return false;
    }

  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GameGameSystemRomExists() - m is NULL. Aborting.");
      return false;
    }

  if (m->MachineRomSHA1_get().empty())
    return true; // Pretend rom exists, if there is no hash, as it is not needed.

  vector<class Row_Game_GameSystem_Rom *> v_RowGameGameSystemRom;
  if (!m_pDatabase->Game_GameSystem_Rom_get()->GetRows("FK_Game="+StringUtils::itos(m->liPK_Game_get())+" AND FK_GameSystem="+StringUtils::itos(GAMESYSTEM_MAME_CONST)+" AND FK_Rom="+StringUtils::itos(m->liPK_Rom_get()),&v_RowGameGameSystemRom))
    {
      return false;
    }
  else
    {
      if (v_RowGameGameSystemRom.empty())
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

long int Database::AddGameGameSystemRom(MAMEMachine* m)
{
  if (m->MachineRomSHA1_get().empty())
    return 0;

  Row_Game_GameSystem_Rom *pRow_GameGameSystemRom = m_pDatabase->Game_GameSystem_Rom_get()->AddRow();
  long int PK_GameGameSystemRom=0;
  if (pRow_GameGameSystemRom)
    {
      pRow_GameGameSystemRom->FK_Game_set(m->liPK_Game_get());
      pRow_GameGameSystemRom->FK_GameSystem_set(GAMESYSTEM_MAME_CONST);
      pRow_GameGameSystemRom->FK_Rom_set(m->liPK_Rom_get());
      pRow_GameGameSystemRom->Table_Game_GameSystem_Rom_get()->Commit();
      PK_GameGameSystemRom=pRow_GameGameSystemRom->PK_Game_GameSystem_Rom_get();
      // FIXME: ANOTHER POSSIBLE LEAK!!!
      return PK_GameGameSystemRom;
    }
  return 0;
}

long int Database::GetPKGameGameSystemRom(MAMEMachine* m)
{
  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::ProcessMachine(m) - m is NULL. Aborting.");
      return false;
    }

  if (m->MachineRomSHA1_get().empty())
    return 0;

  string sWhereQuery = "FK_Game="+StringUtils::itos(m->liPK_Game_get())+" AND FK_GameSystem="+StringUtils::itos(GAMESYSTEM_MAME_CONST)+" AND FK_Rom="+StringUtils::itos(m->liPK_Rom_get());
  vector<class Row_Game_GameSystem_Rom *> v_RowGameGameSystemRom;
  if (!m_pDatabase->Game_GameSystem_Rom_get()->GetRows(sWhereQuery,&v_RowGameGameSystemRom))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetPKGameGameSystemRom() - could not fetch rows. Aborting.");
      return 0;
    }
  if (v_RowGameGameSystemRom.size()==0)
    {
      return 0;
    }
  else
    {
      Row_Game_GameSystem_Rom* pRow_GameGameSystemRom = v_RowGameGameSystemRom[0];
      if (!pRow_GameGameSystemRom)
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetPKGameGameSystemRom() - How in the hell did we get a NULL?");
	  return 0;
	}
      else
	{
	  return pRow_GameGameSystemRom->PK_Game_GameSystem_Rom_get();
	}
    }
  return 0;
}

bool Database::PictureExists(MAMEMachine* m)
{
  if (!m_bInitialized)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::PictureExists() - Database not initialized, aborting.");
      return false;
    }

  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::PictureExists() - m is NULL. Aborting.");
      return false;
    }

  vector<class Row_Picture *> v_RowPicture;
  if (!m_pDatabase->Picture_get()->GetRows("OriginalFilename=\""+m->MachineName_get()+".png\"",&v_RowPicture))
    {
      return false;
    }
  else
    {
      if (v_RowPicture.empty())
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

long int Database::AddPicture(MAMEMachine* m)
{
  Row_Picture *pRow_Picture = m_pDatabase->Picture_get()->AddRow();
  long int PK_Picture=0;
  if (pRow_Picture)
    {
      pRow_Picture->OriginalFilename_set(m->MachineName_get()+".png");
      pRow_Picture->Table_Picture_get()->Commit();
      PK_Picture=pRow_Picture->PK_Picture_get();
      // FIXME: ANOTHER POSSIBLE LEAK!!!
      return PK_Picture;
    }
  return 0;
}

long int Database::GetPKPicture(MAMEMachine* m)
{
  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::ProcessMachine(m) - m is NULL. Aborting.");
      return false;
    }

  string sWhereQuery = "OriginalFilename=\""+m->MachineName_get()+".png\"";
  vector<class Row_Picture *> v_RowPicture;
  if (!m_pDatabase->Picture_get()->GetRows(sWhereQuery,&v_RowPicture))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetPKPicture() - could not fetch rows. Aborting.");
      return 0;
    }
  if (v_RowPicture.size()==0)
    {
      return 0;
    }
  else
    {
      Row_Picture* pRow_Picture = v_RowPicture[0];
      if (!pRow_Picture)
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetPKPicture() - How in the hell did we get a NULL?");
	  return 0;
	}
      else
	{
	  return pRow_Picture->PK_Picture_get();
	}
    }
  return 0;
}

bool Database::GameGameSystemPictureExists(MAMEMachine* m)
{
  if (!m_bInitialized)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::Game_GameSystem_PictureExists() - Database not initialized, aborting.");
      return false;
    }

  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::Game_GameSystem_PictureExists() - m is NULL. Aborting.");
      return false;
    }

  vector<class Row_Game_GameSystem_Picture *> v_RowGame_GameSystem_Picture;
  if (!m_pDatabase->Game_GameSystem_Picture_get()->GetRows("FK_Game=\""+StringUtils::itos(m->liPK_Game_get())+"\" AND FK_GameSystem=\""+StringUtils::itos(GAMESYSTEM_MAME_CONST)+"\" AND FK_Picture=\""+StringUtils::itos(m->liPK_Picture_get())+"\"",&v_RowGame_GameSystem_Picture))
    {
      return false;
    }
  else
    {
      if (v_RowGame_GameSystem_Picture.empty())
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

long int Database::AddGameGameSystemPicture(MAMEMachine* m)
{
  Row_Game_GameSystem_Picture *pRow_Game_GameSystem_Picture = m_pDatabase->Game_GameSystem_Picture_get()->AddRow();
  long int PK_Game_GameSystem_Picture=0;
  if (pRow_Game_GameSystem_Picture)
    {
      pRow_Game_GameSystem_Picture->FK_Game_set(m->liPK_Game_get());
      pRow_Game_GameSystem_Picture->FK_GameSystem_set(GAMESYSTEM_MAME_CONST);
      pRow_Game_GameSystem_Picture->FK_Picture_set(m->liPK_Picture_get());
      pRow_Game_GameSystem_Picture->Table_Game_GameSystem_Picture_get()->Commit();
      PK_Game_GameSystem_Picture=pRow_Game_GameSystem_Picture->PK_Game_GameSystem_Picture_get();
      // FIXME: ANOTHER POSSIBLE LEAK!!!
      return PK_Game_GameSystem_Picture;
    }
  return 0;
}

long int Database::GetPKGameGameSystemPicture(MAMEMachine* m)
{
  if (!m)
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::ProcessMachine(m) - m is NULL. Aborting.");
      return false;
    }

  string sWhereQuery = "FK_Game=\""+StringUtils::itos(m->liPK_Game_get())+"\" AND FK_GameSystem=\""+StringUtils::itos(GAMESYSTEM_MAME_CONST)+"\" AND FK_Picture=\""+StringUtils::itos(m->liPK_Picture_get())+"\"";
  vector<class Row_Game_GameSystem_Picture *> v_RowGame_GameSystem_Picture;
  if (!m_pDatabase->Game_GameSystem_Picture_get()->GetRows(sWhereQuery,&v_RowGame_GameSystem_Picture))
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetPKGame_GameSystem_Picture() - could not fetch rows. Aborting.");
      return 0;
    }
  if (v_RowGame_GameSystem_Picture.size()==0)
    {
      return 0;
    }
  else
    {
      Row_Game_GameSystem_Picture* pRow_Game_GameSystem_Picture = v_RowGame_GameSystem_Picture[0];
      if (!pRow_Game_GameSystem_Picture)
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Database::GetPKGame_GameSystem_Picture() - How in the hell did we get a NULL?");
	  return 0;
	}
      else
	{
	  return pRow_Game_GameSystem_Picture->PK_Game_GameSystem_Picture_get();
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
  // first, a manufacturer namehash, if needed.
  long int liPK_Manufacturer_NameHash;
  if (!ManufacturerNameHashExists(m))
    {
      // Add Entry
      liPK_Manufacturer_NameHash=AddManufacturerNameHash(m);
    }
  else
    {
      liPK_Manufacturer_NameHash=GetManufacturerPKNameHash(m);
    }

  m->liPK_Manufacturer_NameHash_set(liPK_Manufacturer_NameHash);

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

 // Add Genre entry if needed, otherwise query, come out with a PK_Genre
  // first, a manufacturer namehash, if needed.
  long int liPK_Genre_NameHash;
  if (!GenreNameHashExists(m))
    {
      // Add Entry
      liPK_Genre_NameHash=AddGenreNameHash(m);
    }
  else
    {
      liPK_Genre_NameHash=GetGenrePKNameHash(m);
    }

  m->liPK_Genre_NameHash_set(liPK_Genre_NameHash);

  long int liPK_Genre;
  if (!GenreExists(m))
    {
      // Add Game Entry
      liPK_Genre=AddGenre(m);
    }
  else
    {
      liPK_Genre=GetPKGenre(m);
    }

  m->liPK_Genre_set(liPK_Genre);

  // Add GameGameSystem entry if needed, otherwise query, come out with a PK_GameGameSystem

  long int liPK_GameGameSystem;
  if (!GameGameSystemExists(m))
    {
      // Add GameGameSystem Entry
      liPK_GameGameSystem=AddGameGameSystem(m);
    }
  else
    {
      liPK_GameGameSystem=GetPKGameGameSystem(m);
    }

  m->liPK_Game_GameSystem_set(liPK_GameGameSystem);

  // Add Rom entry, if needed, otherwise query, come out with a PK_Rom
  long int liPK_Rom;
  if (!RomExists(m))
    {
      // Add Rom Entry
      liPK_Rom=AddRom(m);
    }
  else
    {
      liPK_Rom=GetPKRom(m);
    }  

  m->liPK_Rom_set(liPK_Rom);
  
  // Add Game_GameSystem_Rom entry, if needed, otherwise query, come out with PK_Game_GameSystem_Rom
  long int liPK_Game_GameSystem_Rom;
  if (!GameGameSystemRomExists(m))
    {
      // Add Game_GameSystem_Rom Entry
      liPK_Game_GameSystem_Rom=AddGameGameSystemRom(m);
    }
  else
    {
      liPK_Game_GameSystem_Rom=GetPKGameGameSystemRom(m);
    }  

  m->liPK_Game_GameSystem_Rom_set(liPK_Game_GameSystem_Rom);
  
  // Add Picture entry if needed, otherwise query, come out with a PK_Picture
  
  long int liPK_Picture;
  if (!PictureExists(m))
    {
      // Add Picture Entry
      liPK_Picture=AddPicture(m);
    }
  else
    {
      liPK_Picture=GetPKPicture(m);
    }
  
  m->liPK_Picture_set(liPK_Picture);

  // Add Game_GameSystem_Picture entry if needed, otherwise query, come out with a PK_Picture
  
  long int liPK_Game_GameSystem_Picture;
  if (!GameGameSystemPictureExists(m))
    {
      // Add Picture Entry
      liPK_Game_GameSystem_Picture=AddGameGameSystemPicture(m);
    }
  else
    {
      liPK_Game_GameSystem_Picture=GetPKGameGameSystemPicture(m);
    }
  
  m->liPK_Game_GameSystem_Picture_set(liPK_Game_GameSystem_Picture);

  return false;
}
