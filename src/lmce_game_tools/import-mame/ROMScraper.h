/**
 * ROMScraper - Scan ROMs and collect checksums.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#ifndef ROMSCRAPER_H
#define ROMSCRAPER_H

#include "MAMEMachine.h"
#include "ImportMame.h"

class ROMScraper
{
 private:
  std::string m_sRomPath;
  class ImportMAME* m_pImportMAME;

 public:

  ROMScraper(class ImportMAME* pImportMAME, string sRomPath);
  virtual ~ROMScraper();

  void processROM(MAMEMachine* m);

};

#endif /* ROMSCRAPER_H */
