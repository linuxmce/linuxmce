
#include "xineCommon.h"

#include "xineSetupPage.h"
#include "xineSettings.h"
#include "xineLib.h"



namespace PluginXine
{

  void cXineSetupPage::Store()
  {
    m_globalSettings.Store(this);

    bool setupDiffers = m_globalSettings.setupDiffers(m_localSettings);

    m_localSettings = m_globalSettings;

    if (!setupDiffers)
      return;

    m_xineLib->execFuncSetup();
    m_xineLib->ReshowCurrentOSD();
  }
    
  cXineSetupPage::cXineSetupPage(cXineLib *const xineLib, cXineSettings &settings)
    : cMenuSetupPage()
    , m_xineLib(xineLib)
    , m_globalSettings(settings)
    , m_localSettings(settings)
  {
    m_globalSettings.Create(this);
  }

  cXineSetupPage::~cXineSetupPage()
  {
    bool setupDiffers = m_globalSettings.setupDiffers(m_localSettings);

    m_globalSettings = m_localSettings;

    if (!setupDiffers)
      return;

    m_xineLib->execFuncSetup();
    m_xineLib->ReshowCurrentOSD();
  }
  
  eOSState cXineSetupPage::ProcessKey(eKeys Key)
  {
    const cXineSettings prevSettings = m_globalSettings;
    
    eOSState state = cMenuSetupPage::ProcessKey(Key);

    if (prevSettings.setupDiffers(m_globalSettings))
    {
      m_xineLib->execFuncSetup();
      m_xineLib->ReshowCurrentOSD();
    }
    
    return state;
  }

};
