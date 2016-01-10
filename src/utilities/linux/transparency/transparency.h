#ifndef __UTILITIES_TRANSPARENCY_H__
#define __UTILITIES_TRANSPARENCY_H__

#include <X11/Xlib.h>

/**
 * @param dpy    display containing window
 * @param wnd    window to change transparency of
 * @param transp transparency value: 0.0 = transparent, 1.0 = opaque
 * @return true if operation succeeded
 */
bool SetWindowTransparency(Display * dpy, Window wnd, double transp);

/**
 * @param dpy
 * @param wnd
 * @return the parent of this window
 */
Window GetParentWnd( Display * dpy, Window wnd );

#endif //__UTILITIES_TRANSPARENCY_H__
