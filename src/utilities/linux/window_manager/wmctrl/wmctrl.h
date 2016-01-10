#ifndef _WMCTRL_H_
#define _WMCTRL_H_

// please keep this file clean
// it is used as a low level library
//
// do not reorder the code
// it will help upgrades
//
// steps for upgrade (wmctrl.c is the original wmctrl source):
// $ cat wmctrl.c | astyle > old_wmctrl.cpp
// $ cat wmctrl.{h,cpp} > new_wmctrl.cpp
// $ diff -bwB old_wmctrl.cpp new_wmctrl.cpp > diff.txt
// diff.txt should be as short as possible

/* license */
/*

wmctrl
A command line tool to interact with an EWMH/NetWM compatible X Window Manager.

Author, current maintainer: Tomas Styblo <tripie@cpan.org>

Copyright (C) 2003

This program is free software which I release under the GNU General Public
License. You may redistribute and/or modify this program under the terms
of that license as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

To get a copy of the GNU General Public License,  write to the
Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/Xmu/WinUtil.h>
#include <glib.h>

#include <string>
#include <list>

// used in the window list
struct WinInfo
{
    WinInfo()
            : lDesktop(0)
			, ulWindowId(0)
            , ulPid(0)
            , x(0)
            , y(0)
            , w(0)
            , h(0)
        {}
    signed long lDesktop;
	unsigned long ulWindowId;
    unsigned long ulPid;
    int x;
    int y;
    int w;
    int h;
    std::string sClassName;
    std::string sClientMachine;
    std::string sTitle;
};

class WmCtrl
{
public:
    WmCtrl();
    ~WmCtrl();

    struct Options
    {
        Options()
                : verbose(0)
                , force_utf8(0)
                , show_class(0)
                , show_pid(0)
                , show_geometry(0)
                , match_by_id(0)
                , match_by_cls(0)
                , full_window_title_match(0)
                , wa_desktop_titles_invalid_utf8(0)
                , param_window(NULL)
                , param(NULL)
                , list_show_all(false)
            {
            }
        int verbose;
        int force_utf8;
        int show_class;
        int show_pid;
        int show_geometry;
        int match_by_id;
        int match_by_cls;
        int full_window_title_match;
        int wa_desktop_titles_invalid_utf8;
        char *param_window;
        char *param;
        bool list_show_all;
    };

    // int main(int argc, char **argv) // part 1/2
    int InteractiveCommand(int argc, char **argv);

    // int main(int argc, char **argv) // part 2/2
    // used in the library
    int LowLevelCommand(char action, Options cmd_options, std::list<WinInfo> *pListWinInfo=NULL);

    // set some options
    // calls LowLevelCommand
    bool ActionCommand(char action, const char *param_window=NULL, const char *param=NULL, bool use_id_in_param_window=false, std::list<WinInfo> *pListWinInfo=NULL);

	bool CurrentDesktop(unsigned long& ulCurrentDesktop);

protected:
    gboolean wm_supports(Display *disp, const gchar *prop);
    Window *get_client_list(Display *disp, unsigned long *size);
    int client_msg(Display *disp, Window win, char *msg,
                   unsigned long data0, unsigned long data1,
                   unsigned long data2, unsigned long data3,
                   unsigned long data4);
    int set_override_redirect(Display *disp, Window win, bool bValue);
    int set_override_redirect_recursive(Display *disp, Window win, bool bValue);
    int list_windows(Display *disp, std::list<WinInfo> *pListWinInfo=NULL);
    int list_desktops(Display *disp);
    int showing_desktop(Display *disp);
    int change_viewport(Display *disp);
    int change_geometry(Display *disp);
    int change_number_of_desktops(Display *disp);
    int switch_desktop(Display *disp, char *desktop_id);
    int wm_info(Display *disp);
    gchar *get_output_str(gchar *str, gboolean is_utf8);
    int action_window(Display *disp, Window win, char mode);
    int action_window_pid(Display *disp, char mode);
    int action_window_str(Display *disp, char mode);
    int activate_window(Display *disp, Window win,
                        gboolean switch_desktop);
    int close_window(Display *disp, Window win);
    int longest_str(gchar **strv);
    int window_to_desktop(Display *disp, Window win, int desktop);
    void window_set_title(Display *disp, Window win, char *str, char mode);
    gchar *get_window_title(Display *disp, Window win);
    gchar *get_window_class(Display *disp, Window win);
    gchar *get_property(Display *disp, Window win,
                        Atom xa_prop_type, gchar *prop_name, unsigned long *size);
    void init_charset(void);
    int window_move_resize(Display *disp, Window win, char *arg);
    int window_state(Display *disp, Window win, char *arg);
    Window Select_Window(Display *dpy);
    Window get_active_window(Display *dpy);

    Options options;

    gboolean envir_utf8;
};

#endif // _WMCTRL_H_
