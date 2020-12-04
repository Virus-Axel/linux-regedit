#include <browser.h>

Cell::Cell(){
	offset = 0;
}

Cell::~Cell(){
}

KeyCell::KeyCell() : Cell(){
}

int KeyCell::readCell(int position, std::ifstream &stream){
	offset = position;
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

ValueCell::ValueCell() : Cell(){
}

int ValueCell::readCell(int position, std::ifstream &stream){
	offset = position;
	stream.read((char*)&size, sizeof(size));
	stream.read((char*)&ID, sizeof(ID));
	stream.read((char*)&valueNameLength, sizeof(valueNameLength));
	stream.read((char*)&dataLength, sizeof(dataLength));
	stream.read((char*)&dataOffset, sizeof(dataOffset));
	stream.read((char*)&valueType, sizeof(valueType));

	return 0;
}

void ValueCell::print(){
	std::cout << "size: " << size << std::endl;
	std::cout << "node ID: " << ID << std::endl;
	std::cout << "value name length: " << valueNameLength << std::endl;
	std::cout << "data length: " << dataLength << std::endl;
	std::cout << "data offset: " << dataOffset << std::endl;
	std::cout << "value type: " << valueType << std::endl;
}

ValueCell::~ValueCell(){
}

RegistryHive::RegistryHive(){
}

RegistryHive::RegistryHive(const std::string &filename){
	this->filename = filename;
}

RegistryHive::~RegistryHive(){
}
