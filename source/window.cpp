#include <window.h>

Window::Window(){
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "linux-regedit");
	gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

	openButton = gtk_button_new_with_label("Open hive");
	saveButton = gtk_button_new_with_label("Write changes");
	
	createTree();
	createBoxes();

	resize();

	g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
	// g_signal_connect(window, "check-resize", G_CALLBACK(rescale), this);
	g_signal_connect(openButton, "clicked", G_CALLBACK(fileChooser), this);
	g_signal_connect(saveButton, "clicked", G_CALLBACK(writeChanges), window);

	gtk_widget_show_all(window);
	gtk_main();
}

void Window::createBoxes(){
	buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start(GTK_BOX(buttonBox), openButton, 0, 0, 0);
	gtk_box_pack_start(GTK_BOX(buttonBox), saveButton, 0, 0, 4);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
	gtk_box_pack_start(GTK_BOX(vbox), buttonBox, 0, 0, 4);
	gtk_box_pack_start(GTK_BOX(vbox), scrollWindow1, 0, 0, 4);

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, 0, 0, 4);

	gtk_container_add(GTK_CONTAINER(scrollWindow1), treeView);
	gtk_container_add(GTK_CONTAINER(window), hbox);
}

void Window::createTree(){
	GtkTreeIter iterator;
	GtkCellRenderer *renderer;
	treeStore = gtk_tree_store_new(1, G_TYPE_STRING);
	scrollWindow1 = gtk_scrolled_window_new(NULL, NULL);

	/*for(int i = 0; i < 10; i++){
	gtk_tree_store_append(treeStore, &iterator, NULL);
	gtk_tree_store_set(treeStore, &iterator, 0, "SAM", -1);
	gtk_tree_store_append(treeStore, &iterator, &iterator);
	gtk_tree_store_set(treeStore, &iterator, 0, "Key", -1);
	}*/

	treeView = gtk_tree_view_new();

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(treeView), -1, "Registry cells", renderer, "text", 0, NULL);

	treeModel = GTK_TREE_MODEL(treeStore);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeView), treeModel);
	g_object_unref(treeModel);
}

void Window::writeChanges(){

}

void Window::rescale(GtkWidget* window, Window* pointer){
	pointer->resize();
}

void Window::resize(){
	gint x, y;
	gtk_window_get_size(GTK_WINDOW(window), &x, &y);
	gtk_widget_set_size_request(hbox, x - 5, y - 40);
	gtk_widget_set_size_request(vbox, x / 2 - 5, y - 40);
	gtk_widget_set_size_request(buttonBox, x / 2 - 5, 40);
	gtk_widget_set_size_request(scrollWindow1, x / 2 - 5, y - 80);
};

void Window::fileChooser(GtkWidget* button, Window* pointer){
	pointer->openFile();
}

void Window::openFile(){
	GtkWidget *fileChooserWindow = gtk_file_chooser_dialog_new("Select a file", GTK_WINDOW(window), GTK_FILE_CHOOSER_ACTION_OPEN, g_dgettext( "gtk30", "_Cancel"), GTK_RESPONSE_CANCEL, g_dgettext( "gtk30", "_Open"), GTK_RESPONSE_OK, NULL);
	gtk_widget_show_all(fileChooserWindow);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(fileChooserWindow), g_get_home_dir());
	gint ret = gtk_dialog_run(GTK_DIALOG(fileChooserWindow));
	if(ret == GTK_RESPONSE_OK){
		std::string filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fileChooserWindow));
		openHives.push_back(new RegistryHive(filename));
		populateTree();
	}
	gtk_widget_destroy(fileChooserWindow);
}

void Window::populateTree(){
	GtkTreeIter iterator, parent;
	treeModel = gtk_tree_view_get_model(GTK_TREE_VIEW(treeView));
	g_object_ref(treeModel);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeView), NULL);
	treeStore = GTK_TREE_STORE(treeModel);
	for(unsigned int i = 0; i < openHives.size(); i++){
		gtk_tree_store_append(treeStore, &parent, NULL);
		gtk_tree_store_set(treeStore, &parent, 0, openHives[i]->getName().c_str(), -1);
		for(unsigned int j = 0; j < openHives[i]->tree.size(); j++){
			if(openHives[i]->tree[j].get()->getType() == KEY_CELL){
				KeyCell* keyCell = (KeyCell*) openHives[i]->tree[j].get();
				gtk_tree_store_append(treeStore, &iterator, &parent);
				gtk_tree_store_set(treeStore, &iterator, 0, keyCell->name.c_str(), -1);
			}
			else if(openHives[i]->tree[j].get()->getType() == VALUE_CELL){
				ValueCell* valueCell = (ValueCell*) openHives[i]->tree[j].get();
				gtk_tree_store_append(treeStore, &iterator, &parent);
				gtk_tree_store_set(treeStore, &iterator, 0, valueCell->name.c_str(), -1);
			}
		}
	}
	treeModel = GTK_TREE_MODEL(treeStore);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeView), treeModel);
	g_object_unref(treeModel);
}

Window::~Window(){
	for(int i = 0; i < openHives.size(); i++)
		delete openHives[i];
}
