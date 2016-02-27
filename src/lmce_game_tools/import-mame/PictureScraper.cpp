/**
 * ROMScraper - Scan ROMs and collect checksums.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#include "PictureScraper.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include <iostream>

PictureScraper::PictureScraper(string sPicturePath)
{
  m_sPicturePath=sPicturePath;

  if (!FileUtils::DirExists("/tmp/snap"))
    {
      FileUtils::MakeDir("/tmp/snap");
    }

}

PictureScraper::~PictureScraper()
{

}

/**
 * convertPNGToJpg - Given a path and file sans .png extension, convert it to .jpg.
 * ImageMagick needs to be installed.
 */
void PictureScraper::convertPNGtoJPG(string sPictureNamePrefix)
{
  string sPNGFile = sPictureNamePrefix + ".png";
  string sJPGFile = sPictureNamePrefix + ".jpg";
  string sCmd = "convert "+sPNGFile+" "+sJPGFile;
  system(sCmd.c_str());
}

void PictureScraper::processPicture(MAMEMachine* m)
{
  string sPictureNamePrefix = m_sPicturePath + "/" + m->MachineName_get();
  string sPictureName = "";
  string sPictureSHA1 = "";

  if (!FileUtils::FileExists(sPictureNamePrefix+".png"))
    {
      return;
    }

  if (!FileUtils::FileExists(sPictureNamePrefix+".jpg"))
    {
      convertPNGtoJPG(sPictureNamePrefix);
    }

  m->MachinePicturePath_set(sPictureNamePrefix);

}

void PictureScraper::exportPicture(MAMEMachine* m)
{

  if (m->MachinePicturePath_get().empty())
    {
      return;
    }

  string sCmd1 = "cp "+m->MachinePicturePath_get()+".jpg /tmp/snap/"+StringUtils::itos(m->liPK_Picture_get())+".jpg";
  string sCmd2 = "cp "+m->MachinePicturePath_get()+".png /tmp/snap/"+StringUtils::itos(m->liPK_Picture_get())+".png";
  system(sCmd1.c_str());
  system(sCmd2.c_str());
}
