
#ifndef __XINESETUPPAGE_H
#define __XINESETUPPAGE_H



#include "xineCommon.h"

#include <vdr/menuitems.h>

#include "xineSettings.h"



namespace PluginXine
{

  class cXineLib;

  class cXineSetupPage : public cMenuSetupPage
  {
    cXineLib *const m_xineLib;

    cXineSettings &m_globalSettings;
    cXineSettings m_localSettings;

  protected:
    virtual void Store();
    
  public:
    cXineSetupPage(cXineLib *const xineLib, cXineSettings &settings);
    virtual ~cXineSetupPage();

    virtual eOSState ProcessKey(eKeys Key);

    friend class cXineSettings;
  };

};



#endif //__XINESETUPPAGE_H
