
#ifndef __XINESETTINGS_H
#define __XINESETTINGS_H



#include "xineCommon.h"



namespace PluginXine
{

  class cXineSetupPage;
  
  class cXineSettings
  {
  public:
    enum eUsageMode
    {
      modeLiveTV
      , modeReplay
    };

    enum eOsdMode
    {
      osdOverlay
      , osdBlendClipped
      , osdBlendScaledLQ
      , osdBlendScaledHQ
      , osdBlendScaledSHQ
      , osdBlendScaledAuto
    };

    enum eAudioMode
    {
      audioDolbyOff
      , audioDolbyOn
    };

    enum eVolumeMode
    {
      volumeIgnore
      , volumeChange
    };

    enum eMuteMode
    {
      muteIgnore
      , muteExecute
      , muteSimulate
    };

    enum eMonitorGamma
    {
      monitorGammaBase = 100
      , monitorGammaMin = 100
      , monitorGammaDefault = 123
      , monitorGammaMax = 250
    };
    
    enum eAutoPrimaryDeviceMode
    {
      autoPrimaryDeviceOff
      , autoPrimaryDeviceOn
    };

    enum eTransparencyMode
    {
      transparencyOff
      , transparencyOn
    };

  private:
    bool m_switchSkin;
    bool m_beQuiet;
    
    eOsdMode m_osdMode;
    eUsageMode m_usageMode /* , m_usageModeDefault */;
    eAudioMode m_audioMode;
    eVolumeMode m_volumeMode;
    eMuteMode m_muteMode;
    eMonitorGamma m_crtGamma;
    eAutoPrimaryDeviceMode m_autoPrimaryDeviceMode;
    eTransparencyMode m_transparencyMode;
    
    class cModeParams
    {
    bool SetupParse(const char *optionName, int &optionValue, const char *Name, const char *Value);
      
    public:
      cModeParams();
      
      int m_prebufferFrames;

      bool SetupParse(const char *prefix, const char *Name, const char *Value);
    }
    m_modeParams[ 2 ];

    bool SetupParse(const char *optionName, eUsageMode &optionValue, const char *Name, const char *Value);
    bool SetupParse(const char *optionName, eOsdMode &optionValue, const char *Name, const char *Value);
    bool SetupParse(const char *optionName, eAudioMode &optionValue, const char *Name, const char *Value);
    bool SetupParse(const char *optionName, eVolumeMode &optionValue, const char *Name, const char *Value);
    bool SetupParse(const char *optionName, eMuteMode &optionValue, const char *Name, const char *Value);
    bool SetupParse(const char *optionName, eMonitorGamma &optionValue, const char *Name, const char *Value);
    bool SetupParse(const char *optionName, eAutoPrimaryDeviceMode &optionValue, const char *Name, const char *Value);
    bool SetupParse(const char *optionName, eTransparencyMode &optionValue, const char *Name, const char *Value);

  public:
    cXineSettings();
    
    bool SetupParse(const char *Name, const char *Value);

    const cModeParams *GetModeParams() const;

    eOsdMode OsdMode() const;
    bool AudioDolbyOn() const;
    bool ChangeVolume() const;
    bool SupportTransparency() const;
    eMuteMode MuteMode() const;

    void Create(cXineSetupPage *const setupPage);
    void Store(cXineSetupPage *const setupPage);

    void SelectReplayPrebufferMode(const bool select = true);
    
    void TogglePrebufferMode();
    const char *GetMainMenuEntry();

    bool LiveTV() const;
    bool AutoPrimaryDevice() const;
    
    bool setupDiffers(const cXineSettings &rhs) const
    {
      return m_osdMode != rhs.m_osdMode
        || m_transparencyMode != rhs.m_transparencyMode
        || m_volumeMode != rhs.m_volumeMode
        || m_muteMode != rhs.m_muteMode;
    }

    int getCrtGamma() const
    {
      return m_crtGamma;
    }

    void SetSwitchSkin(const bool switchSkin);
    bool ShallSwitchSkin() const;

    void SetBeQuiet(const bool beQuiet);
    bool ShallBeQuiet() const;
  };

};



#endif //__XINESETTINGS_H
