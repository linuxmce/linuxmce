/**
 * IdentifierCDDB - identify an inserted disc against FreeDB
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#include "IdentifierCDDB.h"

using namespace DCE;
using namespace std;

IdentifierCDDB::IdentifierCDDB(string sPath) : IdentifierBase(sPath)
{
}

IdentifierCDDB::~IdentifierCDDB()
{
}

bool IdentifierCDDB::Init()
{
  if (!IdentifierBase::Init())
    {
      return false;
    }
  return true;
}

bool IdentifierCDDB::Identify()
{
  return false;
}

string IdentifierCDDB::GetIdentifiedData()
{
  return "";
}

IdentifierBase::eIdentityType IdentifierCDDB::GetIdentityType()
{
  return NONE;
}
