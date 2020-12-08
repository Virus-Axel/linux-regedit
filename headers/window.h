#ifndef WINDOW_H
#define WINDOW_H
#include <iostream>
#include <gtk/gtk.h>
#include <vector>
#include <browser.h>

class Window{
// TODO Maybe switch to namespace later
private:
	std::vector<RegistryHive> openHives;
	GtkWidget *window, *openButton, *saveButton, *hbox, *vbox, *buttonBox, *treeView;
	GtkTreeModel *treeModel;
	GtkTreeStore *treeStore;
	void createBoxes();
	void createTree();
public:
	Window();
	static void fileChooser(GtkWidget* button, gpointer window);
	static void writeChanges();
	~Window();
};

#endif
