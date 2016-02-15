/**
 * NameHash - Generate a namehash given input
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#include "NameHash.h"
#include "PlutoUtils/StringUtils.h"
#include <iostream>

string NameHash::NormalizeString(string sString)
{
  string sNormalizedString;
  for (size_t i=0;i<sString.size();++i)
    {
      if ((sString[i]>='0' && sString[i]<='9') || (sString[i]>='A' && sString[i]<='Z'))
	{
	  sNormalizedString+=sString[i];
	}
      else if ((sString[i]>='a' && sString[i]<='z'))
	{
	  sNormalizedString+=sString[i]-32;
	}
      else if (sString[i]=='(' || sString[i]==')' || sString[i]=='[' || sString[i]==']')
	{
	  /* we don't want ancilliary attributes embedded after the name. stop. */
	  break;
	}
    }

  return sNormalizedString;

}

string NameHash::FromString(string sString)
{
  return StringUtils::StringSHA1(NameHash::NormalizeString(sString));
}
