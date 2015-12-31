/*
 * menuitems.c: New menu item types
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: menuitems.c,v 1.15 2009-08-02 11:38:24 phintuka Exp $
 *
 */

#include <vdr/i18n.h>

#include "menuitems.h"

// --- cMenuEditTypedIntItem -------------------------------------------------

cMenuEditTypedIntItem::cMenuEditTypedIntItem(const char *Name, const char *Type, int *Value, 
					     int Min, int Max, const char *ZeroString,
					     const char *MinString, const char *MaxString)
:cMenuEditIntItem(Name,Value,Min,Max,MinString,MaxString)
{
  type = Type ? Type : "";
  zeroString = ZeroString ? ZeroString : NULL;
  Set();
}

void cMenuEditTypedIntItem::Set(void)
{
  if(*value == 0 && *zeroString) 
    SetValue(zeroString);
  else if (minString && *value == min)
    SetValue(minString);
  else if (maxString && *value == max)
    SetValue(maxString);
  else
    SetValue(cString::sprintf("%d %s", *value, *type));
}

// --- cMenuEditOddIntItem ------------------------------------------------------

cMenuEditOddIntItem::cMenuEditOddIntItem(const char *Name, int *Value, int Min, int Max, const char *MinString, const char *MaxString)
:cMenuEditIntItem(Name,Value,Min,Max,MinString,MaxString)
{
  value = Value;
  min = Min;
  max = Max;
  minString = MinString;
  maxString = MaxString;
  if (*value < min)
     *value = min;
  else if (*value > max)
     *value = max;
  Set();
}

eOSState cMenuEditOddIntItem::ProcessKey(eKeys Key)
{
  eOSState state = cMenuEditItem::ProcessKey(Key);

  if (state == osUnknown) {
     int newValue = *value;
     bool IsRepeat = Key & k_Repeat;
     Key = NORMALKEY(Key);
     switch (Key) {
       case kNone: break;
       case kLeft:
            newValue = *value - 2;
            fresh = true;
            if (!IsRepeat && newValue < min && max != INT_MAX)
               newValue = max;
            break;
       case kRight:
            newValue = *value + 2;
            fresh = true;
            if (!IsRepeat && newValue > max && min != INT_MIN)
               newValue = min;
            break;
       default:
            if (*value < min) { *value = min; Set(); }
            if (*value > max) { *value = max; Set(); }
            return state;
       }
     if (newValue != *value && (!fresh || min <= newValue) && newValue <= max) {
        *value = newValue;
        Set();
        }
     state = osContinue;
     }
  return state;
}

// --- cMenuEditFpIntItem ----------------------------------------------------

cMenuEditFpIntItem::cMenuEditFpIntItem(const char *Name, int *Value, int Min, int Max,
                                       int Decimals, const char *ZeroString,
                                       const char *MinString, const char *MaxString)
:cMenuEditIntItem(Name,Value,Min,Max,MinString,MaxString)
{
  decimals = Decimals;
  zeroString = ZeroString ? ZeroString : NULL;
  Set();
}

static int my_exp10(int x)
{
  int r = 1;
  for (; x > 0; x--, r *= 10) ;
  return r;
}

void cMenuEditFpIntItem::Set(void)
{
  if(*value == 0 && *zeroString) 
    SetValue(zeroString);
  else if (minString && *value == min)
    SetValue(minString);
  else if (maxString && *value == max)
    SetValue(maxString);
  else
    SetValue(cString::sprintf("%1.1f", ((float)(*value)) / (float)my_exp10(decimals)));
}

// --- cMenuEditStraI18nItem -------------------------------------------------

cMenuEditStraI18nItem::cMenuEditStraI18nItem(const char *Name, int *Value, int NumStrings, const char * const *Strings)
:cMenuEditIntItem(Name, Value, 0, NumStrings - 1)
{
  strings = Strings;
  Set();
}

void cMenuEditStraI18nItem::Set(void)
{
  SetValue(tr(strings[*value]));
}

// --- cFileListItem -------------------------------------------------

cFileListItem::cFileListItem(const char *name, bool isDir)
{
  m_Name = name;
  m_IsDir = isDir;
  m_IsDvd = false;
  m_IsBluRay  = false;
  m_HasResume = false;
  m_SubFile   = NULL;
  m_ShowFlags = false;
  m_Up = m_IsDir && !strcmp(m_Name, "..");
  Set();
}

cFileListItem::cFileListItem(const char *name, bool IsDir, 
			     bool HasResume, const char *subfile,
			     bool IsDvd, bool IsBluRay)
{
  m_Name = name;
  m_IsDir = IsDir;
  m_IsDvd = IsDvd;
  m_IsBluRay  = IsBluRay;
  m_HasResume = HasResume;
  m_SubFile   = subfile;
  m_ShowFlags = true;
  m_Up = m_IsDir && !strcmp(m_Name, "..");
  Set();
}

void cFileListItem::Set(void)
{
  cString txt;
  const char *pt;
  if(m_ShowFlags) {
    if(m_IsDir) {
      if(m_IsDvd)
	txt = cString::sprintf("\tD\t[%s] ", *m_Name); // text2skin requires space at end of string to display item correctly ...
      else if (m_IsBluRay)
	txt = cString::sprintf("\tB\t[%s] ", *m_Name);
      else
	txt = cString::sprintf("\t\t[%s] ", *m_Name); // text2skin requires space at end of string to display item correctly ...
    } else {
      txt = cString::sprintf("%c\t%c\t%s", m_HasResume ? ' ' : '*', *m_SubFile ? 'S' : m_IsDvd ? 'D' : m_IsBluRay ? 'B' : ' ', *m_Name);
      if(NULL != (pt = strrchr(txt,'.')))
	txt.Truncate(pt - txt);
    }
  } else {
    if(m_IsDir) {
      txt = cString::sprintf("[%s] ", *m_Name); // text2skin requires space at end of string to display item correctly ...
    } else {
      txt = m_Name;
      if(NULL != (pt = strrchr(txt,'.')))
	txt.Truncate(pt - txt);
    }
  }
  SetText(txt);
}

int cFileListItem::Compare(const cListObject &ListObject) const
{
  cFileListItem *other = (cFileListItem *)&ListObject;

  if(m_IsDir && !other->m_IsDir)
    return -1;
  if(!m_IsDir && other->m_IsDir)
    return 1;
  if(m_Up && !other->m_Up)
    return -1;
  if(!m_Up && other->m_Up)
    return 1;
  return strcmp(m_Name, other->m_Name);
}

bool cFileListItem::operator< (const cListObject &ListObject)
{
  cFileListItem *other = (cFileListItem *)&ListObject;

  if(m_IsDir && !other->m_IsDir)
    return true;
  if(!m_IsDir && other->m_IsDir)
    return false;
  if(m_Up && !other->m_Up)
    return true;
  if(!m_Up && other->m_Up)
    return false;
  return strcmp(m_Name, other->m_Name) < 0;
}
