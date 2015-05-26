/*
 * Copyright (C) 2008-2011 Frank Schmirler <vdr@schmirler.de>
 *
 * This file is part of VDR Plugin remotetimers.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 */

#include "menuitems.h"
#include <ctype.h>
#include <wctype.h>
#include <stdlib.h>
#include <vdr/remote.h>

namespace PluginRemoteTimers {

// --- cMenuEditUserItem ------------------------------------------------------

bool OneBitSet(int Bits)
{
  for (int mask = USER_MASK(1); mask <= USER_MASK(MAX_USER); mask <<= 1) {
     if ((Bits & mask) == mask)
        return (Bits & mask) == Bits;
  }
  return false;
}

cMenuEditUserItem::cMenuEditUserItem(const char *Name, int *Value, const char *NegativeString)
:cMenuEditItem(Name)
{
  value = Value;
  negativeString = NegativeString;
  Set();
}

int cMenuEditUserItem::Parse(const char *s)
{
  int result = 0;
  const char *p = s;
  while (p && *p) {
     int i = strtol(p, (char**) &p, 10);
     if (i <= 0)	// also returns 0 on error
        return i;
     else if (i <= MAX_USER)
        result |= USER_MASK(i);
     while (*p == ',')
        p++;
  }
  return result;
}

cString cMenuEditUserItem::ToString(int Value)
{
  if (Value <= 0)
     return cString::sprintf("%d", Value);

  char buffer[MAX_USER * 2];
  char *p = buffer;
  for (int i = 1; i <= MAX_USER; i++) {
     if (Value & USER_MASK(i)) {
	if (p != buffer)
	   *p++ = ',';
        *p++ = '0' + i;
     }
  }
  *p = 0;
  return cString(buffer);
}

void cMenuEditUserItem::Set()
{
  if (*value < 0 && negativeString)
  {
     SetValue(negativeString);
  }
  else if (*value == 0)
  {
     SetValue("0");
  }
  else
  {
     char buffer[] = USER_BUFFER;
     int mask = USER_MASK(1);
     for (char *p = buffer; *p; p++) {
        if ((*value & mask) == 0)
	   *p = '-';
	mask <<= 1;
     }
     buffer[MAX_USER] = 0;
     SetValue(buffer);
  }
}

eOSState cMenuEditUserItem::ProcessKey(eKeys Key)
{
  eOSState state = cMenuEditItem::ProcessKey(Key);

  if (state == osUnknown) {
     int newValue = *value;
     Key = NORMALKEY(Key);
     switch (Key) {
        case k0:
	   newValue = 0;
	   break;
	case k1 ... k9:
	   if (newValue < 0)
	      newValue = 0;
	   if (Key - k0 <= MAX_USER)
	      newValue ^= USER_MASK(Key - k0);
	   break;
	case kLeft:
	   if (newValue <= 0 && negativeString)
	      newValue = -1;
	   else if (newValue == USER_MASK_ALL)
	      newValue = USER_MASK(MAX_USER);
	   else if (OneBitSet(newValue))
	      newValue >>= 1;
	   else
	      newValue = 0;
	   break;
	case kRight:
	   if (newValue < 0)
	      newValue = 0;
	   else if (newValue == 0) 
	      newValue = USER_MASK(1);
	   else if (OneBitSet(newValue) && newValue != USER_MASK(MAX_USER))
	      newValue <<= 1;
	   else
	      newValue = USER_MASK_ALL;
	   break;
	default:
	   return state;
     }
     if (newValue != -1)
        newValue &= USER_MASK_ALL;
     if (newValue != *value) {
        *value = newValue;
	Set();
     }
     state = osContinue;
  }
  return state;
}

}; // namespace PluginRemoteTimers
