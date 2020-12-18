#include "browser.h"

template<typename ... Ts>
std::array<std::byte, sizeof...(Ts)>
makeBytes(Ts&&... args) noexcept {
	return {std::byte(std::forward<Ts>(args))...};
}

template<typename T>
struct reversionWrapper { T& iterable; };

template<typename T>
auto begin(reversionWrapper<T> w) {
	return std::rbegin(w.iterable);
}

template<typename T>
auto end(reversionWrapper<T> w) {
	return std::rend(w.iterable);
}

template <typename T>
reversionWrapper<T> reverse(T&& iterable) {
	return { iterable };
}

template<std::size_t SIZE, typename T>
auto makeInteger(std::array<std::byte, SIZE> &bytes, T &result, bool littleEndian = true) {
	result = 0;
	if (littleEndian)
		for (auto &&i : reverse(bytes))
			result = (result << 8) + std::to_integer<T>(i);
	else
		for (auto &&i : bytes)
			result = (result << 8) + std::to_integer<T>(i);
	return result;
}

template<typename T>
auto vectorToInteger(const std::vector<std::byte> &buffer, size_t offset, T &result, bool littleEndian = true) {
	result = 0;
	std::array<std::byte, sizeof(T)> bytes;
	std::copy_n(buffer.begin() + offset, bytes.size(), bytes.begin());
	if (littleEndian)
		for (auto &&i : reverse(bytes))
			result = (result << 8) + std::to_integer<T>(i);
	else
		for (auto &&i : bytes)
			result = (result << 8) + std::to_integer<T>(i);
	return sizeof(T);
}

Cell::Cell(){
	offset = 0;
}

Cell::~Cell(){
}

KeyCell::KeyCell() : Cell(){
}

KeyCell::KeyCell(const std::vector<std::byte> &buffer, size_t offset) : Cell() {
	this->offset = offset;
	size_t readOffset = this->offset;
	readOffset += vectorToInteger(buffer, readOffset, this->size);
	readOffset += vectorToInteger(buffer, readOffset, this->ID);
	readOffset += vectorToInteger(buffer, readOffset, this->nodeType);
	readOffset += vectorToInteger(buffer, readOffset, this->lastWriteTime);
	readOffset += 4;
	readOffset += vectorToInteger(buffer, readOffset, this->parentOffset);
	readOffset += vectorToInteger(buffer, readOffset, this->numberOfSubkeys);
	readOffset += 4;
	readOffset += vectorToInteger(buffer, readOffset, this->subkeyListOffset);
	readOffset += 4;
	readOffset += vectorToInteger(buffer, readOffset, this->numberOfValues);
	readOffset += vectorToInteger(buffer, readOffset, this->valueListOffset);
	readOffset += vectorToInteger(buffer, readOffset, this->securityIdentifierOffset);
	readOffset += 24;
	readOffset += vectorToInteger(buffer, readOffset, this->keyNameLength);
	readOffset += 2;
	for (size_t i = 0; i < this->keyNameLength; i++)
		this->name.push_back(std::to_integer<char>(buffer.at(readOffset + i)));
}

int KeyCell::readCell(std::ifstream &stream){
	offset = stream.tellg();
	stream.read((char*)&size, sizeof(size));
	stream.read((char*)&ID, sizeof(ID));
	stream.read((char*)&nodeType, sizeof(nodeType));
	stream.read((char*)&lastWriteTime, sizeof(lastWriteTime));
	stream.seekg(4, std::ios::cur);
	stream.read((char*)&parentOffset, sizeof(parentOffset));
	stream.read((char*)&numberOfSubkeys, sizeof(numberOfSubkeys));
	stream.seekg(4, std::ios::cur);
	stream.read((char*)&subkeyListOffset, sizeof(subkeyListOffset));
	stream.seekg(4, std::ios::cur);
	stream.read((char*)&numberOfValues, sizeof(numberOfValues));
	stream.read((char*)&valueListOffset, sizeof(valueListOffset));
	stream.read((char*)&securityIdentifierOffset, sizeof(securityIdentifierOffset));
	stream.read((char*)&keyNameLength, sizeof(keyNameLength));

	return 0;
}

KeyCell::~KeyCell(){
}

void KeyCell::print() {
	std::cout << "offset: " << std::hex << this->offset << std::endl;
	std::cout << "size: " << size << std::endl;
	std::cout << "node ID: " << ID << std::endl;
	std::cout << "node type: " << nodeType << std::endl;
	std::cout << "last write time:" << lastWriteTime << std::endl;
	std::cout << "parent offset: " << parentOffset << std::endl;
	std::cout << "nr of subkeys: " << numberOfSubkeys << std::endl;
	std::cout << "subkey list offset: " << subkeyListOffset << std::endl;
	std::cout << "nr of values: " << numberOfValues << std::endl;
	std::cout << "value list offset: " << valueListOffset << std::endl;
	std::cout << "security identifier offset: " << securityIdentifierOffset << std::endl;
	std::cout << "key name length: " << keyNameLength << std::endl;
	std::cout << this->name << std::endl;

	std::cout << "SUBKEYSSIZE: " << this->subKeys.size() << std::endl;
}

unsigned int KeyCell::getOffset() {
	return this->offset;
}

void KeyCell::makeTree(const std::vector<std::byte> &buffer, std::map<unsigned int, std::unique_ptr<Cell>> &cellMap) {
	unsigned int offset;
	for (size_t i = 0; i < this->numberOfSubkeys; i++) {
		vectorToInteger(buffer, HBIN + this->subkeyListOffset + (8 * (i + 1)), offset);
		offset += HBIN;
		this->subKeys.emplace_back(static_cast<KeyCell*>(std::move(cellMap[offset].release())));
		cellMap.erase(offset);

		for (size_t j = 0; j < this->subKeys.back()->numberOfValues; j++) {
			vectorToInteger(buffer, HBIN + this->subKeys.back()->valueListOffset + (4 * (j + 1)), offset);
			//this->subKeys.back()->print();
			//std::cout << "OFF: " << offset << std::endl;
			offset += HBIN;
			this->subKeys.back()->values.emplace_back(static_cast<ValueCell*>(std::move(cellMap[offset].release())));
			//std::cout << this->subKeys.back()->numberOfValues << " " << offset << std::endl;
			if (offset == 7896)
				this->subKeys.back()->values.back()->print();
			cellMap.erase(offset);
		}
		
		this->subKeys.back()->makeTree(buffer, cellMap);
	}
}

short int KeyCell::getType(){
	return ID;
}

std::vector<std::unique_ptr<KeyCell>> &KeyCell::getSubKeys(){
	return subKeys;
}

int KeyCell::getSize(){
	return size;
}

time_t *KeyCell::getLastWriteTime(){
	return &lastWriteTime;
}

int KeyCell::getNumberOfValues(){
	return numberOfValues;
}

ValueCell::ValueCell() : Cell(){
	
}

ValueCell::ValueCell(const std::vector<std::byte> &buffer, size_t offset) : Cell() {
	this->offset = offset;
	size_t readOffset = this->offset;
	readOffset += vectorToInteger(buffer, readOffset, this->size);
	readOffset += vectorToInteger(buffer, readOffset, this->ID);
	readOffset += vectorToInteger(buffer, readOffset, this->valueNameLength);
	readOffset += vectorToInteger(buffer, readOffset, this->dataLength);
	if(this->dataLength < 0) {
		this->dataOffset = 0;
		readOffset += sizeof(this->dataOffset);
		for (size_t i = 4; i < this->dataLength - 0x80000000; i--) 
			this->data.push_back(std::to_integer<char>(buffer.at(readOffset + i)));
	}
	else {
		readOffset += vectorToInteger(buffer, readOffset, this->dataOffset);
		for (size_t i = 0; i < this->dataLength; i++)
			this->data.push_back(std::to_integer<char>(buffer.at(this->dataOffset + HBIN + i)));
	}
	readOffset += vectorToInteger(buffer, readOffset, this->valueType);
	readOffset += 4;
	if(this->valueNameLength == 0){
		name = "(Default)";
	}
	for (size_t i = 0; i < this->valueNameLength; i++)
		this->name.push_back(std::to_integer<char>(buffer.at(readOffset + i)));
}

int ValueCell::readCell(std::ifstream &stream){
	this->offset = stream.tellg();
	stream.read((char*)&size, sizeof(size));
	stream.read((char*)&ID, sizeof(ID));
	stream.read((char*)&valueNameLength, sizeof(valueNameLength));
	stream.read((char*)&dataLength, sizeof(dataLength));
	stream.read((char*)&dataOffset, sizeof(dataOffset));
	stream.read((char*)&valueType, sizeof(valueType));

	return 0;
}

void ValueCell::print(){
	std::cout << "offset: " << std::hex << this->offset << std::endl;
	std::cout << "size: " << this->size << std::endl;
	std::cout << "node ID: " << this->ID << std::endl;
	std::cout << "value name length: " << this->valueNameLength << std::endl;
	std::cout << "data length: " << this->dataLength << std::endl;
	std::cout << "data offset: " << this->dataOffset << std::endl;
	std::cout << "value type: " << this->valueType << std::endl;
	std::cout << this->data << std::endl;
}

short int ValueCell::getType(){
	return ID;
}

int ValueCell::getSize(){
	return size;
}

int ValueCell::getDataLength(){
	return dataLength;
}

int ValueCell::getValueType(){
	return valueType;
}

unsigned int ValueCell::getOffset() {
	return this->offset;
}

ValueCell::~ValueCell(){
}

RegistryHive::RegistryHive(){
}

std::map<unsigned int, std::unique_ptr<Cell>> makeMap(std::vector<std::unique_ptr<Cell>> &cells) {
	std::map<unsigned int, std::unique_ptr<Cell>> cellMap;
	for (auto &&i : cells) {
		cellMap.insert(std::make_pair(i->getOffset(), std::move(i)));
	}
	return cellMap;
}

std::vector<std::unique_ptr<Cell>> RegistryHive::getCells(const std::vector<std::byte> &buffer) {
	std::array<std::byte, 2> header;
	std::vector<std::unique_ptr<Cell>> cells;

	for (size_t i = HBIN; i < buffer.size(); i+=header.size()) {
		std::copy_n(buffer.begin() + i, header.size(), header.begin());
		if (header == makeBytes(0x6e, 0x6b)) { // nk
			cells.emplace_back(std::make_unique<KeyCell>(buffer, i - 4));
			i = i + abs(cells.back()->getSize()) - 4;
		}
		if (header == makeBytes(0x76, 0x6b)) { // vk 
			cells.emplace_back(std::make_unique<ValueCell>(buffer, i - 4));
			i = i + abs(cells.back()->getSize()) - 4;
		}
	}

	return cells;
}

std::vector<std::byte> RegistryHive::readFile(const std::string &filepath) {
	std::ifstream ifs(filepath, std::ios::binary|std::ios::ate);

	if(!ifs)
		throw std::runtime_error(filepath + ": " + strerror(errno));

	auto end = ifs.tellg();
	ifs.seekg(0, std::ios::beg);

	auto size = std::size_t(end - ifs.tellg());

	if(size == 0) // Aviod undefined behaviour
		return {};
	
	std::vector<std::byte> buffer(size);

	if(!ifs.read((char*)buffer.data(), buffer.size()))
		throw std::runtime_error(filepath + ": " + strerror(errno));

	return buffer;
}

std::vector<std::unique_ptr<KeyCell>> &RegistryHive::getTree(){
	return tree;
}

RegistryHive::RegistryHive(const std::string &filepath) : RegistryHive::RegistryHive(readFile(filepath)){
	this->filename = filepath;
}

RegistryHive::RegistryHive(const std::vector<std::byte> &buffer) {
	auto HEADER = makeBytes(0x72, 0x65, 0x67, 0x66); // regf
	std::array<std::byte, 4> header;
	std::copy_n(buffer.begin(), header.size(), header.begin());

	if(header != HEADER)
		throw std::runtime_error("Invalid header");

	std::array<std::byte, 2> signature;
	std::vector<std::unique_ptr<Cell>> cells = this->getCells(buffer);

	std::cout << std::dec << "Cells: " << cells.size() << std::endl;

	std::map<unsigned int, std::unique_ptr<Cell>> cellMap = makeMap(cells);

	std::cout << "Mapped: " << std::dec << cellMap.size() << std::endl;

	// Create tree
	while (!cellMap.empty()) {
		auto key = cellMap.begin()->first;
		if (cellMap.size() == 1)
			cellMap.begin()->second->print();
		auto cell = static_cast<KeyCell*>(std::move(cellMap[key].release()));
		this->tree.emplace_back(std::move(cell));
		cellMap.erase(key);
		this->tree.back()->makeTree(buffer, cellMap);
	}

	std::cout << "Remaining: " << cellMap.size() << std::endl;

	std::cout << "Trees: " << this->tree.size() << std::endl;

	// this->tree.back()->print();

	// std::for_each(std::begin(header),
	// 			std::end(header),
	// 			[](const std::byte &e) {
	// 				std::cout << std::to_integer<int>(e) << " ";
	// 			});
}

std::string RegistryHive::getName(){
	return this->filename;
}

RegistryHive::~RegistryHive() {

}
