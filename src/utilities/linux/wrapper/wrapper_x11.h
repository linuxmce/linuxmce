//
// Author : Remus C.
//
// Changed by : ...
//

// notes:
//        use Lock() and Unlock()
//        use a single instance
//        initialize it from the start

#ifndef _WRAPPER_X11_H_
#define _WRAPPER_X11_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wrapper_x11.h"
#endif

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>

#include <string>
#include <iostream>
using namespace std;

// definition from in Xlib.h
// WARNING, this type not in Xlib spec
typedef int (*XErrorHandler) (Display*, XErrorEvent*);

// new type
typedef int (*X_AfterFunction) (Display*);

// constants
const unsigned long WINDOW_OPAQUE = 0xFFFFFFFF;

/// sync and lock functions
bool X11_Sync(Display *pDisplay); // call flush
bool X11_Lock(Display *pDisplay);
bool X11_Unlock(Display *pDisplay); // call sync

// small helper class to be used with X11 code
// locks in the constructor and unlocks in the destructor
// making it much more difficult to forget to unlock
// (which, in general, will lead to serious and difficult to debug problems)
class X11_Locker
{
public:
    X11_Locker(Display *pDisplay);
    ~X11_Locker();
protected:
    Display *v_pDisplay;
};

// small helper class to be used with X11 code
// locks in the constructor and unlocks in the destructor
// making it much more difficult to forget to unlock
// (which, in general, will lead to serious and difficult to debug problems)
// this one open a new display, and closes it in the destructor
class X11_Locker_NewDisplay
{
public:
    X11_Locker_NewDisplay();
    ~X11_Locker_NewDisplay();
    Display * GetDisplay();
protected:
    Display *v_pDisplay;
};

// X11 wrapper class
// if the display connection is assigned, it will not be closed at exit
// the main window can be kept here, but it is not handled
class X11wrapper
{
public:
    // allow to reuse an already opened display connection
    X11wrapper(Display *pDisplay=NULL);

    // if the display connection was reused, it is not closed
    ~X11wrapper();

    bool IsInitialized();

    bool Clean_Exit();

    /// display

    // reuse an already opened display connection
    void Assign_Display(Display *pDisplay);

    Display * Display_Open();
    bool Display_Close();

    /// X11 info

    Display * GetDisplay();
    int GetScreen();
    string GetDisplayString();
    int GetDisplayWidth();
    int GetDisplayHeight();
    bool GetDisplaySize(int &nWidth, int &nHeight);

    Window Window_GetRoot();

    /// main window info

    // reuse the already created main window
    void Assign_MainWindow(Window window);
    Window GetMainWindow();
    
    /** Sets the window created by the window manager.*/
    void Assign_WmWindow(Window window);
    
    /** Returns the window created by the window manager.*/
    Window GetWmWindow();

    // reuse the already opened display for the main window
    void Assign_MainDisplay(Window window);
    Window GetMainDisplay();

    /// static functions
public:
    static string GetErrorText(Display *pDisplay, int error_code);
    static int ErrorHandler_Grabber(Display *, XErrorEvent *);

    /// instance functions
public:

    /// error handler

    bool ErrorHandler_Set();
    bool ErrorHandler_Restore();

    /// sync and lock functions

    void Sync();
    void Lock();
    void Unlock();

    /// window

    // warning: the wm may change the window position
    // use Window_Create_Show, which will correct the position
    Window Window_Create(int nPosX, int nPosY, unsigned int nWidth, unsigned int nHeight, Window parent_window=None);

    // create the window, show it and correct the position
    Window Window_Create_Show(int nPosX, int nPosY, unsigned int nWidth, unsigned int nHeight, Window parent_window=None, bool bSendToDesktop = false);

    // close a window
    bool Window_Destroy(Window window);

    // show or hide a window
    bool Window_Show(Window window, bool bShow=true);

    // will check if the window was actually moved
    // bUse_WM_Window : use the associated wm-window
    // bWaitForCompletion : try again few times if the operation failed
    bool Window_MoveResize(const Window window, const int nPosX, const int nPosY, const unsigned int nWidth, const unsigned int nHeight, bool bUse_WM_Window, bool bWaitForCompletion);

    bool Window_ClassName(Window window, const char *s_class, const char *s_name);
    bool Window_Name(Window window, const char *s_name);

    bool Window_Raise(Window window);
    bool Window_Lower(Window window);

    // opacity will be set recursive to parents
    // WINDOW_OPAQUE : normal window
    // 1 : very transparent
    // 0 : better not use it
    bool Window_SetOpacity(Window window, unsigned long nOpacity);

    // return 0 on error
    // can return the root window
    Window Window_GetParent(Window window);

    // returns the last parent before the root window
    // can return the same window
    // returns 0 if there is none, or an error occurs
    Window Window_GetDeepestParent(Window window);

    /// keyboard

    bool Keyboard_Grab(Window window_grab);
    bool Keyboard_Ungrab();

    bool Keyboard_SetAutoRepeat(bool bOn);

    /// mouse

    bool Mouse_Grab(Window window_grab, Window window_confine_to=None);
    bool Mouse_Ungrab();

    // if the parent window is the root window, use the same cursor as X11
    bool Mouse_ShowStandardCursor(Window window);
    bool Mouse_HideCursor(Window window);
    bool Mouse_SetCursor_Inherit(Window window);
    bool Mouse_SetCursor_Font(Window window, int nShape);
    bool Mouse_SetCursor_Image(Window window, const string &sPath, const string &sPathMask);
    bool Mouse_QueryMaxCursorSize(Window window, unsigned int &width_return, unsigned int &height_return);

    // accel is : accel_numerator/accel_denominator
    // threshold : used for "quick" user moves
    // defaults seems to be : accel_numerator==2 accel_denominator==1 threshold==4
    // defaults can be set with : -1 -1 -1
    bool Mouse_SetSpeed(int accel_numerator=-1, int accel_denominator=-1, int threshold=-1);

    bool Mouse_GetSpeed(int &accel_numerator_return, int &accel_denominator_return, int &threshold_return);

    // change the position relative to the given window
    // using the root window by default
    bool Mouse_SetPosition(int nPosX, int nPosY, Window relative_to_window=None);

    // returns the position relative to the given window
    // using the root window by default
    bool Mouse_GetPosition(int &nPosX, int &nPosY, Window relative_to_window=None);

    // activate   : both width and height must be > 0
    // deactivate : both width and height == 0
    bool Mouse_Constrain(int nPosX, int nPosY, unsigned int nWidth, unsigned int nHeight, Window window_grab=None);
    bool Mouse_Constrain_Release();
    bool Mouse_Constrain_ReactivateIfActive();
    bool Mouse_IsConstrainActive();
    Window Mouse_Constrain_GetWindow();

    /// cursors

    struct string_int
    {
        std::string sName;
        int nValue;
    };
    static const string_int v_aStdCursors_NameValue[];
    static const int v_aStdCursors_Count;

    // return -1 on error
    static int StdCursor_GetValueByName(const std::string &sName);

    /// graphics

    // depth==1 for a bitmap
    // Delete_Pixmap() should be called to free the memory
    Pixmap Pixmap_Create(Window window, unsigned int width, unsigned int height, unsigned int depth);

    // Delete_Pixmap() should be called to free the memory
    bool Pixmap_ReadFile(Window window, const string &sPath, Pixmap &pixmap_return, unsigned int &width_return, unsigned int &height_return, int &x_hot_return, int &y_hot_return);

    // Delete_Object() should be called to free the data_return
    bool Pixmap_ReadFileData(const string &sPath, unsigned char *&data_return, unsigned int &width_return, unsigned int &height_return, int &x_hot_return, int &y_hot_return);

    /// other

    // actually XFreePixmap() with error-checking
    // and zeroing the id in case of success
    bool Delete_Pixmap(Pixmap &pixmap);

    // actually XFree() with error-checking
    // and zeroing the id in case of success
    bool Delete_Object(void *&id_object);

    // drawable can be: Window, Pixmap
    // NULL arguments can be passed, for un-needed values
    // x and y are always 0 for a Pixmap
    // x and y members are set to the upper-left outer corner relative to the parent window's origin
    // in case of error, nothing is changed
    bool Object_GetGeometry(Drawable drawable, int *x_return, int *y_return, unsigned int *width_return, unsigned int *height_return);

    // only for windows
    // NULL arguments can be passed, for un-needed values
    // deepest parent window, if exist, is checked
    // x and y are absolute
    // in case of error, nothing is changed
    bool Window_GetGeometry(Window window, int *x_return, int *y_return, unsigned int *width_return, unsigned int *height_return);

    // NULL arguments can be passed, for un-needed values
    // inner_window_dest_return is a mapped child of the destination window
    //   which contains those coordinates
    // in case of error, nothing is changed
    bool Window_TranslateCoordinates(Window window_src, Window window_dest, int x_src, int y_src, int *x_dest_return, int *y_dest_return, Window *inner_window_dest_return);

protected:
    // opacity only for this window
    bool Window_SetOpacity_Helper(Window window, unsigned long nOpacity);

    Display *v_pDisplay;
    bool v_bIsAssigned_Display;

    Window v_oMainWindow;
    Window v_oWmWindow;

    XErrorHandler v_pOld_XErrorHandler;
    bool v_bIsChanged_XErrorHandler;

    Window v_window_Mouse_Constrain;
    struct Info_Mouse_Constrain
    {
        Info_Mouse_Constrain()
                : window_grab(None)
                , nPosX(0)
                , nPosY(0)
                , nWidth(0)
                , nHeight(0)
            {
            }
        Window window_grab;
        int nPosX;
        int nPosY;
        unsigned int nWidth;
        unsigned int nHeight;
    };
    Info_Mouse_Constrain previous_mouse_constrain;

    /// shape extension

    // shape usage (A)
    // creating a callback for drawing :  not flexible enough
    // usage:
    //   call Shape_Context_Enter()
    //   remember the return values : Pixmap, GC, Display
    //   DRAW ONLY masked pixels in the Pixmap, using GC and Display
    //   call Shape_Window_Apply()
    //   call Shape_Context_Leave()
    // warning: drawing every pixel may be slow for big images

    // shape usage (B)
    // using the bitmap mask from a file
    //   call Shape_Window_Apply(window, sPath)
    //   delete the bitmap

    // shape usage (C)
    // support for caching shapes
    //   create the bitmap data
    //     by loading the pixmap from disk with Pixmap_ReadFileData()
    //     or by creating the data in memory
    //   call Shape_Window_Apply(window, *data, width, height)
    //   delete the bitmap data

public:
    bool Extension_Shape_IsAvailable();
    bool Extension_Shape_GetVersion_Major();
    bool Extension_Shape_GetVersion_Minor();

    // if returns false, then the return values are not valid
    // return values : Pixmap, GC, Display
    // Pixmap depth == 1
    bool Shape_Context_Enter(Window window, unsigned int width, unsigned int height, Pixmap &bitmap_mask, GC &gc, Display * &pDisplay);

    // returns false if the shape cannot be set
    // cleans up : Pixmap, GC, Display
    bool Shape_Context_Leave(Window window, Pixmap &bitmap_mask, GC &gc, Display * &pDisplay);

    // shape_op : ShapeSet, ShapeUnion, ShapeIntersect, ShapeSubtract, ShapeInvert
    // ShapeSet       0
    // ShapeUnion     1
    // ShapeIntersect 2
    // ShapeSubtract  3
    // ShapeInvert    4
    // defined in <X11/extensions/shape.h>

    // Pixmap should have depth==1
    bool Shape_Window_Apply(Window window, Pixmap &pixmap, int shape_op=0);

    // use a file bitmap
    bool Shape_Window_Apply(Window window, const string &sPath, int shape_op=0);
    
    // use a file bitmap and fill the pixmap parameter
    bool Shape_Window_Apply(Window window, const string &sPath, Pixmap &pixmap, int shape_op=0);

    // use the bitmap data
    bool Shape_Window_Apply(Window window, const char *data_bitmap, unsigned int width, unsigned int height, int shape_op=0);

    bool Shape_Window_Reset(Window window);
    bool Shape_Window_Hide(Window window);
	
	bool Shape_PixmapMask_Rectangle(
		Pixmap &pixmap,
		unsigned int x,
		unsigned int y,
		unsigned int width,
		unsigned int height,
		bool bOpaque );

	bool Shape_PixmapMask_Copy(
		Pixmap &srcPixmap,
		Pixmap &destPixmap,
		unsigned int x,
		unsigned int y,
		unsigned int width,
		unsigned int height,
		unsigned int destX,
		unsigned int destY,
		int op_copy = GXcopy);
	
	bool Shape_PixmapMask_Copy(
		Window window,
		const string &sPath,
		Pixmap &destPixmap,
		unsigned int destX,
		unsigned int destY,
		int op_copy = GXcopy);
	
protected:
    bool Extension_Shape_Initialize();

    int v_bExtension_Shape_IsInitialized;
    bool v_bExtension_Shape_IsAvailable;
    int v_nExtension_Shape_Version_Major;
    int v_nExtension_Shape_Version_Minor;

    /// debug
public:
    Window Debug_Window(bool bCreate_NotClose=true);
    bool Debug_Shape_Context_Example(Window window, unsigned int width, unsigned int height, int nApplyMethod=0);

    static int AfterFunction_Grabber(Display *);
    bool AfterFunction_Set();
    bool AfterFunction_Restore();

    X_AfterFunction v_pOld_X_AfterFunction;
    bool v_bIsChanged_X_AfterFunction;

    /// not yet finished
public:
    bool Shape_Window_Alpha(Window window, unsigned char nAlphaThreshold=128);

    bool Window_DrawImage(Window window, XImage *pImage);
    // expecting a 32-bit image
    bool Window_DrawImage(Window window, char *data, unsigned int width, unsigned int height);

    // XDestroyImage should be called
    XImage * Window_GetImage(Window window, bool bOnlyMask=false);
    Pixmap ConvertImageToPixmap(XImage *pXImage, Window window);
    bool Window_PerPixel_Transparency(Window window, unsigned long nAlphaValue=1);

    // works only in X
    bool Pixmap_WriteFile(const string &sPath, const Pixmap &pixmap, unsigned int width, unsigned int height, int x_hot=-1, int y_hot=-1);

	static bool RemoveWindowDecoration(Window window);
	static bool RemoveWindowDecoration(string sWindowId);
};

#endif
// _WRAPPER_X11_H_
