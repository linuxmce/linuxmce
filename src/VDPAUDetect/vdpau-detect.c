/** 
 * vdpau-detect. Try and figure out if a graphics card can create
 * a VDPAU surface, and return a 0 on success, or 1 on failure.
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/
#include <X11/Xlib.h>
#include <vdpau/vdpau.h>
#include <vdpau/vdpau_x11.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	/* This program is really simple */

	Display 		*myDisplay;
	int			myScreen;
	VdpStatus 		vdp_st;
	VdpDevice		vdp_device;
	VdpGetProcAddress       *vdp_get_proc_address;
	VdpDeviceDestroy	*vdp_device_destroy;

	myDisplay = XOpenDisplay(NULL);

	if (!myDisplay)
	{
		printf("Could Not Open Display.\n");
		return 1;
	}

	myScreen = DefaultScreen(myDisplay);
	vdp_st = vdp_device_create_x11(
		myDisplay,
		myScreen,
		&vdp_device,
		&vdp_get_proc_address
	);

	if ( vdp_st == VDP_STATUS_OK ) 
	{
		// We got a valid VDPAU device, return success.
		// We simply need to destroy the device at this point.
#ifdef DEBUG
		printf("Success!\n");
#endif
		vdp_st =   vdp_get_proc_address(vdp_device,
						VDP_FUNC_ID_DEVICE_DESTROY,
						(void **)&vdp_device_destroy); // avoid cast warning.

		if (vdp_st == VDP_STATUS_OK && vdp_device_destroy && vdp_device)
		{
			vdp_device_destroy(vdp_device);
			vdp_device = 0;
		}

		return 0;
	}
	else
	{
		// We didn't get a valid VDPAU device. Return failure.
#ifdef DEBUG
		printf("Failure!\n");
#endif
		return 1;
	}

	// end.
}
