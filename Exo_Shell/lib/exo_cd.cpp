// exo_cd.cpp
#include <iostream>
#include <unistd.h> // for chdir()

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: cd <directory>" << std::endl;
        return 1;
    }

    if (chdir(argv[1]) == 0) {
        return 0; // Successfully changed directory
    } else {
        std::cerr << "Error: Unable to change directory to " << argv[1] << std::endl;
        return 1;
    }
}
