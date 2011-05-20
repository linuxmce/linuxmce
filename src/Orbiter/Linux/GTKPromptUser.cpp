/**
 * GTKPromptUser
 *
 * Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
 *
 * Version 1.0
 */

#include "PlutoUtils/CommonIncludes.h"
#include "GTKPromptUser.h"
#include "DCE/Logger.h"
#include <sstream>

using namespace DCE;

GTKPromptUser::GTKPromptUser(string sPrompt, int iTimeoutSeconds, map<int, string> *pMapPrompts/*=NULL*/)
{
  m_wndName = "UserPrompt";
  m_sPrompt = sPrompt;
  m_pMapPrompts = pMapPrompts;
  m_pWindow = NULL;
  m_iUserOption = 0; // Nothing has been selected by default.
  m_tTimeout = iTimeoutSeconds ? time(NULL) + iTimeoutSeconds : 0;
}

GTKPromptUser::~GTKPromptUser()
{
  // Nothing yet.
}

void GTKPromptUser::ButtonClicked(GtkWidget *pWidget, gpointer pThis)
{
  GTKPromptUser *pInstance = (GTKPromptUser*)pThis;
  pInstance->buttonClickedCallback(pWidget);
}

void GTKPromptUser::buttonClickedCallback(GtkWidget *pWidget)
{
  string sName = gtk_widget_get_name(pWidget);
  istringstream ssName;
  ssName.str(sName);
  int iName;
  ssName >> iName;
  setSelected(iName);
  
  gtk_main_quit();
}

void GTKPromptUser::wrapped_label_size_allocate_callback(GtkWidget *label,
							  GtkAllocation *allocation,                       
							  gpointer data) 
{
  gtk_widget_set_size_request(label, allocation->width, -1);
} 

void GTKPromptUser::Init()
{

  GtkBuilder	*pBuilder;
  GtkWidget	*pWindow;
  GtkWidget	*pScrollContainer;
  GtkWidget	*pPrompt;
  string	sFinalPrompt;

  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"+++ GTK: Calling gtk_init()");
  gtk_init(NULL, NULL);

  // Parse the gtkrc
  gtk_rc_parse("/usr/pluto/share/orbiter-gtkrc");

    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"+++ GTK: Loading Built GTKOrbiter Glade");
  pBuilder = gtk_builder_new();
  gtk_builder_add_from_file(pBuilder, "/usr/pluto/share/GTKOrbiter.glade", NULL);

  pWindow = GTK_WIDGET(gtk_builder_get_object(pBuilder, "promptWindow"));
  m_pWindow = pWindow;

  // I know this is wrong! but we do not call gtk_init() from main() !
  // It has to be explicitly set here.
  gtk_window_set_wmclass(GTK_WINDOW(pWindow),m_wndName.c_str(),m_wndName.c_str());

  pPrompt = GTK_WIDGET(gtk_builder_get_object(pBuilder, "promptLabel"));

  sFinalPrompt = "<span size=\"x-large\"><b>" + m_sPrompt + "</b></span>";
  gtk_label_set_markup(GTK_LABEL(pPrompt),sFinalPrompt.c_str());

    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"+++ GTK: Connecting Signals");
  gtk_builder_connect_signals(pBuilder, NULL);

  pScrollContainer = GTK_WIDGET (gtk_builder_get_object (pBuilder, 
							 "promptScrollArea"));

  // Hack-o-rama to resize the label properly.
  //g_signal_connect(G_OBJECT(pPrompt), "size-allocate",
  //		   G_CALLBACK(&wrapped_label_size_allocate_callback), this);

  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"+++ GTK: Show Window");
  gtk_widget_show(pWindow);

  g_object_unref(G_OBJECT(pBuilder));

  /////////////////////////////////////////////
  // Prompt code follows
  /////////////////////////////////////////////

  if (m_pMapPrompts)
  {
    // Grab reference to the scroll area and add the hbox.

    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"+++ GTK: Adding vbox ");
    GtkWidget *pButtonBox = gtk_vbox_new(TRUE, 0);
    gtk_container_add(GTK_CONTAINER(pScrollContainer), pButtonBox);
    LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"+++ GTK: Show hbox");
    gtk_widget_show(pButtonBox);

    // Button Placement Hints are IGNORED!
    map<int, string>::iterator it = m_pMapPrompts->begin();

    while (it != m_pMapPrompts->end())
    {
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"+++ GTK: Adding Button: %s",it->second.c_str());
      string sButtonPrompt = ""+it->second+"";
      GtkWidget *pCurrButton = gtk_button_new_with_label(sButtonPrompt.c_str());
      stringstream ssNum;
      ssNum << it->first;
      string sNum = ssNum.str();
      gtk_widget_set_name(pCurrButton, sNum.c_str());

      // Fill callback handler, and Attach the signal handler.
      g_signal_connect(G_OBJECT(pCurrButton),"clicked",G_CALLBACK(&ButtonClicked), this);
      gtk_box_pack_start (GTK_BOX (pButtonBox), pCurrButton, TRUE, TRUE, 0);
      LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"+++ GTK: Show button: %s",it->second.c_str());
      gtk_widget_show(pCurrButton);
      it++;
    }

  }

  // end prompt code //////////////////////////

  // finally show the window.
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"+++ GTK: Show Whole Window");
  gtk_widget_show_all(pWindow);
  gtk_window_fullscreen(GTK_WINDOW(pWindow));  
}

void GTKPromptUser::DeInit()
{
 LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"+++ GTK: DeInit");
  // We need to clean up the callback references.
}

int GTKPromptUser::RunModal()
{
  // run the runloop. This is okay, because we only have one of these up at any given time in a process.
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"+++ GTK: gtk_main(): Run Modal Runloop");
  //gtk_main();
  EventLoop(); // our custom little event loop with a timeout instead of gtk_main();

  return getSelected();   // This is set in a callback above.

}

bool GTKPromptUser::EventLoop()
{
  // Pretty much a manual GTK+ event loop, with a timeout.
  while (true)
    {
      // no draw window here, we're not updating anything, just accepting events.
      if (gtk_events_pending())
	{
	  gtk_main_iteration();
	}

      if (m_tTimeout && m_tTimeout<time(NULL))
	{
	  // we timed out waiting for things to finish.
	  gtk_main_quit();
	  return true;
	}

      usleep(100000); // sleep for a bit to keep from spinning the CPU.

    }
}

void GTKPromptUser::SetButtonPlacement(BtnPlacement placement)
{
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"+++ GTK: Set Button Placement currently ignored.");
  // completely ignored.
}

int GTKPromptUser::getSelected()
{
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"+++ GTK: getSelected is %d",m_iUserOption);
  return m_iUserOption;
}

void GTKPromptUser::setSelected(int iUserSelected)
{
  LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"+++ GTK: setSelected to %d",m_iUserOption);
  m_iUserOption = iUserSelected;
}
