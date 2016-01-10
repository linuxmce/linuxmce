/*
 * $RCSfile: x10_cmd.h,v $  $Revision: 1.2 $  $Name:  $
 * $Id: x10_cmd.h,v 1.2 2007/04/21 04:26:40 bpaauwe Exp $
 * $Author: bpaauwe $
 * $Date: 2007/04/21 04:26:40 $
 * ----------------------------------------------------------------------------
 *
 *  Copyright (c) Bob Paauwe (2007)
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * ----------------------------------------------------------------------------
 *
 *  Description:
 *    Header file for X10 command/Address defines.
 * ----------------------------------------------------------------------------
 */
#ifndef _X10_CMD_H
#define _X10_CMD_H

void send_x10(ilib_t *iplc, unsigned char addr, unsigned char cmd);

/*
 * Macros to create X10 address and command from the house code, unit
 * and command.
 *
 * Usage:
 *   X10_ADDRESS(X10_A, X10_1)  =>  A1
 *   X10_COMMAND(X10_A, X10_ON) =>  AON
 */
#define X10_ADDRESS(h, u) ((h << 8) | u)
#define X10_COMMAND(h, c) ((h << 8) | c)

#define X10_A 0x06
#define X10_B 0x0E
#define X10_C 0x02
#define X10_D 0x0A
#define X10_E 0x01
#define X10_F 0x09
#define X10_G 0x05
#define X10_H 0x0D
#define X10_I 0x07
#define X10_J 0x0F
#define X10_K 0x03
#define X10_L 0x0B
#define X10_M 0x00
#define X10_N 0x08
#define X10_O 0x04
#define X10_P 0x0C

#define X10_1  0x06
#define X10_2  0x0E
#define X10_3  0x02
#define X10_4  0x0A
#define X10_5  0x01
#define X10_6  0x09
#define X10_7  0x05
#define X10_8  0x0D
#define X10_9  0x07
#define X10_10 0x0F
#define X10_11 0x03
#define X10_12 0x0B
#define X10_13 0x00
#define X10_14 0x08
#define X10_15 0x04
#define X10_16 0x0C

#define X10_ALL_UNITS_OFF  0x00
#define X10_ALL_LIGHTS_ON  0x01
#define X10_ON             0x02
#define X10_OFF            0x03
#define X10_DIM            0x04
#define X10_BRIGHT         0x05
#define X10_ALL_LIGHTS_OFF 0x06
#define X10_EXTENDED       0x07
#define X10_HAIL_REQUEST   0x08
#define X10_HAIL_ACK       0x09
#define X10_PRESET_DIM_1   0x0A
#define X10_PRESET_DIM_2   0x0B
#define X10_EXTENDED_DATA  0x0C
#define X10_STATUS_ON      0x0D
#define X10_STATUS_OFF     0x0E
#define X10_STATUS_REQUEST 0x0F


#endif
