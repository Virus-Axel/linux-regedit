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
#include <map>
#include <thread>

#include <errno.h>
#include <time.h>
#include <string.h>

#define KEY_CELL 0x6B6E
#define VALUE_CELL 0x6B76
#define HBIN 4096

class Cell{
// TODO: put ID here and make getType one function
protected:
	unsigned int offset;
public:
	Cell();
	virtual int readCell(std::ifstream& stream) = 0;
	virtual void print() = 0;
	virtual short int getType() = 0;
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
	std::string name;
	ValueCell();
	ValueCell(const std::vector<std::byte> &buffer, size_t offset);
	int readCell(std::ifstream& stream) override;
	void print() override;
	unsigned int getOffset() override;
	int getSize() override;
	short int getType() override;
	int getDataLength();
	int getValueType();
	~ValueCell();
};

class KeyCell : public Cell{
private:
	unsigned int offset;
	int size, parentOffset, numberOfSubkeys, subkeyListOffset, numberOfValues, valueListOffset, securityIdentifierOffset;
	long int lastWriteTime;
	short int ID, nodeType, keyNameLength;
public:
	std::vector<std::unique_ptr<KeyCell>> subKeys;
	std::vector<std::unique_ptr<ValueCell>> values;
	std::string name;
	KeyCell();
	KeyCell(const std::vector<std::byte> &buffer, size_t offset);
	KeyCell(const std::string &buffer, size_t offset);
	void makeTree(const std::vector<std::byte> &buffer, std::map<unsigned int, std::unique_ptr<Cell>> &cellMap);
	void makeTree2(const std::string &buffer, std::map<unsigned int, std::unique_ptr<Cell>> &cellMap);
	int readCell(std::ifstream& stream) override;
	void print() override;
	short int getType() override;
	unsigned int getOffset() override;
	int getSize() override;
	std::vector <std::unique_ptr<KeyCell>> &getSubKeys();
	time_t *getLastWriteTime();
	int getNumberOfValues();
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
	std::vector <std::unique_ptr<Cell>> oldTree;
public:
	//std::vector <std::unique_ptr<Cell>> tree;
	std::vector <std::unique_ptr<KeyCell>> tree;
	RegistryHive();
	std::string getName();
	std::vector <std::unique_ptr<KeyCell>> &getTree();
	RegistryHive(const std::string &filepath);
	RegistryHive(const std::vector<std::byte> &buffer);
	std::vector<std::byte> readFile(const std::string &filepath);
	std::vector<std::unique_ptr<Cell>> getCells(const std::vector<std::byte> &buffer);

	RegistryHive& operator=(const RegistryHive&);
	int openHive(const std::string &filename);
	~RegistryHive();
};

#endif
