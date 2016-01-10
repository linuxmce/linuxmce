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
#ifdef WIN32
#include <windows.h>
#endif

#ifdef LINUX
#include <stdint.h>
#endif

#include <stdio.h>

#include "remote.h"
#include "lowlevel.h"
#include "global.h"

#include "errcode.h"

#ifdef WIN32
#include "winio.h"
#include "winusbio.h"
#endif


void log_print (char msg[],int level)
{
	if (level > (int)(mode_flag & LOG_MASK)) return;
	if (logfp) {
		fprintf (logfp,"%s",msg);
		fflush (logfp);
	}
	else {
		fprintf (stderr,"%s",msg);
		fflush (stderr);
	}
}

void GetError (int res,char st[])
{

	switch (res) {
	case ERR_OPEN:
		sprintf (st,"Error opening COM / USB Port\n");
		break;
	case ERR_RESET:
		sprintf (st,"No IR Transceiver found (Reset not possible)\n");
		break;
	case ERR_READVERSION:
		sprintf (st,"Could not get Transceiver Version\n");
		break;
	case ERR_VERSION:
		sprintf (st,"Wrong IR Transceiver SW Version. Minimum Version: %s\n",MINIMUM_SW_VERSION);
		break;
	case ERR_TIMEOUT:
		sprintf (st,"Timeout (Connection lost ?)\n");
		break;
	case ERR_OPENUSB:
		sprintf (st,"Error opening USB Device / Device not found\n");
		break;
	case ERR_DBOPENINPUT:
		sprintf (st,"Error opening Database file (Access rights ?)\n");
		break;
	case ERR_REMOTENOTFOUND:
		sprintf (st,"Specified Remote Control [%%s] not found\n");
		break;
	case ERR_COMMANDNOTFOUND:
		sprintf (st,"Specified Remote Command [%%s] not found\n");
		break;
	case ERR_TIMINGNOTFOUND:
		sprintf (st,"Specified Remote Timing not found\n");
		break;
	case ERR_OPENASCII:
		sprintf (st,"Could not create new Remote file (Access rights ?)\n");
		break;
	case ERR_NODATABASE:
		sprintf (st,"Could not open Remote Database (No folder 'remotes' / Access rights ?)\n");
		break;
	case ERR_TOGGLE_DUP:
		sprintf (st,"Could not record Toggle Command (No commands yet learned ?)\n");
		break;
	case ERR_DBOPENINCLUDE:
		sprintf (st,"Specified Include File not found\n");
		break;
	case ERR_NOFILEOPEN:
		sprintf (st,"No Remote opened to learn new commands\n");
		break;
	case ERR_FLOCK:
		sprintf (st,"Could not lock input file (USB / TTY)\n");
		break;
	case ERR_STTY:
		sprintf (st,"Could not set serial parameters\n");
		break;
	case ERR_OPENSOCKET:
		sprintf (st,"Could not open IP socket\n");
		break;
	case ERR_BINDSOCKET:
		sprintf (st,"Could not bind to IP socket (Another server running ?)\n");
		break;
	case ERR_HOTCODE:
		sprintf (st,"Hotcode %%s-%%s not found\n");
		break;
	case ERR_NOTIMING:
		sprintf (st,"No timing learned for new commands\n");
		break;
	case ERR_TEMPCOMMAND:
		sprintf (st,"Illegal Temparature Command: %%s\n");
		break;
	case ERR_OPENTRANS:
		sprintf (st,"Error opening translation table file\n");
		break;
	case ERR_WRONGBUS:
		sprintf (st,"Specified Bus %%d does not exist\n");
		break;
	case ERR_ISMACRO:
		sprintf (st,"Cannot get the Device Data for a macro\n");
		break;
	default:
		sprintf (st,"Error %d\n",res);
		break;
	}

}
