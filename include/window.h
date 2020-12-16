#ifndef WINDOW_H
#define WINDOW_H
#include <iostream>
#include <gtk/gtk.h>
#include <vector>
#include <browser.h>
#include <sstream>
#include <time.h>

namespace Window{
	static std::vector<RegistryHive*> openHives;
	static GtkWidget *window, *openButton, *saveButton, *copyNameButton, *copyValueButton, *hbox, *vbox, *buttonBox, *treeView, *listView, *scrollWindow1, *scrollWindow2, *box, *copyButtonBox;
	static glong signalClose, signalOpen, signalWrite, signalCursorChange;
	void createBoxes();
	void createTree();
	void createList();
	static void rescale();
	static void fileChooser();
	static void writeChanges();
	static void cursorChanged();
	void start();
	void appendKey(GtkTreeStore *treeStore, GtkTreeIter *parent, KeyCell* key);
	void populateTree();
	void populateList(Cell* cell);
	void exit();
}

#endif
