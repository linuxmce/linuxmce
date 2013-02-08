#include "gsfrontend.h"
#include "gsbackend.h"
#include "globals.h"

int main(int argc, char *argv[])
{
 
  gchar *uri = NULL;

  if (!frontend_init (&argc, &argv)) {
     g_message ("Frontend initialization failure...."); 
     exit (0);
  }

  if (!backend_init (&argc, &argv)) {
    g_message (" Backend creation failure");
    exit (0);
  }
 
  if (argc > 1) {
    if (g_strrstr (argv[1],"rtsp://") || g_strrstr (argv[1],"http://") || g_strrstr (argv[1],"file://"))
      uri = g_strdup (argv[1]);
    else
      uri = g_strdup_printf ("file://%s", argv[1]);
  }
  else {
    g_message ("Specify the media file name..<gstplayer filename>");
    return 0;
  }

  if (!frontend_start ()) {  
     g_message ("Player UI creation failure...."); 
     exit (0);
  }
  if (!backend_start (uri)) {  
    g_message ("Gstreamer backend creation failure");
    exit (0);
  }

  loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);

  backend_stop ();

}
