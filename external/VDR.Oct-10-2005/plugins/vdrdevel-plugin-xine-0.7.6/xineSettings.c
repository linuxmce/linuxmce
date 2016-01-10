
#include "xineCommon.h"

#include <vdr/interface.h>
#include <vdr/i18n.h>

#include "xineSettings.h"
#include "xineSetupPage.h"
#include "xineDevice.h"



namespace PluginXine
{

  cXineSettings::cModeParams::cModeParams()
    : m_prebufferFrames(0)
  {
  }
  
  bool cXineSettings::cModeParams::SetupParse(const char *optionName, int &optionValue, const char *Name, const char *Value)
  {
    if (0 != ::strcasecmp(optionName, Name))
      return false;

    char c;
    int v = 0;    
    if (1 != ::sscanf(Value, "%d%c", &v, &c))
      return false;

    if (0 <= v && v <= 100)
    {
      optionValue = v;
      return true;
    }   
    
    return false;
  }

  bool cXineSettings::cModeParams::SetupParse(const char *prefix, const char *Name, const char *Value)
  {
    const int prefixLen = ::strlen(prefix);
    
    if (0 != ::strncasecmp(prefix, Name, prefixLen))
      return false;

    Name += prefixLen;
    
    if (SetupParse("prebufferFrames", m_prebufferFrames, Name, Value))
      return true;

    return false;
  }

  
  
  cXineSettings::cXineSettings()
    : m_switchSkin(false)
    , m_beQuiet(false)
    , m_osdMode(osdBlendScaledAuto)
    , m_usageMode(modeLiveTV)
//    , m_usageModeDefault(modeLiveTV)
    , m_audioMode(audioDolbyOff)
    , m_volumeMode(volumeChange)
    , m_muteMode(muteSimulate)
    , m_crtGamma(monitorGammaDefault)
    , m_autoPrimaryDeviceMode(autoPrimaryDeviceOn)
    , m_transparencyMode(transparencyOn)
  {
    m_modeParams[ modeLiveTV ].m_prebufferFrames = 2 * 15 + 1;
  }

  bool cXineSettings::SetupParse(const char *optionName, eUsageMode &optionValue, const char *Name, const char *Value)
  {
    if (0 != ::strcasecmp(optionName, Name))
      return false;

    if (0 == ::strcasecmp("modeLiveTV", Value))
    {
      optionValue = modeLiveTV;
      return true;
    }

    if (0 == ::strcasecmp("modeReplay", Value))
    {
      optionValue = modeReplay;
      return true;
    }

    return false;
  }
  
  bool cXineSettings::SetupParse(const char *optionName, eVolumeMode &optionValue, const char *Name, const char *Value)
  {
    if (0 != ::strcasecmp(optionName, Name))
      return false;

    if (0 == ::strcasecmp("volumeChange", Value))
    {
      optionValue = volumeChange;
      return true;
    }

    if (0 == ::strcasecmp("volumeIgnore", Value))
    {
      optionValue = volumeIgnore;
      return true;
    }

    // backward compatibilty
    if (0 == ::strcasecmp("volumeDontTouch", Value))
    {
      optionValue = volumeIgnore;
      return true;
    }

    return false;
  }
  
  bool cXineSettings::SetupParse(const char *optionName, eMuteMode &optionValue, const char *Name, const char *Value)
  {
    if (0 != ::strcasecmp(optionName, Name))
      return false;

    if (0 == ::strcasecmp("muteExecute", Value))
    {
      optionValue = muteExecute;
      return true;
    }

    if (0 == ::strcasecmp("muteSimulate", Value))
    {
      optionValue = muteSimulate;
      return true;
    }

    if (0 == ::strcasecmp("muteIgnore", Value))
    {
      optionValue = muteIgnore;
      return true;
    }

    return false;
  }
  
  bool cXineSettings::SetupParse(const char *optionName, eOsdMode &optionValue, const char *Name, const char *Value)
  {
    if (0 != ::strcasecmp(optionName, Name))
      return false;

    if (0 == ::strcasecmp("osdOverlay", Value))
    {
      optionValue = osdOverlay;
      return true;
    }

    if (0 == ::strcasecmp("osdBlendClipped", Value))
    {
      optionValue = osdBlendClipped;
      return true;
    }

    if (0 == ::strcasecmp("osdBlendScaledLQ", Value))
    {
      optionValue = osdBlendScaledLQ;
      return true;
    }

    if (0 == ::strcasecmp("osdBlendScaledHQ", Value))
    {
      optionValue = osdBlendScaledHQ;
      return true;
    }

    if (0 == ::strcasecmp("osdBlendScaledSHQ", Value))
    {
      optionValue = osdBlendScaledSHQ;
      return true;
    }

    if (0 == ::strcasecmp("osdBlendScaledAuto", Value))
    {
      optionValue = osdBlendScaledAuto;
      return true;
    }

    // backward compatibilty
    if (0 == ::strcasecmp("osdScaled", Value))
    {
      optionValue = osdBlendScaledAuto;
      return true;
    }

    if (0 == ::strcasecmp("osdUnscaled", Value))
    {
      optionValue = osdOverlay;
      return true;
    }

    return false;
  }
  
  bool cXineSettings::SetupParse(const char *optionName, eMonitorGamma &optionValue, const char *Name, const char *Value)
  {
    if (0 != ::strcasecmp(optionName, Name))
      return false;

    char c;
    int v = 0;    
    if (1 != ::sscanf(Value, "%d%c", &v, &c))
      return false;

    if (monitorGammaMin <= v && v <= monitorGammaMax)
    {
      optionValue = (eMonitorGamma)v;
      return true;
    }   
    
    return false;
  }
  
  bool cXineSettings::SetupParse(const char *optionName, eAudioMode &optionValue, const char *Name, const char *Value)
  {
    if (0 != ::strcasecmp(optionName, Name))
      return false;

    if (0 == ::strcasecmp("audioDolbyOff", Value))
    {
      optionValue = audioDolbyOff;
      return true;
    }

    if (0 == ::strcasecmp("audioDolbyOn", Value))
    {
      optionValue = audioDolbyOn;
      return true;
    }

    return false;
  }
  
  bool cXineSettings::SetupParse(const char *optionName, eAutoPrimaryDeviceMode &optionValue, const char *Name, const char *Value)
  {
    if (0 != ::strcasecmp(optionName, Name))
      return false;

    if (0 == ::strcasecmp("autoPrimaryDeviceOff", Value))
    {
      optionValue = autoPrimaryDeviceOff;
      return true;
    }

    if (0 == ::strcasecmp("autoPrimaryDeviceOn", Value))
    {
      optionValue = autoPrimaryDeviceOn;
      return true;
    }

    return false;
  }
  
  bool cXineSettings::SetupParse(const char *optionName, eTransparencyMode &optionValue, const char *Name, const char *Value)
  {
    if (0 != ::strcasecmp(optionName, Name))
      return false;

    if (0 == ::strcasecmp("transparencyOff", Value))
    {
      optionValue = transparencyOff;
      return true;
    }

    if (0 == ::strcasecmp("transparencyOn", Value))
    {
      optionValue = transparencyOn;
      return true;
    }

    return false;
  }
  
  bool cXineSettings::SetupParse(const char *Name, const char *Value)
  {
    if (m_modeParams[ modeLiveTV ].SetupParse("modeLiveTV.", Name, Value))
      return true;
/*    
    if (m_modeParams[ modeReplay ].SetupParse("modeReplay.", Name, Value))
      return true;

    if (SetupParse("usageModeDefault", m_usageModeDefault, Name, Value))
    {
      m_usageMode = m_usageModeDefault;
      return true;
    }
*/
    if (SetupParse("osdMode", m_osdMode, Name, Value))
      return true;

    if (SetupParse("osdGammaCorrection", m_crtGamma, Name, Value))
      return true;

    if (SetupParse("audioMode", m_audioMode, Name, Value))
      return true;

    if (SetupParse("volumeMode", m_volumeMode, Name, Value))
      return true;

    if (SetupParse("muteMode", m_muteMode, Name, Value))
      return true;

    if (SetupParse("autoPrimaryDeviceMode", m_autoPrimaryDeviceMode, Name, Value))
      return true;

    if (SetupParse("transparencyMode", m_transparencyMode, Name, Value))
      return true;

    return false;    
  }

  const cXineSettings::cModeParams *cXineSettings::GetModeParams() const
  {
    return &m_modeParams[ m_usageMode ];
  }

  cXineSettings::eOsdMode cXineSettings::OsdMode() const
  {
    return m_osdMode;
  }

  cXineSettings::eMuteMode cXineSettings::MuteMode() const
  {
    return m_muteMode;
  }

  bool cXineSettings::AudioDolbyOn() const
  {
    return audioDolbyOn == m_audioMode;
  }

  void cXineSettings::Create(cXineSetupPage *const setupPage)
  {
    static const char *osdModes[ 6 ];
    osdModes[ osdOverlay ]         = tr("X11 overlay");
    osdModes[ osdBlendClipped ]    = tr("Blend clipped");
    osdModes[ osdBlendScaledLQ ]   = tr("Blend scaled LQ");
    osdModes[ osdBlendScaledHQ ]   = tr("Blend scaled HQ");
    osdModes[ osdBlendScaledSHQ ]  = tr("Blend scaled SHQ");
    osdModes[ osdBlendScaledAuto ] = tr("Blend scaled Auto");
    
    static const char *muteModes[ 3 ];
    muteModes[ muteIgnore ]   = tr("Ignore");
    muteModes[ muteExecute ]  = tr("Execute");
    muteModes[ muteSimulate ] = tr("Simulate");
    
    setupPage->Add(new cMenuEditIntItem(tr("Live-TV prebuffer [frames]"), &m_modeParams[ modeLiveTV ].m_prebufferFrames, 0, 100));
//    setupPage->Add(new cMenuEditIntItem(tr("Replay prebuffer [frames]"), &m_modeParams[ modeReplay ].m_prebufferFrames, 0, 100));
//    setupPage->Add(new cMenuEditBoolItem(tr("Default settings optimized for"), (int *)&m_usageModeDefault, tr("Live-TV"), tr("Replay")));
    setupPage->Add(new cMenuEditStraItem(tr("OSD display mode"), (int *)&m_osdMode, sizeof (osdModes) / sizeof (*osdModes), osdModes));
    setupPage->Add(new cMenuEditIntItem(tr("OSD gamma correction [ 123 => 1.23 ]"), (int *)&m_crtGamma, monitorGammaMin, monitorGammaMax));
    setupPage->Add(new cMenuEditBoolItem(tr("Audio mode"), (int *)&m_audioMode, tr("Dolby off"), tr("Dolby on")));
    setupPage->Add(new cMenuEditBoolItem(tr("Control xine's volume"), (int *)&m_volumeMode, tr("No"), tr("Yes")));
    setupPage->Add(new cMenuEditStraItem(tr("Muting"), (int *)&m_muteMode, sizeof (muteModes) / sizeof (*muteModes), muteModes));
    setupPage->Add(new cMenuEditBoolItem(tr("Get primary device when xine connects"), (int *)&m_autoPrimaryDeviceMode, tr("No"), tr("Yes")));
    setupPage->Add(new cMenuEditBoolItem(tr("Support semi transparent colors"), (int *)&m_transparencyMode, tr("No"), tr("Yes")));
  }
  
  void cXineSettings::Store(cXineSetupPage *const setupPage)
  {
    setupPage->SetupStore("modeLiveTV.prebufferFrames", m_modeParams[ modeLiveTV ].m_prebufferFrames);
//    setupPage->SetupStore("modeReplay.prebufferFrames", m_modeParams[ modeReplay ].m_prebufferFrames);
//    setupPage->SetupStore("usageModeDefault", (m_usageModeDefault ? "modeReplay" : "modeLiveTV"));
    {
      char *mode = 0;

      switch (m_osdMode)
      {
      case osdOverlay:          mode = "osdOverlay";          break;
      case osdBlendClipped:     mode = "osdBlendClipped";     break;
      case osdBlendScaledLQ:    mode = "osdBlendScaledLQ";    break;
      case osdBlendScaledHQ:    mode = "osdBlendScaledHQ";    break;
      case osdBlendScaledSHQ:   mode = "osdBlendScaledSHQ";   break;
      case osdBlendScaledAuto:  mode = "osdBlendScaledAuto";  break;
      default:
        assert(false);
      }
      
      setupPage->SetupStore("osdMode", mode);
    }
    
    setupPage->SetupStore("osdGammaCorrection", (int)m_crtGamma);
    setupPage->SetupStore("audioMode", (m_audioMode ? "audioDolbyOn" : "audioDolbyOff"));
    setupPage->SetupStore("volumeMode", (m_volumeMode ? "volumeChange" : "volumeDontTouch"));

    {
      char *mode = 0;

      switch (m_muteMode)
      {
      case muteIgnore:    mode = "muteIgnore";    break;
      case muteExecute:   mode = "muteExecute";   break;
      case muteSimulate:  mode = "muteSimulate";  break;
      default:
        assert(false);
      }
      
      setupPage->SetupStore("muteMode", mode);
    }

    setupPage->SetupStore("autoPrimaryDeviceMode", (m_autoPrimaryDeviceMode ? "autoPrimaryDeviceOn" : "autoPrimaryDeviceOff"));
    setupPage->SetupStore("transparencyMode", (m_transparencyMode ? "transparencyOn" : "transparencyOff"));
  }

  void cXineSettings::SelectReplayPrebufferMode(const bool select /* = true */)
  {
    m_usageMode = (!select ? modeLiveTV : modeReplay);
/*
    if (Interface)
      Interface->Info(m_usageMode ? tr("Settings are now optimized for 'Replay'") : tr("Settings are now optimized for 'Live-TV'"));
*/
//    cXineDevice::GetDevice()->AdjustPrebufferMode();
  } 

  void cXineSettings::TogglePrebufferMode()    
  {
    m_usageMode = (m_usageMode ? modeLiveTV : modeReplay);
/*
    if (Interface)
      Interface->Info(m_usageMode ? tr("Settings are now optimized for 'Replay'") : tr("Settings are now optimized for 'Live-TV'"));
*/
//    cXineDevice::GetDevice()->AdjustPrebufferMode();
  }
  
  const char *cXineSettings::GetMainMenuEntry()
  {
    return (m_usageMode ? tr("xine - Choose optimized settings for 'Live-TV'") : tr("xine - Choose optimized settings for 'Replay'"));
  }

  bool cXineSettings::LiveTV() const
  {
    return (m_usageMode == modeLiveTV);
  }  

  bool cXineSettings::AutoPrimaryDevice() const
  {
    return (m_autoPrimaryDeviceMode == autoPrimaryDeviceOn);
  }  

  bool cXineSettings::ChangeVolume() const
  {
    return (m_volumeMode == volumeChange);
  }  

  bool cXineSettings::SupportTransparency() const
  {
    return (m_transparencyMode == transparencyOn);
  }

  void cXineSettings::SetSwitchSkin(const bool switchSkin)
  {
    m_switchSkin = switchSkin;
  }
  
  bool cXineSettings::ShallSwitchSkin() const
  {
    return m_switchSkin;
  }

  bool beQuiet = false;

  void cXineSettings::SetBeQuiet(const bool beQuiet)
  {
    m_beQuiet = beQuiet;
  
    PluginXine::beQuiet = m_beQuiet;
  }
  
  bool cXineSettings::ShallBeQuiet() const
  {
    return m_beQuiet;
  }
  
};
