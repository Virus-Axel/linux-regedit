#ifndef BROWSER_H
#define BROWSER_H
#include <iostream>

class KeyCell{
private:
	int size, nodeType;
	long int lastWriteTime;
	std::string name;
	char ID[2];
public:
};

class RegistryHive{
private:
	std::string filename;
public:
	RegistryHive();
	RegistryHive(const std::string &filename);
	int openHive(const std::string &filename);
	~RegistryHive();
};

#endif
