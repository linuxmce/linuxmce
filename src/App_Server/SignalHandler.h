/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#ifndef WIN32 // we only have signals on Linux and hte global var is only used there. so we ifndef it..
#include "App_Server.h"

using namespace DCE;

void SignalHandler_Start(App_Server *pApp_Server);
void SignalHandler_Stop();
#else /*ifndef WIN32*/
#define SignalHandler_Start (void)
#define SignalHandler_Stop (void)
#endif /*ifndef WIN32*/

#endif /*ifndef SIGNAL_HANDLER_H*/
