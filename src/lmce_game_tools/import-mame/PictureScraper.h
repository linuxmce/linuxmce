/**
 * PictureScraper - Scan Pictures, convert png to jpeg, if needed.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#ifndef PICTURESCRAPER_H
#define PICTURESCRAPER_H

#include "MAMEMachine.h"
#include <string>

using namespace std;

class PictureScraper
{
 public:
  std::string m_sPicturePath;

  PictureScraper(string sPicturePath);
  virtual ~PictureScraper();

  void convertPNGtoJPG(string sPictureNamePrefix);
  void processPicture(MAMEMachine* m);
  void exportPicture(MAMEMachine* m);

};

#endif /* PICTURESCRAPER_H */
