/**
 * GTKPromptUser
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version 1.0
 */

#ifndef GTKProgressWnd_H
#define GTKProgressWnd_H

#include <gtk/gtk.h>
#include <string>

#include "PlutoUtils/MultiThreadIncludes.h"

#include <pthread.h>


namespace DCE
{
  class GTKProgressWnd
  {
  protected:
    std::string m_sText;
    int m_nProgress;

    bool m_bCanceled;
    bool m_bDone;
    bool m_bDestroy;

    // The Widgets
    GtkWidget *m_pWindow;
    GtkWidget *m_pPrompt;
    GtkWidget *m_pProgressBar;

  public:
    pthread_t m_thisThread;
    std::string	m_wndName;

    // Methods ////////////////////////////////////////////////////////
  public:
    GTKProgressWnd();
    ~GTKProgressWnd();

    /* virtual */bool DrawWindow();
    /* virtual */bool ShowWindow(bool bShow = true);
    /* virtual */void CreateWindow();
    void DestroyWindow();
    bool UpdateProgress(std::string sText, int nProgress);
    bool IsCancelled(){ return m_bCanceled; };
    bool Destroy(){ return m_bDestroy; }

    static void wrapped_label_size_allocate_callback(GtkWidget *label,
                                          GtkAllocation *allocation,
                                          gpointer data);  // Part of the hack-o-rama.

    virtual bool EventLoop();
    virtual pthread_t Run();
    virtual void Terminate();

  };
}

#endif
