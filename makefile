OPTIONS = -Iheaders

linux-regedit: sources/main.cpp sources/browser.cpp headers/browser.h
	g++ sources/main.cpp sources/browser.cpp $(OPTIONS) -o $@

gui: sources/main.cpp sources/browser.cpp sources/window.cpp headers/browser.h headers/window.h
	g++ sources/main.cpp sources/browser.cpp sources/window.cpp $(OPTIONS) `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` -o linux-regedit
