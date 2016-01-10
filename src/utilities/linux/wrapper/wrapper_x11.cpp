//
// Author : Remus C.
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wrapper_x11.h"
#endif

#include "wrapper_x11.h"
#include "image_file.h"
#include <X11/extensions/shape.h>

#ifdef USE_DEBUG_CODE
#include "define_all.h"
#else
#include "../../defines/define_all.h"
#endif // USE_DEBUG_CODE

#define OPAQUE  0xffffffff
#define OPACITY "_NET_WM_WINDOW_OPACITY"



// used in loops which waits the WM or X11 to complete an action
#define WAIT_FOR_COMPLETION_MAX_ITERATIONS  10
#define WAIT_FOR_COMPLETION_INTERVAL_USLEEP 1000

// do not use the error handler grabber
static const bool g_bNoErrorHandler    = getenv("X11_NO_ERRORHANDLER");

// synchronize the entire app, for debug purposes
// WARNING: the app will run slowly
static const bool g_bXSynchronize_All  = getenv("X11_XSYNCHRONIZE_ALL");

// synchronize each lock-unlock calls, for debug purposes
// WARNING: the app will run slowly
static const bool g_bXSynchronize_Each = getenv("X11_XSYNCHRONIZE_EACH");

// call a log function after all Xlib functions that generate protocol requests
// for debug purposes
// WARNING: the app will run very very slow
static const bool g_bXAfterFunction  = getenv("X11_XAFTERFUNCTION");

// WARNING: the app will behave very badly
static const bool g_bXNoLock  = getenv("X11_NO_XLOCK");

// algorithm
/*
    {
    // X11 status return
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
    .......
    xcode = Xfunction(args); // status return
    _COND_XSTAT_LOG_ERR_BREAK(xcode);
    xcode = Xfunction(args); // bool return
    _COND_XBOOL_LOG_ERR_BREAK(xcode)
    .......
    _LOG_XERROR_BREAK("error");
    .......
    } while ((xcode = 0));
    // cleanup
    return (xcode == 0);
    }
*/

// xcode : X11 status return
#define _COND_XSTAT_LOG_ERR_BREAK(xcode) \
    if (xcode != 0) \
    { \
        _LOG_ERR("error_code==%d => ErrorText=='%s'", xcode, GetErrorText(GetDisplay(), xcode).c_str()); \
        break; \
    } \
    else do {} while (0)

// xcode : X11 boolean return
#define _COND_XBOOL_LOG_ERR_BREAK(xcode) \
    if (xcode != True) \
    { \
        _LOG_ERR("(%s) retBool==false", #xcode); \
        break; \
    } \
    else do {} while (0)

#define _LOG_XERROR_BREAK(...) \
    if (true) \
    { \
        _LOG_ERR(__VA_ARGS__); \
        break; \
    } \
    else do {} while (0)

// sync and lock functions implementation

bool X11_Sync(Display *pDisplay)
{
    if (pDisplay == NULL)
    {
        _LOG_ERR("NULL Display pointer");
        return false;
    }
    //_LOG_NFO("pDisplay==%p", pDisplay);
    //XFlush(pDisplay); // already called by XSync
    XSync(pDisplay, false);
    return true;
}

bool X11_Lock(Display *pDisplay)
{
    if (pDisplay == NULL)
    {
        _LOG_ERR("NULL Display pointer");
        return false;
    }
    //_LOG_NFO("pDisplay==%p", pDisplay);
    if (g_bXSynchronize_Each)
        XSynchronize(pDisplay, true);
    if (! g_bXNoLock)
        XLockDisplay(pDisplay);
    return true;
}

bool X11_Unlock(Display *pDisplay)
{
    if (pDisplay == NULL)
    {
        _LOG_ERR("NULL Display pointer");
        return false;
    }
    //_LOG_NFO("pDisplay==%p", pDisplay);
    X11_Sync(pDisplay);
    if (! g_bXNoLock)
        XUnlockDisplay(pDisplay);
    if (g_bXSynchronize_Each)
        XSynchronize(pDisplay, false);
    return true;
}

// class X11_Locker implementation

X11_Locker::X11_Locker(Display *pDisplay)
        : v_pDisplay(pDisplay)
{
    X11_Lock(v_pDisplay);
}

X11_Locker::~X11_Locker()
{
    X11_Unlock(v_pDisplay);
}

// class X11_Locker_NewDisplay implementation

X11_Locker_NewDisplay::X11_Locker_NewDisplay()
        : v_pDisplay(NULL)
{
    v_pDisplay = XOpenDisplay(NULL);
    X11_Lock(v_pDisplay);
}

X11_Locker_NewDisplay::~X11_Locker_NewDisplay()
{
    X11_Unlock(v_pDisplay);
    if (v_pDisplay)
        XCloseDisplay(v_pDisplay);
}

Display * X11_Locker_NewDisplay::GetDisplay()
{
    return v_pDisplay;
}

// class X11wrapper implementation

X11wrapper::X11wrapper(Display * pDisplay/*=NULL*/)
        : v_pDisplay(NULL)
        , v_bIsAssigned_Display(false)
        , v_oMainWindow(0)
        , v_pOld_XErrorHandler(NULL)
        , v_bIsChanged_XErrorHandler(false)
        , v_window_Mouse_Constrain(0)
        , v_bExtension_Shape_IsInitialized(false)
        , v_bExtension_Shape_IsAvailable(false)
        , v_nExtension_Shape_Version_Major(0)
        , v_nExtension_Shape_Version_Minor(0)
        , v_pOld_X_AfterFunction(NULL)
        , v_bIsChanged_X_AfterFunction(false)
{
#ifdef DEBUG	
    _LOG_NFO("env : X11_NO_ERRORHANDLER==%d", g_bNoErrorHandler);
    _LOG_NFO("env : X11_XSYNCHRONIZE_ALL==%d", g_bXSynchronize_All);
    _LOG_NFO("env : X11_XSYNCHRONIZE_EACH==%d", g_bXSynchronize_Each);
    _LOG_NFO("env : X11_XAFTERFUNCTION==%d", g_bXAfterFunction);
    _LOG_NFO("env : X11_NO_XLOCK==%d", g_bXNoLock);
    _LOG_NFO("pDisplay==%p", pDisplay);
#endif

    if (pDisplay)
        Assign_Display(pDisplay);
    if (! g_bNoErrorHandler)
        ErrorHandler_Set();
#ifdef USE_XDGA
    _COND(XDGA_Init());
#endif // USE_XDGA
}

X11wrapper::~X11wrapper()
{
#ifdef DEBUG
    _LOG_NFO("pDisplay==%p", GetDisplay());
#endif

#ifdef USE_XDGA
    _COND(XDGA_Exit());
#endif // USE_XDGA
    Clean_Exit();
}

bool X11wrapper::IsInitialized()
{
#ifdef DEBUG
    _LOG_NFO("pDisplay==%p", GetDisplay());
#endif

    return (GetDisplay() != NULL);
}

bool X11wrapper::Clean_Exit()
{
    if (GetDisplay() == NULL)
        return true;

#ifdef DEBUG
    _LOG_NFO();
#endif

    if (v_bIsChanged_X_AfterFunction)
        AfterFunction_Restore();
    if (v_bIsChanged_XErrorHandler)
        ErrorHandler_Restore();
    Mouse_Constrain_Release();
    Display_Close();
    v_pDisplay = NULL;
    return true;
}

void X11wrapper::Assign_Display(Display * pDisplay)
{
#ifdef DEBUG
    if (v_pDisplay && pDisplay)
    {
        _LOG_WRN("will overwrite the display");
    }
#endif

    v_pDisplay = pDisplay;

#ifdef DEBUG
    _LOG_NFO("pDisplay==%p", GetDisplay());
#endif

    v_bIsAssigned_Display = (GetDisplay() != NULL);
    if (g_bXSynchronize_All)
        XSynchronize(GetDisplay(), true);
    if (g_bXAfterFunction)
        AfterFunction_Set();
}

Display * X11wrapper::Display_Open()
{
    if (GetDisplay() != NULL)
    {
#ifdef DEBUG
        _LOG_NFO("pDisplay==%p", GetDisplay());
#endif
        return GetDisplay();
    }
    v_pDisplay = XOpenDisplay(NULL);

#ifdef DEBUG
    if (GetDisplay() == NULL)
        _LOG_ERR("pDisplay==%p", GetDisplay());
    else
        _LOG_NFO("pDisplay==%p", GetDisplay());
#endif

    return GetDisplay();
}

bool X11wrapper::Display_Close()
{
    if (GetDisplay() == NULL)
    {
#ifdef DEBUG
        _LOG_ERR("pDisplay==%p", GetDisplay());
#endif
        return false;
    }
    if (v_bIsAssigned_Display)
    {
#ifdef DEBUG
        _LOG_NFO("pDisplay==%p was assigned, not closing", GetDisplay());
#endif
        return true;
    }

#ifdef DEBUG
    _LOG_NFO("closing pDisplay==%p", GetDisplay());
#endif

    return XCloseDisplay(GetDisplay());
}

void X11wrapper::Assign_MainWindow(Window window)
{
#ifdef DEBUG
    if (v_oMainWindow && window)
    {
        _LOG_WRN("will overwrite main window id");
    }
    _LOG_NFO("window==%d", window);
#endif

    v_oMainWindow = window;
}

Window X11wrapper::GetMainWindow()
{
    return v_oMainWindow;
}

void X11wrapper::Assign_WmWindow(Window window)
{
#ifdef DEBUG
    if (v_oWmWindow && window)
    {
        _LOG_WRN("will overwrite wm window id");
    }
    _LOG_NFO("wmwindow==%d", window);
#endif

    v_oWmWindow = window;
}

Window X11wrapper::GetWmWindow()
{
    return v_oWmWindow;
}

Display * X11wrapper::GetDisplay()
{
    return v_pDisplay;
}

int X11wrapper::GetScreen()
{
    return XDefaultScreen(GetDisplay());
}

string X11wrapper::GetDisplayString()
{
    return XDisplayString(GetDisplay());
}

int X11wrapper::GetDisplayWidth()
{
    return XDisplayWidth(GetDisplay(), GetScreen());
}

int X11wrapper::GetDisplayHeight()
{
    return XDisplayHeight(GetDisplay(), GetScreen());
}

bool X11wrapper::GetDisplaySize(int &nWidth, int &nHeight)
{
    nWidth = GetDisplayWidth();
    nHeight = GetDisplayHeight();
    return true;
}

Window X11wrapper::Window_GetRoot()
{
    return XDefaultRootWindow(GetDisplay());
}

string X11wrapper::GetErrorText(Display * pDisplay, int error_code)
{
    if (error_code == 0)
        return "";
    static const int size_buffer = 10000;
    char buffer[size_buffer];
    buffer[0] = '\0';
    buffer[0] = 0;
    XGetErrorText(pDisplay, error_code, buffer, size_buffer);
    return buffer;
}

int X11wrapper::ErrorHandler_Grabber(Display *pDisplay, XErrorEvent *pXErrorEvent)
{
    static const char s_header[] = "Xlib: ErrorHandler_Grabber()";
    _LOG_ERR("%s", s_header);
    cerr << "\n" << s_header << "\n";
    cerr << "  type : " << pXErrorEvent->type << "\n";
    cerr << "  display : " << pXErrorEvent->display << "\n";
    cerr << "  serial : " << pXErrorEvent->serial << "\n";
    cerr << "  error_code : " << pXErrorEvent->error_code << "\n";
    cerr << "  request_code : " << pXErrorEvent->request_code << "\n";
    cerr << "  minor_code : " << pXErrorEvent->minor_code << "\n";
    cerr << "  resourceid : " << pXErrorEvent->resourceid << "\n";
    cerr << "ErrorText : " << GetErrorText(pDisplay, pXErrorEvent->error_code) << "\n";
    cerr << "\n";
    return 0;
}

bool X11wrapper::ErrorHandler_Set()
{
    _LOG_NFO();
    if (v_bIsChanged_XErrorHandler)
    {
        _LOG_ERR("already changed");
        return false;
    }
    v_pOld_XErrorHandler = XSetErrorHandler(X11wrapper::ErrorHandler_Grabber);
    _LOG_NFO("pOld_XErrorHandler==%p", v_pOld_XErrorHandler);
    v_bIsChanged_XErrorHandler = true;
    return true;
}

bool X11wrapper::ErrorHandler_Restore()
{
    _LOG_NFO("pOld_XErrorHandler==%p", v_pOld_XErrorHandler);
    if (! v_bIsChanged_XErrorHandler)
    {
        _LOG_WRN("nothing to restore");
        return true;
    }
    XSetErrorHandler(v_pOld_XErrorHandler);
    v_bIsChanged_XErrorHandler = false;
    return true;
}

void X11wrapper::Sync()
{
    X11_Sync(GetDisplay());
}

void X11wrapper::Lock()
{
    X11_Lock(GetDisplay());
}

void X11wrapper::Unlock()
{
    X11_Unlock(GetDisplay());
}

Window X11wrapper::Window_Create(int nPosX, int nPosY, unsigned int nWidth, unsigned int nHeight, Window parent_window/*=None*/)
{
    if (parent_window == None)
    {
#ifdef DEBUG
        _LOG_NFO("changing the parent window to root window");
#endif
        parent_window = Window_GetRoot();
    }

#ifdef DEBUG
    _LOG_NFO("pos==(%d, %d, %d, %d), parent_window==%d", nPosX, nPosY, nWidth, nHeight, parent_window);
#endif

    Window window = 0;
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
		window = XCreateWindow(
			GetDisplay(), parent_window, 
			nPosX, nPosY, nWidth, nHeight, InputOnly, 
			CopyFromParent,
			CopyFromParent, 
			CopyFromParent,
			0, 0);

		//make it transparent
		unsigned int opacity = 0;
		XChangeProperty(GetDisplay(), window, XInternAtom(GetDisplay(), OPACITY, False), XA_CARDINAL, 32, PropModeReplace, (unsigned char *) &opacity, 1L);

        _COND_XBOOL_LOG_ERR_BREAK(window != 0);
    } while ((xcode = 0));
    if (xcode != 0)
        return 0;
    return window;
}

Window X11wrapper::Window_Create_Show(int nPosX, int nPosY, unsigned int nWidth, unsigned int nHeight, Window parent_window/*=None*/, bool bSendToDesktop/* = false*/)
{
#ifdef DEBUG
    _LOG_NFO("pos==(%d, %d, %d, %d), parent_window==%d", nPosX, nPosY, nWidth, nHeight, parent_window);
#endif

    Window window = 0;
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        window = Window_Create(nPosX, nPosY, nWidth, nHeight, parent_window);
        _COND_XBOOL_LOG_ERR_BREAK(window != 0);
		if (! Window_Show(window))
			_LOG_XERROR_BREAK("cannot show window==%d", window);

		if(bSendToDesktop)
		{
			if (! Window_Lower(window))
				_LOG_WRN("cannot lower window");
			Sync();
		}

		RemoveWindowDecoration(window);

		if (! Window_MoveResize(window, nPosX, nPosY, nWidth, nHeight, false, true))
			_LOG_XERROR_BREAK("cannot re-position window==%d", window);
    } while ((xcode = 0));
    // cleanup
    if (xcode != 0)
    {
        Window_Destroy(window);
        window = 0;
    }
    return window;
}

bool X11wrapper::Window_Destroy(Window window)
{
#ifdef DEBUG
    _LOG_NFO("window==%d", window);
#endif

    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        xcode = XDestroyWindow(GetDisplay(), window);
        _COND_XBOOL_LOG_ERR_BREAK(xcode);
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Window_Show(Window window, bool bShow/*=true*/)
{
#ifdef DEBUG
    _LOG_NFO("window==%d, bShow==%d", window, bShow);
#endif

    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        if (bShow)
        {
            // We want to get MapNotify events
            XSelectInput(GetDisplay(), window, StructureNotifyMask);
            // show here
            xcode = XMapWindow(GetDisplay(), window);
            _COND_XBOOL_LOG_ERR_BREAK(xcode);
            Sync();
            // Wait for the MapNotify event
            for(;;)
            {
                XEvent xevent;
                XNextEvent(GetDisplay(), &xevent);
                if (xevent.type == MapNotify)
                    break;
            }
        }
        else
        {
            xcode = XUnmapWindow(GetDisplay(), window);
            _COND_XBOOL_LOG_ERR_BREAK(xcode);
        }
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Window_MoveResize(const Window window, const int nPosX, const int nPosY, const unsigned int nWidth, const unsigned int nHeight, bool bUse_WM_Window=true, bool bWaitForCompletion=true)
{
#ifdef DEBUG
	_LOG_NFO("Window_MoveResize window==%d, pos==(%d, %d, %d, %d)", window, nPosX, nPosY, nWidth, nHeight);
#endif
	// cannot wait for Expose events: they may be generated or not
	// correct position checker
	bool bGoodPos = false; // initial window
	bool bGoodPos_wm = false; // wm window
	// start
	int xcode = -1;
	X11_Locker x11_locker(GetDisplay());
	do
	{
		// coordinates for our window
		int x = 0;
		int y = 0;
		unsigned int w = 0;
		unsigned int h = 0;
		// checking again in a loop
		int nIterations = bWaitForCompletion ? WAIT_FOR_COMPLETION_MAX_ITERATIONS : 1;
		for (int i=1; i<=nIterations; usleep(WAIT_FOR_COMPLETION_INTERVAL_USLEEP), ++i)
		{
			//_LOG_NFO("i==%d/%d", i, nIterations);
			// required window which will be changed according to arguments
			Window window_req = window;
			int x_req = nPosX;
			int y_req = nPosY;
			unsigned int w_req = nWidth;
			unsigned int h_req = nHeight;
			// coordinates for the corresponding wm-parent-window
			Window window_wm = window;
			int x_wm = 0;
			int y_wm = 0;
			unsigned int w_wm = 0;
			unsigned int h_wm = 0;
			if (bUse_WM_Window)
			{
				// check if the wm reparented the window
				window_wm = Window_GetDeepestParent(window);
				if ( (window_wm == 0) || (window_wm == window) )
				{
					//_LOG_WRN("Window_GetDeepestParent(window==%d) => window_wm==%d", window, window_wm);
					continue;
				}
				else
				{
					// we have a container wm-window here
					// will translate the origin
					int x_delta = 0;
					int y_delta = 0;
					bool bResult = Window_TranslateCoordinates(window, window_wm, 0, 0, &x_delta, &y_delta, NULL);
					if (! bResult)
					{
						_LOG_ERR("cannot translate coordinates");
						continue;
					}
					//_LOG_NFO("x_delta==%d, y_delta==%d", x_delta, y_delta);
					// read the actual position for window_wm
					if (! Window_GetGeometry(window_wm, &x_wm, &y_wm, &w_wm, &h_wm))
					{
#ifdef DEBUG
						_LOG_ERR("get delta Window_GetGeometry(window_wm) => retCode==false");
#endif
						continue;
					}
					_LOG_NFO("window_wm==%d => pos==(%d, %d, %d, %d)", window_wm, x_wm, y_wm, w_wm, h_wm);


					// compute the new coordinates
					window_req = window_wm;
					x_req = nPosX - x_delta;
					y_req = nPosY - y_delta;
					w_req = w_wm;
					h_req = h_wm;
				}
			}

			// real call
#ifdef DEBUG
			_LOG_NFO("window_req==%d => pos==(%d, %d, %d, %d)", window_req, x_req, y_req, w_req, h_req);
#endif			
			xcode = XMoveResizeWindow(GetDisplay(), window_req, x_req, y_req, w_req, h_req);
			usleep(WAIT_FOR_COMPLETION_INTERVAL_USLEEP);
			Sync();
			if (xcode != True)
			{
#ifdef DEBUG
				_LOG_WRN("XMoveResizeWindow() => retCode==%d", xcode);
#endif
				continue;
			}
			//_LOG_NFO("XMoveResizeWindow() => retCode==%d", xcode);
			// read the actual position
			if (! Window_GetGeometry(window, &x, &y, &w, &h))
			{
#ifdef DEBUG
				_LOG_WRN("Window_GetGeometry() => retCode==false");
#endif
				continue;
			}
			//_LOG_NFO("window==%d => pos==(%d, %d, %d, %d)", window, x, y, w, h);
			bGoodPos = ( (x==nPosX) && (y==nPosY) && (w==nWidth) && (h==nHeight) );
			//_LOG_NFO("bGoodPos==%d", bGoodPos);
			// read the actual position for window_wm
			if (bUse_WM_Window)
			{
				if (! Window_GetGeometry(window_wm, &x_wm, &y_wm, &w_wm, &h_wm))
				{
#ifdef DEBUG
					_LOG_ERR("Window_GetGeometry(window_wm) => retCode==false");
#endif
					continue;
				}

				bGoodPos_wm = ( (x_req==x_wm) && (y_req==y_wm) && (w_req==w_wm) && (h_req==h_wm) );
				//_LOG_NFO("bGoodPos_wm==%d", bGoodPos_wm);
				// check the real position
				if (bGoodPos && bGoodPos_wm)
				{
#ifdef DEBUG
					_LOG_NFO("both correct : window==%d pos(%d, %d, %d, %d), window_wm==%d pos==(%d, %d, %d, %d)",
						window, x, y, w, h,
						window_wm, x_wm, y_wm, w_wm, h_wm);
#endif
					break;
				}

			}
			// check the real position
			if (bGoodPos)
			{
#ifdef DEBUG
				_LOG_NFO("correct : window==%d pos(%d, %d, %d, %d)", window, x, y, w, h);
#endif
				break;
			}
		}
	} while ((xcode = 0));
	if (!bGoodPos && !bGoodPos_wm)
		_LOG_ERR("cannot re-position window==%d", window);

	return true;
//	return (bGoodPos || bGoodPos_wm);
}

bool X11wrapper::Window_ClassName(Window window, const char *s_class, const char *s_name)
{
#ifdef DEBUG
    _LOG_NFO("window==%d, s_class=='%s', s_name=='%s')", window, s_class, s_name);
#endif

    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        XClassHint classHint;
        classHint.res_name = const_cast<char *>(s_name);
        classHint.res_class = const_cast<char *>(s_class);
        xcode = XSetClassHint(GetDisplay(), window, &classHint);
        _COND_XBOOL_LOG_ERR_BREAK(xcode);
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Window_Name(Window window, const char *s_name)
{
#ifdef DEBUG
    _LOG_NFO("window==%d, s_name==%s", window, s_name);
#endif

    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        xcode = XStoreName(GetDisplay(), window, s_name);
        _COND_XBOOL_LOG_ERR_BREAK(xcode);
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Window_Raise(Window window)
{
#ifdef DEBUG
    _LOG_NFO("window==%d", window);
#endif

    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        xcode = XRaiseWindow(GetDisplay(), window);
        _COND_XBOOL_LOG_ERR_BREAK(xcode);
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Window_Lower(Window window)
{
#ifdef DEBUG
    _LOG_NFO("window==%d", window);
#endif

    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        xcode = XLowerWindow(GetDisplay(), window);
        _COND_XBOOL_LOG_ERR_BREAK(xcode);
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Window_SetOpacity_Helper(Window window, unsigned long nOpacity)
{
    //_LOG_NFO("window==%d, nOpacity==0x%lx", window, nOpacity);
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        Display *pDisplay = GetDisplay();
        // create property if does not exist
        Atom atom_property = XInternAtom(pDisplay, "_NET_WM_WINDOW_OPACITY", False);
        if (atom_property == None)
            _LOG_XERROR_BREAK("cannot use opacity property");
        if (nOpacity == WINDOW_OPAQUE)
        {
#ifdef DEBUG
            _LOG_NFO("deleting opacity property");
#endif
            xcode = XDeleteProperty(pDisplay, window, atom_property);
            _COND_XBOOL_LOG_ERR_BREAK(xcode);
        }
        else
        {
            //_LOG_NFO("changing opacity property");
            xcode = XChangeProperty(pDisplay, window, atom_property,
                                   XA_CARDINAL, 32, PropModeReplace,
                                   (unsigned char *) &nOpacity, 1L
                                   );
            _COND_XBOOL_LOG_ERR_BREAK(xcode);
        }
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Window_SetOpacity(Window window, unsigned long nOpacity)
{
    Window root_window = Window_GetRoot();

#ifdef DEBUG
    _LOG_NFO("window==%d, root_window==%d, nOpacity==0x%lx", window, root_window, nOpacity);
    if (nOpacity == 0)
        _LOG_WRN("Opacity value should not be 0, using it anyway, you have been warned");
#endif

    bool bResult = true;
    while ( (window) && (window != root_window) )
    {
        bResult = Window_SetOpacity_Helper(window, nOpacity);
        if (! bResult)
            _LOG_XERROR_BREAK("stopping tree-walk");
        window = Window_GetParent(window);
    }
    return bResult;
}

Window X11wrapper::Window_GetParent(Window window)
{
    //_LOG_NFO("window==%d", window);
    Window root_return = 0;
    Window parent_return = 0;
    Window *children_return = NULL;
    unsigned int nchildren_return = 0;
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        xcode = XQueryTree(
            GetDisplay(), window,
            &root_return, &parent_return, &children_return, &nchildren_return
            );
        //_LOG_NFO("window==%d, root_return==%d, parent_return==%d, children_return==%p, nchildren_return==%d", window, root_return, parent_return, children_return, nchildren_return);
        if (xcode == 0)
            _LOG_XERROR_BREAK("cannot read window tree");
    } while ((xcode = 0));
    // cleanup
    if (children_return != None)
        XFree(children_return);
    if (xcode != 0)
        return 0;
    //if (parent_return == root_return)
    //    _LOG_NFO("window==%d => root parent==%d", window, parent_return);
    //else
    //    _LOG_NFO("window==%d => parent==%d", window, parent_return);
    return parent_return;
}

Window X11wrapper::Window_GetDeepestParent(Window window)
{
    //_LOG_NFO("window==%d", window);
    Window parent_return = 0;
    if (window == None)
    {
        _LOG_WRN("window==%d => parent_return==%d", window, parent_return);
        return parent_return;
    }
    Window root_window = Window_GetRoot();
    if (window == root_window)
    {
        _LOG_WRN("root window==%d => parent_return==%d", window, parent_return);
        return parent_return;
    }
    parent_return = window;
    // going recursive
    Window some_window = window;
    do
    {
        some_window = Window_GetParent(some_window);
        if (some_window == 0)
        {
            _LOG_ERR("cannot get parent for window==%d", window);
            return 0;
        }
        if (some_window == root_window)
            break;
        parent_return = some_window;
    } while (true);
    //_LOG_NFO("window==%d => parent_return==%d", window, parent_return);
    return parent_return;
}

bool X11wrapper::Keyboard_Grab(Window window_grab)
{
#ifdef DEBUG
    _LOG_NFO("window_grab==%d", window_grab);
#endif

    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        xcode = XGrabKeyboard(
            GetDisplay(),
            window_grab, // window_grab
            false, // owner_events
            GrabModeAsync, // pointer_mode
            GrabModeAsync, // keyboard_mode
            CurrentTime // time
            );
        _COND_XSTAT_LOG_ERR_BREAK(xcode);
#ifdef DEBUG
        _LOG_NFO("XGrabKeyboard(%d) => retCode==%d", window_grab, xcode);
#endif
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Keyboard_Ungrab()
{
#ifdef DEBUG
    _LOG_NFO();
#endif
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        xcode = XUngrabKeyboard(GetDisplay(), CurrentTime);
        _COND_XBOOL_LOG_ERR_BREAK(xcode);
#ifdef DEBUG
        _LOG_NFO("XUngrabKeyboard() => retCode==%d", xcode);
#endif
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Keyboard_SetAutoRepeat(bool bOn)
{
#ifdef DEBUG
    _LOG_NFO("bOn==%d", bOn);
#endif
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        if (bOn)
            xcode = XAutoRepeatOn(GetDisplay());
        else
            xcode = XAutoRepeatOff(GetDisplay());
        _COND_XBOOL_LOG_ERR_BREAK(xcode);
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Mouse_Grab(Window window_grab, Window window_confine_to/*=None*/)
{
#ifdef DEBUG
    _LOG_NFO("window_grab==%d, window_confine_to==%d", window_grab, window_confine_to);
#endif
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        // in a loop, forcing the grab again
        // because the time moment may not be good right now
        for (int i=0; i<WAIT_FOR_COMPLETION_MAX_ITERATIONS; usleep(WAIT_FOR_COMPLETION_INTERVAL_USLEEP), ++i)
        {
            //_LOG_NFO("i==%d/%d", i, WAIT_FOR_COMPLETION_MAX_ITERATIONS);
            Sync();
            xcode = XGrabPointer(
                GetDisplay(),
                window_grab, // window_grab
                false, // owner_events
                ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask | PointerMotionHintMask | Button1MotionMask | Button2MotionMask | Button3MotionMask | Button4MotionMask | Button5MotionMask | ButtonMotionMask, // event_mask
                GrabModeAsync, // pointer_mode
                GrabModeAsync, // keyboard_mode
                window_confine_to, // confine_to_window
                None, // cursor
                CurrentTime // time
                );
            if (xcode == 0)
                break;
            //_LOG_WRN("i==%d, xcode==%d => ErrorText=='%s'", i, xcode, GetErrorText(GetDisplay(), xcode).c_str());
        }
        _COND_XSTAT_LOG_ERR_BREAK(xcode);
#ifdef DEBUG
        _LOG_NFO("XGrabPointer(window_grab==%d, window_confine_to==%d) => retCode==%d", window_grab, window_confine_to, xcode);
#endif
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Mouse_Ungrab()
{
#ifdef DEBUG
    _LOG_NFO();
#endif
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        xcode = XUngrabPointer(GetDisplay(), CurrentTime);
#ifdef DEBUG
        _LOG_NFO("XUngrabPointer() => retCode==%d", xcode);
#endif
        _COND_XBOOL_LOG_ERR_BREAK(xcode);
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Mouse_ShowStandardCursor(Window window)
{
    return Mouse_SetCursor_Font(window, XC_left_ptr);
}

bool X11wrapper::Mouse_HideCursor(Window window)
{
#ifdef DEBUG
    _LOG_NFO("window==%d", window);
#endif
    static const char dataNoCursor[] = { 0x00 };
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        Pixmap pixmap = XCreateBitmapFromData(GetDisplay(), window, dataNoCursor, 1, 1);
        if (pixmap == None)
            _LOG_XERROR_BREAK("cannot create the pixmap");
        XColor color;
        memset(&color, 0, sizeof(color));
        Cursor cursor = XCreatePixmapCursor(GetDisplay(), pixmap, pixmap, &color, &color, 0, 0);
        if (! cursor)
            _LOG_XERROR_BREAK("Cannot create pixmap cursor");
        xcode = XDefineCursor(GetDisplay(), window, cursor);
        _COND_XBOOL_LOG_ERR_BREAK(xcode);
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Mouse_SetCursor_Inherit(Window window)
{
#ifdef DEBUG
    _LOG_NFO("window==%d", window);
#endif
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        xcode = XUndefineCursor(GetDisplay(), window);
        _COND_XBOOL_LOG_ERR_BREAK(xcode);
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Mouse_SetCursor_Font(Window window, int nShape)
{
#ifdef DEBUG
    _LOG_NFO("window==%d, nShape==%d", window, nShape);
#endif

    // default cursor case
    if (nShape < 0)
    {
        return Mouse_SetCursor_Inherit(window);
    }
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        // create the cursor
        Cursor cursor = XCreateFontCursor(GetDisplay(), nShape);
        if (! cursor)
            _LOG_XERROR_BREAK("cannot create the font cursor");
        // change the cursor
        xcode = XDefineCursor(GetDisplay(), window, cursor);
        _COND_XBOOL_LOG_ERR_BREAK(xcode);
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Mouse_SetCursor_Image(Window window, const string &sPath, const string &sPathMask)
{
#ifdef DEBUG
    _LOG_NFO("window==%d, sPath=='%s', sPathMask=='%s'", window, sPath.c_str(), sPathMask.c_str());
#endif

    Pixmap pixmap = None;
    Pixmap pixmap_mask = None;
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        // read pixmap
        unsigned int width_return = 0;
        unsigned int height_return = 0;
        int x_hot_return = 0;
        int y_hot_return = 0;
        if (! Pixmap_ReadFile(window, sPath.c_str(), pixmap, width_return, height_return, x_hot_return, y_hot_return))
            _LOG_XERROR_BREAK("cannot read x-bitmap");
        // read pixmap mask
        int x_hot_return_mask = 0;
        int y_hot_return_mask = 0;
        unsigned int width_return_mask = 0;
        unsigned int height_return_mask = 0;
        if (! Pixmap_ReadFile(window, sPathMask.c_str(), pixmap_mask, width_return_mask, height_return_mask, x_hot_return_mask, y_hot_return_mask))
            _LOG_XERROR_BREAK("cannot read x-bitmap mask");
        // check values
        if ( ( width_return != width_return_mask ) || ( height_return != height_return_mask ) )
            _LOG_XERROR_BREAK("size of the x-bitmap and his mask differ");
        // prepare extra data
        Colormap colormap = XDefaultColormap(GetDisplay(), GetScreen());
        XColor color_fg;
        color_fg.pixel = BlackPixel(GetDisplay(), GetScreen());
        xcode = XQueryColor(GetDisplay(), colormap, &color_fg);
        _COND_XBOOL_LOG_ERR_BREAK(xcode);
        Sync();
        XColor color_bg;
        color_bg.pixel = WhitePixel(GetDisplay(), GetScreen());
        xcode = XQueryColor(GetDisplay(), colormap, &color_bg);
        _COND_XBOOL_LOG_ERR_BREAK(xcode);
        Sync();
        // create the cursor
        Cursor cursor = XCreatePixmapCursor(GetDisplay(), pixmap, pixmap_mask, &color_fg, &color_bg, x_hot_return, y_hot_return);
        if (! cursor)
            _LOG_XERROR_BREAK("Cannot create pixmap cursor");
        // change the cursor
        xcode = XDefineCursor(GetDisplay(), window, cursor);
        _COND_XBOOL_LOG_ERR_BREAK(xcode);
    } while ((xcode = 0));
    // cleanup
    Delete_Pixmap(pixmap);
    Delete_Pixmap(pixmap_mask);
    return (xcode == 0);
}

bool X11wrapper::Mouse_QueryMaxCursorSize(Window window, unsigned int &width_return, unsigned int &height_return)
{
    unsigned int width_max = GetDisplayWidth();
    unsigned int height_max = GetDisplayWidth();
    bool bResult = XQueryBestCursor(GetDisplay(), window, width_max, height_max, &width_return, &height_return);
#ifdef DEBUG
    _LOG_NFO("window==%d, width_return==%d, height_return==%d", window, width_return, height_return);
#endif
    return bResult;
}

bool X11wrapper::Mouse_SetSpeed(int accel_numerator/*=-1*/, int accel_denominator/*=-1*/, int threshold/*=-1*/)
{
#ifdef DEBUG
    _LOG_NFO("accel_numerator==%d, accel_denominator==%d, threshold==%d", accel_numerator, accel_denominator, threshold);
#endif
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        xcode = XChangePointerControl(GetDisplay(), true, true, accel_numerator, accel_denominator, threshold);
        _COND_XBOOL_LOG_ERR_BREAK(xcode);
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Mouse_GetSpeed(int &accel_numerator_return, int &accel_denominator_return, int &threshold_return)
{
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        xcode = XGetPointerControl(GetDisplay(), &accel_numerator_return, &accel_denominator_return, &threshold_return);
        _COND_XBOOL_LOG_ERR_BREAK(xcode);
#ifdef DEBUG
        _LOG_NFO("accel_numerator==%d, accel_denominator==%d, threshold==%d", accel_numerator_return, accel_denominator_return, threshold_return);
#endif
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Mouse_SetPosition(int nPosX, int nPosY, Window relative_to_window/*=None*/)
{
#ifdef DEBUG
    _LOG_NFO("pointer pos==(%d, %d), relative to window==%d", nPosX, nPosY, relative_to_window);
#endif
    if (relative_to_window == 0)
    {
        //_LOG_WRN("changing the relative window to root window");
        relative_to_window = Window_GetRoot();
    }
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        xcode = XWarpPointer(GetDisplay(), None, relative_to_window, 0, 0, 0, 0, nPosX, nPosY);
        _COND_XBOOL_LOG_ERR_BREAK(xcode);
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Mouse_GetPosition(int &nPosX, int &nPosY, Window relative_to_window/*=None*/)
{
    if (relative_to_window == 0)
    {
        //_LOG_WRN("changing the relative window to root window");
        relative_to_window = Window_GetRoot();
    }
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        Window root_return;
        Window child_return;
        int root_x_return;
        int root_y_return;
        int win_x_return;
        int win_y_return;
        unsigned int mask_return;
        xcode = XQueryPointer(
            GetDisplay(),
            relative_to_window,
            &root_return,
            &child_return,
            &root_x_return,
            &root_y_return,
            &win_x_return,
            &win_y_return,
            &mask_return
            );
        _COND_XBOOL_LOG_ERR_BREAK(xcode);
        //_LOG_NFO(
        //    "root_return==%d, child_return==%d, root_x_return==%d, root_y_return==%d, win_x_return==%d, win_y_return==%d, mask_return==%d",
        //    root_return, child_return, root_x_return, root_y_return, win_x_return, win_y_return, mask_return
        //    );
        nPosX = win_x_return;
        nPosY = win_y_return;
#ifdef DEBUG
        _LOG_NFO("pointer pos==(%d, %d), relative to window==%d", nPosX, nPosY, relative_to_window);
#endif
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Mouse_Constrain(int nPosX, int nPosY, unsigned int nWidth, unsigned int nHeight, Window window_grab/*=None*/)
{
    if (window_grab == None)
    {
#ifdef DEBUG
        _LOG_NFO("changing the grab window to root window");
#endif
        window_grab = Window_GetRoot();
    }

#ifdef DEBUG
    _LOG_NFO("pos==(%d, %d, %d, %d), window_grab==%d", nPosX, nPosY, nWidth, nHeight, window_grab);
#endif

    // hide window case
    if ( (nWidth == 0) && (nHeight == 0) )
    {
        return Mouse_Constrain_Release();
    }
    else
    {
	    if(nWidth <= 0 || nHeight <= 0) //they are invalid, but they are not both zero
	    {
		_LOG_WRN("Won't apply contrain: bad arguments, size must be greater than 0");
		 return false;
	    }
    }
    // show window case
    Window &window = v_window_Mouse_Constrain;
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        if (window != 0)
        {
            _LOG_NFO("closing previous window==%d", window);
            if (! Window_Destroy(window))
            {
                _LOG_WRN("cannot destroy window %d", window);
                window = 0;
            }
            Sync();
        }
        window = Window_Create_Show(nPosX, nPosY, nWidth, nHeight, 0, true);
        if (window <= 0)
            _LOG_XERROR_BREAK("cannot create window");
		if (! Window_SetOpacity(window, 1))
			_LOG_WRN("cannot set opacity");
		if (! Window_ClassName(window, "constrain_mouse", "constrain_mouse"))
            _LOG_XERROR_BREAK("cannot set window class");
        if (! Window_Name(window, "constrain_mouse"))
            _LOG_XERROR_BREAK("cannot set window name");
		if (! Mouse_Grab(window_grab, window))
			_LOG_XERROR_BREAK("cannot grab pointer");
        // done
        previous_mouse_constrain.window_grab = window_grab;
        previous_mouse_constrain.nPosX = nPosX;
        previous_mouse_constrain.nPosY = nPosY;
        previous_mouse_constrain.nWidth = nWidth;
        previous_mouse_constrain.nHeight = nHeight;
    } while ((xcode = 0));
    // cleanup
    if (xcode != 0)
    {
        Window_Destroy(window);
        window = 0;
    }
    if (xcode != 0)
    {
        _LOG_ERR("pos==(%d, %d, %d, %d), window_grab==%d => retCode==%d", nPosX, nPosY, nWidth, nHeight, window_grab, xcode);
        return false;
    }

#ifdef DEBUG
    _LOG_NFO("pos==(%d, %d, %d, %d), window_grab==%d => retCode==%d", nPosX, nPosY, nWidth, nHeight, window_grab, xcode);
#endif

    return true;
}

bool X11wrapper::Mouse_Constrain_Release()
{
#ifdef DEBUG
    _LOG_NFO();
#endif

    Window &window = v_window_Mouse_Constrain;
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        if (! Mouse_IsConstrainActive())
        {
#ifdef DEBUG
            _LOG_NFO("constrain not active");
#endif
            xcode = 0;
            break;
        }
        if (! Mouse_Ungrab())
            _LOG_WRN("Mouse_Ungrab");
        if (! Window_Destroy(window))
            _LOG_WRN("Window_Destroy");
        // done
        window = 0;
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Mouse_Constrain_ReactivateIfActive()
{
    if (! Mouse_IsConstrainActive())
    {
#ifdef DEBUG
        _LOG_NFO("constrain is not active, nothing to do");
#endif
        return true;
    }

#ifdef DEBUG
    _LOG_NFO("constrain is active, reactivating");
#endif

    Mouse_Constrain_Release();
    Mouse_Constrain(previous_mouse_constrain.nPosX, previous_mouse_constrain.nPosY, previous_mouse_constrain.nWidth, previous_mouse_constrain.nHeight, previous_mouse_constrain.window_grab);
    return Mouse_IsConstrainActive();
}

bool X11wrapper::Mouse_IsConstrainActive()
{
    return (v_window_Mouse_Constrain != 0);
}

Window X11wrapper::Mouse_Constrain_GetWindow()
{
    return v_window_Mouse_Constrain;
}

// all from : /usr/include/X11/cursorfont.h
const X11wrapper::string_int X11wrapper::v_aStdCursors_NameValue[] =
{
    // bad value : {"XC_num_glyphs", XC_num_glyphs},
    {"XC_X_cursor", XC_X_cursor},
    {"XC_arrow", XC_arrow},
    {"XC_based_arrow_down", XC_based_arrow_down},
    {"XC_based_arrow_up", XC_based_arrow_up},
    {"XC_boat", XC_boat},
    {"XC_bogosity", XC_bogosity},
    {"XC_bottom_left_corner", XC_bottom_left_corner},
    {"XC_bottom_right_corner", XC_bottom_right_corner},
    {"XC_bottom_side", XC_bottom_side},
    {"XC_bottom_tee", XC_bottom_tee},
    {"XC_box_spiral", XC_box_spiral},
    {"XC_center_ptr", XC_center_ptr},
    {"XC_circle", XC_circle},
    {"XC_clock", XC_clock},
    {"XC_coffee_mug", XC_coffee_mug},
    {"XC_cross", XC_cross},
    {"XC_cross_reverse", XC_cross_reverse},
    {"XC_crosshair", XC_crosshair},
    {"XC_diamond_cross", XC_diamond_cross},
    {"XC_dot", XC_dot},
    {"XC_dotbox", XC_dotbox},
    {"XC_double_arrow", XC_double_arrow},
    {"XC_draft_large", XC_draft_large},
    {"XC_draft_small", XC_draft_small},
    {"XC_draped_box", XC_draped_box},
    {"XC_exchange", XC_exchange},
    {"XC_fleur", XC_fleur},
    {"XC_gobbler", XC_gobbler},
    {"XC_gumby", XC_gumby},
    {"XC_hand1", XC_hand1},
    {"XC_hand2", XC_hand2},
    {"XC_heart", XC_heart},
    {"XC_icon", XC_icon},
    {"XC_iron_cross", XC_iron_cross},
    {"XC_left_ptr", XC_left_ptr},
    {"XC_left_side", XC_left_side},
    {"XC_left_tee", XC_left_tee},
    {"XC_leftbutton", XC_leftbutton},
    {"XC_ll_angle", XC_ll_angle},
    {"XC_lr_angle", XC_lr_angle},
    {"XC_man", XC_man},
    {"XC_middlebutton", XC_middlebutton},
    {"XC_mouse", XC_mouse},
    {"XC_pencil", XC_pencil},
    {"XC_pirate", XC_pirate},
    {"XC_plus", XC_plus},
    {"XC_question_arrow", XC_question_arrow},
    {"XC_right_ptr", XC_right_ptr},
    {"XC_right_side", XC_right_side},
    {"XC_right_tee", XC_right_tee},
    {"XC_rightbutton", XC_rightbutton},
    {"XC_rtl_logo", XC_rtl_logo},
    {"XC_sailboat", XC_sailboat},
    {"XC_sb_down_arrow", XC_sb_down_arrow},
    {"XC_sb_h_double_arrow", XC_sb_h_double_arrow},
    {"XC_sb_left_arrow", XC_sb_left_arrow},
    {"XC_sb_right_arrow", XC_sb_right_arrow},
    {"XC_sb_up_arrow", XC_sb_up_arrow},
    {"XC_sb_v_double_arrow", XC_sb_v_double_arrow},
    {"XC_shuttle", XC_shuttle},
    {"XC_sizing", XC_sizing},
    {"XC_spider", XC_spider},
    {"XC_spraycan", XC_spraycan},
    {"XC_star", XC_star},
    {"XC_target", XC_target},
    {"XC_tcross", XC_tcross},
    {"XC_top_left_arrow", XC_top_left_arrow},
    {"XC_top_left_corner", XC_top_left_corner},
    {"XC_top_right_corner", XC_top_right_corner},
    {"XC_top_side", XC_top_side},
    {"XC_top_tee", XC_top_tee},
    {"XC_trek", XC_trek},
    {"XC_ul_angle", XC_ul_angle},
    {"XC_umbrella", XC_umbrella},
    {"XC_ur_angle", XC_ur_angle},
    {"XC_watch", XC_watch},
    {"XC_xterm", XC_xterm},
};
const int X11wrapper::v_aStdCursors_Count = ( sizeof(v_aStdCursors_NameValue) / sizeof(v_aStdCursors_NameValue[0]) );

int X11wrapper::StdCursor_GetValueByName(const std::string &sName)
{
    for (int i=0; i<v_aStdCursors_Count; ++i)
    {
        if (v_aStdCursors_NameValue[i].sName == sName)
            return v_aStdCursors_NameValue[i].nValue;
    }
    return -1;
}

Pixmap X11wrapper::Pixmap_Create(Window window, unsigned int width, unsigned int height, unsigned int depth)
{
#ifdef DEBUG
    _LOG_NFO("window==%d, width==%d, height==%d, depth==%d", window, width, height, depth);
#endif

    Pixmap pixmap = XCreatePixmap(GetDisplay(), window, width, height, depth);
    if (pixmap == 0)
        _LOG_ERR("cannot create pixmap: window==%d, width==%d, height==%d, depth==%d", window, width, height, depth);
#ifdef DEBUG
    else
        _LOG_NFO("window==%d, width==%d, height==%d, depth==%d => pixmap==%d", window, width, height, depth, pixmap);
#endif
    return pixmap;
}

bool X11wrapper::Pixmap_ReadFile(Window window, const string &sPath, Pixmap &pixmap_return, unsigned int &width_return, unsigned int &height_return, int &x_hot_return, int &y_hot_return)
{
#ifdef DEBUG
    _LOG_NFO("window==%d, sPath=='%s'", window, sPath.c_str());
#endif

    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        // read x-bitmap from file
        xcode = XReadBitmapFile(GetDisplay(), window, sPath.c_str(), &width_return, &height_return, &pixmap_return, &x_hot_return, &y_hot_return);
        _COND_XSTAT_LOG_ERR_BREAK(xcode);

#ifdef DEBUG
        _LOG_NFO("window==%d, sPath=='%s' => width==%d, height==%d, pixmap==%d, x_hot==%d, y_hot==%d",
                 window, sPath.c_str(), width_return, height_return, pixmap_return, x_hot_return, y_hot_return);
#endif
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Pixmap_ReadFileData(const string &sPath, unsigned char *&data_return, unsigned int &width_return, unsigned int &height_return, int &x_hot_return, int &y_hot_return)
{
#ifdef DEBUG
    _LOG_NFO("sPath=='%s'", sPath.c_str());
#endif

    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        // read x-bitmap from file
        xcode = XReadBitmapFileData(sPath.c_str(), &width_return, &height_return, &data_return, &x_hot_return, &y_hot_return);
        _COND_XSTAT_LOG_ERR_BREAK(xcode);

#ifdef DEBUG
        _LOG_NFO("sPath=='%s' => width==%d, height==%d, pixmap==%d, x_hot==%d, y_hot==%d",
                 sPath.c_str(), width_return, height_return, data_return, x_hot_return, y_hot_return);
#endif

    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Delete_Pixmap(Pixmap &pixmap)
{
#ifdef DEBUG
    _LOG_NFO("pixmap==%d", pixmap);
#endif

    if (pixmap==None)
        return true;
    X11_Locker x11_locker(GetDisplay());
    int xcode = -1;
    do
    {
        xcode = XFreePixmap(GetDisplay(), pixmap);
        _COND_XBOOL_LOG_ERR_BREAK(xcode);
        pixmap = None;
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Delete_Object(void *&id_object)
{
#ifdef DEBUG
    _LOG_NFO("id_object==%p", id_object);
#endif

    if (id_object==None)
        return true;
    X11_Locker x11_locker(GetDisplay());
    int xcode = -1;
    do
    {
        xcode = XFree(id_object);
        _COND_XBOOL_LOG_ERR_BREAK(xcode);
        id_object = None;
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Object_GetGeometry(Drawable drawable, int *x_return, int *y_return, unsigned int *width_return, unsigned int *height_return)
{
    //_LOG_NFO("drawable==%d, x_return->%p, y_return->%p, width_return->%p, height_return->%p",
    //         drawable, x_return, y_return, width_return, height_return
    //         );
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        Window root_ret = 0;
        int x_ret = 0;
        int y_ret = 0;
        unsigned int width_ret = 0;
        unsigned int height_ret = 0;
        unsigned int border_width_ret = 0;
        unsigned int depth_ret = 0;
        // XGetWindowAttributes is only for windows
        xcode = XGetGeometry(
            GetDisplay(), drawable,
            &root_ret,
            &x_ret, &y_ret, &width_ret, &height_ret,
            &border_width_ret,
            &depth_ret
            );
        if (xcode == 0)
            _LOG_XERROR_BREAK("cannot read object geometry");
        // write results
        if (x_return)
            *x_return = x_ret;
        if (y_return)
            *y_return = y_ret;
        if (width_return)
            *width_return = width_ret;
        if (height_return)
            *height_return = height_ret;
        //_LOG_NFO("drawable==%d => pos==(%d, %d, %d, %d)", drawable, x_ret, y_ret, width_ret, height_ret);
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Window_GetGeometry(Window window, int *x_return, int *y_return, unsigned int *width_return, unsigned int *height_return)
{
    //_LOG_NFO("window==%d, x_return->%p, y_return->%p, width_return->%p, height_return->%p",
    //         window, x_return, y_return, width_return, height_return
    //         );
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        XWindowAttributes attr;
        xcode = XGetWindowAttributes(GetDisplay(), window, &attr);
        if (xcode == 0)
        {
            xcode = -1;
            _LOG_XERROR_BREAK("cannot read window geometry");
        }
        // check the deepest parent window
        Window window_wm = Window_GetDeepestParent(window);
        if ( (window_wm != 0) && (window_wm != window) )
        {
            // we have a container wm-window here
            // will translate the origin
            int x_delta = 0;
            int y_delta = 0;
            bool bResult = Window_TranslateCoordinates(window, window_wm, 0, 0, &x_delta, &y_delta, NULL);
            if (! bResult)
            {
                _LOG_WRN("cannot translate coordinates");
            }
            else
            {
                XWindowAttributes attr_wm;
                xcode = XGetWindowAttributes(GetDisplay(), window_wm, &attr_wm);
                if (xcode == 0)
                    _LOG_XERROR_BREAK("cannot read window_wm geometry");
                attr.x  = attr_wm.x + x_delta;
                attr.y  = attr_wm.y + y_delta;
            }
        }
        // write results
        if (x_return)
            *x_return = attr.x;
        if (y_return)
            *y_return = attr.y;
        if (width_return)
            *width_return = attr.width;
        if (height_return)
            *height_return = attr.height;
        //_LOG_NFO("window==%d => pos==(%d, %d, %d, %d)", window, attr.x, attr.y, attr.width, attr.height);
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Window_TranslateCoordinates(Window window_src, Window window_dest, int x_src, int y_src, int *x_dest_return, int *y_dest_return, Window *inner_window_dest_return)
{
    //_LOG_NFO("window_src==%d, window_dest==%d, x_src==%d, y_src==%d, x_dest_return->%p, y_dest_return->%p, inner_window_dest_return->%p",
    //         window_src, window_dest, x_src, y_src, x_dest_return, y_dest_return, inner_window_dest_return
    //         );
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        int delta_x_return = 0;
        int delta_y_return = 0;
        Window inner_window_return = 0;
        bool bResult = XTranslateCoordinates(
            GetDisplay(),
            window_src, window_dest,
            x_src, y_src, &delta_x_return, &delta_y_return, &inner_window_return
            );
        if (! bResult)
            _LOG_XERROR_BREAK("cannot translate coordinates");
        // write results
        if (x_dest_return)
            *x_dest_return = delta_x_return;
        if (y_dest_return)
            *y_dest_return = delta_y_return;
        if (inner_window_dest_return)
            *inner_window_dest_return = inner_window_return;
        //_LOG_NFO("window_src==%d, window_dest==%d, x_src==%d, y_src==%d => x_dest_return==%d, y_dest_return==%d, inner_window_dest_return==%d",
        //         window_src, window_dest, x_src, y_src, delta_x_return, delta_y_return, inner_window_return
        //         );
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Extension_Shape_IsAvailable()
{
    if (! v_bExtension_Shape_IsInitialized)
        Extension_Shape_Initialize();
    return v_bExtension_Shape_IsAvailable;
}

bool X11wrapper::Extension_Shape_GetVersion_Major()
{
    if (! v_bExtension_Shape_IsInitialized)
        Extension_Shape_Initialize();
    return v_nExtension_Shape_Version_Major;
}

bool X11wrapper::Extension_Shape_GetVersion_Minor()
{
    if (! v_bExtension_Shape_IsInitialized)
        Extension_Shape_Initialize();
    return v_nExtension_Shape_Version_Minor;
}

bool X11wrapper::Shape_Context_Enter(Window window, unsigned int width, unsigned int height, Pixmap &bitmap_mask, GC &gc, Display * &pDisplay)
{
#ifdef DEBUG
    _LOG_NFO("window==%d, width==%d, height==%d", window, width, height);
#endif

    bool b_gc_created = false;
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        if (! Extension_Shape_IsAvailable())
            _LOG_XERROR_BREAK("X11 SHAPE extension: not available");
        pDisplay = GetDisplay();
        bitmap_mask = XCreatePixmap(pDisplay, window, width, height, 1);
        if (bitmap_mask == None)
            _LOG_XERROR_BREAK("cannot create bitmap for mask");
        gc = XCreateGC(pDisplay, bitmap_mask, 0, NULL);
        b_gc_created = true;
        // clean background
        xcode = XFillRectangle(pDisplay, bitmap_mask, gc, 0, 0, width, height);
        _COND_XBOOL_LOG_ERR_BREAK(xcode);
        xcode = XSetForeground(pDisplay, gc, 1);
        _COND_XBOOL_LOG_ERR_BREAK(xcode);

#ifdef DEBUG
        _LOG_NFO("window==%d, width==%d, height==%d => bitmap_mask==%d, gc==%d, pDisplay==%p", window, width, height, bitmap_mask, gc, pDisplay);
#endif

    } while ((xcode = 0));
    // cleanup
    if (xcode != 0)
    {
        Delete_Pixmap(bitmap_mask);
        if (b_gc_created)
            XFreeGC(GetDisplay(), gc);
    }
    return (xcode == 0);
}

bool X11wrapper::Shape_Context_Leave(Window window, Pixmap &bitmap_mask, GC &gc, Display * &pDisplay)
{
#ifdef DEBUG
    _LOG_NFO("window==%d, bitmap_mask==%d, gc==%d, pDisplay==%p", window, bitmap_mask, gc, pDisplay);
#endif

    bool bResult = Shape_Window_Apply(window, bitmap_mask);
    if (! bResult)
        _LOG_ERR("cannot apply shape");
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        xcode = XFreeGC(pDisplay, gc);
        if (xcode != True)
            _LOG_WRN("XFreeGC() => retCode==%d", xcode);
    } while ((xcode = 0));
    // cleanup
    Delete_Pixmap(bitmap_mask);
    return ((xcode == 0) && bResult);
}

bool X11wrapper::Shape_Window_Apply(Window window, Pixmap &pixmap, int shape_op/*=0*/)
{
#ifdef DEBUG
    _LOG_NFO("window==%d, pixmap==%d, op==%d", window, pixmap, shape_op);
#endif

    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        if (! Extension_Shape_IsAvailable())
            _LOG_XERROR_BREAK("X11 SHAPE extension: not available");
        XShapeCombineMask(GetDisplay(), window, ShapeBounding, 0, 0, pixmap, shape_op);
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Shape_Window_Apply(Window window, const string &sPath, Pixmap & bitmap_mask, int shape_op/*=0*/)
{
#ifdef DEBUG
    _LOG_NFO("window==%d, sPath=='%s', op==%d", window, sPath.c_str(), shape_op);
#endif

    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    
    // cleanup
    Delete_Pixmap(bitmap_mask);
    
    do
    {
        unsigned int width = 0;
        unsigned int height = 0;
        int x_hot = 0;
        int y_hot = 0;
        if (! Pixmap_ReadFile(window, sPath, bitmap_mask, width, height, x_hot, y_hot))
            _LOG_XERROR_BREAK("cannot read the file '%s'", sPath.c_str());
        if (! Shape_Window_Apply(window, bitmap_mask, shape_op))
            _LOG_XERROR_BREAK("cannot apply shape");
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Shape_Window_Apply(Window window, const string &sPath, int shape_op/*=0*/)
{
#ifdef DEBUG
    _LOG_NFO("window==%d, sPath=='%s', op==%d", window, sPath.c_str(), shape_op);
#endif

    Pixmap bitmap_mask = None;
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        unsigned int width = 0;
        unsigned int height = 0;
        int x_hot = 0;
        int y_hot = 0;
        if (! Pixmap_ReadFile(window, sPath, bitmap_mask, width, height, x_hot, y_hot))
            _LOG_XERROR_BREAK("cannot read the file '%s'", sPath.c_str());
        if (! Shape_Window_Apply(window, bitmap_mask, shape_op))
            _LOG_XERROR_BREAK("cannot apply shape");
    } while ((xcode = 0));
    // cleanup
    Delete_Pixmap(bitmap_mask);
    return (xcode == 0);
}

bool X11wrapper::Shape_Window_Apply(Window window, const char *data_bitmap, unsigned int width, unsigned int height, int shape_op/*=0*/)
{
#ifdef DEBUG
    _LOG_NFO("window==%d, data_bitmap==%p, width==%d, height==%d, op==%d", window, data_bitmap, width, height, shape_op);
#endif

    Pixmap bitmap_mask = None;
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        bitmap_mask = XCreateBitmapFromData(GetDisplay(), window, data_bitmap, width, height);
        if (bitmap_mask == None)
            _LOG_XERROR_BREAK("cannot create the pixmap");
        if (! Shape_Window_Apply(window, bitmap_mask, shape_op))
            _LOG_XERROR_BREAK("cannot apply shape");
    } while ((xcode = 0));
    // cleanup
    Delete_Pixmap(bitmap_mask);
    return (xcode == 0);
}

bool X11wrapper::Shape_Window_Reset(Window window)
{
#ifdef DEBUG
    _LOG_NFO("window==%d", window);
#endif

    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        if (! Extension_Shape_IsAvailable())
            _LOG_XERROR_BREAK("X11 SHAPE extension: not available");
        XShapeCombineMask(GetDisplay(), window, ShapeBounding, 0, 0, None, ShapeSet);
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Shape_Window_Hide(Window window)
{
#ifdef DEBUG
    _LOG_NFO("window==%d", window);
#endif

    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        if (! Extension_Shape_IsAvailable())
            _LOG_XERROR_BREAK("X11 SHAPE extension: not available");
        XRectangle aXRectangle[1];
        aXRectangle[0].x = 0;
        aXRectangle[0].y = 0;
        aXRectangle[0].width = 0;
        aXRectangle[0].height = 0;
        XShapeCombineRectangles(GetDisplay(), window, ShapeBounding, 0, 0, aXRectangle, 0, ShapeSet, Unsorted);
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Shape_PixmapMask_Rectangle(
	Pixmap &pixmap,
	unsigned int x,
	unsigned int y,
	unsigned int width,
	unsigned int height,
	bool bOpaque )
{
#ifdef DEBUG
    _LOG_NFO("Shape_PixmapMask_Rectangle");
#endif

	int xcode = -1;
	X11_Locker x11_locker(GetDisplay());
	do
	{
		XGCValues values;
		Display * disp = GetDisplay();
		if( disp != NULL )
		{
			if( bOpaque )
			{
				values.foreground = WhitePixel(disp, GetScreen());
				GC whiteGC = XCreateGC (disp, pixmap, GCForeground, &values);
				XFillRectangle (disp, pixmap, whiteGC, x, y, width, height);
			}
			else
			{
				values.foreground = BlackPixel(disp, GetScreen());
				GC blackGC = XCreateGC (disp, pixmap, GCForeground, &values);
				XFillRectangle (disp, pixmap, blackGC, x, y, width, height);
			}
		}
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Shape_PixmapMask_Copy(
	Window window,
	const string &sPath,
	Pixmap &destPixmap,
	unsigned int destX,
	unsigned int destY,
	int op_copy/*= GXcopy*/)
{
#ifdef DEBUG
    _LOG_NFO("Shape_PixmapMask_Copy");
#endif

	int xcode = -1;
	X11_Locker x11_locker(GetDisplay());
	do
	{
		int x_hot = 0;
		int y_hot = 0;
		unsigned int width = 0;
		unsigned int height = 0;
		XGCValues values;
		Display * disp = GetDisplay();
		Pixmap srcPixmap;
		
		if( disp != NULL )
		{
			if (! Pixmap_ReadFile(window, sPath, srcPixmap, width, height, x_hot, y_hot))
				_LOG_XERROR_BREAK("cannot read the file '%s'", sPath.c_str());
				
			values.foreground = WhitePixel(disp, GetScreen());
			GC whiteGC = XCreateGC (disp, destPixmap, GCForeground, &values);
			XSetFunction( disp, whiteGC, op_copy);
			XCopyArea (disp, srcPixmap, destPixmap, whiteGC, 0, 0, width, height, destX, destY);
		}
		
		Delete_Pixmap(srcPixmap);
	} while ((xcode = 0));
	return (xcode == 0);
}

bool X11wrapper::Shape_PixmapMask_Copy(
	Pixmap &srcPixmap,
	Pixmap &destPixmap,
	unsigned int x,
	unsigned int y,
	unsigned int width,
	unsigned int height,
	unsigned int destX,
	unsigned int destY,
	int op_copy/* = GXcopy*/)
{
#ifdef DEBUG
    _LOG_NFO("Shape_PixmapMask_Copy");
#endif

	int xcode = -1;
	X11_Locker x11_locker(GetDisplay());
	do
	{
		XGCValues values;
		Display * disp = GetDisplay();
	
		if( disp != NULL )
		{
			values.foreground = WhitePixel(disp, GetScreen());
			GC whiteGC = XCreateGC (disp, destPixmap, GCForeground, &values);
			XSetFunction( disp, whiteGC, op_copy);
			XCopyArea (disp, srcPixmap, destPixmap, whiteGC, x, y, width, height, destX, destY);
		}
	} while ((xcode = 0));
	return (xcode == 0);
}

bool X11wrapper::Extension_Shape_Initialize()
{
#ifdef DEBUG
    _LOG_NFO();
#endif

    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        v_bExtension_Shape_IsInitialized = true;
        int major_opcode = 0;
        int first_event = 0;
        int first_error = 0;
        xcode = XQueryExtension(GetDisplay(),"SHAPE",&major_opcode,&first_event,&first_error);
        if (! xcode)
            _LOG_XERROR_BREAK("X11 SHAPE extension not available : major_opcode==%d, first_event==%d, first_error==%d",
                              major_opcode, first_event, first_error
                              );
        xcode = XShapeQueryVersion(GetDisplay(), &v_nExtension_Shape_Version_Major, &v_nExtension_Shape_Version_Minor);
        if (xcode == 0)
            _LOG_XERROR_BREAK("X11 SHAPE extension: not available");
        v_bExtension_Shape_IsAvailable = true;

#ifdef DEBUG
        _LOG_NFO("X11 SHAPE extension: version %d.%d", v_nExtension_Shape_Version_Major, v_nExtension_Shape_Version_Minor);
#endif

    } while ((xcode = 0));
    return v_bExtension_Shape_IsAvailable;
}

Window X11wrapper::Debug_Window(bool bCreate_NotClose/*=true*/)
{
#ifdef DEBUG
    _LOG_NFO("bCreate_NotClose==%d", bCreate_NotClose);
#endif
    static Window windowDebug = 0;
    if (bCreate_NotClose)
    {
        if (windowDebug == 0)
        {
            windowDebug = Window_Create_Show(400, 100, 400, 300);
            Window_ClassName(windowDebug, "Debug_Class", "Debug_Name");
            Window_Name(windowDebug, "Debug_Window");
#ifdef DEBUG
            _LOG_NFO("windowDebug==%d", windowDebug);
#endif
        }
    }
    else
    {
        if (windowDebug != 0)
        {
            Window_Destroy(windowDebug);
            windowDebug = 0;
        }
    }
    return windowDebug;
}

bool X11wrapper::Debug_Shape_Context_Example(Window window, unsigned int width, unsigned int height, int nApplyMethod/*=0*/)
{
    // compute the image at runtime
    if (nApplyMethod == 0)
    {
        // remembered values
        Pixmap bitmap_mask = 0;
        GC gc;
        Display *pDisplay = NULL;
        // initialize
        bool bResult = Shape_Context_Enter(window, width, height, bitmap_mask, gc, pDisplay);
        if (! bResult)
        {
            _LOG_ERR("Shape_Context_Enter(window==%d, width==%d, height==%d, bitmap_mask==%d, gc==%d, pDisplay==%p) => retCode==%d",
                     window, width, height, bitmap_mask, gc, pDisplay, bResult);
            return false;
        }
        // creating the desired shape
        for (unsigned int y=0; y<height; ++y)
            for (unsigned int x=0; x<width; ++x)
                if (x % 100 < y % 100)
                    XDrawPoint(pDisplay, bitmap_mask, gc, x, y);
        // done
        bResult = Shape_Context_Leave(window, bitmap_mask, gc, pDisplay);
        if (! bResult)
        {
#ifdef DEBUG
            _LOG_WRN("Shape_Context_Leave(window==%d, width==%d, height==%d, bitmap_mask==%d, gc==%d, pDisplay==%p) => retCode==%d",
                     window, width, height, bitmap_mask, gc, pDisplay, bResult);
#endif
        }
        return true;
    }
    // write to file, and read again, just for testing
    if (nApplyMethod == 1)
    {
        string sPath = "/tmp/bitmap_mask.xbm";
        char * pRawData = new char[width * height];
        int xcode = -1;
        do
        {
            for (unsigned int y=0; y<height; ++y)
                for (unsigned int x=0; x<width; ++x)
                    *(pRawData + y*width + x) = (x % 100 < y % 100);
#ifdef DEBUG
            _LOG_NFO("width==%d, height==%d, pRawData->%p", width, height, pRawData);
#endif
            if (! Xbm_WriteFile(sPath, pRawData, width, height))
                _LOG_XERROR_BREAK("cannot write to file '%s'", sPath.c_str());
            if (! Shape_Window_Apply(window, sPath))
                _LOG_XERROR_BREAK("cannot apply shape");
        } while ((xcode = 0));
        if (pRawData)
        {
            delete pRawData;
            pRawData = NULL;
        }
        return (xcode == 0);
    }
    // write to file, and read again, using the bitmap-data
    if (nApplyMethod == 2)
    {
        unsigned char *pDataBitmap = NULL;
        string sPath = "/tmp/bitmap_mask.xbm";
        char * pRawData = new char[width * height];
        int xcode = -1;
        do
        {
            for (unsigned int y=0; y<height; ++y)
                for (unsigned int x=0; x<width; ++x)
                    *(pRawData + y*width + x) = (x % 100 < y % 100);
#ifdef DEBUG
            _LOG_NFO("width==%d, height==%d, pRawData->%p", width, height, pRawData);
#endif
            if (! Xbm_WriteFile(sPath, pRawData, width, height))
                _LOG_XERROR_BREAK("cannot write to file '%s'", sPath.c_str());
            unsigned int width_data = 0;
            unsigned int height_data = 0;
            int x_hot_data = 0;
            int y_hot_data = 0;
            if (! Pixmap_ReadFileData(sPath, pDataBitmap, width_data, height_data, x_hot_data, y_hot_data))
                _LOG_XERROR_BREAK("cannot read bitmap data");
            if (! Shape_Window_Apply(window, sPath))
                _LOG_XERROR_BREAK("cannot apply shape");
        } while ((xcode = 0));
        if (pRawData)
        {
            delete pRawData;
            pRawData = NULL;
        }
        Delete_Object((void*&)pDataBitmap);
        return (xcode == 0);
    }
    return false;
}

int X11wrapper::AfterFunction_Grabber(Display *pDisplay)
{
#ifdef DEBUG
    static const char s_header[] = "Xlib: AfterFunction_Grabber()";
    _LOG_NFO("%s : pDisplay==%p", s_header, pDisplay);
#endif
    return 0;
}

bool X11wrapper::AfterFunction_Set()
{
#ifdef DEBUG
    _LOG_NFO();
#endif

    if (v_bIsChanged_X_AfterFunction)
    {
        _LOG_ERR("already changed");
        return false;
    }
    v_pOld_X_AfterFunction = XSetAfterFunction(GetDisplay(), X11wrapper::AfterFunction_Grabber);

#ifdef DEBUG
    _LOG_NFO("pOld_X_AfterFunction==%p", v_pOld_X_AfterFunction);
#endif

    v_bIsChanged_X_AfterFunction = true;
    return true;
}

bool X11wrapper::AfterFunction_Restore()
{
#ifdef DEBUG
    _LOG_NFO("pOld_X_AfterFunction==%p", v_pOld_X_AfterFunction);
#endif

    if (! v_bIsChanged_X_AfterFunction)
    {
        _LOG_WRN("nothing to restore");
        return true;
    }
    XSetAfterFunction(GetDisplay(), v_pOld_X_AfterFunction);
    v_bIsChanged_X_AfterFunction = false;
    return true;
}

bool X11wrapper::Shape_Window_Alpha(Window window, unsigned char nAlphaThreshold/*=128*/)
{
#ifdef DEBUG
    _LOG_NFO("window==%d, nAlphaThreshold==%d", window, nAlphaThreshold);
#endif

    XImage *pXImage = NULL;
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        if (! Extension_Shape_IsAvailable())
            _LOG_XERROR_BREAK("X11 SHAPE extension: not available");
        pXImage = Window_GetImage(window);
        if (! pXImage)
            _LOG_XERROR_BREAK("GetImage");
        Pixmap pixmap = ConvertImageToPixmap(pXImage, window);
        XShapeCombineMask(GetDisplay(), window, ShapeBounding, 0, 0, pixmap, ShapeSet);
    } while ((xcode = 0));
    // cleanup
    if (pXImage)
    {
        XDestroyImage(pXImage);
        pXImage = None;
    }
    return (xcode == 0);
}

bool X11wrapper::Window_DrawImage(Window window, XImage *pXImage)
{
#ifdef DEBUG
    _LOG_NFO("window==%d, pXImage==%p", window, pXImage);
#endif

    GC gc;
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        Display *pDisplay = GetDisplay();
        if (1)
        {
#ifdef DEBUG
            _LOG_NFO("waiting an Expose event");
#endif
            XEvent e;
            do
            {
                XNextEvent(pDisplay, &e);
            } while (e.type != Expose || e.xexpose.count);
            XFlush(pDisplay);
#ifdef DEBUG
            _LOG_NFO("waiting an Expose event : done");
#endif
        }
        XGCValues gcvalues;
        GC gc = XCreateGC(pDisplay, window, 0, &gcvalues);
        unsigned long bg_pixel = 0;
        xcode = XSetForeground(pDisplay, gc, bg_pixel);
        _COND_XSTAT_LOG_ERR_BREAK(xcode);
        xcode = XFillRectangle(pDisplay, window, gc, 0, 0, pXImage->width, pXImage->height);
        _COND_XSTAT_LOG_ERR_BREAK(xcode);
        xcode = XPutImage(pDisplay, window, gc, pXImage, 0, 0, 0, 0, pXImage->width, pXImage->height);
        _COND_XSTAT_LOG_ERR_BREAK(xcode);
    } while ((xcode = 0));
    // cleanup
    XFreeGC(GetDisplay(), gc);
    return (xcode == 0);
}

bool X11wrapper::Window_DrawImage(Window window, char *pData, unsigned int width, unsigned int height)
{
#ifdef DEBUG
    _LOG_NFO("window==%d, pData==%p, width==%d, height==%d", window, pData, width, height);
#endif

    XImage *pXImage = NULL;
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        Display *pDisplay = GetDisplay();
        int screen = GetScreen();
        int depth = XDisplayPlanes(pDisplay, screen);
        XVisualInfo visual_info;
        xcode = XMatchVisualInfo(pDisplay, screen, depth, TrueColor, &visual_info);
        _COND_XSTAT_LOG_ERR_BREAK(xcode);
        Visual *visual = visual_info.visual;
        pXImage = XCreateImage(pDisplay, visual, depth, ZPixmap, 0, pData, width, height, 32, 0);
        if (pXImage == NULL)
            _LOG_XERROR_BREAK("cannot create image");
        pXImage->byte_order = MSBFirst;
        Window_DrawImage(window, pXImage);
    } while ((xcode = 0));
    // cleanup
    if (pXImage)
    {
        XDestroyImage(pXImage);
        pXImage = NULL;
    }
    return (xcode == 0);
}

XImage * X11wrapper::Window_GetImage(Window window, bool bOnlyMask/*=false*/)
{
    // Note that bytes_per_line in concert with offset can be used to extract a subset of the image
    // The first byte of the first line of plane n must be located at the address
    // (data + (n * height * bytes_per_line))

#ifdef DEBUG
    _LOG_NFO("window==%d", window);
#endif 

    XImage *pXImage = NULL;
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        int x = 0;
        int y = 0;
        unsigned int width = 0;
        unsigned int height = 0;
        if (! Object_GetGeometry(window, &x, &y, &width, &height))
            _LOG_XERROR_BREAK("GetPosition");
        if (bOnlyMask)
            pXImage = XGetImage(GetDisplay(), window, 0, 0, width, height, 1, XYPixmap);
        else
            pXImage = XGetImage(GetDisplay(), window, 0, 0, width, height, AllPlanes, ZPixmap);
        if (! pXImage)
            _LOG_XERROR_BREAK("GetImage");
    } while ((xcode = 0));
    return pXImage;
}

Pixmap X11wrapper::ConvertImageToPixmap(XImage *pXImage, Window window)
{
#ifdef DEBUG
    _LOG_NFO("window==%d", window);
#endif

    Pixmap pixmap = None;
    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        GC gc = XDefaultGC(GetDisplay(), GetScreen());
        //Pixmap pixmap = XCreatePixmap(GetDisplay(), Window_GetRoot(), width, height, depth);
        //XPutImage(GetDisplay(), pixmap, gc, image, 0, 0, 0, 0, width, height);
        pixmap = XCreatePixmapFromBitmapData(GetDisplay(), window, pXImage->data, pXImage->width, pXImage->height, 0, 0, pXImage->depth);
        XFlushGC(GetDisplay(), gc);
    } while ((xcode = 0));
    return pixmap;
}

bool X11wrapper::Window_PerPixel_Transparency(Window window, unsigned long nAlphaValue/*=1*/)
{
#ifdef DEBUG
    _LOG_NFO();
#endif

    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        Display *pDisplay = GetDisplay();
        //int screen = GetScreen();
        //int blackColor = BlackPixel(pDisplay, screen);
        //int whiteColor = WhitePixel(pDisplay, screen);
        // draw something
        GC gc = XCreateGC(pDisplay, window, 0, NULL);
        XSetForeground(pDisplay, gc, nAlphaValue);
        XSetBackground(pDisplay, gc, (unsigned long)-nAlphaValue);
        for (int i=0; i<10; i++)
            XDrawLine(pDisplay, window, gc, 10, i+60, 180, i+20);
        XFlush(pDisplay);
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::Pixmap_WriteFile(const string &sPath, const Pixmap &pixmap, unsigned int width, unsigned int height, int x_hot/*=-1*/, int y_hot/*=-1*/)
{
#ifdef DEBUG
    _LOG_NFO("sPath=='%s', pixmap==%d, width==%d, height==%d, x_hot==%d, y_hot==%d",
             sPath.c_str(), pixmap, width, height, x_hot, y_hot);
#endif

    int xcode = -1;
    X11_Locker x11_locker(GetDisplay());
    do
    {
        xcode = XWriteBitmapFile(GetDisplay(), sPath.c_str(), width, height, pixmap, x_hot, y_hot);
        _COND_XSTAT_LOG_ERR_BREAK(xcode);
    } while ((xcode = 0));
    return (xcode == 0);
}

bool X11wrapper::RemoveWindowDecoration(Window window)
{
    _LOG_NFO("X11wrapper: removing window decoration for 0x%x", window);

	Atom WM_HINTS;

	Display *display = XOpenDisplay(NULL);
	if(NULL == display)
	{
		_LOG_ERR("WinListManager: failed to open display");
		return false;
	}

	/* First try to set MWM hints */
	WM_HINTS = XInternAtom(display, "_MOTIF_WM_HINTS", True);
	if(WM_HINTS != None) 
	{
			/* Hints used by Motif compliant window managers */
			struct 
			{
					unsigned long flags;
					unsigned long functions;
					unsigned long decorations;
					long input_mode;
					unsigned long status;
			} MWMHints = { (1L << 1), 0, 0, 0, 0 };

			XChangeProperty(display, window, WM_HINTS, WM_HINTS, 32, PropModeReplace,
				(unsigned char *)&MWMHints, sizeof(MWMHints)/sizeof(long));
	}
	/* Now try to set KWM hints */
	WM_HINTS = XInternAtom(display, "KWM_WIN_DECORATION", True);
	if(WM_HINTS != None) 
	{
			long KWMHints = 0;

			XChangeProperty(display, window, WM_HINTS, WM_HINTS, 32, PropModeReplace,
							(unsigned char *)&KWMHints, sizeof(KWMHints)/sizeof(long));
	}

	/* Now try to set GNOME hints */
	WM_HINTS = XInternAtom(display, "_WIN_HINTS", True);
	if(WM_HINTS != None) 
	{
			long GNOMEHints = 0;

			XChangeProperty(display, window, WM_HINTS, WM_HINTS, 32, PropModeReplace,
							(unsigned char *)&GNOMEHints, sizeof(GNOMEHints)/sizeof(long));
	}

        /* Remove any stick flag */
        WM_HINTS = XInternAtom(display, "_NET_WM_STATE_STICKY", True);
        if(WM_HINTS != None)
        {
                        long GNOMEHints = 0;

                        XChangeProperty(display, window, WM_HINTS, WM_HINTS, 32, PropModeReplace,
                                                        (unsigned char *)&GNOMEHints, sizeof(GNOMEHints)/sizeof(long));
        }	

	XCloseDisplay(display);

	return true;
}

bool X11wrapper::RemoveWindowDecoration(string sWindowId)
{
    if(sWindowId.empty())
	    return false;

    _LOG_NFO("X11wrapper: removing window decoration for %s", sWindowId.c_str());

	unsigned long wid;
    if (sscanf(sWindowId.c_str(), "0x%lx", &wid) != 1 &&
        sscanf(sWindowId.c_str(), "0X%lx", &wid) != 1 &&
        sscanf(sWindowId.c_str(), "%lu", &wid) != 1)
    {
		_LOG_ERR("X11wrapper: failed to parse window id %s", sWindowId.c_str());
        return false;
    }

    return RemoveWindowDecoration((Window)wid); 
}

