#ifndef MINIGIT_HPP
#define MINIGIT_HPP

#include <string>

class MiniGitSystem {
public:
    MiniGitSystem();
    void init();

private:
    std::string minigitDir = ".minigit";
    std::string objectsDir = ".minigit/objects";
    std::string referencesDir = ".minigit/references";

    void createDirectory(const std::string& path);
    void writeHEAD(const std::string& branch);
};

#endif
