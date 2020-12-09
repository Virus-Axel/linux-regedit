#ifndef BROWSER_H
#define BROWSER_H

#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <memory>
#include <cstddef>

#include <errno.h>
#include <time.h>
#include <string.h>

class Cell{
protected:
	int offset;
public:
	Cell();
	virtual int readCell(std::ifstream& stream) = 0;
	virtual void print() = 0;
	virtual ~Cell();
};

class ValueCell : public Cell{
private:
	int offset, size, dataLength, valueType, dataOffset;
	short int ID, valueNameLength;
public:
	ValueCell();
	int readCell(std::ifstream& stream) override;
	void print() override;
	~ValueCell();
};

class KeyCell : public Cell{
private:
	int size, parentOffset, numberOfSubkeys, subkeyListOffset, numberOfValues, valueListOffset, securityIdentifierOffset;
	long int lastWriteTime;
	std::string name;
	short int ID, nodeType, keyNameLength;
	std::vector<KeyCell> subKeys;
	std::vector<ValueCell> values;
public:
	KeyCell();
	int readCell(std::ifstream& stream) override;
	void print() override;
	~KeyCell();
};



class RegistryHive{
private:
	std::string filename;
	long int localWriteTime;
	int majorVersion;
	int minorVersion;
	int rootCellOffset;
	int length;
	std::string name;
	std::vector <std::unique_ptr<Cell>> tree;
public:
	RegistryHive();
	RegistryHive(const std::string &filepath);
	RegistryHive(std::vector<std::byte> &buffer);
	std::vector<std::byte> readFile(const std::string &filepath);
	RegistryHive* operator=(const RegistryHive&);
	int openHive(const std::string &filename);
	~RegistryHive();
};

#endif
