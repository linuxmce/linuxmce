#ifndef __GST_BACKEND__
#define __GST_BACKEND__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include<gst/gst.h>
#include<gst/interfaces/xoverlay.h>

typedef struct Media {
  GstElement *pipeline;
  GstBus *bus;
  gchar *uri;
}Media;


gboolean backend_init (int *argc, char **argv[]);

gboolean backend_start (gchar *uri);

static gboolean handle_message (GstBus *bus, GstMessage *message , gpointer data);

GstBusSyncReply CreateWindow (GstBus *bus, GstMessage *message, gpointer data);

void backend_play ();
void backend_pause ();
void backend_seek ();

#endif
