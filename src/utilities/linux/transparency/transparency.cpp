#include "transparency.h"
#include <X11/Xatom.h>

#define OPAQUE  0xffffffff
#define OPACITY "_NET_WM_WINDOW_OPACITY"

bool SetWindowTransparency(Display * dpy, Window wnd, double transp)
{
	unsigned int opacity = (unsigned int)(transp * OPAQUE);
	if (opacity == OPAQUE) {
		XDeleteProperty (dpy, wnd, XInternAtom(dpy, OPACITY, False));
	}
	else { /* set it */
		XChangeProperty(dpy, wnd, XInternAtom(dpy, OPACITY, False),
			XA_CARDINAL, 32, PropModeReplace,
			(unsigned char *) &opacity, 1L);
	}
	XSync(dpy, False);
	return true;
}

Window GetParentWnd( Display * dpy, Window wnd )
{
	Window root;
	Window parent;
	//Window grandparent; /* Grandparent is the WM frame */
	Window *children;
	unsigned int children_count;
	
	XQueryTree(	dpy,
				wnd,
				&root,
				&parent,
				&children,
				&children_count);
	return parent;
}
