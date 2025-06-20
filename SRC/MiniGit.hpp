#ifndef MINIGIT_HPP
#define MINIGIT_HPP
#include <unordered_set>
#include <string>

class MiniGitSystem {
public:
    MiniGitSystem();
    void init();
    void add(const std::string& filename);

private:
    std::string minigitDir = ".minigit";
    std::string objectsDir = ".minigit/objects";
    std::string referencesDir = ".minigit/references";
    std::unordered_set<std::string> stagingArea;
    void createDirectory(const std::string& path);
    void writeHEAD(const std::string& branch);
    std::string hashFile(const std::string& filename); 
    void writeBlob(const std::string& hash, const std::string& content);
};

#endif
