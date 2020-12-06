#ifndef WINDOW_H
#define WINDOW_H
#include <iostream>
#include <gtk/gtk.h>

class Window{
private:
	GtkWidget *window, *button, *buttonBox;
public:
	Window();
	static void fileChooser(GtkWidget* button, gpointer window);
	~Window();
};

#endif
