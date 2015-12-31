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
    OutputMiscTab(unsigned int iDiskId)
    {
       m_iDiskId = iDiskId;
    };
    ~OutputMiscTab() { };

  private:
    unsigned int m_iDiskId;
    vector<OutputMiscTabAttribute> m_vAttributes;

  public:
    void setDiskId(unsigned int iDiskId)
    {
      m_iDiskId = iDiskId;
    };

    void addAttribute(int iTrack, int iAttributeType, int iSection, string sWholeName)
    {
      OutputMiscTabAttribute Attribute;

      Attribute.m_iTrack = iTrack;
      Attribute.m_iPK_AttributeType = iAttributeType;
      Attribute.m_iSection = iSection;
      Attribute.m_sWholeName = sWholeName;

      m_vAttributes.push_back(Attribute);
    };

    string OutputAttributes()
    {
      string sOutput;

      sOutput = m_iDiskId + "\n";

      for(vector<OutputMiscTabAttribute>::iterator it = m_vAttributes.begin(); it != m_vAttributes.end(); ++it)
      {
        OutputMiscTabAttribute Attribute = *it;

        sOutput += Attribute.m_iTrack + "\t";
	sOutput += Attribute.m_iPK_AttributeType + "\t";
	sOutput += Attribute.m_iSection + "\t";
	sOutput += Attribute.m_sWholeName + "\n";
      }

      return sOutput;
    };
  };
};

#endif /* OUTPUTMISCTAB_H */
