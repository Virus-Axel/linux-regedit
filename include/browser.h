#ifndef BROWSER_H
#define BROWSER_H

#include <algorithm>
#include <iterator>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <memory>
#include <cstddef>

#include <errno.h>
#include <time.h>
#include <string.h>

#define KEY_CELL 0x6B6E
#define VALUE_CELL 0x6B76

class Cell{
// TODO: put ID here and make getType one function
protected:
	int offset;
public:
	Cell();
	virtual int readCell(std::ifstream& stream) = 0;
	virtual void print() = 0;
	virtual short int getType() = 0;
	virtual ~Cell();
};

class KeyCell : public Cell{
private:
	int size, parentOffset, numberOfSubkeys, subkeyListOffset, numberOfValues, valueListOffset, securityIdentifierOffset;
	long int lastWriteTime;
	short int ID, nodeType, keyNameLength;
public:
	std::string name;
	KeyCell();
	int readCell(std::ifstream& stream) override;
	void print() override;
	short int getType() override;
	int getSize();
	time_t *getLastWriteTime();
	int getNumberOfValues();
	~KeyCell();
};

class ValueCell : public Cell{
private:
	int offset, dataLength, valueType, dataOffset;
	short int ID, valueNameLength;
	int size;
public:
	std::string name;
	ValueCell();
	int readCell(std::ifstream& stream) override;
	void print() override;
	short int getType() override;
	int getSize();
	int getDataLength();
	int getValueType();
	~ValueCell();
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
public:
	std::vector <std::unique_ptr<Cell>> tree;
	RegistryHive();
	std::string getName();
	RegistryHive(const std::string &filepath);
	RegistryHive(std::vector<std::byte> &buffer);
	std::vector<std::byte> readFile(const std::string &filepath);
	RegistryHive& operator=(const RegistryHive& other);
	int openHive(const std::string &filename);
	~RegistryHive();
};

#endif
