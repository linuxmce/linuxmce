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
