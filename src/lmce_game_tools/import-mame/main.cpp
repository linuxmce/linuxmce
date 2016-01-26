/** 
 * import-mame - A tool to parse mame -listxml and output metadata
 * for lmce_game v2
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Start Date: 2016-01-24
 */

// PlutoUtils Defines ///////////////
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

// DCE Defines //////////////////////
#include "DCE/Logger.h"

#include "ImportMame.h"

// Other Defines
#include <string>
#include <vector>
#include <map>

int main(int argc, char* argv[])
{
  bool bError = true;  // Set when there was an error parsing command line.
  string sMamePath;    // Path to MAME binary.
  string sCategoryPath; // Path to Category.ini file.
  char c;              // Current Option.
  int iRetCode;        // Return code

  cout << "import-mame, v. 2.0" << endl;

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
	  if (argc > 4)
	    {
	      sMamePath = argv[++optnum];
	      bError=false;
	    }
	  break;
	case 'c':
	  if (argc > 4)
	    {
	      sCategoryPath = argv[++optnum];
	      bError=false;
	    }
	}
    }

  if (bError) 
    {
      cout << "Imports new MAME ROMs into lmce_game for inclusion via sqlCVS." << endl
	   << endl 
	   << "Usage: import-mame -p /path/to/mame" << endl
	   << endl
	   << "\t -p -- The Path to the MAME binary. (Required)" << endl
	   << "\t -c -- The Path to the Category.ini file." << endl;
      exit(1);
    }

  ImportMAME *pImportMAME = new ImportMAME(sMamePath,sCategoryPath);
  iRetCode = pImportMAME->Run();

  return iRetCode;
  
}
