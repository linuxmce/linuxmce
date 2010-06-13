/*
 * Frontend Status Monitor plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __FEMONCFG_H
#define __FEMONCFG_H

#define MaxSvdrpIp 15 // xxx.xxx.xxx.xxx

enum eFemonModes
{
  eFemonModeBasic,
  eFemonModeTransponder,
  eFemonModeStream,
  eFemonModeAC3,
  eFemonModeMaxNumber
};

struct cFemonConfig
{
public:
  cFemonConfig(void);
  int hidemenu;
  int displaymode;
  int skin;
  int theme;
  int position;
  int downscale;
  int redlimit;
  int greenlimit;
  int updateinterval;
  int analyzestream;
  int calcinterval;
  int usesvdrp;
  int svdrpport;
  char svdrpip[MaxSvdrpIp + 1]; // must end with additional null
};

extern cFemonConfig femonConfig;

enum eFemonSkins
{
  eFemonSkinClassic,
  eFemonSkinElchi,
  eFemonSkinMaxNumber
};

enum eFemonThemes
{
  eFemonThemeClassic,
  eFemonThemeElchi,
  eFemonThemeSTTNG,
  eFemonThemeDeepBlue,
  eFemonThemeMoronimo,
  eFemonThemeEnigma,
  eFemonThemeEgalsTry,
  eFemonThemeDuotone,
  eFemonThemeSilverGreen,
  eFemonThemeMaxNumber
};

struct cFemonTheme
{
  int bpp;
  int clrBackground;
  int clrTitleBackground;
  int clrTitleText;
  int clrActiveText;
  int clrInactiveText;
  int clrRed;
  int clrYellow;
  int clrGreen;
};

extern const cFemonTheme femonTheme[eFemonThemeMaxNumber];

#endif // __FEMONCFG_H
