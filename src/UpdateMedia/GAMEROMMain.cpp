/* quick game rom query */

#include "GAMEROM.h"
#include <string>
#include <iostream>

int main(int argc, char *argv[])
{

	if (argc > 1)
	{
		GAMEROM *m_pGAMEROM;
		m_pGAMEROM = new(GAMEROM);

		m_pGAMEROM->InitDatabase();

		string sROM = argv[1];
		string sROMTitle = m_pGAMEROM->getTitleForROM(sROM);
 		string sROMYear = m_pGAMEROM->getYearForROM(sROM);
		string sROMManufacturer = m_pGAMEROM->getManufacturerForROM(sROM);
 		string sROMGenre = m_pGAMEROM->getGenreForROM(sROM);
	
		cout << sROMTitle << endl;
		cout << sROMYear << endl;
		cout << sROMManufacturer << endl;
		cout << sROMGenre << endl;
	}
	return 0;

}
