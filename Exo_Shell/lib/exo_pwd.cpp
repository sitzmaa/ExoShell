// exo_pwd.cpp
#include <iostream>
#include <unistd.h> // for getcwd()
#include <limits.h> // for PATH_MAX

int main() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        std::cout << cwd << std::endl;
    } else {
        std::cerr << "Error: Unable to get current directory" << std::endl;
        return 1;
    }
    return 0;
}
