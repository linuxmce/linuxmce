/**
 * main.cpp - main() for lmce_game_utils
 *
 * Author:        Thomas Cherryhomes
 * Version:       1.0
 *
 */

#include "lmce_game_utils.h"
#include <iostream>

int main(int argc, char* argv[])
{
  bool bError = true;
  char c;
  int iRetCode;

  cout << "lmce_game_utils, v. 1.0" << endl;

  LMCE_Game_Utils *pLMCE_Game_Utils = new LMCE_Game_Utils();

  for (int optnum=1;optnum<argc;++optnum)
    {
      c=argv[optnum][1];
      switch (c)
	{
	case 'U':  // update
	  pLMCE_Game_Utils->m_eOperation = LMCE_Game_Utils::OPERATION_UPDATE;
	  bError=false;
	  break;
	case 'D':  // delete
	  pLMCE_Game_Utils->m_eOperation = LMCE_Game_Utils::OPERATION_DELETE;
	  bError=false;
	  break;
	case 'v':  // verbose
	  pLMCE_Game_Utils->m_bVerbose=true;
	  break;
	case 'f':  // Game Filename
	  pLMCE_Game_Utils->m_sRomFilename = argv[++optnum];
	  break;
	case 'n':  // Game MD5
	  pLMCE_Game_Utils->m_sRomMD5 = argv[++optnum];
	  break;
	case 'd':  // Game Description
	  pLMCE_Game_Utils->m_sRomDescription = argv[++optnum];
	  break;
	case 'm':  // Game Manufacturer
	  pLMCE_Game_Utils->m_sRomManufacturer = argv[++optnum];
	  break;
	case 'y':  // Game Year
	  pLMCE_Game_Utils->m_sRomYear = argv[++optnum];
	  break;
	case 'g':  // Game Genre
	  pLMCE_Game_Utils->m_sRomGenre = argv[++optnum];
	  break;
	case 's':  // Game System
	  pLMCE_Game_Utils->m_sRomSystem = argv[++optnum];
	  break;
	}      
    }

  if (bError)
    {
      cout << "Manipulates ROM Entries in lmce_game for inclusion via sqlCVS. " << endl
	   << endl
	   << "lmce_game_utils -UD -n [name] -d [desc] -m [manu] -y [year] -g [genre] -s [system]" << endl
	   << endl
	   << "\t At a minimum, one operation and the option -n must be used." << endl
	   << endl
	   << "\t Operations: " << endl
	   << endl
	   << "\t -U -- Updates a ROM entry given the attributes specified below." << endl
	   << "\t -D -- Deletes a ROM entry given the -n name " << endl
	   << endl
	   << "\t Options: " << endl
	   << endl
	   << "\t -n -- Specify a Game name or MD5" << endl
	   << "\t -d -- Specify a Game Description" << endl
	   << "\t -m -- Specify a Game Manufacturer" << endl
	   << "\t -y -- Specify a Game Year" << endl
	   << "\t -g -- Specify a Game Genre" << endl
	   << "\t -s -- Specify a Game System" << endl;
      exit(1);
    }
  
  if (!pLMCE_Game_Utils->Run(iRetCode))
    {
      bError = true;
    }
  else
    {
      bError = false;
    }
  
  return iRetCode;
}
