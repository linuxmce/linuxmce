/**
 * WMMCDI - Class to generate an MDR-CD ID from an inserted CD.
 *
 * @author Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * @version 1.0
 */

#ifndef WMMCDI_H
#define WMMCDI_H

#include <string>

using namespace std;

class WMMCDI
{
  string m_sPath;
  string m_sWMMCDIId; 
  int m_iNumtracks;

 public:
  WMMCDI(string sPath);
  virtual ~WMMCDI();

  bool calculate();

  string WMMCDIId_get() {return m_sWMMCDIId;}
  int WMMCDINumtracks_get() {return m_iNumtracks;}

};

#endif
