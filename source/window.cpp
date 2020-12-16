#include <window.h>

void Window::start(){
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "linux-regedit");
	gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
	//gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

	openButton = gtk_button_new_with_label("Open hive");
	saveButton = gtk_button_new_with_label("Write changes");
	copyNameButton = gtk_button_new_with_label("Copy name");
	copyValueButton = gtk_button_new_with_label("Copy value");
	
	createTree();
	createList();
	createBoxes();

	rescale();

	signalClose = g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	// g_signal_connect(window, "check-resize", G_CALLBACK(rescale), this);
	signalOpen = g_signal_connect(openButton, "clicked", G_CALLBACK(fileChooser), NULL);
	signalWrite = g_signal_connect(saveButton, "clicked", G_CALLBACK(writeChanges), NULL);
	signalCursorChange = g_signal_connect(treeView, "cursor-changed", G_CALLBACK(cursorChanged), NULL);

	gtk_widget_show_all(window);
	gtk_main();
}

void Window::createBoxes(){
	buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	copyButtonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);

	gtk_box_pack_start(GTK_BOX(buttonBox), openButton, 0, 0, 0);
	gtk_box_pack_start(GTK_BOX(buttonBox), saveButton, 0, 0, 4);

	gtk_box_pack_start(GTK_BOX(copyButtonBox), copyNameButton, 0, 0, 0);
	gtk_box_pack_start(GTK_BOX(copyButtonBox), copyValueButton, 0, 0, 4);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
	gtk_box_pack_start(GTK_BOX(vbox), buttonBox, 0, 0, 4);
	gtk_box_pack_start(GTK_BOX(vbox), scrollWindow1, 0, 0, 4);

	gtk_box_pack_start(GTK_BOX(box), copyButtonBox, 0, 0, 4);
	gtk_box_pack_start(GTK_BOX(box), scrollWindow2, 0, 0, 4);

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, 0, 0, 4);
	gtk_box_pack_start(GTK_BOX(hbox), box, 0, 0, 4);

	gtk_container_add(GTK_CONTAINER(scrollWindow1), treeView);
	gtk_container_add(GTK_CONTAINER(scrollWindow2), listView);
	gtk_container_add(GTK_CONTAINER(window), hbox);
}

void Window::createTree(){
	GtkCellRenderer *renderer;
	GtkTreeModel * model;
	GtkTreeStore *treeStore = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
	scrollWindow1 = gtk_scrolled_window_new(NULL, NULL);

	treeView = gtk_tree_view_new();

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(treeView), -1, "Registry cells", renderer, "text", 0, NULL);

	model = GTK_TREE_MODEL(treeStore);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeView), model);
	g_object_unref(model);
}

void Window::createList(){
	GtkCellRenderer *renderer;
	GtkListStore *listStore = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	scrollWindow2 = gtk_scrolled_window_new(NULL, NULL);

	listView = gtk_tree_view_new();

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(listView), -1, "Header field", renderer, "text", 0, NULL);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(listView), -1, "Value", renderer, "text", 1, NULL);

	GtkTreeModel* model = GTK_TREE_MODEL(listStore);
	gtk_tree_view_set_model(GTK_TREE_VIEW(listView), model);

	g_object_unref(model);
}

void Window::writeChanges(){

}

void Window::cursorChanged(){
	GtkTreePath *path;
	GtkTreeIter iterator;

	gtk_tree_view_get_cursor(GTK_TREE_VIEW(treeView), &path, NULL);
	if(path == NULL)
		return;

	GtkTreeModel *treeModel = gtk_tree_view_get_model(GTK_TREE_VIEW(treeView));
	gtk_tree_model_get_iter(GTK_TREE_MODEL(treeModel), &iterator, path);

	gtk_tree_path_free(path);

	void *cell;
	char *buff;
	
	gtk_tree_model_get(GTK_TREE_MODEL(treeModel), &iterator, 0, &buff, 1, &cell, -1);
	if(cell == NULL){
		if(buff != NULL)
			free(buff);
		return;
	}

	if(buff != NULL)
		free(buff);

	((Cell*)cell)->print();
	populateList((Cell*) cell);
	std::cout << std::endl;
}

void Window::rescale(){
	gint x, y;
	gtk_window_get_size(GTK_WINDOW(window), &x, &y);
	gtk_widget_set_size_request(hbox, x, y - 40);
	gtk_widget_set_size_request(vbox, x / 2 - 5, y - 40);
	gtk_widget_set_size_request(buttonBox, x / 2 - 5, 40);
	gtk_widget_set_size_request(copyButtonBox, x / 2 - 5, 40);
	gtk_widget_set_size_request(scrollWindow1, x / 2 - 5, y - 80);
	gtk_widget_set_size_request(scrollWindow2, x / 2 - 5, y - 80);
}

void Window::fileChooser(){
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
	GtkTreeModel *treeModel = gtk_tree_view_get_model(GTK_TREE_VIEW(treeView));
	g_object_ref(treeModel);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeView), NULL);
	GtkTreeStore *treeStore = GTK_TREE_STORE(treeModel);
	unsigned int i = openHives.size() - 1;
	if(i >= 0){
		gtk_tree_store_append(treeStore, &parent, NULL);
		gtk_tree_store_set(treeStore, &parent, 0, openHives[i]->getName().c_str(), 1, NULL, -1);
		for(unsigned int j = 0; j < openHives[i]->tree.size(); j++){
			if(openHives[i]->tree[j].get()->getType() == KEY_CELL){
				KeyCell* keyCell = (KeyCell*) openHives[i]->tree[j].get();
				gtk_tree_store_append(treeStore, &iterator, &parent);
				gtk_tree_store_set(treeStore, &iterator, 0, keyCell->name.c_str(), 1, (void*) openHives[i]->tree[j].get(), -1);
			}
			else if(openHives[i]->tree[j].get()->getType() == VALUE_CELL){
				ValueCell* valueCell = (ValueCell*) openHives[i]->tree[j].get();
				gtk_tree_store_append(treeStore, &iterator, &parent);
				gtk_tree_store_set(treeStore, &iterator, 0, "value", 1, (void*) openHives[i]->tree[j].get(), -1);
			}
		}
	}
	treeModel = GTK_TREE_MODEL(treeStore);
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeView), treeModel);
	g_object_unref(treeModel);
}

void Window::populateList(Cell* cell){
	GtkTreeIter iterator, parent;
	GtkListStore *listStore = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

	if(cell->getType() == VALUE_CELL){
		ValueCell* valueCell = (ValueCell*)cell;
		gtk_list_store_append(listStore, &iterator);
		gtk_list_store_set(listStore, &iterator, 0, "Size", 1, std::to_string(-valueCell->getSize()).c_str(), -1);
		gtk_list_store_append(listStore, &iterator);
		gtk_list_store_set(listStore, &iterator, 0, "Node ID", 1, std::to_string(valueCell->getType()).c_str(), -1);
		gtk_list_store_append(listStore, &iterator);
		gtk_list_store_set(listStore, &iterator, 0, "Data length", 1, std::to_string(valueCell->getDataLength()).c_str(), -1);
		gtk_list_store_append(listStore, &iterator);
		gtk_list_store_set(listStore, &iterator, 0, "Value type", 1, std::to_string(valueCell->getValueType()).c_str(), -1);
	}
	else if(cell->getType() == KEY_CELL){
		KeyCell* keyCell = (KeyCell*)cell;
		gtk_list_store_append(listStore, &iterator);
		gtk_list_store_set(listStore, &iterator, 0, "Size", 1, std::to_string(-keyCell->getSize()).c_str(), -1);
		gtk_list_store_append(listStore, &iterator);
		gtk_list_store_set(listStore, &iterator, 0, "Node ID", 1, std::to_string(keyCell->getType()).c_str(), -1);
		gtk_list_store_append(listStore, &iterator);
		tm* localTime = localtime(keyCell->getLastWriteTime());
		std::stringstream timeString;
		if(localTime == NULL)
			timeString << "Bad time format";
		else
			timeString << 1900 + localTime->tm_year << '-' << localTime->tm_mon << '-' << localTime->tm_mday;
		gtk_list_store_set(listStore, &iterator, 0, "Last write time", 1, timeString.str().c_str(), -1);
		gtk_list_store_append(listStore, &iterator);
		gtk_list_store_set(listStore, &iterator, 0, "Number of values", 1, std::to_string(keyCell->getNumberOfValues()).c_str(), -1);
	}

	GtkTreeModel *listModel = GTK_TREE_MODEL(listStore);
	gtk_tree_view_set_model(GTK_TREE_VIEW(listView), listModel);
	g_object_unref(listModel);
}

void Window::exit(){
	for(int i = 0; i < openHives.size(); i++)
		delete openHives[i];
}

