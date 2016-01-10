/*

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "common.h"

enum
{
  COL_NAME = 0,
  COL_ICONDIFF,
  COL_ICONREST,
  NUM_COLS
} ;

gchar* file_name;

void on_StepConfFiles_forward_clicked(GtkWidget *widget, gpointer data) {
	g_queue_push_head(history, (gpointer)STEPCONF);
        displayStep6();
}

void on_StepConfFiles_finish_clicked(GtkWidget *widget, gpointer data) {

}

static GtkTreeModel * create_liststore(void) {
	
	GtkListStore  *store;
	GtkTreeIter    iter;
    	GdkPixbuf     *icon1, *icon2;
    	GError        *error = NULL;

    	store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, GDK_TYPE_PIXBUF, GDK_TYPE_PIXBUF);

    	icon1 = gdk_pixbuf_new_from_file("diff.png", &error);
    	if (error)
    	{
      		g_warning ("Could not load icon: %s\n", error->message);
      		g_error_free(error);
      		error = NULL;
    	}

	icon2 = gdk_pixbuf_new_from_file("Restore.png", &error);
    	if (error)
    	{
      		g_warning ("Could not load icon: %s\n", error->message);
      		g_error_free(error);
      		error = NULL;
    	}

	int fd[2];
	pid_t script_pid;

	pipe(fd);
	script_pid=fork();

	if (script_pid == 0) {
		close(fd[0]);
		close(1);
		dup(fd[1]);
		execlp("./mce-installer-getconffiles.sh",NULL);

	} else if (script_pid > 0) {
		close(fd[1]);
		gchar readbuffer[1024];
		int size=0;

		memset(readbuffer, 0, 1024);
		size = read(fd[0], readbuffer, 1023);
		if (size!=0){
			readbuffer[size-1] = '\0';
			printf("Read: %s --\n",readbuffer);
			gtk_list_store_append(store, &iter);
	  	 	gtk_list_store_set(store, &iter,
                       			COL_NAME, readbuffer,
 		                      	COL_ICONDIFF, icon1,
					COL_ICONREST, icon2,
                       			-1);
		}

		while (1){
			memset(readbuffer, 0, 1024);
			size = read(fd[0], readbuffer, 1023);
			if (size!=0){
				readbuffer[size-1] = '\0';
				printf("Read: %s --\n",readbuffer);
				gtk_list_store_append(store, &iter);
		  	 	gtk_list_store_set(store, &iter,
                       			COL_NAME, readbuffer,
 		                      	COL_ICONDIFF, icon1,
					COL_ICONREST, icon2,
                       			-1);
			} else {
				break;
			}
		}
	}
	
	return store;
}

static GtkWidget * create_view_and_model(void) {

	GtkTreeViewColumn	*col;
	GtkCellRenderer		*renderer;
	GtkTreeModel		*model;
	GtkWidget		*view;

	view = gtk_tree_view_new ();
	
	// Column 1
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
						     -1,
						     "Config file",
						     renderer,
						     "text", COL_NAME,
						     NULL);
	// Column 2
	col = gtk_tree_view_column_new(); 
	renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
						     -1,
						     "View diffs",
						     renderer,
						     "pixbuf", COL_ICONDIFF,
						     NULL);

	// Column 3
	col = gtk_tree_view_column_new(); 
	renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
						     -1,
						     "Restore initial file",
						     renderer,
						     "pixbuf", COL_ICONREST,
						     NULL);

	model = create_liststore ();
	
	gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);

	g_object_unref (model);

	return view;	
}



guint tree_view_get_cell_from_pos(GtkTreeView *view, guint x, guint y, GtkCellRenderer **cell)
{
	GtkTreeViewColumn *col = NULL;
	GList             *node, *columns, *cells;
	guint              colx = 0;

	g_return_val_if_fail ( view != NULL, FALSE );
	g_return_val_if_fail ( cell != NULL, FALSE );

	/* (1) find column and column x relative to tree view coordinates */

	columns = gtk_tree_view_get_columns(view);

	for (node = columns;  node != NULL && col == NULL;  node = node->next)
	{
		GtkTreeViewColumn *checkcol = (GtkTreeViewColumn*) node->data;

		if (x >= colx  &&  x < (colx + checkcol->width))
			col = checkcol;
		else
			colx += checkcol->width;
	}

	printf("coloana: %d\n",colx);
	g_list_free(columns);

	GtkTreeSelection *selection;
  	GtkTreeModel     *model;
	GtkTreeIter       iter;

	model = gtk_tree_view_get_model(view);

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
  	if (gtk_tree_selection_get_selected(selection, &model, &iter))
  	{

		gtk_tree_model_get (model, &iter, COL_NAME, &file_name, -1);

    		g_print ("selected row is: %s\n", file_name);

  	}
  	else {
    		g_print ("no row selected.\n");
  	}

	if (col == NULL)
		return -1; /* not found */

	return colx;
	
}

static gboolean
onButtonPress (GtkWidget *view, GdkEventButton *bevent, gpointer data)
{
	GtkCellRenderer *renderer = NULL;
	guint colx;

	colx=tree_view_get_cell_from_pos(GTK_TREE_VIEW(view), bevent->x, bevent->y, &renderer);

	switch (colx){
		case 133:
			viewDiffs(file_name);
			break;
		case 212:
			restoreOrigFile(file_name);
			break;
		default:
			printf("nu este apasat un buton\n");			
	}

	return FALSE;
}

void viewDiffs(gchar* file) {
	printf("este apasat diff\n");
	gchar *pfile, *diff_text;
	pfile=g_strdup_printf("%s.pbackup",file);
	int fd[2];
	pid_t script_pid;
	pipe(fd);
	script_pid=fork();
	if (script_pid == 0) {
		close(fd[0]);
		close(1);
		dup(fd[1]);
		execlp("./mce-installer-diffs.sh","./mce-installer-diffs.sh",file,pfile,NULL);

	} else if (script_pid > 0) {
		close(fd[1]);
		gchar readbuffer[1024];
		int size=0;

		memset(readbuffer, 0, 1024);
		size = read(fd[0], readbuffer, 1023);
		printf(">>>>>Marimea textului %d\n",size);
		if (size > 1){
			//readbuffer[size-1] = '\0';
			//printf("Read: %s --\n",readbuffer);
			diff_text=g_strdup(readbuffer);
		} else {
			printf("intru pe aici\n");
			diff_text=g_strdup("There are no changesfor this file\n");
		}
		while (1){
			memset(readbuffer, 0, 1024);
			size = read(fd[0], readbuffer, 1023);
			if (size!=0){
				//readbuffer[size-1] = '\0';
				//printf("Read: %s --\n",readbuffer);
				diff_text=g_strconcat(diff_text,readbuffer,NULL);
			} else {
				break;
			}
		}
	}

	GtkWidget *window;
    	GtkTextBuffer *txt_buff;
	GtkWidget *txt_view;
    	//gtk_init (&argc, &argv);
    
    	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "Diffs betwen the original file and modified file");
	gtk_window_set_default_size (GTK_WINDOW (window), 250, 250);
	txt_buff = gtk_text_buffer_new(NULL);
	gtk_text_buffer_insert_at_cursor(txt_buff,diff_text,strlen(diff_text));
	txt_view = gtk_text_view_new_with_buffer (txt_buff);
	gtk_text_view_set_editable(txt_view,FALSE);

	gtk_container_add (GTK_CONTAINER (window), txt_view);

    	gtk_widget_show  (txt_view);	
    	gtk_widget_show  (window);

	printf("Tot textul este:\n %s\n",diff_text);
	g_free(pfile); 	g_free(diff_text); 
}

void restoreOrigFile(gchar* file) {
	printf("este apasta restore\n");
	gchar *pfile;
	pfile=g_strdup_printf("mv -f %s.pbackup %s",file,file);
	printf("comanda %s \n",pfile);
	system(pfile);
	g_free(pfile);
}

void displayStepConfFiles(void) {
	printf("StepConfFiles\n");

	// Cleanup window
	cleanupContainer(mainBox);
	cleanupContainer(mainButtonBox);

	// Wizard text
	GtkWidget *label = gtk_label_new_for_wizard ("Config files that installer changed: ");
	gtk_box_pack_start(GTK_BOX(mainBox), label, TRUE, TRUE, 0);

	GtkWidget *view;
	GtkTreeSelection  *selection;
	view = create_view_and_model ();

	g_signal_connect(GTK_WIDGET(view), "button-press-event", onButtonPress, NULL);
	gtk_box_pack_start(GTK_BOX(mainBox), view, TRUE, FALSE, 1);
	

	// Back Button	
	GtkWidget *buttonBack = gtk_button_new_from_stock(GTK_STOCK_GO_BACK);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonBack);
	g_signal_connect(G_OBJECT(buttonBack), "clicked", G_CALLBACK(on_back_clicked), NULL);

	
	// Button Forward
	GtkWidget *buttonForward = gtk_button_new_from_stock(GTK_STOCK_GO_FORWARD);
	gtk_container_add(GTK_CONTAINER(mainButtonBox), buttonForward);
	g_signal_connect(G_OBJECT(buttonForward), "clicked", G_CALLBACK(on_StepConfFiles_forward_clicked), NULL);
	

	// Redraw window	
	gtk_widget_show_all(mainBox);
	gtk_widget_show_all(mainButtonBox);
}
