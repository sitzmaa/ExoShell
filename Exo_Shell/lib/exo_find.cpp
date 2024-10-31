#include <iostream>
#include <filesystem>
#include <string>
#include <map>

#define FLAG_name 0x01
#define FLAG_type 0x02
#define FLAG_filter 0x04
#define FLAG_sort 0x08

#define FILTER_exclude 0x01 // whether the filter is inclusive or exclusive (inclusive by default)
#define FILTER_created 0x02 // filter relates to date created
#define FILTER_modified 0x04 // filter relates to date modified
#define FILTER_type 0x08

#define SORT_dec 0x01
#define SORT_created 0x02
#define SORT_modified 0x04
#define SORT_type 0x08

namespace fs = std:: filesystem;

// prototypes
void printError(const std::string& message);
int parseArgs(int argc, char*  argv[], uint32_t& flags, uint16_t& filter, uint16_t& sort, std::string& path,  std::string& name,  std::string& filter_param);


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: find <flags> <path>\n";
        return -1;
    }

    uint32_t flags = 0;
    uint16_t filter = 0, sort = 0;
    std::string path, name, filter_param;

    int parse_result = parseArgs(argc, argv, flags, filter, sort, path, name, filter_param);
    if (parse_result != 0) {
        return parse_result;  // Exit if parsing failed
    }

    // Debug output (remove or replace with actual functionality)
    std::cout << "Flags: " << flags << "\n";
    std::cout << "Filter: " << filter << ", Filter Parameter: " << filter_param << "\n";
    std::cout << "Sort: " << sort << "\n";
    std::cout << "Path: " << path << "\n";

    // Perform actions based on parsed flags, filters, and sort options
    // For example, if FLAG_sort is set, call sort logic here

    return 0;
}

int parseArgs(int argc, char* argv[], uint32_t& flags, uint16_t& filter, uint16_t& sort, std::string& path, std::string& name, std::string& filter_param) {
    // Flag and option mappings
    std::map<char, int> flag_map = {{'n', FLAG_name}, {'t', FLAG_type}, {'f', FLAG_filter}, {'s', FLAG_sort}};
    std::map<char, int> filter_map = {{'e', FILTER_exclude}, {'c', FILTER_created}, {'m', FILTER_modified}, {'t', FILTER_type}};
    std::map<char, int> sort_map = {{'d', SORT_dec}, {'c', SORT_created}, {'m', SORT_modified}, {'t', SORT_type}};

    bool sort_lock = false, filter_lock = false;
    bool sort_check = false, filter_check = false;
    int required_args = 3;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg[0] == '-') {  // Handling flags
            for (size_t j = 1; j < arg.size(); ++j) {
                char flag_char = arg[j];
                
                if (flag_map.count(flag_char)) {
                    flags |= flag_map[flag_char];
                } else {
                    std::cerr << "Unknown flag: -" << flag_char << "\n";
                    return -1;  // Exit on unknown flag
                }
            }
            
            // Handle special cases for sort and filter
            if ((flags & FLAG_sort) && !sort_lock) {
                sort_check = true;
                sort_lock = true;
                required_args += 1;
                if (argc < required_args) {
                    std::cerr << "Too few arguments for the sort flag. Usage: find -s <conditions> <directory>\n";
                    return -1;
                }
            }
            
            if ((flags & FLAG_filter) && !filter_lock) {
                filter_check = true;
                filter_lock = true;
                required_args += 2;
                if (argc < required_args) {
                    std::cerr << "Too few arguments for the filter flag. Usage: find -f <conditions> <parameters> <directory>\n";
                    return -1;
                }
            }
        } else if (filter_check) {  // Handling filter conditions
            for (char filter_char : arg) {
                if (filter_map.count(filter_char)) {
                    filter |= filter_map[filter_char];
                } else {
                    std::cerr << "Unknown filter condition: " << filter_char << "\n";
                }
            }
            
            // Filter parameters
            if (++i < argc) {
                filter_param = argv[i];
            } else {
                std::cerr << "Missing parameter for filter condition\n";
                return -1;
            }
            
            filter_check = false;  // Reset after handling
        } else if (sort_check) {  // Handling sort conditions
            for (char sort_char : arg) {
                if (sort_map.count(sort_char)) {
                    sort |= sort_map[sort_char];
                } else {
                    std::cerr << "Unknown sort condition: " << sort_char << "\n";
                }
            }
            sort_check = false;  // Reset after handling
        } else {
            path = arg;  // Final argument as path
        }
    }

    return 0;  // Success
}

void printError(const std::string& message){
	std::cout << message << "\r\n";
}
