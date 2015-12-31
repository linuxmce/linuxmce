/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef PlutoPhoneCommands_h
#define PlutoPhoneCommands_h

#define BD_PC_DISCONNECT				2
#define BD_PC_KEY_WAS_PRESSED			3
#define BD_PC_VMC_TERMINATED			4
#define BD_PC_VMC_HIDDEN				5
#define BD_PC_REPORTING_VARIABLES		6
#define BD_PC_ORDER_PLACED				7
#define BD_PC_SELECTED_FROM_LIST		8
#define BD_PC_SET_VARIABLE				9
#define BD_PC_REPORT_MY_VERSION			10
#define BD_PC_GET_SIGNAL_STRENGTH       11
#define BD_PC_SET_IMAGE_QUALITY         12
#define BD_PC_ITEM_SELECTED				13
#define BD_PC_CONFIGURE					14
#define	BD_PC_MOUSE_EVENT				15
#define BD_PC_GETMENUIMAGES				16

#define BD_CP_SEND_ME_KEYSTROKES		101
#define BD_CP_SHOW_IMAGE				102
#define BD_CP_SHOW_VMC					103
#define BD_CP_SHOW_LIST					104
#define BD_CP_DISCONNECT				105
#define BD_CP_UPDATE_VARIABLES			106
#define BD_CP_CAPTURE_KEYBOARD			107
#define BD_CP_SEND_FILE					108
#define BD_CP_SIMULATE_EVENT			109
#define BD_CP_CURRENT_SIGNAL_STRENGTH   110
#define BD_CP_SET_BKGND_IMAGE			111
#define BD_CP_MENU_DATA					112
#define BD_CP_SHOW_MENU					113
#define BD_CP_SETMENUIMAGES				114


class BDCommand *BuildCommandFromData( unsigned long dwType );

#endif
