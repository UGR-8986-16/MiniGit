#ifndef MINIGIT_HPP
#define MINIGIT_HPP
#include <unordered_set>
#include <string>
#include <vector>
#include <ctime>
#include <unordered_map>

class MiniGitSystem
{
    struct Blob
    {
        std::string filename;
        std::string hash;
    };

    struct Commit
    {
        std::string message;
        std::string timestamp;
        std::string hash;
        std::vector<Blob> blobs;
        Commit *parent = nullptr;
    };
    Commit *head = nullptr;

public:
    MiniGitSystem();
    void init();
    void add(const std::string &filename);
    void commit(const std::string &message);
    void log() const;
    void branch(const std::string &branchName);
    void checkout(const std::string &branchName);

private:
    std::string minigitDir = ".minigit";
    std::string objectsDir = ".minigit/objects";
    std::string referencesDir = ".minigit/references";
    std::unordered_set<std::string> stagingArea;

    std::string getCurrentTime();
    std::string generateCommitHash(const std::string &message, const std::string &time);
    std::string hashFile(const std::string &filename);
    void createDirectory(const std::string &path);
    void writeHEAD(const std::string &branch);
    void writeBlob(const std::string &hash, const std::string &content);
};

#endif
