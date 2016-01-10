/**
 * A little utility to return a title hash based on the StringUtils::TitleHash function.
 * This utility is intended to be used in shell scripts.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#include "PlutoUtils/StringUtils.h"
#include <string>
#include <iostream>

int main(int argc, char* argv[])
{
  if (argc<2)
    {
      string sProcessName = argv[0];
      cerr << "usage: " << sProcessName << " <title>" << endl;
      return 1;
    }

  string sTitle = argv[1];
  string sHash = StringUtils::TitleHash(sTitle);
  
  cout << sHash;

  return 0;
}
