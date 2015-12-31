/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "listener.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

listener_args *data;

void listener_create_pipe(void) {

}

void* listener_thread(void *args) {
	int err;
	gchar* buff;
	gchar** buffv;

	GIOChannel* iochan;

	/* fetch passed arguments */
	data = (listener_args *)args;

	/* create a pipe file */
	err = mkfifo(LISTENER, 0666);

	iochan = g_io_channel_new_file(LISTENER, "r", NULL);
	while ( 1 ) {

		err = g_io_channel_read_line(iochan, &buff, NULL, NULL, NULL);

		if (err != G_IO_STATUS_NORMAL || buff == NULL) {
			continue;
		}

		buffv = g_strsplit(buff, "%", 2);
		if (buffv[1]) {
			int progress = atoi(buffv[0]);
			printf("Progress: %s | Message: %s\n", buffv[0], buffv[1]);
	
			gdk_threads_enter();
			
			gchar *labelText = g_strdup_printf("<b><big>%s</big></b>", g_strdelimit(buffv[1],"\n",' '));
			gtk_label_set_markup(GTK_LABEL(data->labelMain), labelText);
			g_free(labelText);
			if (progress != 0 ) {
				gtk_progress_bar_update(GTK_PROGRESS_BAR(data->progressMain), (gfloat)progress/100);
			}
			gdk_threads_leave();
		}

		g_free(buff);
		g_strfreev(buffv);

	}

	return NULL;
}
