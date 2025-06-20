#include "MiniGit.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

MiniGitSystem::MiniGitSystem() {
    // Constructor placeholder
}

void MiniGitSystem::createDirectory(const std::string& path) {
    if (!fs::exists(path)) {
        fs::create_directory(path);
        std::cout << "Created: " << path << '\n';
    }
}

void MiniGitSystem::writeHEAD(const std::string& branch) {
    std::ofstream headFile(".minigit/HEAD");
    if (headFile.is_open()) {
        headFile << "ref: " << branch;
        headFile.close();
    }
}

void MiniGitSystem::init() {
    createDirectory(minigitDir);
    createDirectory(objectsDir);
    createDirectory(referencesDir);

    std::ofstream masterFile(referencesDir + "/master");
    if (masterFile.is_open()) {
        masterFile << ""; // No commit yet
        masterFile.close();
    }

    writeHEAD("references/master");

    std::cout << "MiniGit repository initialized.\n";
}
