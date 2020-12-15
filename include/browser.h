#ifndef BROWSER_H
#define BROWSER_H

#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <memory>
#include <cstddef>
#include <map>
#include <thread>

#include <errno.h>
#include <time.h>
#include <string.h>

#define HBIN 4096

class Cell{
protected:
	unsigned int offset;
public:
	Cell();
	virtual int readCell(std::ifstream& stream) = 0;
	virtual void print() = 0;
	virtual unsigned int getOffset() = 0;
	virtual int getSize() = 0;
	virtual ~Cell();
};

class ValueCell : public Cell{
private:
	unsigned int offset;
	int size, dataLength, valueType, dataOffset;
	short int ID, valueNameLength;
public:
	ValueCell();
	ValueCell(const std::vector<std::byte> &buffer, size_t offset);
	int readCell(std::ifstream& stream) override;
	void print() override;
	unsigned int getOffset() override;
	int getSize() override;
	~ValueCell();
};

class KeyCell : public Cell{
private:
	unsigned int offset;
	int size, parentOffset, numberOfSubkeys, subkeyListOffset, numberOfValues, valueListOffset, securityIdentifierOffset;
	long int lastWriteTime;
	std::string name;
	short int ID, nodeType, keyNameLength;
	std::vector<std::unique_ptr<KeyCell>> subKeys;
	std::vector<std::unique_ptr<ValueCell>> values;
public:
	KeyCell();
	KeyCell(const std::vector<std::byte> &buffer, size_t offset);
	void makeTree(const std::vector<std::byte> &buffer, std::map<unsigned int, std::unique_ptr<Cell>> &cellMap);
	int readCell(std::ifstream& stream) override;
	void print() override;
	unsigned int getOffset() override;
	int getSize() override;
	~KeyCell();
};



class RegistryHive{
private:
	std::string filename;
	long int localWriteTime;
	int majorVersion;
	int minorVersion;
	int rootCellOffset;
	int hbinOffset;
	int length;
	std::string name;
	std::vector <std::unique_ptr<KeyCell>> tree;
	std::vector <std::unique_ptr<Cell>> oldTree;
public:
	RegistryHive();
	RegistryHive(const std::string &filepath) : RegistryHive(readFile(filepath)) {}
	RegistryHive(const std::vector<std::byte> &buffer);
	std::vector<std::byte> readFile(const std::string &filepath);
	std::vector<std::unique_ptr<Cell>> getCells(const std::vector<std::byte> &buffer);

	RegistryHive* operator=(const RegistryHive&);
	int openHive(const std::string &filename);
	~RegistryHive();
};

#endif
