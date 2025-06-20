#include "MiniGit.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>

namespace fs = std::filesystem;

MiniGitSystem::MiniGitSystem()
{
    // Constructor placeholder
}

void MiniGitSystem::createDirectory(const std::string &path)
{
    if (!fs::exists(path))
    {
        fs::create_directory(path);
        std::cout << "Created: " << path << '\n';
    }
}

void MiniGitSystem::writeHEAD(const std::string &branch)
{
    std::ofstream headFile(".minigit/HEAD");
    if (headFile.is_open())
    {
        headFile << "ref: " << branch;
        headFile.close();
    }
}

void MiniGitSystem::init()
{
    createDirectory(minigitDir);
    createDirectory(objectsDir);
    createDirectory(referencesDir);

    std::ofstream masterFile(referencesDir + "/master");
    if (masterFile.is_open())
    {
        masterFile << ""; // No commit yet
        masterFile.close();
    }

    writeHEAD("references/master");

    std::cout << "MiniGit repository initialized.\n";
}

void MiniGitSystem::log() const
{
    if (!head)
    {
        std::cout << "No commits yet.\n";
        return;
    }

    const Commit *current = head;
    while (current)
    {
        std::cout << "Commit: " << current->hash << '\n';
        std::cout << "Date:   " << current->timestamp << '\n';
        std::cout << "Message: " << current->message << "\n\n";
        current = current->parent;
    }
}

std::string MiniGitSystem::hashFile(const std::string &filename)
{
    std::ifstream in(filename);
    if (!in.is_open())
        return "";

    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string content = buffer.str();
    in.close();

    std::hash<std::string> hasher;
    return std::to_string(hasher(content));
}
void MiniGitSystem::writeBlob(const std::string &hash, const std::string &content)
{
    std::string path = objectsDir + "/" + hash;

    if (!std::filesystem::exists(path))
    {
        std::ofstream out(path);
        out << content;
        out.close();
    }
}
void MiniGitSystem::add(const std::string &filename)
{
    std::ifstream in(filename);
    if (!in.is_open())
    {
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

std::string MiniGitSystem::getCurrentTime()
{
    std::time_t now = std::time(nullptr);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

std::string MiniGitSystem::generateCommitHash(const std::string &message, const std::string &time)
{
    std::hash<std::string> hasher;
    return std::to_string(hasher(message + time));
}

void MiniGitSystem::commit(const std::string &message)
{
    if (stagingArea.empty())
    {
        std::cout << "Nothing to commit.\n";
        return;
    }

    std::string time = getCurrentTime();
    std::string commitHash = generateCommitHash(message, time);

    Commit *newCommit = new Commit;
    newCommit->message = message;
    newCommit->timestamp = time;
    newCommit->hash = commitHash;
    newCommit->parent = head;

    for (const auto &filename : stagingArea)
    {
        Blob blob;
        blob.filename = filename;
        blob.hash = hashFile(filename);
        newCommit->blobs.push_back(blob);
    }

    head = newCommit;

    std::ofstream out(".minigit/commit_" + commitHash + ".txt");
    out << "commit: " << commitHash << "\n";
    out << "date: " << time << "\n";
    out << "message: " << message << "\n";
    for (const auto &blob : newCommit->blobs)
    {
        out << "file: " << blob.filename << " hash: " << blob.hash << "\n";
    }
    out.close();

    stagingArea.clear();

    std::cout << " Commit created: " << commitHash << '\n';
}

void MiniGitSystem::branch(const std::string &branchName)
{
    if (!head)
    {
        std::cout << "No commits yet. You must commit before creating a branch.\n";
        return;
    }

    std::string path = referencesDir + "/" + branchName;

    // Check if the branch already exists
    if (std::filesystem::exists(path))
    {
        std::cout << "Branch '" << branchName << "' already exists.\n";
        return;
    }

    // Create a file for the new branch and point it to the current commit
    std::ofstream out(path);
    if (out.is_open())
    {
        out << head->hash;
        out.close();
        std::cout << "Branch '" << branchName << "' created at commit " << head->hash << "\n";
    }
    else
    {
        std::cout << "Failed to create branch file.\n";
    }
}
void MiniGitSystem::checkout(const std::string &branchName)
{
    if (branches.find(branchName) != branches.end())
    {
        currentBranch = branchName;
        currentCommit = branches[branchName];
        std::cout << "Switched to branch '" << branchName << "'\n";

        // restore files from currentCommit
    }
    else
    {
        std::cout << "Error: branch '" << branchName << "' does not exist.\n";
    }
}
