/**
 * OutputMiscTab - a class that outputs identity data in MISC-TAB format, as defined
 * in ../Media_Plugin/MediaAttributes_Lowlevel.cpp Parse_Misc_Media_ID(), currently
 * around line 728.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 */

#ifndef OUTPUTMISCTAB_H
#define OUTPUTMISCTAB_H

#include <vector>
#include <string>

namespace DCE
{
  class OutputMiscTabAttribute
  {
    int m_iPK_Attribute;
    string m_sDescription;
    OutputMiscTabAttribute();
    ~OutputMiscTabAttribute();
    
  };

  class OutputMiscTab 
  {
    
    OutputMiscTab();
    ~OutputMiscTab();
  };
}

#endif /* OUTPUTMISCTAB_H */
