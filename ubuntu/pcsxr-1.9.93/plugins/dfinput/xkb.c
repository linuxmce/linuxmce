/*
 * Copyright (c) 2009, Wei Mingzhi <whistler@openoffice.org>.
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 */

#include "pad.h"
#include "util.h"

static Atom wmprotocols, wmdelwindow;
static int g_currentMouse_X;
static int g_currentMouse_Y;
static Window window;

void InitKeyboard() {
	wmprotocols = XInternAtom(g.Disp, "WM_PROTOCOLS", 0);
	wmdelwindow = XInternAtom(g.Disp, "WM_DELETE_WINDOW", 0);

	XkbSetDetectableAutoRepeat(g.Disp, 1, NULL);

    if (g.cfg.PadDef[0].Type == PSE_PAD_TYPE_MOUSE ||
        g.cfg.PadDef[1].Type == PSE_PAD_TYPE_MOUSE) {
        int revert_to;
        XGetInputFocus(g.Disp, &window, &revert_to);
        grabCursor(g.Disp, window, 1);
        showCursor(g.Disp, window, 0);
    }
    
    g_currentMouse_X = 0;
    g_currentMouse_Y = 0;
    
	g.PadState[0].KeyStatus = 0xFFFF;
	g.PadState[1].KeyStatus = 0xFFFF;
}

void DestroyKeyboard() {
	XkbSetDetectableAutoRepeat(g.Disp, 0, NULL);
    
    if (g.cfg.PadDef[0].Type == PSE_PAD_TYPE_MOUSE ||
        g.cfg.PadDef[1].Type == PSE_PAD_TYPE_MOUSE) {
        grabCursor(g.Disp, window, 0);
        showCursor(g.Disp, window, 1);
    }
}

static void bdown(int pad, int bit)
{
	if(bit < 16)
		g.PadState[pad].KeyStatus &= ~(1 << bit);
	else if(bit == DKEY_ANALOG)
		g.PadState[pad].PadModeSwitch = 1;
}

static void bup(int pad, int bit)
{
	if(bit < 16)
		g.PadState[pad].KeyStatus |= (1 << bit);
}

void CheckKeyboard() {
	uint8_t					i, j, found;
	XEvent					evt;
	XClientMessageEvent		*xce;
	uint16_t				Key;
    
	while (XPending(g.Disp)) {
		XNextEvent(g.Disp, &evt);
		switch (evt.type) {
            case ButtonPress:
                for(i = 0; i < 2; ++i) {
                    if(g.cfg.PadDef[i].Type == PSE_PAD_TYPE_MOUSE) {
                            switch(evt.xbutton.button) {
                                case 1:
                                    bdown(i, 11);
                                    break;
                                case 3:
                                    bdown(i, 10);
                                    break;
                            }
                        }
                    }
                break;
            case ButtonRelease:
                for(i = 0; i < 2; ++i) {
                    if(g.cfg.PadDef[i].Type == PSE_PAD_TYPE_MOUSE) {
                            switch(evt.xbutton.button) {
                                case 1:
                                    bup(i, 11);
                                    break;
                                case 3:
                                    bup(i, 10);
                                    break;
                            }
                        }
                    }
                break;
            case MotionNotify:
                g_currentMouse_X = evt.xmotion.x - 160;
                g_currentMouse_Y = evt.xmotion.y - 120;
                if( g_currentMouse_X < -128) g_currentMouse_X = -128;
                if( g_currentMouse_X > 127) g_currentMouse_X = 127;
                if( g_currentMouse_Y < -128) g_currentMouse_Y = -128;
                if( g_currentMouse_Y > 127) g_currentMouse_Y = 127;
                break;
			case KeyPress:
				Key = XLookupKeysym((XKeyEvent *)&evt, 0);
				found = 0;
				for (i = 0; i < 2; i++) {
					for (j = 0; j < DKEY_TOTAL; j++) {
						if (g.cfg.PadDef[i].KeyDef[j].Key == Key) {
							found = 1;
							bdown(i, j);
						}
					}
				}
				if (!found && !AnalogKeyPressed(Key)) {
					g.KeyLeftOver = Key;
				}
				break;
			case KeyRelease:
				Key = XLookupKeysym((XKeyEvent *)&evt, 0);
				found = 0;
				for (i = 0; i < 2; i++) {
					for (j = 0; j < DKEY_TOTAL; j++) {
						if (g.cfg.PadDef[i].KeyDef[j].Key == Key) {
							found = 1;
							bup(i, j);
						}
					}
				}
				if (!found && !AnalogKeyReleased(Key)) {
					g.KeyLeftOver = ((long)Key | 0x40000000);
				}
				break;
			case ClientMessage:
				xce = (XClientMessageEvent *)&evt;
				if (xce->message_type == wmprotocols && (Atom)xce->data.l[0] == wmdelwindow) {
					// Fake an ESC key if user clicked the close button on window
					g.KeyLeftOver = XK_Escape;
					return;
				}
				break;
		}
    }
    
	g.PadState[0].MouseAxis[0][0] = g_currentMouse_X;
    g.PadState[0].MouseAxis[0][1] = g_currentMouse_Y;
    g_currentMouse_X *= 0.7;
    g_currentMouse_Y *= 0.7;
    
    if (g.cfg.PadDef[0].Type == PSE_PAD_TYPE_MOUSE ||
        g.cfg.PadDef[1].Type == PSE_PAD_TYPE_MOUSE) {
        XWarpPointer(g.Disp, None, window, 0, 0, 0, 0, 160, 120);
    }
}
