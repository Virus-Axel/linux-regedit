#include "browser.h"

Cell::Cell(){
	offset = 0;
}

Cell::~Cell(){
}

KeyCell::KeyCell() : Cell(){
}

int KeyCell::readCell(std::ifstream &stream){
	offset = stream.tellg();
	stream.read((char*)&size, sizeof(size));
	stream.read((char*)&ID, sizeof(ID));
	stream.read((char*)&nodeType, sizeof(nodeType));
	stream.read((char*)&lastWriteTime, sizeof(lastWriteTime));
	stream.read((char*)&parentOffset, sizeof(parentOffset));
	stream.read((char*)&numberOfSubkeys, sizeof(numberOfSubkeys));
	stream.read((char*)&subkeyListOffset, sizeof(subkeyListOffset));
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

}

short int KeyCell::getType(){
	return ID;
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

ValueCell::~ValueCell(){
}

RegistryHive::RegistryHive(){
}

RegistryHive::RegistryHive(const std::string &filepath){
	this->filename = filepath;
	std::ifstream file;
	file.open(filepath, std::ifstream::binary);
	
	char registryHeader[4];
	std::string header ("regf");
	if (file.is_open()) {
		file.read(registryHeader, 4);
		if (header.compare(registryHeader) != 0) {
			// TODO
			// Ensure object does not created here
			std::cout << std::string(registryHeader) << std::endl;
			return;
		}
		file.seekg(8, std::ios::cur);
		file.read((char*)&this->majorVersion, sizeof(this->majorVersion));
		file.read((char*)&this->minorVersion, sizeof(this->minorVersion));
		file.read((char*)&this->rootCellOffset, sizeof(this->rootCellOffset));
		file.read((char*)&this->length, sizeof(this->length));

	}
	char cellHeader[2];
	while (file.good()) {
		file.read(cellHeader, 2);
		if (cellHeader[0] == 'n' && cellHeader[1] == 'k') {
			file.seekg(-6, std::ios::cur);
			tree.emplace_back(std::make_unique<KeyCell>());
			tree.back()->readCell(file);
		}
		else if (cellHeader[0] == 'v' && cellHeader[1] == 'k') {
			file.seekg(-6, std::ios::cur);
			tree.emplace_back(std::make_unique<ValueCell>());
			tree.back()->readCell(file);
		}
	}
	std::cout << "Cells created: " << std::dec << tree.size() << std::endl;

	return;

	for (auto& cell : tree) {
		cell->print();
	}
}

template<typename ... Ts>
std::array<std::byte, sizeof...(Ts)>
makeBytes(Ts&&... args) noexcept {
	return {std::byte(std::forward<Ts>(args))...};
}

template<std::size_t SIZE>
auto makeInteger(std::array<std::byte, SIZE> &bytes) {

}


RegistryHive::RegistryHive(std::vector<std::byte> &buffer) {
	auto HEADER = makeBytes(0x72, 0x65, 0x67, 0x66); // regf
	std::array<std::byte, 4> header;
	std::copy_n(buffer.begin(), header.size(), header.begin());

	if(header != HEADER)
		throw std::runtime_error("Invalid header");

	std::for_each(std::begin(header),
				std::end(header),
				[](const std::byte &e) {
					std::cout << std::to_integer<int>(e) << " ";
				});

	

}

std::string RegistryHive::getName(){
	return this->filename;
}

RegistryHive::~RegistryHive() {
	// for (auto pointer : tree) {
	// 	delete pointer;
	// }
	// tree.clear();
}
