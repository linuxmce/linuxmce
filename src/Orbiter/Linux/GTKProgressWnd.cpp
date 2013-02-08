/**
 * GTKProgressWnd
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version 1.0
 */

#include <sstream>

#include "GTKProgressWnd.h"
#include "DCE/Logger.h"

using namespace DCE;

GTKProgressWnd::GTKProgressWnd()
{
  m_sText = "";
  m_nProgress = 0;
  m_wndName = "progressWindow";
}

GTKProgressWnd::~GTKProgressWnd()
{

}

bool GTKProgressWnd::UpdateProgress(std::string sText, int nProgress)
{
   if (nProgress == -1) {
      m_bDone = true;
   }

    m_sText = sText;
    m_nProgress = nProgress;

    return true;
}

static void *MyThreadFunc(void *pWindow)
{
    if (LoggerWrapper::GetInstance()) LoggerWrapper::GetInstance()->Write(LV_STATUS, "Thread func start ...");
    GTKProgressWnd *pWnd = (GTKProgressWnd *)pWindow;

    pWnd->EventLoop();

    if (LoggerWrapper::GetInstance()) LoggerWrapper::GetInstance()->Write(LV_STATUS, "Thread func ending ...");
    pWnd->DestroyWindow();

    if (LoggerWrapper::GetInstance()) LoggerWrapper::GetInstance()->Write(LV_STATUS, "Thread func ended.");

    if (pWnd->Destroy())
    {
    	delete pWnd;
    }

    return NULL;
}

pthread_t GTKProgressWnd::Run()
{
    pthread_t threadID;
    if (LoggerWrapper::GetInstance()) LoggerWrapper::GetInstance()->Write(LV_STATUS, "Starting the thread ...");
    m_bCanceled = false;
    m_bDone = false;

    gtk_init(NULL, NULL);
    gtk_rc_parse("/usr/pluto/share/orbiter-gtkrc");

    CreateWindow();
    ShowWindow();
    DrawWindow();

    int iResult = pthread_create( &threadID, NULL, MyThreadFunc, (void *)this );
    if ( iResult != 0 )
    {
	if (LoggerWrapper::GetInstance()) LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Fatal error: Cannot start thread!");
    }
    else
    {
        if (LoggerWrapper::GetInstance())
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "Thread started ...");
    }

    m_thisThread = threadID;
    return threadID;
}

void GTKProgressWnd::Terminate()
{
	m_bDestroy = true;
	m_bDone = true;

	pthread_join(m_thisThread, NULL);
}

void GTKProgressWnd::wrapped_label_size_allocate_callback(GtkWidget *label,
					  GtkAllocation *allocation,
					  gpointer data) {
     
     gtk_widget_set_size_request(label, allocation->width, -1);
}

void GTKProgressWnd::CreateWindow()
{
  GtkBuilder		*pBuilder;
  GtkWidget		*pWindow;
  GtkWidget		*pPrompt;
  GtkWidget		*pProgressBar;

  pBuilder = gtk_builder_new();
  gtk_builder_add_from_file(pBuilder, "/usr/pluto/share/GTKOrbiter.glade", NULL);

  pWindow = GTK_WIDGET(gtk_builder_get_object(pBuilder, "progressWindow"));
  gtk_window_set_wmclass(GTK_WINDOW(pWindow),m_wndName.c_str(),m_wndName.c_str());
  m_pWindow = pWindow;

  pPrompt = GTK_WIDGET(gtk_builder_get_object(pBuilder, "progressLabel"));
  m_pPrompt = pPrompt;

  pProgressBar = GTK_WIDGET(gtk_builder_get_object(pBuilder, "progressBar"));
  m_pProgressBar = pProgressBar;

  gtk_builder_connect_signals(pBuilder, NULL);

  g_object_unref(G_OBJECT(pBuilder));	// done with the builder.

  // Hack-o-rama, All this because the #@($#@( GTK developers refuse to fix
  // their shitty GtkLabel code.

  g_signal_connect(G_OBJECT(m_pPrompt), "size-allocate",
		      G_CALLBACK(&wrapped_label_size_allocate_callback), this);


}

bool GTKProgressWnd::ShowWindow(bool bShow)
{
  gtk_widget_show_all(m_pWindow);
  gtk_window_fullscreen(GTK_WINDOW(m_pWindow));

  return true;

}

bool GTKProgressWnd::DrawWindow()
{
  string sFinalPrompt;
  string sPercentageText;
  stringstream ssPercentage;
  gdouble gdPerc;
  gdouble gdPercentage;

  sFinalPrompt = "<span font=\"60\"><b>" + m_sText + "</b></span>";
  gtk_label_set_markup(GTK_LABEL(m_pPrompt),sFinalPrompt.c_str());

  gdPerc = (gdouble)m_nProgress;
  gdPercentage = gdPerc / 100;

  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(m_pProgressBar),gdPercentage);

  ssPercentage << m_nProgress;
  sPercentageText = "" + ssPercentage.str() + "%";
  gtk_progress_bar_set_text(GTK_PROGRESS_BAR(m_pProgressBar), sPercentageText.c_str());

  return true;

}

void GTKProgressWnd::DestroyWindow()
{
  gtk_widget_hide_all(m_pWindow);
  gtk_main_quit();
}

bool GTKProgressWnd::EventLoop()
{
  while (not m_bDone)
  {

    DrawWindow();	// This basically just updates the progress bar

    if (gtk_events_pending())
    {
      gtk_main_iteration(); // do the GTK event loop iteration.
    }

    usleep(20000);

  }

  return true;

}
