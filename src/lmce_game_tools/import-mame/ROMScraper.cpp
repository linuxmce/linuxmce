/**
 * ROMScraper - Scan ROMs and collect checksums.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#include "ROMScraper.h"
#include "PlutoUtils/FileUtils.h"
#include <string>
#include <iostream>

using namespace std;

ROMScraper::ROMScraper(string sRomPath)
{
  m_sRomPath=sRomPath;
}

ROMScraper::~ROMScraper()
{

}

void ROMScraper::processROM(MAMEMachine* m)
{
  string sROMNamePrefix = m_sRomPath + "/" + m->MachineName_get();
  string sROMName = "";
  string sROMSHA1 = "";

  if (FileUtils::FileExists(sROMNamePrefix+".zip"))
    {
      sROMName=sROMNamePrefix+".zip";
    }
  else if (FileUtils::FileExists(sROMNamePrefix+".7z"))
    {
      sROMName=sROMNamePrefix+".7z";
    }

  m->MachineRomSHA1_set(FileUtils::FileSHA1(sROMName));

}
