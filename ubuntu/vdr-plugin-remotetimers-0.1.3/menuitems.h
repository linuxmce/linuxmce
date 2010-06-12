/*
 * menuitems.h: General purpose menu items
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id: menuitems.h 1.25 2008/02/16 16:09:58 kls Exp $
 */

#ifndef _REMOTETIMERS_MENUITEMS__H
#define _REMOTETIMERS_MENUITEMS__H

#include <vdr/menuitems.h>

#include <vdr/recording.h>
#ifndef FOLDERDELIMCHAR
#define FOLDERDELIMCHAR '~'
#endif

#define USER_BUFFER "123456789"
#define MAX_USER ((int) (sizeof(USER_BUFFER) / sizeof(char)) - 1)
#define USER_MASK(x) (0x1 << (x-1))
#define USER_MASK_ALL (USER_MASK(MAX_USER + 1) - 1)
#define USER_FROM_SETUP(x) (x >= 0 ? x : (Setup.ResumeID <= MAX_USER && Setup.ResumeID > 0 ? Setup.ResumeID : 0))
#define MASK_FROM_SETUP(x) (x >= 0 ? x : (Setup.ResumeID <= MAX_USER && Setup.ResumeID > 0 ? USER_MASK(Setup.ResumeID) : 0))

namespace PluginRemoteTimers {

class cMenuEditUserItem : public cMenuEditItem {
private:
  int *value;
  const char *negativeString;
  void Set();
public:
  static int Parse(const char *Value);
  static cString ToString(int Value);

  cMenuEditUserItem(const char *Name, int *Value, const char *NegativeString = NULL);
  virtual eOSState ProcessKey(eKeys Key);
};

}; //namespace PluginRemoteTimers

#endif //_REMOTETIMERS_MENUITEMS__H
