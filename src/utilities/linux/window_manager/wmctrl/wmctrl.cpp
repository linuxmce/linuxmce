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

#include "wmctrl.h"
#include "../../wrapper/wrapper_x11.h"
#include "Logger.h"
using namespace DCE;
/* version */
#define VERSION "1.07"

/* help */
#define HELP "wmctrl " VERSION "\n" \
"Usage: wmctrl [OPTION]...\n" \
"Actions:\n" \
"  -m                   Show information about the window manager and\n" \
"                       about the environment.\n" \
"  -l                   List windows managed by the window manager.\n" \
"  -d                   List desktops. The current desktop is marked\n" \
"                       with an asterisk.\n" \
"  -s <DESK>            Switch to the specified desktop.\n" \
"  -a <WIN>             Activate the window by switching to its desktop and\n" \
"                       raising it.\n" \
"  -c <WIN>             Close the window gracefully.\n" \
"  -R <WIN>             Move the window to the current desktop and\n" \
"                       activate it.\n" \
"  -r <WIN> -t <DESK>   Move the window to the specified desktop.\n" \
"  -r <WIN> -e <MVARG>  Resize and move the window around the desktop.\n" \
"                       The format of the <MVARG> argument is described below.\n" \
"  -r <WIN> -b <STARG>  Change the state of the window. Using this option it's\n" \
"                       possible for example to make the window maximized,\n" \
"                       minimized or fullscreen. The format of the <STARG>\n" \
"                       argument and list of possible states is given below.\n" \
"  -r <WIN> -N <STR>    Set the name (long title) of the window.\n" \
"  -r <WIN> -I <STR>    Set the icon name (short title) of the window.\n" \
"  -r <WIN> -T <STR>    Set both the name and the icon name of the window.\n" \
"  -k (on|off)          Activate or deactivate window manager's\n" \
"                       \"showing the desktop\" mode. Many window managers\n" \
"                       do not implement this mode.\n" \
"  -o <X>,<Y>           Change the viewport for the current desktop.\n" \
"                       The X and Y values are separated with a comma.\n" \
"                       They define the top left corner of the viewport.\n" \
"                       The window manager may ignore the request.\n" \
"  -n <NUM>             Change number of desktops.\n" \
"                       The window manager may ignore the request.\n" \
"  -g <W>,<H>           Change geometry (common size) of all desktops.\n" \
"                       The window manager may ignore the request.\n" \
"  -h                   Print help.\n" \
"\n" \
"Options:\n" \
"  -i                   Interpret <WIN> as a numerical window ID.\n" \
"  -p                   Include PIDs in the window list. Very few\n" \
"                       X applications support this feature.\n" \
"  -G                   Include geometry in the window list.\n" \
"  -x                   Include WM_CLASS in the window list or\n" \
"                       interpret <WIN> as the WM_CLASS name.\n" \
"  -u                   Override auto-detection and force UTF-8 mode.\n" \
"  -F                   Modifies the behavior of the window title matching\n" \
"                       algorithm. It will match only the full window title\n" \
"                       instead of a substring, when this option is used.\n" \
"                       Furthermore it makes the matching case sensitive.\n" \
"  -v                   Be verbose. Useful for debugging.\n" \
"  -w <WA>              Use a workaround. The option may appear multiple\n" \
"                       times. List of available workarounds is given below.\n" \
"\n" \
"Arguments:\n" \
"  <WIN>                This argument specifies the window. By default it's\n" \
"                       interpreted as a string. The string is matched\n" \
"                       against the window titles and the first matching\n" \
"                       window is used. The matching isn't case sensitive\n" \
"                       and the string may appear in any position\n" \
"                       of the title.\n" \
"\n" \
"                       The -i option may be used to interpret the argument\n" \
"                       as a numerical window ID represented as a decimal\n" \
"                       number. If it starts with \"0x\", then\n" \
"                       it will be interpreted as a hexadecimal number.\n" \
"\n" \
"                       The -x option may be used to interpret the argument\n" \
"                       as a string, which is matched against the window's\n" \
"                       class name (WM_CLASS property). Th first matching\n" \
"                       window is used. The matching isn't case sensitive\n" \
"                       and the string may appear in any position\n" \
"                       of the class name. So it's recommended to  always use\n" \
"                       the -F option in conjunction with the -x option.\n" \
"\n" \
"                       The special string \":SELECT:\" (without the quotes)\n" \
"                       may be used to instruct wmctrl to let you select the\n" \
"                       window by clicking on it.\n" \
"\n" \
"                       The special string \":ACTIVE:\" (without the quotes)\n" \
"                       may be used to instruct wmctrl to use the currently\n" \
"                       active window for the action.\n" \
"\n" \
"  <DESK>               A desktop number. Desktops are counted from zero.\n" \
"\n" \
"  <MVARG>              Specifies a change to the position and size\n" \
"                       of the window. The format of the argument is:\n" \
"\n" \
"                       <G>,<X>,<Y>,<W>,<H>\n" \
"\n" \
"                       <G>: Gravity specified as a number. The numbers are\n" \
"                          defined in the EWMH specification. The value of\n" \
"                          zero is particularly useful, it means \"use the\n" \
"                          default gravity of the window\".\n" \
"                       <X>,<Y>: Coordinates of new position of the window.\n" \
"                       <W>,<H>: New width and height of the window.\n" \
"\n" \
"                       The value of -1 may appear in place of\n" \
"                       any of the <X>, <Y>, <W> and <H> properties\n" \
"                       to left the property unchanged.\n" \
"\n" \
"  <STARG>              Specifies a change to the state of the window\n" \
"                       by the means of _NET_WM_STATE request.\n" \
"                       This option allows two properties to be changed\n" \
"                       simultaneously, specifically to allow both\n" \
"                       horizontal and vertical maximization to be\n" \
"                       altered together.\n" \
"\n" \
"                       The format of the argument is:\n" \
"\n" \
"                       (remove|add|toggle),<PROP1>[,<PROP2>]\n" \
"\n" \
"                       The EWMH specification defines the\n" \
"                       following properties:\n" \
"\n" \
"                           modal, sticky, maximized_vert, maximized_horz,\n" \
"                           shaded, skip_taskbar, skip_pager, hidden,\n" \
"                           fullscreen, above, below\n" \
"\n" \
"Shortcuts/new options:\n" \
"  -L                   List with header.\n" \
"  -O value             Change the override_redirect flag, recursive for parent windows.\n" \
"\n" \
"Shortcuts (not yet implemented):\n" \
"\n" \
"  -r <WIN> -E <MVARG>  like -e, but removes :\n" \
"                       maximized_vert, maximized_horz, fullscreen\n" \
"\n" \
"  <STARG>              (set):\n" \
"                           above => remove fullscreen, below\n" \
"                           below => remove fullscreen, above\n" \
"                           fullscreen => remove fullscreen, above\n" \
"\n" \
"  <STARG>              maximized == maximized_vert & maximized_horz\n" \
"\n" \
"\n" \
"Workarounds:\n" \
"\n" \
"  DESKTOP_TITLES_INVALID_UTF8      Print non-ASCII desktop titles correctly\n" \
"                                   when using Window Maker.\n" \
"\n" \
"The format of the window list:\n" \
"\n" \
"  <window ID> <desktop ID> <client machine> <window title>\n" \
"\n" \
"The format of the desktop list:\n" \
"\n" \
"  <desktop ID> [-*] <geometry> <viewport> <workarea> <title>\n" \
"\n" \
"\n" \
"Author, current maintainer: Tomas Styblo <tripie@cpan.org>\n" \
"Released under the GNU General Public License.\n" \
"Copyright (C) 2003\n"

#define _NET_WM_STATE_REMOVE        0    /* remove/unset property */
#define _NET_WM_STATE_ADD           1    /* add/set property */
#define _NET_WM_STATE_TOGGLE        2    /* toggle property  */

#define _NET_WM_STATE_SET          10    /* shortcut: remove & add property */

#define MAX_PROPERTY_VALUE_LEN 4096
#define SELECT_WINDOW_MAGIC ":SELECT:"
#define ACTIVE_WINDOW_MAGIC ":ACTIVE:"

#define p_verbose(...) \
{ \
    if (options.verbose) \
        fprintf(stderr, __VA_ARGS__); \
}

WmCtrl::WmCtrl()
        : options()
        , envir_utf8(false)
{
}

WmCtrl::~WmCtrl()
{
}

// int main(int argc, char **argv) // part 1
int WmCtrl::InteractiveCommand(int argc, char **argv)
{
    // new code
    Options options;

    // old code
    int opt = 0;
    char action = '_';
    int missing_option = 1;

    /* necessary to make g_get_charset() and g_locale_*() work */
    setlocale(LC_ALL, "");

    init_charset();

    /* make "--help" and "--version" work. I don't want to use
     * getopt_long for portability reasons */
    if (argc == 2 && argv[1])
    {
        if (strcmp(argv[1], "--help") == 0)
        {
            fputs(HELP, stdout);
            return EXIT_SUCCESS;
        }
        else if (strcmp(argv[1], "--version") == 0)
        {
            puts(VERSION);
            return EXIT_SUCCESS;
        }
    }

    while ((opt = getopt(argc, argv, "FGVvhlLupidmxaO:r:s:c:t:w:k:o:n:g:e:b:N:I:T:R:")) != -1)
    {
        missing_option = 0;
        switch (opt)
        {
            case 'F':
                options.full_window_title_match = 1;
                break;
            case 'G':
                options.show_geometry = 1;
                break;
            case 'i':
                options.match_by_id = 1;
                break;
            case 'v':
                options.verbose = 1;
                break;
            case 'u':
                options.force_utf8 = 1;
                break;
            case 'x':
                options.match_by_cls = 1;
                options.show_class = 1;
                break;
            case 'p':
                options.show_pid = 1;
                break;
            case 'a':
            case 'c':
            case 'R':
                options.param_window = optarg;
                action = opt;
                break;
            case 'r':
                options.param_window = optarg;
                break;
            case 't':
            case 'e':
            case 'b':
            case 'N':
            case 'I':
            case 'T':
            case 'O':
                options.param = optarg;
                action = opt;
                break;
            case 's':
                options.param = optarg;
                action = opt;
                break;
            case 'w':
                if (strcmp(optarg, "DESKTOP_TITLES_INVALID_UTF8") == 0)
                {
                    options.wa_desktop_titles_invalid_utf8 = 1;
                }
                else
                {
                    fprintf(stderr, "Unknown workaround: %s\n", optarg);
                    return EXIT_FAILURE;
                }
                break;
            case 'k':
            case 'o':
            case 'n':
            case 'g':
                options.param = optarg;
                action = opt;
                break;
            case '?':
                return EXIT_FAILURE;
            default:
                action = opt;
        }
    }

    if (missing_option)
    {
        fputs(HELP, stderr);
        return EXIT_FAILURE;
    }

    return WmCtrl::LowLevelCommand(action, options);
}

int WmCtrl::LowLevelCommand(char action, Options cmd_options, std::list<WinInfo> *pListWinInfo/*=NULL*/)
{
    // new code
    Display *disp = NULL;
    options = cmd_options;

    // old code
    int ret = EXIT_SUCCESS;

    /* necessary to make g_get_charset() and g_locale_*() work */
    setlocale(LC_ALL, "");

    init_charset();

    if (! (disp = XOpenDisplay(NULL)))
    {
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WmCtrl::LowLevelCommand Cannot open display.");
        fputs("Cannot open display.\n", stderr);
        return EXIT_FAILURE;
    }
#ifdef DEBUG
    p_verbose("WmCtrl::LowLevelCommand() : opened pDisplay=%p\n", disp)
#endif
    XLockDisplay(disp);

    switch (action)
    {
        case 'V':
            puts(VERSION);
            break;
        case 'h':
            fputs(HELP, stdout);
            break;
        case 'L':
            options.list_show_all = true;
        case 'l':
            ret = list_windows(disp, pListWinInfo);
            break;
        case 'd':
            ret = list_desktops(disp);
            break;
        case 's':
            ret = switch_desktop(disp, cmd_options.param);
            break;
        case 'm':
            ret = wm_info(disp);
            break;
        case 'a':
        case 'c':
        case 'R':
        case 't':
        case 'e':
        case 'b':
        case 'N':
        case 'I':
        case 'T':
        case 'O':
            if (! options.param_window)
            {
                fputs("No window was specified.\n", stderr);
                return EXIT_FAILURE;
            }
            if (options.match_by_id)
            {
                ret = action_window_pid(disp, action);
            }
            else
            {
                ret = action_window_str(disp, action);
            }
            break;
        case 'k':
            ret = showing_desktop(disp);
            break;
        case 'o':
            ret = change_viewport(disp);
            break;
        case 'n':
            ret = change_number_of_desktops(disp);
            break;
        case 'g':
            ret = change_geometry(disp);
            break;
    }

    XSync(disp, false);
    XUnlockDisplay(disp);
    if (disp)
        XCloseDisplay(disp);
#ifdef DEBUG
    p_verbose("WmCtrl::LowLevelCommand() : closed pDisplay=%p\n", disp)
#endif
    return ret;
}

bool WmCtrl::ActionCommand(char action, const char *param_window/*=NULL*/, const char *param/*=NULL*/, bool use_id_in_param_window/*=false*/, std::list<WinInfo> *pListWinInfo/*=NULL*/)
{
	LoggerWrapper::GetInstance()->Write(LV_DEBUG,"WmCtrl::ActionCommand('%c', '%s', '%s', %d, %p)\n", action, param_window, param, use_id_in_param_window, pListWinInfo);
    Options options;
    options.verbose = true;
    options.show_class = true;
    options.show_pid = true;
    options.show_geometry = true;
    if (use_id_in_param_window)
        options.match_by_id = true;
    else
        options.match_by_cls = true;
    if (param_window != NULL)
    {
        size_t len = strlen(param_window);
        options.param_window = new char[len + 1];
        strcpy(options.param_window, param_window);
    }
    if (param != NULL)
    {
        size_t len = strlen(param);
        options.param = new char[len + 1];
        strcpy(options.param, param);
    }
    int nRetCode = LowLevelCommand(action, options, pListWinInfo);
    bool bRetCode = ( 0 == nRetCode );
    delete[] options.param_window;
    delete[] options.param;
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_DEBUG,"WmCtrl::ActionCommand('%c', '%s', '%s', %d, %p) => RetCode==%d\n", action, param_window, param, use_id_in_param_window, pListWinInfo, nRetCode);
#endif
    return bRetCode;
}

void WmCtrl::init_charset(void)
{
    const gchar *charset = NULL; /* unused */
    gchar *lang = getenv("LANG") ? g_ascii_strup(getenv("LANG"), -1) : NULL;
    gchar *lc_ctype = getenv("LC_CTYPE") ? g_ascii_strup(getenv("LC_CTYPE"), -1) : NULL;

    /* this glib function doesn't work on my system ... */
    envir_utf8 = g_get_charset(&charset);

    /* ... therefore we will examine the environment variables */
    if (lc_ctype && (strstr(lc_ctype, "UTF8") || strstr(lc_ctype, "UTF-8")))
    {
        envir_utf8 = TRUE;
    }
    else if (lang && (strstr(lang, "UTF8") || strstr(lang, "UTF-8")))
    {
        envir_utf8 = TRUE;
    }

    g_free(lang);
    g_free(lc_ctype);

    if (options.force_utf8)
    {
        envir_utf8 = TRUE;
    }
#ifdef DEBUG
    p_verbose("envir_utf8: %d\n", envir_utf8);
#endif
}

int WmCtrl::client_msg(Display *disp, Window win, char *msg,
                       unsigned long data0, unsigned long data1,
                       unsigned long data2, unsigned long data3,
                       unsigned long data4)
{
    XEvent event;
    long mask = SubstructureRedirectMask | SubstructureNotifyMask;

    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.send_event = True;
    event.xclient.message_type = XInternAtom(disp, msg, False);
    event.xclient.window = win;
    event.xclient.format = 32;
    event.xclient.data.l[0] = data0;
    event.xclient.data.l[1] = data1;
    event.xclient.data.l[2] = data2;
    event.xclient.data.l[3] = data3;
    event.xclient.data.l[4] = data4;

    if (XSendEvent(disp, DefaultRootWindow(disp), False, mask, &event))
    {
        return EXIT_SUCCESS;
    }
    else
    {
        fprintf(stderr, "Cannot send %s event.\n", msg);
        return EXIT_FAILURE;
    }
}

gchar * WmCtrl::get_output_str(gchar *str, gboolean is_utf8)
{
    gchar *out;

    if (str == NULL)
    {
        return NULL;
    }

    if (envir_utf8)
    {
        if (is_utf8)
        {
            out = g_strdup(str);
        }
        else
        {
            if (! (out = g_locale_to_utf8(str, -1, NULL, NULL, NULL)))
            {
                p_verbose("Cannot convert string from locale charset to UTF-8.\n");
                out = g_strdup(str);
            }
        }
    }
    else
    {
        if (is_utf8)
        {
            if (! (out = g_locale_from_utf8(str, -1, NULL, NULL, NULL)))
            {
                p_verbose("Cannot convert string from UTF-8 to locale charset.\n");
                out = g_strdup(str);
            }
        }
        else
        {
            out = g_strdup(str);
        }
    }

    return out;
}

int WmCtrl::wm_info(Display *disp)
{
    Window *sup_window = NULL;
    gchar *wm_name = NULL;
    gchar *wm_class = NULL;
    unsigned long *wm_pid = NULL;
    unsigned long *showing_desktop = NULL;
    gboolean name_is_utf8 = TRUE;
    gchar *name_out;
    gchar *class_out;

    if (! (sup_window = (Window *)get_property(disp, DefaultRootWindow(disp),
                                               XA_WINDOW, "_NET_SUPPORTING_WM_CHECK", NULL)))
    {
        if (! (sup_window = (Window *)get_property(disp, DefaultRootWindow(disp),
                                                   XA_CARDINAL, "_WIN_SUPPORTING_WM_CHECK", NULL)))
        {
            fputs("Cannot get window manager info properties.\n"
                  "(_NET_SUPPORTING_WM_CHECK or _WIN_SUPPORTING_WM_CHECK)\n", stderr);
            return EXIT_FAILURE;
        }
    }

    /* WM_NAME */
    if (! (wm_name = get_property(disp, *sup_window,
                                  XInternAtom(disp, "UTF8_STRING", False), "_NET_WM_NAME", NULL)))
    {
        name_is_utf8 = FALSE;
        if (! (wm_name = get_property(disp, *sup_window,
                                      XA_STRING, "_NET_WM_NAME", NULL)))
        {
            p_verbose("Cannot get name of the window manager (_NET_WM_NAME).\n");
        }
    }
    name_out = get_output_str(wm_name, name_is_utf8);

    /* WM_CLASS */
    if (! (wm_class = get_property(disp, *sup_window,
                                   XInternAtom(disp, "UTF8_STRING", False), "WM_CLASS", NULL)))
    {
        name_is_utf8 = FALSE;
        if (! (wm_class = get_property(disp, *sup_window,
                                       XA_STRING, "WM_CLASS", NULL)))
        {
            p_verbose("Cannot get class of the window manager (WM_CLASS).\n");
        }
    }
    class_out = get_output_str(wm_class, name_is_utf8);


    /* WM_PID */
    if (! (wm_pid = (unsigned long *)get_property(disp, *sup_window,
                                                  XA_CARDINAL, "_NET_WM_PID", NULL)))
    {
        p_verbose("Cannot get pid of the window manager (_NET_WM_PID).\n");
    }

    /* _NET_SHOWING_DESKTOP */
    if (! (showing_desktop = (unsigned long *)get_property(disp, DefaultRootWindow(disp),
                                                           XA_CARDINAL, "_NET_SHOWING_DESKTOP", NULL)))
    {
        p_verbose("Cannot get the _NET_SHOWING_DESKTOP property.\n");
    }

#ifdef DEBUG
    /* print out the info */
    printf("Name: %s\n", name_out ? name_out : "N/A");
    printf("Class: %s\n", class_out ? class_out : "N/A");

    if (wm_pid)
    {
        printf("PID: %lu\n", *wm_pid);
    }
    else
    {
        printf("PID: N/A\n");
    }

    if (showing_desktop)
    {
        printf("Window manager's \"showing the desktop\" mode: %s\n",
               *showing_desktop == 1 ? "ON" : "OFF");
    }
    else
    {
        printf("Window manager's \"showing the desktop\" mode: N/A\n");
    }
#endif

    g_free(name_out);
    g_free(sup_window);
    g_free(wm_name);
    g_free(wm_class);
    g_free(wm_pid);
    g_free(showing_desktop);

    return EXIT_SUCCESS;
}

int WmCtrl::showing_desktop(Display *disp)
{
    unsigned long state;

    if (strcmp(options.param, "on") == 0)
    {
        state = 1;
    }
    else if (strcmp(options.param, "off") == 0)
    {
        state = 0;
    }
    else
    {
        fputs("The argument to the -k option must be either \"on\" or \"off\"\n", stderr);
        return EXIT_FAILURE;
    }

    return client_msg(disp, DefaultRootWindow(disp), "_NET_SHOWING_DESKTOP",
                      state, 0, 0, 0, 0);
}

int WmCtrl::change_viewport(Display *disp)
{
    unsigned long x, y;
    const char *argerr = "The -o option expects two integers separated with a comma.\n";

    if (sscanf(options.param, "%lu,%lu", &x, &y) == 2)
    {
        return client_msg(disp, DefaultRootWindow(disp), "_NET_DESKTOP_VIEWPORT",
                          x, y, 0, 0, 0);
    }
    else
    {
        fputs(argerr, stderr);
        return EXIT_FAILURE;
    }
}

int WmCtrl::change_geometry(Display *disp)
{
    unsigned long x, y;
    const char *argerr = "The -g option expects two integers separated with a comma.\n";

    if (sscanf(options.param, "%lu,%lu", &x, &y) == 2)
    {
        return client_msg(disp, DefaultRootWindow(disp), "_NET_DESKTOP_GEOMETRY",
                          x, y, 0, 0, 0);
    }
    else
    {
        fputs(argerr, stderr);
        return EXIT_FAILURE;
    }
}

int WmCtrl::change_number_of_desktops(Display *disp)
{
    unsigned long n;

    if (sscanf(options.param, "%lu", &n) != 1)
    {
        fputs("The -n option expects an integer.\n", stderr);
        return EXIT_FAILURE;
    }

    return client_msg(disp, DefaultRootWindow(disp), "_NET_NUMBER_OF_DESKTOPS",
                      n, 0, 0, 0, 0);
}

int WmCtrl::switch_desktop(Display *disp, char *desktop_id)
{
    int target = -1;

	if(NULL == desktop_id)
	{
        fputs("Invalid desktop ID.\n", stderr);
        return EXIT_FAILURE;
	}

    target = atoi(desktop_id);
    if (target == -1)
    {
        fputs("Invalid desktop ID.\n", stderr);
        return EXIT_FAILURE;
    }

    return client_msg(disp, DefaultRootWindow(disp), "_NET_CURRENT_DESKTOP",
                      (unsigned long)target, 0, 0, 0, 0);
}

void WmCtrl::window_set_title(Display *disp, Window win,
                              char *title, char mode)
{
    gchar *title_utf8;
    gchar *title_local;

    if (envir_utf8)
    {
        title_utf8 = g_strdup(title);
        title_local = NULL;
    }
    else
    {
        if (! (title_utf8 = g_locale_to_utf8(title, -1, NULL, NULL, NULL)))
        {
            title_utf8 = g_strdup(title);
        }
        title_local = g_strdup(title);
    }

    if (mode == 'T' || mode == 'N')
    {
        /* set name */
        if (title_local)
        {
            XChangeProperty(disp, win, XA_WM_NAME, XA_STRING, 8, PropModeReplace,
                            (unsigned char *)title_local, strlen(title_local));
        }
        else
        {
            XDeleteProperty(disp, win, XA_WM_NAME);
        }
        XChangeProperty(disp, win, XInternAtom(disp, "_NET_WM_NAME", False),
                        XInternAtom(disp, "UTF8_STRING", False), 8, PropModeReplace,
                        (unsigned char *)title_utf8, strlen(title_utf8));
    }

    if (mode == 'T' || mode == 'I')
    {
        /* set icon name */
        if (title_local)
        {
            XChangeProperty(disp, win, XA_WM_ICON_NAME, XA_STRING, 8, PropModeReplace,
                            (unsigned char *)title_local, strlen(title_local));
        }
        else
        {
            XDeleteProperty(disp, win, XA_WM_ICON_NAME);
        }
        XChangeProperty(disp, win, XInternAtom(disp, "_NET_WM_ICON_NAME", False),
                        XInternAtom(disp, "UTF8_STRING", False), 8, PropModeReplace,
                        (unsigned char *)title_utf8, strlen(title_utf8));
    }

    g_free(title_utf8);
    g_free(title_local);

}

int WmCtrl::window_to_desktop(Display *disp, Window win, int desktop)
{
    unsigned long *cur_desktop = NULL;
    Window root = DefaultRootWindow(disp);

    if (desktop == -1)
    {
        if (! (cur_desktop = (unsigned long *)get_property(disp, root,
                                                           XA_CARDINAL, "_NET_CURRENT_DESKTOP", NULL)))
        {
            if (! (cur_desktop = (unsigned long *)get_property(disp, root,
                                                               XA_CARDINAL, "_WIN_WORKSPACE", NULL)))
            {
                fputs("Cannot get current desktop properties. "
                      "(_NET_CURRENT_DESKTOP or _WIN_WORKSPACE property)"
                      "\n", stderr);
                return EXIT_FAILURE;
            }
        }
        desktop = *cur_desktop;
    }
    g_free(cur_desktop);

    return client_msg(disp, win, "_NET_WM_DESKTOP", (unsigned long)desktop,
                      0, 0, 0, 0);
}

int WmCtrl::activate_window(Display *disp, Window win,
                            gboolean switch_desktop)
{
    unsigned long *desktop;

    /* desktop ID */
    if ((desktop = (unsigned long *)get_property(disp, win,
                                                 XA_CARDINAL, "_NET_WM_DESKTOP", NULL)) == NULL)
    {
        if ((desktop = (unsigned long *)get_property(disp, win,
                                                     XA_CARDINAL, "_WIN_WORKSPACE", NULL)) == NULL)
        {
            p_verbose("Cannot find desktop ID of the window.\n");
        }
    }

    if (switch_desktop && desktop)
    {
        if (client_msg(disp, DefaultRootWindow(disp),
                       "_NET_CURRENT_DESKTOP",
                       *desktop, 0, 0, 0, 0) != EXIT_SUCCESS)
        {
            p_verbose("Cannot switch desktop.\n");
        }
    }

    if (desktop)
	g_free(desktop);

    client_msg(disp, win, "_NET_ACTIVE_WINDOW",
               0, 0, 0, 0, 0);
    XMapRaised(disp, win);

    return EXIT_SUCCESS;
}

int WmCtrl::close_window(Display *disp, Window win)
{
    return client_msg(disp, win, "_NET_CLOSE_WINDOW",
                      0, 0, 0, 0, 0);
}

int WmCtrl::window_state(Display *disp, Window win, char *arg)
{
    unsigned long action=0;
    Atom prop1 = 0;
    Atom prop2 = 0;
    char *p1, *p2;
    const char *argerr = "The -b option expects a list of comma separated parameters: \"(remove|add|toggle),<PROP1>[,<PROP2>]\"\n";

    if (!arg || strlen(arg) == 0)
    {
        fputs(argerr, stderr);
        return EXIT_FAILURE;
    }

    if ((p1 = strchr(arg, ',')))
    {
        gchar *tmp_prop1, *tmp1;

        *p1 = '\0';

        /* action */
        if (strcmp(arg, "remove") == 0)
        {
            action = _NET_WM_STATE_REMOVE;
        }
        else if (strcmp(arg, "add") == 0)
        {
            action = _NET_WM_STATE_ADD;
        }
        else if (strcmp(arg, "set") == 0)
        {
            action = _NET_WM_STATE_SET;
        }
        else if (strcmp(arg, "toggle") == 0)
        {
            action = _NET_WM_STATE_TOGGLE;
        }
        else
        {
            fputs("Invalid action. Use either remove, add or toggle.\n", stderr);
            return EXIT_FAILURE;
        }
        p1++;
        //if (strcasecmp(p1, "maximized") == 0)
        //{
        //    p1 = "maximized_horz,maximized_vert";
        //    p2 = "maximized_vert";
        //}

        /* the second property */
        if ((p2 = strchr(p1, ',')))
        {
            gchar *tmp_prop2, *tmp2;
            *p2 = '\0';
            p2++;
            if (strlen(p2) == 0)
            {
                fputs("Invalid zero length property.\n", stderr);
                return EXIT_FAILURE;
            }
            tmp_prop2 = g_strdup_printf("_NET_WM_STATE_%s", tmp2 = g_ascii_strup(p2, -1));
#ifdef DEBUG
            p_verbose("State 2: %s\n", tmp_prop2);
#endif
            prop2 = XInternAtom(disp, tmp_prop2, False);
            g_free(tmp2);
            g_free(tmp_prop2);
        }

        /* the first property */
        if (strlen(p1) == 0)
        {
            fputs("Invalid zero length property.\n", stderr);
            return EXIT_FAILURE;
        }
        tmp_prop1 = g_strdup_printf("_NET_WM_STATE_%s", tmp1 = g_ascii_strup(p1, -1));
#ifdef DEBUG
        p_verbose("State 1: %s\n", tmp_prop1);
#endif
        prop1 = XInternAtom(disp, tmp_prop1, False);
        g_free(tmp1);
        g_free(tmp_prop1);


        return client_msg(disp, win, "_NET_WM_STATE",
                          action, (unsigned long)prop1, (unsigned long)prop2, 0, 0);
    }
    else
    {
        fputs(argerr, stderr);
        return EXIT_FAILURE;
    }
}

gboolean WmCtrl::wm_supports(Display *disp, const gchar *prop)
{
    Atom xa_prop = XInternAtom(disp, prop, False);
    Atom *list;
    unsigned long size;
    size_t i;

    if (! (list = (Atom *)get_property(disp, DefaultRootWindow(disp),
                                       XA_ATOM, "_NET_SUPPORTED", &size)))
    {
        p_verbose("Cannot get _NET_SUPPORTED property.\n");
        return FALSE;
    }

    for (i = 0; i < size / sizeof(Atom); i++)
    {
        if (list[i] == xa_prop)
        {
            g_free(list);
            return TRUE;
        }
    }

    g_free(list);
    return FALSE;
}

int WmCtrl::window_move_resize(Display *disp, Window win, char *arg)
{
    signed long grav, x, y, w, h;
    unsigned long grflags;
    const char *argerr = "The -e option expects a list of comma separated integers: \"gravity,X,Y,width,height\"\n";

    if (!arg || strlen(arg) == 0)
    {
        fputs(argerr, stderr);
        return EXIT_FAILURE;
    }

    if (sscanf(arg, "%ld,%ld,%ld,%ld,%ld", &grav, &x, &y, &w, &h) != 5)
    {
        fputs(argerr, stderr);
        return EXIT_FAILURE;
    }

    if (grav < 0)
    {
        fputs("Value of gravity mustn't be negative. Use zero to use the default gravity of the window.\n", stderr);
        return EXIT_FAILURE;
    }

    grflags = grav;
    if (x != -1)
        grflags |= (1 << 8);
    if (y != -1)
        grflags |= (1 << 9);
    if (w != -1)
        grflags |= (1 << 10);
    if (h != -1)
        grflags |= (1 << 11);

#ifdef DEBUG
    p_verbose("grflags: %lu\n", grflags);
#endif

    if (wm_supports(disp, "_NET_MOVERESIZE_WINDOW"))
    {
        return client_msg(disp, win, "_NET_MOVERESIZE_WINDOW",
                          grflags, (unsigned long)x, (unsigned long)y, (unsigned long)w, (unsigned long)h);
    }
    else
    {
#ifdef DEBUG
        p_verbose("WM doesn't support _NET_MOVERESIZE_WINDOW. Gravity will be ignored.\n");
#endif
        if ((w < 1 || h < 1) && (x >= 0 && y >= 0))
        {
            XMoveWindow(disp, win, x, y);
        }
        else if ((x < 0 || y < 0) && (w >= 1 && h >= -1))
        {
            XResizeWindow(disp, win, w, h);
        }
        else if (x >= 0 && y >= 0 && w >= 1 && h >= 1)
        {
            XMoveResizeWindow(disp, win, x, y, w, h);
        }
        return EXIT_SUCCESS;
    }
}

int WmCtrl::action_window(Display *disp, Window win, char mode)
{
#ifdef DEBUG
    p_verbose("Using window: 0x%.8lx\n", win);
#endif

    switch (mode)
    {
        case 'a':
            return activate_window(disp, win, TRUE);

        case 'c':
            return close_window(disp, win);

        case 'e':
            /* resize/move the window around the desktop => -r -e */
            return window_move_resize(disp, win, options.param);

        case 'b':
            /* change state of a window => -r -b */
            return window_state(disp, win, options.param);

        case 't':
            /* move the window to the specified desktop => -r -t */
            return window_to_desktop(disp, win, atoi(options.param));

        case 'R':
            /* move the window to the current desktop and activate it => -r */
            if (window_to_desktop(disp, win, -1) == EXIT_SUCCESS)
            {
                usleep(100000); /* 100 ms - make sure the WM has enough
                                   time to move the window, before we activate it */
                return activate_window(disp, win, FALSE);
            }
            else
            {
                return EXIT_FAILURE;
            }

        case 'O':
            if ( set_override_redirect_recursive(disp, win, atoi(options.param)) == True )
                return EXIT_SUCCESS;
            else
                return EXIT_FAILURE;

        case 'N':
        case 'I':
        case 'T':
            window_set_title(disp, win, options.param, mode);
            return EXIT_SUCCESS;

        default:
            fprintf(stderr, "Unknown action: '%c'\n", mode);
            return EXIT_FAILURE;
    }
}

int WmCtrl::action_window_pid(Display *disp, char mode)
{
    unsigned long wid;

    if (sscanf(options.param_window, "0x%lx", &wid) != 1 &&
        sscanf(options.param_window, "0X%lx", &wid) != 1 &&
        sscanf(options.param_window, "%lu", &wid) != 1)
    {
        fputs("Cannot convert argument to number.\n", stderr);
        return EXIT_FAILURE;
    }

    return action_window(disp, (Window)wid, mode);
}

int WmCtrl::action_window_str(Display *disp, char mode)
{
    Window activate = 0;
    Window *client_list;
    unsigned long client_list_size;
    size_t i;

    if (strcmp(SELECT_WINDOW_MAGIC, options.param_window) == 0)
    {
        activate = Select_Window(disp);
        if (activate)
        {
            return action_window(disp, activate, mode);
        }
        else
        {
            return EXIT_FAILURE;
        }
    }
    if (strcmp(ACTIVE_WINDOW_MAGIC, options.param_window) == 0)
    {
        activate = get_active_window(disp);
        if (activate)
        {
            return action_window(disp, activate, mode);
        }
        else
        {
            return EXIT_FAILURE;
        }
    }
    else
    {
        if ((client_list = get_client_list(disp, &client_list_size)) == NULL)
        {
            return EXIT_FAILURE;
        }

        for (i = 0; i < client_list_size / sizeof(Window); i++)
        {
            gchar *match_utf8;
            if (options.show_class)
            {
                match_utf8 = get_window_class(disp, client_list[i]); /* UTF8 */
            }
            else
            {
                match_utf8 = get_window_title(disp, client_list[i]); /* UTF8 */
            }
            if (match_utf8)
            {
                gchar *match;
                gchar *match_cf;
                gchar *match_utf8_cf = NULL;
                if (envir_utf8)
                {
                    match = g_strdup(options.param_window);
                    match_cf = g_utf8_casefold(options.param_window, -1);
                }
                else
                {
                    if (! (match = g_locale_to_utf8(options.param_window, -1, NULL, NULL, NULL)))
                    {
                        match = g_strdup(options.param_window);
                    }
                    match_cf = g_utf8_casefold(match, -1);
                }

                if (!match || !match_cf)
                {
                    continue;
                }

                match_utf8_cf = g_utf8_casefold(match_utf8, -1);

                if ((options.full_window_title_match && strcmp(match_utf8, match) == 0) ||
                    (!options.full_window_title_match && strstr(match_utf8_cf, match_cf)))
                {
                    activate = client_list[i];
                    g_free(match);
                    g_free(match_cf);
                    g_free(match_utf8);
                    g_free(match_utf8_cf);
                    break;
                }
                g_free(match);
                g_free(match_cf);
                g_free(match_utf8);
                g_free(match_utf8_cf);
            }
        }
        g_free(client_list);

        if (activate)
        {
            return action_window(disp, activate, mode);
        }
        else
        {
            return EXIT_FAILURE;
        }
    }
}

bool WmCtrl::CurrentDesktop(unsigned long& ulCurrentDesktop)
{
    // new code
    Display *disp = NULL;

	/* necessary to make g_get_charset() and g_locale_*() work */
    setlocale(LC_ALL, "");

    init_charset();

    if (! (disp = XOpenDisplay(NULL)))
    {
        fputs("Cannot open display.\n", stderr);
        return false;
    }

	XLockDisplay(disp);

	Window root = DefaultRootWindow(disp);

	unsigned long *pCurrentDesktop = NULL;

	if (! (pCurrentDesktop = (unsigned long *)get_property(disp, root, XA_CARDINAL, "_NET_CURRENT_DESKTOP", NULL)))
    {
        if (! (pCurrentDesktop = (unsigned long *)get_property(disp, root, XA_CARDINAL, "_WIN_WORKSPACE", NULL)))
		{
			return false;
		}
	}

	ulCurrentDesktop = *pCurrentDesktop;

	g_free(pCurrentDesktop);

    XSync(disp, false);
    XUnlockDisplay(disp);
    if (disp)
        XCloseDisplay(disp);

	return true;
}

int WmCtrl::list_desktops(Display *disp)
{
    unsigned long *num_desktops = NULL;
    unsigned long *cur_desktop = NULL;
    unsigned long desktop_list_size = 0;
    unsigned long *desktop_geometry = NULL;
    unsigned long desktop_geometry_size = 0;
    gchar **desktop_geometry_str = NULL;
    unsigned long *desktop_viewport = NULL;
    unsigned long desktop_viewport_size = 0;
    gchar **desktop_viewport_str = NULL;
    unsigned long *desktop_workarea = NULL;
    unsigned long desktop_workarea_size = 0;
    gchar **desktop_workarea_str = NULL;
    gchar *list = NULL;
    size_t i;
    size_t id;
    Window root = DefaultRootWindow(disp);
    int ret = EXIT_FAILURE;
    gchar **names = NULL;
    gboolean names_are_utf8 = TRUE;

    if (! (num_desktops = (unsigned long *)get_property(disp, root,
                                                        XA_CARDINAL, "_NET_NUMBER_OF_DESKTOPS", NULL)))
    {
        if (! (num_desktops = (unsigned long *)get_property(disp, root,
                                                            XA_CARDINAL, "_WIN_WORKSPACE_COUNT", NULL)))
        {
            fputs("Cannot get number of desktops properties. "
                  "(_NET_NUMBER_OF_DESKTOPS or _WIN_WORKSPACE_COUNT)"
                  "\n", stderr);
            goto cleanup;
        }
    }

    if (! (cur_desktop = (unsigned long *)get_property(disp, root,
                                                       XA_CARDINAL, "_NET_CURRENT_DESKTOP", NULL)))
    {
        if (! (cur_desktop = (unsigned long *)get_property(disp, root,
                                                           XA_CARDINAL, "_WIN_WORKSPACE", NULL)))
        {
            fputs("Cannot get current desktop properties. "
                  "(_NET_CURRENT_DESKTOP or _WIN_WORKSPACE property)"
                  "\n", stderr);
            goto cleanup;
        }
    }

    if (options.wa_desktop_titles_invalid_utf8 ||
        (list = get_property(disp, root,
                             XInternAtom(disp, "UTF8_STRING", False),
                             "_NET_DESKTOP_NAMES", &desktop_list_size)) == NULL)
    {
        names_are_utf8 = FALSE;
        if ((list = get_property(disp, root,
                                 XA_STRING,
                                 "_WIN_WORKSPACE_NAMES", &desktop_list_size)) == NULL)
        {
            p_verbose("Cannot get desktop names properties. "
                      "(_NET_DESKTOP_NAMES or _WIN_WORKSPACE_NAMES)"
                      "\n");
            /* ignore the error - list the desktops without names */
        }
    }

    /* common size of all desktops */
    if (! (desktop_geometry = (unsigned long *)get_property(disp, DefaultRootWindow(disp),
                                                            XA_CARDINAL, "_NET_DESKTOP_GEOMETRY", &desktop_geometry_size)))
    {
        p_verbose("Cannot get common size of all desktops (_NET_DESKTOP_GEOMETRY).\n");
    }

    /* desktop viewport */
    if (! (desktop_viewport = (unsigned long *)get_property(disp, DefaultRootWindow(disp),
                                                            XA_CARDINAL, "_NET_DESKTOP_VIEWPORT", &desktop_viewport_size)))
    {
        p_verbose("Cannot get common size of all desktops (_NET_DESKTOP_VIEWPORT).\n");
    }

    /* desktop workarea */
    if (! (desktop_workarea = (unsigned long *)get_property(disp, DefaultRootWindow(disp),
                                                            XA_CARDINAL, "_NET_WORKAREA", &desktop_workarea_size)))
    {
        if (! (desktop_workarea = (unsigned long *)get_property(disp, DefaultRootWindow(disp),
                                                                XA_CARDINAL, "_WIN_WORKAREA", &desktop_workarea_size)))
        {
            p_verbose("Cannot get _NET_WORKAREA property.\n");
        }
    }

    /* prepare the array of desktop names */
    names = (char **)g_malloc0(*num_desktops * sizeof(char *));
    if (list)
    {
        id = 0;
        names[id++] = list;
        for (i = 0; i < desktop_list_size; i++)
        {
            if (list[i] == '\0')
            {
                if (id >= *num_desktops)
                {
                    break;
                }
                names[id++] = list + i + 1;
            }
        }
    }

    /* prepare desktop geometry strings */
    desktop_geometry_str = (char **)g_malloc0((*num_desktops + 1) * sizeof(char *));
    if (desktop_geometry && desktop_geometry_size > 0)
    {
        if (desktop_geometry_size == 2 * sizeof(*desktop_geometry))
        {
            /* only one value - use it for all desktops */
            p_verbose("WM provides _NET_DESKTOP_GEOMETRY value common for all desktops.\n");
            for (i = 0; i < *num_desktops; i++)
            {
                desktop_geometry_str[i] = g_strdup_printf("%lux%lu",
                                                          desktop_geometry[0], desktop_geometry[1]);
            }
        }
        else
        {
            /* separate values for desktops of different size */
            p_verbose("WM provides separate _NET_DESKTOP_GEOMETRY value for each desktop.\n");
            for (i = 0; i < *num_desktops; i++)
            {
                if (i < desktop_geometry_size / sizeof(*desktop_geometry) / 2)
                {
                    desktop_geometry_str[i] = g_strdup_printf("%lux%lu",
                                                              desktop_geometry[i*2], desktop_geometry[i*2+1]);
                }
                else
                {
                    desktop_geometry_str[i] = g_strdup("N/A");
                }
            }
        }
    }
    else
    {
        for (i = 0; i < *num_desktops; i++)
        {
            desktop_geometry_str[i] = g_strdup("N/A");
        }
    }

    /* prepare desktop viewport strings */
    desktop_viewport_str = (char **)g_malloc0((*num_desktops + 1) * sizeof(char *));
    if (desktop_viewport && desktop_viewport_size > 0)
    {
        if (desktop_viewport_size == 2 * sizeof(*desktop_viewport))
        {
            /* only one value - use it for current desktop */
            p_verbose("WM provides _NET_DESKTOP_VIEWPORT value only for the current desktop.\n");
            for (i = 0; i < *num_desktops; i++)
            {
                if (i == *cur_desktop)
                {
                    desktop_viewport_str[i] = g_strdup_printf("%lu,%lu",
                                                              desktop_viewport[0], desktop_viewport[1]);
                }
                else
                {
                    desktop_viewport_str[i] = g_strdup("N/A");
                }
            }
        }
        else
        {
            /* separate values for each of desktops */
            for (i = 0; i < *num_desktops; i++)
            {
                if (i < desktop_viewport_size / sizeof(*desktop_viewport) / 2)
                {
                    desktop_viewport_str[i] = g_strdup_printf("%lu,%lu",
                                                              desktop_viewport[i*2], desktop_viewport[i*2+1]);
                }
                else
                {
                    desktop_viewport_str[i] = g_strdup("N/A");
                }
            }
        }
    }
    else
    {
        for (i = 0; i < *num_desktops; i++)
        {
            desktop_viewport_str[i] = g_strdup("N/A");
        }
    }

    /* prepare desktop workarea strings */
    desktop_workarea_str = (char **)g_malloc0((*num_desktops + 1) * sizeof(char *));
    if (desktop_workarea && desktop_workarea_size > 0)
    {
        if (desktop_workarea_size == 4 * sizeof(*desktop_workarea))
        {
            /* only one value - use it for current desktop */
            p_verbose("WM provides _NET_WORKAREA value only for the current desktop.\n");
            for (i = 0; i < *num_desktops; i++)
            {
                if (i == *cur_desktop)
                {
                    desktop_workarea_str[i] = g_strdup_printf("%lu,%lu %lux%lu",
                                                              desktop_workarea[0], desktop_workarea[1],
                                                              desktop_workarea[2], desktop_workarea[3]);
                }
                else
                {
                    desktop_workarea_str[i] = g_strdup("N/A");
                }
            }
        }
        else
        {
            /* separate values for each of desktops */
            for (i = 0; i < *num_desktops; i++)
            {
                if (i < desktop_workarea_size / sizeof(*desktop_workarea) / 4)
                {
                    desktop_workarea_str[i] = g_strdup_printf("%lu,%lu %lux%lu",
                                                              desktop_workarea[i*4], desktop_workarea[i*4+1],
                                                              desktop_workarea[i*4+2], desktop_workarea[i*4+3]);
                }
                else
                {
                    desktop_workarea_str[i] = g_strdup("N/A");
                }
            }
        }
    }
    else
    {
        for (i = 0; i < *num_desktops; i++)
        {
            desktop_workarea_str[i] = g_strdup("N/A");
        }
    }

    /* print the list */
    for (i = 0; i < *num_desktops; i++)
    {
        gchar *out = get_output_str(names[i], names_are_utf8);
        printf("%-2d %c DG: %-*s  VP: %-*s  WA: %-*s  %s\n", i, i == *cur_desktop ? '*' : '-',
               longest_str(desktop_geometry_str), desktop_geometry_str[i],
               longest_str(desktop_viewport_str), desktop_viewport_str[i],
               longest_str(desktop_workarea_str), desktop_workarea_str[i],
               out ? out : "N/A");
        g_free(out);
    }

    p_verbose("Total number of desktops: %lu\n", *num_desktops);
    p_verbose("Current desktop ID (counted from zero): %lu\n", *cur_desktop);

    ret = EXIT_SUCCESS;
    goto cleanup;

  cleanup:
    g_free(names);
    g_free(num_desktops);
    g_free(cur_desktop);
    g_free(desktop_geometry);
    g_strfreev(desktop_geometry_str);
    g_free(desktop_viewport);
    g_strfreev(desktop_viewport_str);
    g_free(desktop_workarea);
    g_strfreev(desktop_workarea_str);
    g_free(list);

    return ret;
}

int WmCtrl::longest_str(gchar **strv)
{
    size_t max = 0;
    size_t i = 0;

    while (strv && strv[i])
    {
        if (strlen(strv[i]) > max)
        {
            max = strlen(strv[i]);
        }
        i++;
    }

    return max;
}

Window * WmCtrl::get_client_list(Display *disp, unsigned long *size)
{
    Window *client_list;

    if ((client_list = (Window *)get_property(disp, DefaultRootWindow(disp),
                                              XA_WINDOW, "_NET_CLIENT_LIST", size)) == NULL)
    {
        if ((client_list = (Window *)get_property(disp, DefaultRootWindow(disp),
                                                  XA_CARDINAL, "_WIN_CLIENT_LIST", size)) == NULL)
        {
            fputs("Cannot get client list properties. \n"
                  "(_NET_CLIENT_LIST or _WIN_CLIENT_LIST)"
                  "\n", stderr);
            return NULL;
        }
    }

    return client_list;
}

int WmCtrl::set_override_redirect(Display *dpy, Window window, bool bValue)
{
    int status = 0;
    XWindowAttributes war;
    status = XGetWindowAttributes(dpy, window, &war);
    XSetWindowAttributes swa;
    swa.override_redirect = bValue ? True : False;
    status = XChangeWindowAttributes(dpy, window, CWOverrideRedirect, &swa);
    fprintf(stderr, "set_override_redirect() : window==0x%lX, override_redirect==%s=>%s\n",
            window,
            war.override_redirect ? "true" : "false",
            swa.override_redirect ? "true" : "false"
            );
    return status;
}

int WmCtrl::set_override_redirect_recursive(Display *dpy, Window window, bool bValue)
{
    Window root_window = XDefaultRootWindow(dpy);
    fprintf(stderr, "set_override_redirect_recursive() : param_window=='%s' root_window==0x%lX\n", options.param_window, root_window);
    int status = 0;
    while ( (window) && (window != root_window) )
    {
        fprintf(stderr, "set_override_redirect_recursive() : window==0x%lX\n", window);
        status = set_override_redirect(dpy, window, bValue);
        if (status != True)
        {
            fprintf(stderr, "set_override_redirect_recursive() : status==%d => stopping tree-walk\n", status);
            break;
        }
        Window root_return = 0;
        Window parent_return = 0;
        Window *children_return = NULL;
        unsigned int nchildren_return = 0;
        status = XQueryTree(
            dpy, window,
            &root_return, &parent_return, &children_return, &nchildren_return
            );
        if (children_return != None)
            XFree(children_return);
        if (status != True)
        {
            fprintf(stderr, "set_override_redirect_recursive() : cannot read window tree\n");
            break;
        }
        window = parent_return;
    }
    fprintf(stderr, "set_override_redirect_recursive() : param_window=='%s' root_window==0x%lX => RetCode==%d\n", options.param_window, root_window, status);
    return status;
}

int WmCtrl::list_windows(Display *disp, std::list<WinInfo> *pListWinInfo/*=NULL*/)
{
    Window *client_list;
    unsigned long client_list_size;
    size_t i;
    int max_client_machine_len = 0;

    if ((client_list = get_client_list(disp, &client_list_size)) == NULL)
    {
        return EXIT_FAILURE;
    }

    /* find the longest client_machine name */
    for (i = 0; i < client_list_size / sizeof(Window); i++)
    {
        gchar *client_machine;
        if ((client_machine = get_property(disp, client_list[i],
                                           XA_STRING, "WM_CLIENT_MACHINE", NULL)))
        {
            max_client_machine_len = strlen(client_machine);
        }
        g_free(client_machine);
    }

#ifdef DEBUG
    /* list header and tail */
    const char s1[]="=======================================================================================\n";
    const char s2[]="| WindowID |Dsk|Layer|  PID  |      Geometry      |        Class         |ClMach|Title|\n";
    const char s3[]="|----------|---|-----|-------|--------------------|----------------------|------|-----|\n";
#endif

#ifdef DEBUG
    if (options.list_show_all)
        fprintf(stderr, "%s%s%s", s1, s2, s3);
#endif

    /* print the list */
    for (i = 0; i < client_list_size / sizeof(Window); i++)
    {
        WinInfo itemWinInfo;
		itemWinInfo.ulWindowId = client_list[i];
        gchar *title_utf8 = get_window_title(disp, client_list[i]); /* UTF8 */
        gchar *title_out = get_output_str(title_utf8, TRUE);
        gchar *client_machine;
        gchar *class_out = get_window_class(disp, client_list[i]); /* UTF8 */
        unsigned long *pid;
        unsigned long *desktop;
        int x, y, junkx, junky;
        unsigned int wwidth, wheight, bw, depth;
        Window junkroot;
        unsigned long *layer;

        /* desktop ID */
        if ((desktop = (unsigned long *)get_property(disp, client_list[i],
                                                     XA_CARDINAL, "_NET_WM_DESKTOP", NULL)) == NULL)
        {
            desktop = (unsigned long *)get_property(disp, client_list[i],
                                                    XA_CARDINAL, "_WIN_WORKSPACE", NULL);
        }

        /* client machine */
        client_machine = get_property(disp, client_list[i],
                                      XA_STRING, "WM_CLIENT_MACHINE", NULL);

        /* pid */
        pid = (unsigned long *)get_property(disp, client_list[i],
                                            XA_CARDINAL, "_NET_WM_PID", NULL);

        /* geometry */
        XGetGeometry (disp, client_list[i], &junkroot, &junkx, &junky,
                      &wwidth, &wheight, &bw, &depth);
        XTranslateCoordinates (disp, client_list[i], junkroot, junkx, junky,
                               &x, &y, &junkroot);

#ifdef DEBUG
        /* special desktop ID -1 means "all desktops", so we
           have to convert the desktop value to signed long */
        printf("%s", (options.list_show_all)?"|":"");
        printf("0x%.8lx", client_list[i]);
        printf("%s", (options.list_show_all)?"|":"");
        printf(" %2ld", desktop ? (signed long)*desktop : 0);
#endif
        itemWinInfo.lDesktop = desktop ? (signed long)*desktop : 0;

        if (options.list_show_all)
        {
            /* layer */
            layer = (unsigned long *)get_property(disp, client_list[i],
                                                  XA_CARDINAL, "_WIN_LAYER", NULL);

#ifdef DEBUG
            printf("%s", (options.list_show_all)?"|":"");
            printf(" %s%3ld",
                   (*layer == 4)
                   ? "N"
                   : ( (*layer < 4)
                       ?"B"
                       :"A" )
                   , *layer
                   );
#endif
        }

#ifdef DEBUG
        printf("%s", (options.list_show_all)?"|":"");
        if (options.show_pid || options.list_show_all)
        {
            printf(" %-6lu", pid ? *pid : 0);
        }
#endif
        itemWinInfo.ulPid = pid ? *pid : 0;

#ifdef DEBUG
        printf("%s", (options.list_show_all)?"|":"");
        if (options.show_geometry || options.list_show_all)
        {
            printf(" %-4d %-4d %-4d %-4d", x, y, wwidth, wheight);
        }
#endif
        itemWinInfo.x = x;
        itemWinInfo.y = y;
        itemWinInfo.w = wwidth;
        itemWinInfo.h = wheight;

#ifdef DEBUG
        printf("%s", (options.list_show_all)?"|":"");
        if (options.show_class || options.list_show_all)
        {
            printf(" %-20s ", class_out ? class_out : "N/A");
        }
#endif

        itemWinInfo.sClassName = class_out ? class_out : "";

#ifdef DEBUG
        printf("%s", (options.list_show_all)?"|":"");
        //printf(" %*s %s\n",
        //       max_client_machine_len,
        //       client_machine ? client_machine : "N/A",
        //       title_out ? title_out : "N/A"
        //       );
        printf(" %*s", max_client_machine_len, client_machine ? client_machine : "N/A");
        printf("%s", (options.list_show_all)?"|":"");
        printf(" %s", title_out ? title_out : "N/A");
#endif

        itemWinInfo.sClientMachine = client_machine ? client_machine : "";
        itemWinInfo.sTitle = title_out ? title_out : "";

#ifdef DEBUG
        printf("%s\n", (options.list_show_all)?" |":"");
#endif

        g_free(title_utf8);
        g_free(title_out);
        g_free(desktop);
        g_free(client_machine);
        g_free(class_out);
        g_free(pid);

        if (pListWinInfo)
        {
            pListWinInfo->push_back(itemWinInfo);
        }
    }
    g_free(client_list);

#ifdef DEBUG
    if (options.list_show_all)
        fprintf(stderr, "%s%s%s", s3, s2, s1);
#endif

    return EXIT_SUCCESS;
}

gchar * WmCtrl::get_window_class(Display *disp, Window win)
{
    gchar *class_utf8;
    gchar *wm_class;
    unsigned long size;

    wm_class = get_property(disp, win, XA_STRING, "WM_CLASS", &size);
    if (wm_class)
    {
        gchar *p_0 = strchr(wm_class, '\0');
        if (wm_class + size - 1 > p_0)
        {
            *(p_0) = '.';
        }
        class_utf8 = g_locale_to_utf8(wm_class, -1, NULL, NULL, NULL);
    }
    else
    {
        class_utf8 = NULL;
    }

    g_free(wm_class);

    return class_utf8;
}

gchar * WmCtrl::get_window_title(Display *disp, Window win)
{
    gchar *title_utf8;
    gchar *wm_name;
    gchar *net_wm_name;

    wm_name = get_property(disp, win, XA_STRING, "WM_NAME", NULL);
    net_wm_name = get_property(disp, win,
                               XInternAtom(disp, "UTF8_STRING", False), "_NET_WM_NAME", NULL);

    if (net_wm_name)
    {
        title_utf8 = g_strdup(net_wm_name);
    }
    else
    {
        if (wm_name)
        {
            title_utf8 = g_locale_to_utf8(wm_name, -1, NULL, NULL, NULL);
        }
        else
        {
            title_utf8 = NULL;
        }
    }

    g_free(wm_name);
    g_free(net_wm_name);

    return title_utf8;
}

gchar * WmCtrl::get_property(Display *disp, Window win,
                             Atom xa_prop_type, gchar *prop_name, unsigned long *size)
{
    Atom xa_prop_name;
    Atom xa_ret_type;
    int ret_format;
    unsigned long ret_nitems;
    unsigned long ret_bytes_after;
    unsigned long tmp_size;
    unsigned char *ret_prop;
    gchar *ret;

    xa_prop_name = XInternAtom(disp, prop_name, False);

    /* MAX_PROPERTY_VALUE_LEN / 4 explanation (XGetWindowProperty manpage):
     *
     * long_length = Specifies the length in 32-bit multiples of the
     *               data to be retrieved.
     */
    if (XGetWindowProperty(disp, win, xa_prop_name, 0, MAX_PROPERTY_VALUE_LEN / 4, False,
                           xa_prop_type, &xa_ret_type, &ret_format,
                           &ret_nitems, &ret_bytes_after, &ret_prop) != Success)
    {
        p_verbose("Cannot get %s property.\n", prop_name);
        return NULL;
    }

    if (xa_ret_type != xa_prop_type)
    {
#ifdef DEBUG
        p_verbose("Invalid type of %s property.\n", prop_name);
#endif
        XFree(ret_prop);
        return NULL;
    }

    /* null terminate the result to make string handling easier */
#ifdef ARCH64
	if(ret_format == 32)
		ret_format = 64; 
#endif

	tmp_size = (ret_format / 8) * ret_nitems;

    ret = (char *)g_malloc(tmp_size + 1);
    memcpy(ret, ret_prop, tmp_size);
    ret[tmp_size] = '\0';

    if (size)
    {
        *size = tmp_size;
    }

    XFree(ret_prop);
    return ret;
}

Window WmCtrl::Select_Window(Display *dpy)
{
    /*
     * Routine to let user select a window using the mouse
     * Taken from xfree86.
     */

    int status;
    Cursor cursor;
    XEvent event;
    Window target_win = None, root = DefaultRootWindow(dpy);
    int buttons = 0;
    int dummyi;
    unsigned int dummy;

    /* Make the target cursor */
    cursor = XCreateFontCursor(dpy, XC_crosshair);

    /* Grab the pointer using target cursor, letting it room all over */
    status = XGrabPointer(dpy, root, False,
                          ButtonPressMask|ButtonReleaseMask, GrabModeSync,
                          GrabModeAsync, root, cursor, CurrentTime);
    if (status != GrabSuccess)
    {
        fputs("ERROR: Cannot grab mouse.\n", stderr);
        return 0;
    }

    /* Let the user select a window... */
    while ((target_win == None) || (buttons != 0))
    {
        /* allow one more event */
        XAllowEvents(dpy, SyncPointer, CurrentTime);
        XWindowEvent(dpy, root, ButtonPressMask|ButtonReleaseMask, &event);
        switch (event.type)
        {
            case ButtonPress:
                if (target_win == None)
                {
                    target_win = event.xbutton.subwindow; /* window selected */
                    if (target_win == None)
                        target_win = root;
                }
                buttons++;
                break;
            case ButtonRelease:
                if (buttons > 0) /* there may have been some down before we started */
                    buttons--;
                break;
        }
    }

    XUngrabPointer(dpy, CurrentTime);      /* Done with pointer */

    if (XGetGeometry (dpy, target_win, &root, &dummyi, &dummyi,
                      &dummy, &dummy, &dummy, &dummy) && target_win != root)
    {
        target_win = XmuClientWindow (dpy, target_win);
    }

    return(target_win);
}

Window WmCtrl::get_active_window(Display *disp)
{
    char *prop;
    unsigned long size;
    Window ret = (Window)0;

    prop = get_property(disp, DefaultRootWindow(disp), XA_WINDOW,
                        "_NET_ACTIVE_WINDOW", &size);
    if (prop)
    {
        ret = *((Window*)prop);
        g_free(prop);
    }

    return(ret);
}
