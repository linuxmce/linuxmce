/*
     Copyright (C) 2008 Locale|CONCEPT for the LinuxMCE project.

	Author: Thomas Cherryhomes <thom.cherryhomes@localeconcept.com> 
	

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

	This code was based in part on:

	wmctrl
	A command line tool to interact with an EWMH/NetWM compatible X Window Manager.

	Author, current maintainer: Tomas Styblo <tripie@cpan.org>

	Copyright (C) 2003

*/

#include "WindowUtils.h"
#include "PlutoUtils/CommonIncludes.h"
#include "DCE/Logger.h"

#include <vector>
#include <map>
#include <string>
#include <cstdlib>
 
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/select.h>

extern int errno;

using namespace std;
using namespace DCE;

#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/PlutoDefs.h"

namespace WindowUtils
{
	using std::map;
	using std::vector;
	using std::string;

}

	/**
	 * GetProperty - Get an X Property.
	 */
	char *WindowUtils::GetProperty(Display *disp, Window win, Atom xa_prop_type, string s_PropertyName, unsigned long& size)
	{
		Atom xa_prop_name;
		Atom xa_ret_type;
		int ret_format;
		unsigned long ret_nitems;
		unsigned long ret_bytes_after;
		unsigned long tmp_size;
		unsigned char *ret_prop;
		char *ret;

		xa_prop_name = XInternAtom(disp, s_PropertyName.c_str(), False);
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"WindowUtils::GetProperty -- getting property %s",s_PropertyName.c_str());
#endif
    		/* MAX_PROPERTY_VALUE_LEN / 4 explanation (XGetWindowProperty manpage):
     		 *
     		 * long_length = Specifies the length in 32-bit multiples of the
     		 *               data to be retrieved.
     		*/

		if (XGetWindowProperty(disp, win, xa_prop_name, 0, MAX_PROPERTY_VALUE_LEN / 4, False,
		    xa_prop_type, &xa_ret_type, &ret_format,     
	            &ret_nitems, &ret_bytes_after, &ret_prop) != Success) 
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot get %s property.",s_PropertyName.c_str());
#endif
			return NULL;
		}

		if (xa_ret_type != xa_prop_type) 
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Invalid type of %s property.", s_PropertyName.c_str());
#endif
			XFree(ret_prop);
			return NULL;
		}


	       /* null terminate the result to make string handling easier */
		tmp_size = (ret_format / 8) * ret_nitems;
		ret = (char *)malloc(tmp_size+1);

		memcpy(ret, ret_prop, tmp_size);
		ret[tmp_size] = '\0';
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"WindowUtils::GetProperty -- Got Property: %s",ret);
#endif
	       	if (size) {
        		size = tmp_size;
    	       	}

		XFree(ret_prop);
		return ret;

	}

	/**
	 * GetClientList - Get the list of clients attached to display.
	 */
	bool WindowUtils::GetClientList(Display *disp,vector<Window>& v_wClientList)
	{

		Window *client_list;
		unsigned int i;
		unsigned long client_list_size;
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"WindowUtils::GetClientList -- root window ID is 0x%x",DefaultRootWindow(disp));
#endif
		if ((client_list = (Window *)GetProperty(disp,DefaultRootWindow(disp),XA_WINDOW,"_NET_CLIENT_LIST",client_list_size)) == NULL)  
		{
			if ((client_list = (Window *)GetProperty(disp,DefaultRootWindow(disp), XA_CARDINAL, "_WIN_CLIENT_LIST",client_list_size)) == NULL)
			{
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WindowUtils: Cannot Get Client List Properties");
#endif
				return false;
			}
		}
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"WindowUtils::GetClientList -- Client List Size %d",client_list_size);
#endif
		// Now transmorgrify it into a vector of Windows 

		for (i=0;i<client_list_size/sizeof(Window);i++)
		{
			v_wClientList.push_back(client_list[i]);
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"WindowUtils::GetClientList -- Client added: 0x%.8lx",client_list[i]);
#endif				
		}
	
		return true;

	}
 
	/**
	 * GetWindowTitle - Get Window Title given Window ID
	 */
	bool WindowUtils::GetWindowTitle(Display *disp, Window win, string& s_Title)
	{
		char *wm_name;
		unsigned long int wm_name_size;
		char *net_wm_name;
		unsigned long int net_wm_name_size;
#ifdef DEBUG
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"WindowUtils::GetWindowTitle -- Getting Window Title for Window 0x%x",win);
#endif
		if ((net_wm_name = GetProperty(disp, win, XInternAtom(disp, "UTF8_STRING", False), "_NET_WM_NAME", net_wm_name_size)) == NULL) 
		{
			// no _NET_WM property for window title, get it the old way.
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WindowUtils::GetWindowTitle -- no _NET_WM_Property, getting old way.");
#endif
			if ((wm_name = GetProperty(disp, win, XA_STRING, "WM_NAME", wm_name_size)) == NULL) 
			{
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"WindowUtils::GetWindowTitle -- no WM_NAME property either...bailing...");
#endif
				return false;
			} else {
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"WindowUtils::GetWindowTitle -- Setting Title to WM_NAME %s",wm_name);
#endif
				s_Title = string(wm_name,(size_t)wm_name_size);
				return true;
			}
		}
		else
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"WindowUtils::GetWindowTitle -- Setting Title to _NET_WM_NAME %s",net_wm_name);
#endif
			s_Title = string(net_wm_name,net_wm_name_size);
			return true;
		}

	}

	/**
	 * FindWindowMatching - Return first Window ID matching a title fragment. 
	 */
	bool WindowUtils::FindWindowMatching(Display *disp, string windowName, Window& win)
	{
		vector<Window> v_wClientList;
		string title;
		unsigned int i;

		if (!GetClientList(disp,v_wClientList)) 
		{
#ifdef DEBUG
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WindowUtils::FindWindowMatching -- Could not get client list!");
#endif
			return false;
		}

		for (i=0;i<v_wClientList.size();i++) 
		{
			if (!GetWindowTitle(disp,v_wClientList[i],title)) 
			{
#ifdef DEBUG
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WindowUtils::FindWindowMatching -- Could not get window title for Window ID %x",v_wClientList[i]);
#endif
				continue;			
			}
			else 
			{
				if (title.find(windowName,0) == string::npos)
				{
#ifdef DEBUG
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WindowUtils::FindWindowMatching -- Couldn't match window title %s",title.c_str());
#endif
					continue;
				}
				else
				{
					win = (Window)v_wClientList[i];
#ifdef DEBUG
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"WindowUtils::FindWindowMatching -- Position: %d, Title Matched %s to Window id 0x%x",title.find_first_of(windowName),title.c_str(),win);
#endif
					return true;
				}		
			}
		}
#ifdef DEBUG	
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"WindowUtils::FindWindowMatching -- Fell all the way through!");	
#endif
		return false;

	}

	/**
	 * SendKeyToWindow - Send a Key to the specified Window
	 */
	void WindowUtils::SendKeyToWindow(Display *disp, unsigned long win, int iXKeySym, int serial_num) 
	{
		XKeyEvent *event_press;
		XKeyEvent *event_release;
    		//Window oldWindow;
    		//int oldRevertBehaviour;

		event_press   = (XKeyEvent *)malloc(sizeof(XKeyEvent));
		event_release = (XKeyEvent *)malloc(sizeof(XKeyEvent));

		XSync(disp,true);	

    		//XGetInputFocus( m_pDisplay, &oldWindow, &oldRevertBehaviour);
    		//XSetInputFocus( m_pDisplay, (Window)m_iMAMEWindowId, RevertToParent, CurrentTime );

  		event_press->root = DefaultRootWindow(disp);
		//event_press->serial = 5000;
  		event_press->subwindow = None;
  		event_press->time = CurrentTime;
  		event_press->x = 1;
  		event_press->y = 1;
  		event_press->x_root = 1;
 		event_press->y_root = 1;
  		event_press->same_screen = True;

  		event_press->type = KeyPress;
  		event_press->keycode = XKeysymToKeycode(disp, iXKeySym);
  		event_press->state = None; 



  		event_release->root = DefaultRootWindow(disp);
		//event_release->serial = 5000;
  		event_release->subwindow = None;
  		event_release->time = CurrentTime;
  		event_release->x = 1;
  		event_release->y = 1;
  		event_release->x_root = 1;
 		event_release->y_root = 1;
  		event_release->same_screen = True;

  		event_release->type = KeyRelease;
  		event_release->keycode = XKeysymToKeycode(disp, iXKeySym);
  		event_release->state = NoEventMask;

  		XSendEvent(disp, win, False, KeyPressMask, (XEvent *)event_press);
		XSync(disp,true);
		Sleep(50);

		XSendEvent(disp, win, False, KeyPressMask, (XEvent *)event_release); 
		XSync(disp,true);


    		//if ( oldWindow )
        	//	XSetInputFocus( m_pDisplay, oldWindow, oldRevertBehaviour, CurrentTime ); 



		free(event_press);
		free(event_release);


    //XTestFakeKeyEvent( m_pDisplay, XKeysymToKeycode(m_pDisplay, iXKeySym), True, 0 );
    //XTestFakeKeyEvent( m_pDisplay, XKeysymToKeycode(m_pDisplay, iXKeySym), False, 0 );


	}	
	
