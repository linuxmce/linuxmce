/**
 * Database - the class to talk to the sql2cpp stuff.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#include "Database.h"

Database::Database()
{
  m_pDatabase=NULL;
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
      return true;
    }
}

