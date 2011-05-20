/**
 * GTKPromptUser
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version 1.0
 */

#ifndef _GTKPROMPTUSER_H
#define _GTKPROMPTUSER_H

#include <gtk/gtk.h>

#include <string>
#include <map>
#include <list>

using namespace std;

namespace DCE 
{

  class GTKPromptUser
  {
  public:
    enum BtnPlacement {BTN_HORIZ, BTN_VERT};  // This is ultimately not used, but must be here for API.
    string m_wndName;	// The Window name to be manipulated by the wmctrl code.

  protected:
    string m_sPrompt;
    map<int, string> *m_pMapPrompts;
    int m_iUserOption;
    GtkWidget *m_pWindow;
    time_t m_tTimeout;
   
  ///////////////////////////////////////////////////////////////
  // Public methods
  ///////////////////////////////////////////////////////////////
  public:

    GTKPromptUser(string sPrompt, int iTimeoutSeconds, map<int, string> *pMapPrompts=NULL);
    virtual ~GTKPromptUser();

    void Init();
    int RunModal();
    void DeInit();
    bool EventLoop();

    static void wrapped_label_size_allocate_callback(GtkWidget *label,
						     GtkAllocation *allocation,
						     gpointer data);  // Part of the hack-o-rama.

    // This is not used, but here for the API.
    void SetButtonPlacement(BtnPlacement placement);

    // callback pair.
    static void ButtonClicked(GtkWidget *pwidget, gpointer pThis);
    void buttonClickedCallback(GtkWidget *pWidget);

    // and a couple accessors for passing data back in
    void setSelected(int iUserSelected);
    int getSelected();

  };

}

#endif
