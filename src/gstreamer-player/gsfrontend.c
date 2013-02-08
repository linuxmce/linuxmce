#include "gsfrontend.h"
#include "globals.h"
//#include <glade/glade.h>

struct Player *player;

static void video_widget_realize_cb (GtkWidget * widget, gpointer data)
{
   Window_Xid = GDK_WINDOW_XID (widget->window);
}

static gboolean 
delete_event (GtkWidget *window,
              GdkEvent *event,
              gpointer data)
{
   g_message ("delete event occurred");
   return FALSE;
}

static void
destroy (GtkWidget *widget, gpointer data)
{
  gtk_widget_destroy (widget);
}

static void
frontend_play (GtkWidget *widget, gpointer data)
{
  backend_play();
}

static void
frontend_pause (GtkWidget *widget, gpointer data)
{
  backend_pause();
}

gboolean frontend_start ()
{
  player->table = gtk_table_new (800,600,FALSE);

  player->app_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_window_set_position (GTK_WINDOW (player->app_window),GTK_WIN_POS_CENTER_ALWAYS);
  gtk_window_set_title (GTK_WINDOW (player->app_window),"LMCE-GStreamer-Player");
  gtk_window_set_default_size (GTK_WINDOW (player->app_window),800,600);
  gtk_container_set_border_width (GTK_CONTAINER (player->app_window), 10);
  
  gtk_signal_connect (GTK_OBJECT (player->app_window), "delete_event",
                        GTK_SIGNAL_FUNC (delete_event), NULL);

  g_signal_connect (G_OBJECT (player->app_window), "destroy",
                      G_CALLBACK (destroy), NULL);

  player->video_window = gtk_drawing_area_new ();
  gtk_widget_set_size_request (player->video_window, 100, 100);

  g_signal_connect (player->video_window, "realize",
    G_CALLBACK (video_widget_realize_cb), NULL);

  gtk_widget_set_double_buffered (player->video_window, FALSE);

  player->play_button = gtk_button_new_with_label ("Play");
  player->pause_button = gtk_button_new_with_label ("Pause");

  
  gtk_container_add (GTK_CONTAINER (player->app_window), player->table);
  gtk_table_attach_defaults (GTK_TABLE(player->table), player->video_window, 0, 700, 0, 500);
  gtk_table_attach_defaults (GTK_TABLE(player->table), player->play_button, 0, 15, 590, 600);
  gtk_table_attach_defaults (GTK_TABLE(player->table), player->pause_button, 15, 25, 590, 600);

  g_signal_connect (G_OBJECT (player->play_button), "clicked",
                          G_CALLBACK (frontend_play), NULL);

  g_signal_connect (G_OBJECT (player->pause_button), "clicked",
                          G_CALLBACK (frontend_pause), NULL);

  gtk_widget_show_all (player->app_window);
  gtk_widget_realize (player->app_window);

  g_assert (Window_Xid != 0);

  return TRUE;
}
gboolean frontend_init (int *argc,char **argv[])
{
  gtk_init (argc, argv);
  player = (struct Player*)malloc(sizeof(Player));
  if (player != NULL)
    return TRUE;
  else
    return FALSE;
}

