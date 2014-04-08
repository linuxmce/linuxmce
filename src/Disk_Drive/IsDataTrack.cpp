/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "fcntl.h"
#include "errno.h"
#include "sys/ioctl.h"
#include <cstring>
#include <cstdlib>
#include <unistd.h>

extern "C"
{
	#include <linux/cdrom.h>
}

#include <iostream>
using namespace std;

int main(int argc, char * argv[])
{
	/*
	 * IsDataTrack /dev/cdrom 1
	 */
	if (argc != 3)
	{
		cout << "Syntax: " << argv[0] << " </dev/cdrom> <track_number>" << endl;
		return -1;
	}

    int fd = open(argv[1], O_RDONLY | O_NONBLOCK);
    if (fd < 0)
	{
		cout << "ERROR: Failed to open CD device: " << strerror(errno) << endl;
		return -1;
	}

	/* Code inspired from cd-discid - Start */
	struct cdrom_tochdr hdr;
	if (ioctl(fd, CDROMREADTOCHDR, &hdr) < 0)
	{
		cout << "ERROR: Failed to read CDROM TOC." << endl;
		return -1;
	}
	/* Code inspired from cd-discid - End */

	int nTrack = atoi(argv[2]);
	if (nTrack < 1 || nTrack > hdr.cdth_trk1)
	{
		cout << "ERROR: Track number outside of disk range: 1-" << (int) hdr.cdth_trk1 << endl;
		return -1;
	}

	/* Code inspired from cd-discid - Start */
	cdrom_tocentry te;
	te.cdte_track = nTrack;
	te.cdte_format = CDROM_LBA;
	if (ioctl(fd, CDROMREADTOCENTRY, &te) < 0)
	{
		cout << "ERROR: Failed to read TOC entry for track " << nTrack << endl;
		return -1;
	}
	/* Code inspired from cd-discid - End */
	
	close(fd);
	if (te.cdte_ctrl & CDROM_DATA_TRACK)
	{
		cout << "RESULT: Is data track" << endl;
		return 0;
	}
	else
	{
		cout << "RESULT: Is not a data track" << endl;
		return 1;
	}
}
