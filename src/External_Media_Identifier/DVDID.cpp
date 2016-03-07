/**
 * DVDID - simple C++ class to wrap libdvdid, and read the DVD ID from a given path
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#include "DVDID.h"

#include <dvdid/dvdid.h>
#include <string.h>
#include <stdio.h>

#include <iostream>

DVDID::DVDID(string sPath)
{
  m_iDVDID=0;
  m_sPath=sPath;
}

DVDID::~DVDID()
{
}

string DVDID::DVDID_get()
{
  char output[18];
  string sOutput;
  if (m_sPath.empty())
    return "";

  int i=0;
  int e=0;

  i=dvdid_calculate(&m_iDVDID,m_sPath.c_str(),&e);

  sprintf(output,"%08" PRIx32 "%08" PRIx32, (uint32_t)(m_iDVDID >> 32), (uint32_t)m_iDVDID);
  sOutput=output;
  return sOutput;
}

// int main(int argc, char** argv)
// {
//   DVDID* d = new DVDID("/tmp/sr0");
//   cout << "DVDID of currently inserted disc is: " << d->DVDID_get();
//   delete d;
//   return 0;
// }
