/**
 * ROMScraper - Scan ROMs and collect checksums.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#ifndef ROMSCRAPER_H
#define ROMSCRAPER_H

#include "MAMEMachine.h"

class ROMScraper
{
 private:
  std::string m_sRomPath;

 public:

  ROMScraper(string sRombPath);
  virtual ~ROMScraper();

  void processROM(MAMEMachine* m);

};

#endif /* ROMSCRAPER_H */
