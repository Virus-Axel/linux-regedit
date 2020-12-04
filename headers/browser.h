#ifndef BROWSER_H
#define BROWSER_H
#include <iostream>
#include <vector>
#include <fstream>

class Cell{
protected:
	int offset;
public:
	Cell();
	virtual int readCell(int position, std::ifstream& stream) = 0;
	~Cell();
};

class KeyCell : public Cell{
private:
	int size, parentOffset, numberOfSubkeys, subkeyListOffset, numberOfValues, valueListOffset, securityIdentifierOffset;
	long int lastWriteTime;
	std::string name;
	short int ID, nodeType, keyNameLength;
public:
	KeyCell();
	int readCell(int position, std::ifstream& stream);
	~KeyCell();
};

class ValueCell : public Cell{
private:
	int size, dataLength, valueType, dataOffset;
	short int ID, valueNameLength;
public:
	ValueCell();
	int readCell(int position, std::ifstream& stream);
	void print();
	~ValueCell();
};

class RegistryHive{
private:
	std::string filename;
	std::vector <Cell*> tree;
public:
	RegistryHive();
	RegistryHive(const std::string &filename);
	int openHive(const std::string &filename);
	~RegistryHive();
};

#endif
