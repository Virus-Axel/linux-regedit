#include <window.h>

Window::Window(){
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	buttonBox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	button = gtk_button_new_with_label("Open hive");

	gtk_window_set_title(GTK_WINDOW(window), "linux-regedit");
	gtk_window_set_default_size(GTK_WINDOW(window), 800, 400);

	gtk_container_add(GTK_CONTAINER(window), buttonBox);
	gtk_container_add(GTK_CONTAINER(buttonBox), button);

	g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(button, "clicked", G_CALLBACK(fileChooser), window);

	gtk_widget_show_all(window);
	gtk_main();
}

void Window::fileChooser(GtkWidget* button, gpointer window){
	GtkWidget *fileChooserWindow = gtk_file_chooser_dialog_new("Select a file", GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_OPEN, g_dgettext( "gtk30", "_Cancel"), GTK_RESPONSE_CANCEL, g_dgettext( "gtk30", "_Open"), GTK_RESPONSE_OK, NULL);
	gtk_widget_show_all(fileChooserWindow);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(fileChooserWindow), g_get_home_dir());
	gint ret = gtk_dialog_run(GTK_DIALOG(fileChooserWindow));
	if(ret == GTK_RESPONSE_OK){
		std::string filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fileChooserWindow));
		// Open file here
	}
	gtk_widget_destroy(fileChooserWindow);
}

Window::~Window(){
}
