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
    OutputMiscTabAttribute();
    ~OutputMiscTabAttribute();
    
    int m_iPK_Attribute;
    string m_sValue;
  };

  class OutputMiscTab 
  {
    OutputMiscTab();
    OutputMiscTab(string sDiskId);
    ~OutputMiscTab();

    string m_sDiskID;
    vector<OutPutMiscTabAttribute> Attribute;

    void setDiskId(string sDiskId);
    void addAttribute(int iTrack, int iAttributeType, int iSection, string sWholeName)
    {
      OutputMiscTabAttribute Attribute;

      // Disk ID		PK_AttributeType=14
      // Album			PK_AttributeType=3
      // Performer		PK_AttributeType=2
      // Conductor		PK_AttributeType=15
      // ComposerWriter		PK_AttributeType=16
      // Producer		PK_AttributeType=38
      // Genre			PK_AttributeType=8
      // Track			PK_AttributeType=5
      // Release Date		PK_AttributeType=19
      // Year			PK_AttributeType=48
    };

  };
}

#endif /* OUTPUTMISCTAB_H */
