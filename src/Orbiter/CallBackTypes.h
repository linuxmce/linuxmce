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
#ifndef _CALLBACKTYPES_H_
#define _CALLBACKTYPES_H_

//used by screen handler class
enum CallBackType
{
    cbUnused,
    cbDataGridSelected,
	cbDataGridRendering,
    cbObjectSelected,
	cbObjectHighlighted,
    cbOnDialogCreate,   // create a unique dialog
    cbOnDialogDelete,   // close dialog without saving data
    cbOnDialogRefresh,  // refresh dialog with new data
    cbOnDialogSave,     // save data from dialog
    cbOnDialogWaitUser, // wait for an user action
    cbOnKeyDown,
    cbOnKeyUp,
    cbOnMouseDown,
    cbOnMouseUp,
    cbCapturedKeyboardBufferChanged,
    cbOnTimer,
    cbMessageIntercepted,
    cbOnRenderScreen,
	cbOnCustomRender,
    cbOnGotoScreen,
	cbOnRemoteKeyCodeIntercepted,
	cbOnVariableChanged
};

#endif
// _CALLBACKTYPES_H_
