/**
 * OutputMiscTab - a class that outputs identity data in MISC-TAB format, as defined
 * in ../Media_Plugin/MediaAttributes_Lowlevel.cpp Parse_Misc_Media_ID(), currently
 * around line 728.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 * Author: phenigma <phenigma@hotmail.com>
 */

#ifndef OUTPUTMISCTAB_H
#define OUTPUTMISCTAB_H

#include <vector>
#include <string>
#include <iostream>

namespace DCE
{
  class OutputMiscTabAttribute
  {
  public:
    OutputMiscTabAttribute() { };
    ~OutputMiscTabAttribute() { };

    int m_iTrack;
    int m_iPK_AttributeType;
    int m_iSection;
    string m_sWholeName;
  };

  class OutputMiscTab
  {
  public:
    OutputMiscTab() { };
    OutputMiscTab(string sDiskId)
    {
       m_sDiskId = sDiskId;
    };
    ~OutputMiscTab() { };

  private:
    string m_sDiskId;
    vector<OutputMiscTabAttribute> m_vAttributes;

  public:
    void setDiskId(string sDiskId)
    {
      m_sDiskId = sDiskId;
    };

    void addAttribute(int iTrack, int iAttributeType, int iSection, string sWholeName)
    {
      OutputMiscTabAttribute Attribute;

      Attribute.m_iTrack = iTrack;
      Attribute.m_iPK_AttributeType = iAttributeType;
      Attribute.m_iSection = iSection;
      Attribute.m_sWholeName = sWholeName;

//cerr << endl << "ERR: " << Attribute.m_iTrack << " " << Attribute.m_iPK_AttributeType << " " << Attribute.m_iSection << " " << Attribute.m_sWholeName << endl;

      m_vAttributes.push_back(Attribute);
    };

    string OutputAttributes()
    {

      string sOutput = "";

      sOutput += m_sDiskId + "\n";
//cerr << "OUTPUT: " << m_sDiskId << " -- " << endl;
      for(vector<OutputMiscTabAttribute>::iterator it = m_vAttributes.begin(); it != m_vAttributes.end(); ++it)
      {
        OutputMiscTabAttribute Attribute = *it;

        sOutput += to_string(Attribute.m_iTrack) + "\t";
	sOutput += to_string(Attribute.m_iPK_AttributeType) + "\t";
	sOutput += to_string(Attribute.m_iSection) + "\t";
	sOutput += Attribute.m_sWholeName + "\n";

//cerr << Attribute.m_iTrack << " " << Attribute.m_iPK_AttributeType << " " << Attribute.m_iSection << " " << Attribute.m_sWholeName << endl;
      }

      return sOutput;
    };
  };
};

#endif /* OUTPUTMISCTAB_H */
