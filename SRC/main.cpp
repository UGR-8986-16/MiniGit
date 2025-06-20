#include "MiniGit.hpp"
#include <iostream>

int main() {
    MiniGitSystem mg;
    std::string command;

    std::cout << "Welcome to MiniGit!\n";
    std::cout << "Type 'init' to initialize the repository.\n";

    while (true) {
        std::cout << "\nEnter a command: ";
        std::cin >> command;

        if (command == "init") {
            mg.init();
        } else if (command == "exit") {
            break;
        } else {
            std::cout << "Unknown command. Try 'init' or 'exit'.\n";
        }
    }

    return 0;
}