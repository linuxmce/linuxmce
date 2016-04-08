/**
 * WMMCDI - Class to generate an MDR-CD ID from an inserted CD.
 *
 * @author Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * @version 1.0
 *
 * Based on C code written by Jeremy D. Zawodny and Brian Ellacott
 *
 */

#include "WMMCDI.h"
#include "PlutoUtils/FileUtils.h"
#include <linux/cdrom.h>
#include <iostream>
#include <stdio.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

WMMCDI::WMMCDI(string sPath)
{
  m_sPath=sPath;
  m_sWMMCDIId="";
}

WMMCDI::~WMMCDI()
{
}

bool WMMCDI::calculate()
{
  if (!FileUtils::FileExists(m_sPath))
    return false;

  struct toc {
    int min, sec, frame;
  } cdtoc[100]; // tracks 0-99 maximum allowed on a CD.
  
  int drive = open(m_sPath.c_str(), O_RDONLY | O_NONBLOCK);
  int num_tracks = 0;
  struct cdrom_tochdr tochdr;
  struct cdrom_tocentry tocentry;

  // Read the TOC and fill out the structs provided by cdrom.h
  
  int i=0;

  ioctl(drive, CDROMREADTOCHDR, &tochdr);
  for (i=tochdr.cdth_trk0; i <= tochdr.cdth_trk1; i++)
    {
      tocentry.cdte_track = i;
      tocentry.cdte_format = CDROM_MSF;
      ioctl(drive, CDROMREADTOCENTRY, &tocentry);
      cdtoc[i-1].min = tocentry.cdte_addr.msf.minute;
      cdtoc[i-1].sec = tocentry.cdte_addr.msf.second;
      cdtoc[i-1].frame = tocentry.cdte_addr.msf.frame;
      cdtoc[i-1].frame += cdtoc[i-1].min*60*75;
      cdtoc[i-1].frame += cdtoc[i-1].sec*75;
    }
  tocentry.cdte_track = 0xAA;
  tocentry.cdte_format = CDROM_MSF;
  ioctl(drive, CDROMREADTOCENTRY, &tocentry);
  cdtoc[tochdr.cdth_trk1].min = tocentry.cdte_addr.msf.minute;
  cdtoc[tochdr.cdth_trk1].sec = tocentry.cdte_addr.msf.second;
  cdtoc[tochdr.cdth_trk1].frame = tocentry.cdte_addr.msf.frame;
  cdtoc[tochdr.cdth_trk1].frame += cdtoc[tochdr.cdth_trk1].min*60*75;
  cdtoc[tochdr.cdth_trk1].frame += cdtoc[tochdr.cdth_trk1].sec*75;
  close(drive);
  num_tracks = tochdr.cdth_trk1;

  i=0;
  
  char cNumTracks[3];
  sprintf(cNumTracks,"%X",num_tracks);
  m_sWMMCDIId+=cNumTracks;
  
  for (i=0; i<=num_tracks; i++)
    {
      char cFrames[17];
      sprintf(cFrames,"+%X",cdtoc[i].frame);
      m_sWMMCDIId+=cFrames;
    }

  return true;
}

// int main(int argc, char** argv)
// {
//   WMMCDI* w = new WMMCDI("/dev/sr0");
//   if (w->calculate())
//     {
//       cout << "Calculated WM/MCDI ID: " << w->WMMCDIId_get() << endl;
//     }
//   delete w;
//   return 0;
// }
