#include <iostream>
#include <filesystem>
#include <map>

namespace fs = std::filesystem;

int main (int argc, char* argv[]) {
	if (argc <2) {
		std::cerr << "Usage: mkdir <directory>\r\n";
		return 1;
	}
	fs::path dir_path(argv[1]);

}
