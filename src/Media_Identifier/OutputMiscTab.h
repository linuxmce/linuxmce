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

    int m_iTrack;
    int m_iPK_AttributeType;
    int m_iSection;
    string m_sValue;
  };

  class OutputMiscTab
  {
    OutputMiscTab();
    OutputMiscTab(string sDiskId);
    ~OutputMiscTab();

    string m_sDiskId;
    vector<OutPutMiscTabAttribute> m_vAttributes;

    void OutputMiscTab::setDiskId(string sDiskId)
    {
      m_sDiskId = sDiskId;
    };

    void OutputMiscTab::addAttribute(int iTrack, int iAttributeType, int iSection, string sWholeName)
    {
      OutputMiscTabAttribute Attribute;

      Attribute.m_iTrack = iTrack;
      Attribute.m_iPK_AttributeType = iAttributeType;
      Attribute.m_iSection = iSection;
      Attribute.m_sValue = sWholeName;

      m_vAttributes.push_back(Attribute);
    };

    string OutputMiscTab::OutputAttributes()
    {
      string sOutput;

      sOutput = m_sDiskId + "\n";

      for(vector<OutputMiscTabAttribute>::iterator it = v.begin(); it != v.end(); ++it) {
        OutputMiscTabAttribute Attribute = *it;

        sOutput += Attribute.m_iTrack + "\t";
	sOutput += Attribute.m_iPK_AttributeType + "\t";
	sOutput += Attbibute.m_iSection + "\t";
	sOutput += Attribute.m_sWholeName + "\n";
      }

      return sOutput;
    };
}

#endif /* OUTPUTMISCTAB_H */
