#ifndef __GST_FRONTEND__
#define __GST_FRONTEND__

#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <glib.h>

typedef struct Player {
  GtkWidget *app_window;
  GtkWidget *video_window;
  GtkWidget *pause_button;
  GtkWidget *play_button;
  GtkWidget *table;
}Player;

gboolean frontend_init (int *argc, char **argv[]);
gboolean frontend_start ();
static void video_widget_realize_cb (GtkWidget * widget, gpointer data);
static void frontend_play (GtkWidget *widget, gpointer data);
static void frontend_pause (GtkWidget *widget, gpointer data);

#endif
