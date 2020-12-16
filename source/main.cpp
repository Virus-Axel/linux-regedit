#include <iostream>
#include <cstddef>

#include <window.h>
#include <browser.h>

std::vector<std::byte> readFile(const std::string &filepath);

std::vector<std::byte> readFile(const std::string &filepath) {
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

int main(int argc, char *argv[]) {
    //std::vector<std::byte> hiveFile = readFile(argv[1]);
    //std::cout << hiveFile.size() << std::endl;
    // for (size_t i = 0; i < 4; i++)
    // {
    //     std::cout << std::hex << std::to_integer<int>(hiveFile.at(i)) << std::endl;
    // }
    
    //RegistryHive hive = RegistryHive(hiveFile);
	gtk_init(&argc, &argv);
	Window::start();
	Window::exit();
}
