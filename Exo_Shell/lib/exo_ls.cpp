#include <iostream>
#include <map>
#include <string>
#include <cstdint>
#include <filesystem>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <sys/stat.h>

// Define each flag as a unique bit position
#define FLAG_L 0x01 // Detailed listing
#define FLAG_A 0x02 // Show hidden files
#define FLAG_H 0x04 // Human-readable sizes
#define FLAG_D 0x08 // List directories themselves, not their contents
#define FLAG_R 0x10 // Recursive listing
#define FLAG_r 0x20 // Reverse the order
#define FLAG_P 0x40 // Append '/' to directories
#define FLAG_N 0x80 // Numeric IDs for user/group
#define FLAG_M 0x100 // Separate entries with commas
#define FLAG_I 0x200 // Ignore files matching a pattern
#define FLAG_T 0x400 // Sort by modification time

// Function prototypes
uint32_t parseFlags(int argc, char* argv[], std::string& ignore_pattern);
void listDirectory(const std::filesystem::path& path, uint32_t flags, const std::string& ignore_pattern);
void handleFileEntry(const std::filesystem::directory_entry& entry, uint32_t flags,const std::string& ignore_pattern);

int main(int argc, char* argv[]) {
    uint32_t flags = 0;
    std::string ignore_pattern;

    // Parse command line arguments for flags
    flags = parseFlags(argc, argv, ignore_pattern);

    // Start listing the current directory
    listDirectory(".", flags, ignore_pattern);

    return 0;
}

uint32_t parseFlags(int argc, char* argv[], std::string& ignore_pattern) {
    std::map<char, int> flag_map = {
        {'l', FLAG_L}, {'a', FLAG_A}, {'h', FLAG_H}, {'d', FLAG_D},
        {'R', FLAG_R}, {'r', FLAG_r}, {'p', FLAG_P}, {'n', FLAG_N},
        {'m', FLAG_M}, {'I', FLAG_I}, {'t', FLAG_T}
    };

    uint32_t flags = 0;

    // Parse flags and any patterns for `-I`
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg[0] == '-') {
            for (size_t j = 1; j < arg.size(); ++j) {
                char flag_char = arg[j];
                if (flag_map.find(flag_char) != flag_map.end()) {
                    flags |= flag_map[flag_char];
                } else {
                    std::cerr << "Unknown flag: -" << flag_char << std::endl;
                }
            }
        } else if (flags & FLAG_I) {
            ignore_pattern = arg; // Capture ignore pattern for `-I`
        }
    }

    return flags;
}

void listDirectory(const std::filesystem::path& path, uint32_t flags, const std::string& ignore_pattern) {
    std::vector<std::filesystem::directory_entry> entries;

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        std::string filename = entry.path().filename().string();

        if ((flags & FLAG_I) && filename.find(ignore_pattern) != std::string::npos) {
            continue; // Skip ignored files
        }

        if (!(flags & FLAG_A) && filename[0] == '.') {
            continue; // Skip hidden files unless `-a` is set
        }

        entries.push_back(entry);
    }

    // Sort entries by time if `-t` flag is set, else by name
    if (flags & FLAG_T) {
        std::sort(entries.begin(), entries.end(),
                  [](const auto& a, const auto& b) {
                      return std::filesystem::last_write_time(a) > std::filesystem::last_write_time(b);
                  });
    } else {
        std::sort(entries.begin(), entries.end(),
                  [](const auto& a, const auto& b) {
                      return a.path().filename() < b.path().filename();
                  });
    }

    if (flags & FLAG_r) {
        std::reverse(entries.begin(), entries.end()); // Reverse order if `-r` is set
    }

    // Display directory entries with applied flags
    for (const auto& entry : entries) {
        handleFileEntry(entry, flags, ignore_pattern);
    }

    // Recursive listing if `-R` is set
    if (flags & FLAG_R) {
        for (const auto& entry : entries) {
            if (entry.is_directory()) {
                std::cout << "\n" << entry.path().string() << ":\n";
                listDirectory(entry.path(), flags, ignore_pattern); // Recursively call on subdirectory
            }
        }
    }
}

void handleFileEntry(const std::filesystem::directory_entry& entry, uint32_t flags, const std::string& ignore_pattern) {
    std::string filename = entry.path().filename().string();

    if (flags & FLAG_D && entry.is_directory()) {
        std::cout << filename << "\r"; // Show only directory name with carriage return
        return;
    }

    if (flags & FLAG_L) {
        struct stat file_stat;
        stat(entry.path().c_str(), &file_stat);
        
        std::cout << ((S_ISDIR(file_stat.st_mode)) ? 'd' : '-') 
                  << ((file_stat.st_mode & S_IRUSR) ? 'r' : '-')
                  << ((file_stat.st_mode & S_IWUSR) ? 'w' : '-')
                  << ((file_stat.st_mode & S_IXUSR) ? 'x' : '-') << " ";

        if (flags & FLAG_N) {
            std::cout << file_stat.st_uid << " " << file_stat.st_gid << " ";
        }

        if (flags & FLAG_H) {
            double size = file_stat.st_size;
            std::string size_unit = "B";
            if (size >= 1024) { size /= 1024; size_unit = "KB"; }
            if (size >= 1024) { size /= 1024; size_unit = "MB"; }
            std::cout << std::fixed << std::setprecision(2) << size << size_unit;
        } else {
            std::cout << file_stat.st_size;
        }
        std::cout << " " << filename;
    } else {
        std::cout << filename;
    }

    if ((flags & FLAG_P) && entry.is_directory()) {
        std::cout << "/"; // Append '/' if `-p` is set and entry is a directory
    }

    std::cout << ((flags & FLAG_M) ? ", " : "\r\n"); // Use carriage return or newline based on `-m` flag
    // Recursive listing if `-R` is set and the entry is a directory
    if (flags & FLAG_R && entry.is_directory()) {
        std::cout << "\n" << entry.path().string() << ":\n";
        listDirectory(entry.path(), flags, ignore_pattern); // Recursively call on subdirectory
    }

}