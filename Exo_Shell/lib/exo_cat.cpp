// exo_cat.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

// Bitwise flags for options
#define FLAG_n 0x01 // Display line numbers
#define FLAG_e 0x02 // Show end of line character
#define FLAG_A 0x04 // display non printable
#define FLAG_h 0x08 // Show help message
#define FLAG_s 0x10 // Squeeze blank lines
#define FLAG_T 0x20 // Display tabs as ^I
#define FLAG_b 0x40 // Number non-empty lines only
#define FLAG_v 0x80 // Show non-printable characters in octal
#define FLAG_I 0x100 // Ignore lines with a pattern
#define FLAG_V 0x200 // Verbose mode





//Function prototypes
uint32_t parseFlags(int argc, char* argv[],std::vector<std::string>& files,std::string& pattern);
void display_help();
void print_line(const std::string& line, int flags);
void printError(const std::string& message, const std::string& detail = "");
int processFiles(std::vector<std::string>& files, uint32_t flags, std::string& pattern);




void display_help() {
    std::cout << "Usage: exo_cat [options] <file>...\r\n"
              << "Options:\r\n"
              << "  -n         Display line numbers\r\n"
              << "  -b         Number non-empty lines only\r\n"
              << "  -e         Show $ at the end of each line\r\n"
              << "  -A         Display non-printable characters\r\n"
              << "  -s         Squeeze multiple blank lines\r\n"
              << "  -T         Display tabs as ^I\r\n"
              << "  -v         Show non-printable characters in octal\r\n"
              << "  -V         Verbose output\r\n"
              << "  -I <pattern> Ignore lines containing the specified pattern\r\n"
              << "  -h         Show this help message\r\n";
}


void print_line(const std::string& line, int flags) {
    for (char ch : line) {
        if (((flags & FLAG_A) || (flags & FLAG_v)) && !isprint(ch)) { 
            // Flag_A takes precedence
            (flags & FLAG_A) ?
            std::cout << "^" << (char)(ch + 64): // Display control character notation
            std:: cout << "\\" << std::oct << static_cast<int>(ch); // Display in octal
        } else if ((flags & FLAG_T) && ch == '\t') {
            std::cout << "^I";
        } else {
            std::cout << ch;
        }
    }
    if (flags & FLAG_e) std::cout << "$";
    std::cout << "\r\n";
}

uint32_t parseFlags(int argc, char* argv[], std::vector<std::string>& files, std::string& pattern) {
    std::map<char, int> flag_map = {
        {'n', FLAG_n}, {'A', FLAG_A}, {'h', FLAG_h}, {'e', FLAG_e},
        {'s', FLAG_s}, {'T', FLAG_T}, {'b', FLAG_b}, {'v', FLAG_v},
        {'I', FLAG_I}, {'V', FLAG_V}
    };


    uint32_t flags = 0;
    bool patternFlag = false;
    int errCount = 0;

    // Parse flags and any patterns for `-I`
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg[0] == '-') {
            for (size_t j = 1; j < arg.size(); ++j) {
                char flag_char = arg[j];
                if (flag_map.find(flag_char) != flag_map.end()) {
                    flags |= flag_map[flag_char];
                } else {
                    printError("Unknown flag encountered: -", std::to_string(flag_char));
                    errCount++;
                }
            }
            if (!patternFlag && (flags & FLAG_I) && i + 1 < argc){
                patternFlag = true;
                ++i;
                pattern = argv[i];
            }
        } else {
            files.push_back(arg);
        }
        
    }

    if (errCount>0) printError("Unknown flags encountered: ", std::to_string(errCount));

    return flags;
}

int processFiles(std::vector<std::string>& files, uint32_t flags, std::string& pattern) {
    // Process each file
    for (const auto& file_name : files) {
        std::ifstream file(file_name);
        if (!file) {
            printError("Could not open file:", file_name);
            continue;
        }
        if (flags & FLAG_V) {
            std::cout << "Processing file: " << file_name << "\r\n";
        }
        std::string line;
        int line_number = 1;
        bool isPrevLineBlank = false;
        while (std::getline(file, line)) {
            
            if (flags & FLAG_s && line.empty() && isPrevLineBlank) continue;
            isPrevLineBlank = line.empty();
            
            if ((flags & FLAG_I) && line.find(pattern) != std::string::npos) {
                continue;
            }

            if ((flags & FLAG_b) && !line.empty()) {
                std::cout << line_number++ << ": ";
            } else if (flags & FLAG_n) {
                std::cout << line_number++ << ": ";
            }

            print_line(line, flags);
        }

        file.close();
    }
    return 0;
}

int main(int argc, char* argv[]) {
    uint32_t flags = 0;
    std::vector<std::string> files;
    std::string pattern;
    // Parse arguments and set flags
    flags = parseFlags(argc, argv, files, pattern);

    // If help flag is set, display help and exit
    if (flags & FLAG_h) {
        display_help();
        return 0;
    }

    if (files.empty()) {
        std::cerr << "Error: No file specified.\n";
        display_help();
        return 1;
    }

    return processFiles(files, flags, pattern);


}
// Function to print error messages
void printError(const std::string& message, const std::string& detail) {
    std::cerr << message;
    if (!detail.empty()) std::cerr << " " << detail;
    std::cerr << "\r\n";
}
