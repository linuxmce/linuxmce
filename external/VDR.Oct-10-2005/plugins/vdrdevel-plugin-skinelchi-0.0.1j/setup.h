#ifndef __SKINELCHI_SETUP_H
#define __SKINELCHI_SETUP_H

#include <vdr/plugin.h>
#include <vdr/themes.h>

#define ScrollWidth  5

#define MAX_ELCHI_THEME_COLORS 71 

struct cSkinElchiSetup {
	cSkinElchiSetup(void);
	bool SetupParse(const char *Name, const char *Value);
	int logo;
//for epgimages
#ifdef HAVE_IMAGEMAGICK
	int epgimages;
	int epgshowtime;
	int resize;
	char epgimagedir[MaxFileName];//but filename is longer, maybe a problem?
	bool commandlinedir;
#endif
//
	int singlearea;
	int vidwin;
	int bit;
#ifdef VIDEOSTATUS
	int videostatus;
	int switchvideodisplayformat;
#endif
#ifdef HAVE_YAEPG_PATCH
#ifdef HAVE_IMAGEMAGICK
	int format;
	int quality;
	int intervall;
	int videoxoffset;
	int videoyoffset;
#endif
#endif
	int usenofixedfont;
	int color;
	int hidemenu;
	char menuentry[50];
	char tempcolor[8];
	tColor ThemeColors[MAX_ELCHI_THEME_COLORS];
	bool changing;
	char *change;
};

extern cSkinElchiSetup SkinElchiSetup;

extern cTheme Theme;

THEME_CLR(Theme, clrBackground,             0x77000066);//  0
THEME_CLR(Theme, clrButtonRedFg,            clrWhite);  //  1
THEME_CLR(Theme, clrButtonRedBg,            0xCCCC1111);//  2
THEME_CLR(Theme, clrButtonGreenFg,          clrBlack);  //  3
THEME_CLR(Theme, clrButtonGreenBg,          0xCC22BB22);//  4
THEME_CLR(Theme, clrButtonYellowFg,         clrBlack);  //  5
THEME_CLR(Theme, clrButtonYellowBg,         0xCCEEEE22);//  6
THEME_CLR(Theme, clrButtonBlueFg,           clrWhite);  //  7
THEME_CLR(Theme, clrButtonBlueBg,           0xCC2233CC);//  8
THEME_CLR(Theme, clrMessageStatusFg,        clrBlack);  //  9
THEME_CLR(Theme, clrMessageStatusBg,        0xCC2BA7F1);// 10
THEME_CLR(Theme, clrMessageInfoFg,          clrBlack);  // 11
THEME_CLR(Theme, clrMessageInfoBg,          0xCC22BB22);// 12
THEME_CLR(Theme, clrMessageWarningFg,       clrBlack);  // 13
THEME_CLR(Theme, clrMessageWarningBg,       clrYellow); // 14
THEME_CLR(Theme, clrMessageErrorFg,         clrWhite);  // 15
THEME_CLR(Theme, clrMessageErrorBg,         clrRed);    // 16
THEME_CLR(Theme, clrVolumePrompt,           clrWhite);  // 17
THEME_CLR(Theme, clrVolumeBarUpper,         0xFFCCBB22);// 18
THEME_CLR(Theme, clrVolumeBarLower,         0x77000066);// 19
THEME_CLR(Theme, clrVolumeSymbolMuteBg,     clrWhite);  // 20
THEME_CLR(Theme, clrVolumeSymbolMuteFg,     0xCCCC1111);// 21
THEME_CLR(Theme, clrVolumeSymbolVolumeBg,   0x77000066);// 22
THEME_CLR(Theme, clrVolumeSymbolVolumeFg,   0xFFCCBB22);// 23
THEME_CLR(Theme, clrChannelNameFg,          clrWhite);  // 24
THEME_CLR(Theme, clrChannelNameBg,          0xCC2BA7F1);// 25
THEME_CLR(Theme, clrChannelNumberFg,        clrBlack);  // 26
THEME_CLR(Theme, clrChannelNumberBg,        clrWhite);  // 27 - currently not used
THEME_CLR(Theme, clrChannelDateFg,          clrBlack);  // 28
THEME_CLR(Theme, clrChannelDateBg,          clrWhite);  // 29
THEME_CLR(Theme, clrChannelEpgTimeFg,       0xFFDDDDDD);// 30
THEME_CLR(Theme, clrChannelEpgTimeBg,       0xAABB0000);// 31
THEME_CLR(Theme, clrChannelEpgTitle,        0xFF00FCFC);// 32
THEME_CLR(Theme, clrChannelEpgTitleBg,      0x88000000);// 33
THEME_CLR(Theme, clrChannelEpgShortText,    0xFFCCBB22);// 34
THEME_CLR(Theme, clrChannelSymbolOn,        clrYellow); // 35
THEME_CLR(Theme, clrChannelSymbolOff,       0x77777777);// 36
THEME_CLR(Theme, clrChannelSymbolRecFg,     clrWhite);  // 37
THEME_CLR(Theme, clrChannelSymbolRecBg,     clrRed);    // 38
THEME_CLR(Theme, clrChannelTimebarSeen,     clrYellow); // 39
THEME_CLR(Theme, clrChannelTimebarRest,     clrGray50); // 40
THEME_CLR(Theme, clrMenuTitleFg,            clrBlack);  // 41
THEME_CLR(Theme, clrMenuTitleBg,            0xCC2BA7F1);// 42
THEME_CLR(Theme, clrMenuDate,               clrBlack);  // 43
THEME_CLR(Theme, clrMenuItemCurrentFg,      clrBlack);  // 44
THEME_CLR(Theme, clrMenuItemCurrentBg,      0xCC2BA7F1);// 45
THEME_CLR(Theme, clrMenuItemSelectable,     clrWhite);  // 46
THEME_CLR(Theme, clrMenuItemNonSelectable,  0xCC2BA7F1);// 47
THEME_CLR(Theme, clrMenuEventTime,          clrWhite);  // 48
THEME_CLR(Theme, clrMenuEventVpsFg,         clrBlack);  // 49
THEME_CLR(Theme, clrMenuEventVpsBg,         clrWhite);  // 50
THEME_CLR(Theme, clrMenuEventTitle,         clrYellow); // 51
THEME_CLR(Theme, clrMenuEventShortText,     clrWhite);  // 52
THEME_CLR(Theme, clrMenuEventDescription,   clrYellow); // 53
THEME_CLR(Theme, clrMenuScrollbarTotal,     clrWhite);  // 54
THEME_CLR(Theme, clrMenuScrollbarShown,     clrYellow); // 55
THEME_CLR(Theme, clrMenuText,               clrWhite);  // 56
THEME_CLR(Theme, clrReplayTitleFg,          clrBlack);  // 57
THEME_CLR(Theme, clrReplayTitleBg,          clrWhite);  // 58
THEME_CLR(Theme, clrReplayCurrent,          clrWhite);  // 59
THEME_CLR(Theme, clrReplayTotal,            clrWhite);  // 60
THEME_CLR(Theme, clrReplayModeJump,         clrWhite);  // 61
THEME_CLR(Theme, clrReplayProgressSeen,     0xCC22BB22);// 62
THEME_CLR(Theme, clrReplayProgressRest,     clrWhite);  // 63
THEME_CLR(Theme, clrReplayProgressSelected, 0xCCCC1111);// 64
THEME_CLR(Theme, clrReplayProgressMark,     clrBlack);  // 65
THEME_CLR(Theme, clrReplayProgressCurrent,  0xCCCC1111);// 66
THEME_CLR(Theme, clrReplaySymbolOn,         clrYellow); // 67
THEME_CLR(Theme, clrReplaySymbolOff,        0x77777777);// 68
THEME_CLR(Theme, Transparent,	    	    0x01000000);// 69
THEME_CLR(Theme, changing,	    	    0x00000000);// 70

class cSkinElchiSetupPage : public cMenuSetupPage {
private:
    cSkinElchiSetup mySkinElchiSetup;
    void ShowItems(void);

protected:
    virtual eOSState ProcessKey(eKeys Key);

public:
    cSkinElchiSetupPage(void);
    virtual ~cSkinElchiSetupPage();
    virtual void Store(void);
};

#endif //__SKINELCHI_SETUP_H

