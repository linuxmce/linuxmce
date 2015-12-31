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
#ifndef _DIALOG_TYPES_H_
#define _DIALOG_TYPES_H_

enum E_DIALOG_TYPE
{
    E_Dialog_Undefined = 0, // should not be used
    E_Dialog_RoomWizard,
    E_Dialog_WaitGrid,
    E_Dialog_WaitList,
    E_Dialog_WaitUser,
    E_Dialog_SpeedControl,
    E_Dialog_VolumeControl,
    E_Dialog_LightControl,
    E_Dialog_None,
	E_Dialog_Last_Wx_Dialog,
	E_Dialog_Dummy_Orbiter_Dialog
};

#endif
// _DIALOG_TYPES_H_
