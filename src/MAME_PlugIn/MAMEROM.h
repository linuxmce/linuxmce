/****************************************/
/* MAMEROM - Class to return metadata 	*/
/* For a given MAME ROM. Currently a  	*/
/* Static table changed per release   	*/
/* for Performance reasons            	*/
/*				      	*/
/* @author <thom.cherryhomes@gmail.com> */
/****************************************/

#define MAME_RELEASE 0.122u7

#include <string>
#include <map>
#include <iostream>

/* sql2cpp classes */
#include "lmce_game/Database_lmce_game.h"  
#include "lmce_game/Define_RomAttributeType.h"
#include "lmce_game/Define_Rom_RomAttribute.h"
#include "lmce_game/TableRow.h"
#include "lmce_game/Table_RomAttributeType.h"  
#include "lmce_game/Table_Rom_RomAttribute.h"
#include "lmce_game/Define_RomAttribute.h"   
#include "lmce_game/Define_Rom.h" 
#include "lmce_game/Define_GameSystem.h"      
#include "lmce_game/Table_RomAttribute.h"  
#include "lmce_game/Table_Rom.h"           
#include "lmce_game/Table_GameSystem.h"

#include "DCE/Logger.h"
#include "PlutoUtils/DBHelper.h"

using namespace std;

	class MAMEROM 
	{
	
			public:
			
			map<string,string> mRomTitleList, mRomYearList, mRomManufacturerList, mRomList, mRomGenreList;	
			Database_lmce_game *m_pMyDatabase;

			string getTitleForROM(string sROMName);
			string getYearForROM(string sROMName);
			string getManufacturerForROM(string sROMName);
			string getGenreForROM(string sROMName);		
			void InitDatabase();

	 		MAMEROM();	
			virtual ~MAMEROM();

	};


