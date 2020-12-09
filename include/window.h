#ifndef WINDOW_H
#define WINDOW_H
#include <iostream>
#include <gtk/gtk.h>
#include <vector>
#include <browser.h>

class Window{
// TODO Maybe switch to namespace later
private:
	std::vector<RegistryHive*> openHives;
	GtkWidget *window, *openButton, *saveButton, *hbox, *vbox, *buttonBox, *treeView, *listView, *scrollWindow1, *scrollWindow2;
	void createBoxes();
	void createTree();
	void createList();
	static void rescale(GtkWidget* window, Window* pointer);
	static void fileChooser(GtkWidget* button, Window* pointer);
	static void writeChanges();
	static void cursorChanged(GtkWidget* parent, Window* pointer);
public:
	Window();
	void resize();
	void openFile();
	void populateTree();
	void displayCell();
	~Window();
};

#endif
