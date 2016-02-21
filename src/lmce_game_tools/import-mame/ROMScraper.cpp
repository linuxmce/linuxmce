/**
 * ROMScraper - Scan ROMs and collect checksums.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#include "ROMScraper.h"
#include "PlutoUtils/FileUtils.h"
#include "DCE/Logger.h"
#include <string>
#include <iostream>

using namespace std;

ROMScraper::ROMScraper(class ImportMAME* pImportMAME, string sRomPath)
{
  m_pImportMAME=pImportMAME;
  m_sRomPath=sRomPath;
}

ROMScraper::~ROMScraper()
{

}

void ROMScraper::processROM(MAMEMachine* m)
{
  string sROMNamePrefix = m_sRomPath + "/" + m->MachineName_get();
  string sROMOfPrefix = m_sRomPath + "/" + m->MachineRomOf_get();
  string sROMName = "";
  string sROMSHA1 = "";

  // First check the ROM referenced by name
  if (FileUtils::FileExists(sROMNamePrefix+".zip"))
    {
      sROMName=sROMNamePrefix+".zip";
    }
  else if (FileUtils::FileExists(sROMNamePrefix+".7z"))
    {
      sROMName=sROMNamePrefix+".7z";
    }

  // If sROMName isn't set, it wasn't found. check the romOf parent
  if (sROMName.empty())
    {
      if (FileUtils::FileExists(sROMOfPrefix+".zip"))
	{
	  sROMName=sROMOfPrefix+".zip";
	}
      else if (FileUtils::FileExists(sROMOfPrefix+".7z"))
	{
	  sROMName=sROMOfPrefix+".7z";
	}
      else
	{
	  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"ROMScraper::processROM - Could not find file for Rom %s or RomOf %s, no checksum calculated.");	
	}
    }

  m->MachineRomSHA1_set(FileUtils::FileSHA1(sROMName));

}
