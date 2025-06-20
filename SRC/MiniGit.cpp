#include "MiniGit.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

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
std::string MiniGitSystem::hashFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) return "";

    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string content = buffer.str();
    in.close();

    std::hash<std::string> hasher;
    return std::to_string(hasher(content));
}
void MiniGitSystem::writeBlob(const std::string& hash, const std::string& content) {
    std::string path = objectsDir + "/" + hash;

    if (!std::filesystem::exists(path)) {
        std::ofstream out(path);
        out << content;
        out.close();
    }
}
void MiniGitSystem::add(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cout << "File not found: " << filename << '\n';
        return;
    }

    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string content = buffer.str();
    in.close();

    std::string hash = hashFile(filename);
    writeBlob(hash, content);

    stagingArea.insert(filename);
    std::cout << "Staged " << filename << " with hash " << hash << '\n';
}