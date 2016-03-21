/**
 * DVDID - simple C++ class to wrap libdvdid, and read the DVD ID from a given path
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Version: 1.0
 */

#ifndef CPPDVDID_H
#define CPPDVDID_H

#include <string>
#include <inttypes.h>
#include <stdint.h>

using namespace std;

class DVDID 
{
  string m_sPath;
  uint64_t m_iDVDID;

 public:
  DVDID(string sPath);
  virtual ~DVDID();
  string Path_get() {return m_sPath;}
  string DVDID_get();
};

#endif /* CPPDVDID_H */
