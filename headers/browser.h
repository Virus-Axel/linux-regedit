#ifndef BROWSER_H
#define BROWSER_H
#include <iostream>
#include <vector>

class KeyCell{
private:
	int size, parentOffset, numberOfSubkeys, subkeyListOffset, numberOfValues, valueListOffset, securityIdentityOffset;
	long int lastWriteTime;
	std::string name;
	short int ID, nodeType, keyNameLength;
public:
};

class ValueCell{
private:
	int size, dataLength, valueType;
	short int ID, valueNameLength;
public:
};

class RegistryHive{
private:
	std::string filename;
	std::vector <KeyCell> keyCells;
	std::vector <ValueCell> valueCells;
public:
	RegistryHive();
	RegistryHive(const std::string &filename);
	int openHive(const std::string &filename);
	~RegistryHive();
};

#endif
