#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <regex>

#define FLAG_i 0x01 // case insensitive search
#define FLAG_v 0x02 // inverse matching
#define FLAG_c 0x04 // count occurences		    



void printError(const std::string& message);
int parseArgs(int argc, char*  argv[], uint32_t& flags, std::string& pattern, std::string& file_name);
void findPattern(uint32_t flags, std::string& pattern, std::ifstream file);


int main(int argc, char* argv[]) {

	if (argc<3) {
		printError("Usage: grep <pattern> <file>\r\n");
		return 1;
	}
	
	std::string pattern, file_name;
	uint32_t flags = 0;
	parseArgs(argc, argv, flags, pattern, file_name);
	std::ifstream file(file_name);
	if (!file) {
		printError("Error opening file");
		return 1;	
	}

	return 0;
}

int parseArgs(int argc, char*  argv[], uint32_t& flags, std::string& pattern, std::string& file_name){

	std::map<char, int> flag_map = {
		{'i', FLAG_i}, {'v', FLAG_v}, {'c', FLAG_c},
	};
	bool pattern_found = false;
	std::string arg;
	for (int i = 1; i < argc; i++){
		arg = argv[i];
		if (arg[0] == '-') {
			for (int ii = 1; ii < arg.size(); ii++){
				char flag_char = arg[ii];
				if (flag_map.find(flag_char) != flag_map.end()){
					flags |= flag_map[flag_char];
				} else {
					std::cerr << "Unknown flag: -" << flag_char << "\r\n";
				}
			}
		} else if (!pattern_found) {
			pattern = arg;
			pattern_found = true;
		} else {
			file_name = arg;
		}
	}
	return 1;
}


void findPattern(uint32_t flags, std::string& pattern, std::ifstream file){
	int matches = 0;

	std::string line;
	std::regex regex_pattern(pattern);
	while (std::getline(file, line)){
		if (std::regex_search(line, regex_pattern)) {
			if (FLAG_c & flags){
				matches++;
			} else {
				std::cout <<line << "\r\n";
			}	
		}
	}

	if (FLAG_c & flags) {
		std::cout << matches << "\r\n";
	}
			


}


void printError(const std::string& message){
	std::cerr << message << "\r\n";
}
