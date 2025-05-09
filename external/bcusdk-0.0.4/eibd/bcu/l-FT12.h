/*
    EIBD eib bus access and management daemon
    Copyright (C) 2005-2008 Martin Koegler <mkoegler@auto.tuwien.ac.at>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef C_FT12_H
#define C_FT12_H

#include "ft12.h"

#define FT12_URL "ft12:/dev/ttySx\n"
#define FT12_DOC "ft12 connects over a serial line without any driver with the FT1.2 Protocol to a BCU 2\n\n"
#define FT12_PREFIX "ft12"
#define FT12_CREATE ft12_ll_Create

inline LowLevelDriverInterface *
ft12_ll_Create (const char *dev, Trace * t)
{
  return new FT12LowLevelDriver (dev, t);
}

#endif
