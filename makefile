OPTIONS = -Iheaders

linux-regedit: sources/main.cpp sources/browser.cpp headers/browser.h
	g++ $< $(OPTIONS) -o $@

