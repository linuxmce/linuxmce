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

// copy of VDR's cMenuEditStrItem

#if VDRVERSNUM > 10503

class cMenuEditPathItem : public cMenuEditItem {
private:
  char *value;
  int length;
  const char *allowed;
  int pos, offset;
  bool insert, newchar, uppercase;
  int lengthUtf8;
  uint *valueUtf8;
  uint *allowedUtf8;
  uint *charMapUtf8;
  uint *currentCharUtf8;
  eKeys lastKey;
  cTimeMs autoAdvanceTimeout;
  void SetHelpKeys(void);
  uint *IsAllowed(uint c);
  void AdvancePos(void);
  virtual void Set(void);
  uint Inc(uint c, bool Up);
  void Insert(void);
  void Delete(void);
protected:
  void EnterEditMode(void);
  void LeaveEditMode(bool SaveValue = false);
  bool InEditMode(void) { return valueUtf8 != NULL; }
public:
  cMenuEditPathItem(const char *Name, char *Value, int Length, const char *Allowed = NULL);
  ~cMenuEditPathItem();
  virtual eOSState ProcessKey(eKeys Key);
  };

#else

class cMenuEditPathItem : public cMenuEditItem {
private:
  char *orgValue;
  char *value;
  int length;
  char *allowed;
  int pos;
  bool insert, newchar, uppercase;
  const char *charMap;
  const char *currentChar;
  eKeys lastKey;
  cTimeMs autoAdvanceTimeout;
  void SetHelpKeys(void);
  void AdvancePos(void);
  virtual void Set(void);
  char Inc(char c, bool Up);
protected:
  bool InEditMode(void) { return pos >= 0; }
public:
  cMenuEditPathItem(const char *Name, char *Value, int Length, const char *Allowed);
  ~cMenuEditPathItem();
  virtual eOSState ProcessKey(eKeys Key);
  };

#endif

}; //namespace PluginRemoteTimers

#endif //_REMOTETIMERS_MENUITEMS__H
