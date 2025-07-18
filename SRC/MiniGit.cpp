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
    std::string branchPath = referencesDir + "/" + branchName;

    if (!std::filesystem::exists(branchPath))
    {
        std::cout << "Branch '" << branchName << "' does not exist.\n";
        return;
    }

    std::ifstream in(branchPath);
    std::string targetHash;
    std::getline(in, targetHash);
    in.close();

    if (targetHash.empty())
    {
        std::cout << "Branch is empty (no commits yet). HEAD updated.\n";
        head = nullptr;
        writeHEAD("references/" + branchName);
        return;
    }

    Commit *current = head;
    while (current != nullptr && current->hash != targetHash)
    {
        current = current->parent;
    }

    if (current == nullptr)
    {
        std::cout << "Commit not found in current history. You may need to rebuild full commit tree in future.\n";
        return;
    }

    head = current;
    writeHEAD("references/" + branchName);
    std::cout << "Checked out branch '" << branchName << "' (commit: " << targetHash << ")\n";
}
void MiniGitSystem::merge(const std::string &branchName)
{
    std::string branchPath = referencesDir + "/" + branchName;

    if (!std::filesystem::exists(branchPath))
    {
        std::cout << "Branch '" << branchName << "' does not exist.\n";
        return;
    }

    std::ifstream in(branchPath);
    std::string targetHash;
    std::getline(in, targetHash);
    in.close();

    if (targetHash.empty())
    {
        std::cout << "Branch has no commits.\n";
        return;
    }

    Commit *current = head;
    Commit *targetCommit = nullptr;

    while (current != nullptr)
    {
        if (current->hash == targetHash)
        {
            std::cout << "Already up to date with '" << branchName << "'.\n";
            return;
        }
        current = current->parent;
    }

    current = head;

    Commit *scan = head;
    while (scan != nullptr)
    {
        if (scan->hash == targetHash)
        {
            targetCommit = scan;
            break;
        }
        scan = scan->parent;
    }

    if (targetCommit == nullptr)
    {
        std::cout << "Merging in commit " << targetHash << " from branch '" << branchName << "'...\n";

        Commit *newCommit = new Commit;
        newCommit->parent = head;
        newCommit->message = "Merge from branch " + branchName;
        newCommit->timestamp = getCurrentTime();
        newCommit->hash = generateCommitHash(newCommit->message, newCommit->timestamp);

        if (head)
        {
            for (const auto &b : head->blobs)
            {
                newCommit->blobs.push_back(b);
            }
        }

        std::ifstream inCommit(".minigit/commit_" + targetHash + ".txt");
        std::string line;
        while (std::getline(inCommit, line))
        {
            if (line.rfind("file: ", 0) == 0)
            {
                std::istringstream iss(line);
                std::string label, filename, hashLabel, filehash;
                iss >> label >> filename >> hashLabel >> filehash;

                bool alreadyExists = false;
                for (const auto &b : newCommit->blobs)
                {
                    if (b.filename == filename && b.hash == filehash)
                    {
                        alreadyExists = true;
                        break;
                    }
                }

                if (!alreadyExists)
                {
                    newCommit->blobs.push_back({filename, filehash});
                }
            }
        }
        inCommit.close();

        // Save merged commit
        std::ofstream out(".minigit/commit_" + newCommit->hash + ".txt");
        out << "commit: " << newCommit->hash << "\n";
        out << "date: " << newCommit->timestamp << "\n";
        out << "message: " << newCommit->message << "\n";
        for (const auto &blob : newCommit->blobs)
        {
            out << "file: " << blob.filename << " hash: " << blob.hash << "\n";
        }
        out.close();

        head = newCommit;
        std::cout << "Merge commit created: " << newCommit->hash << "\n";
    }
    else
    {
        std::cout << "Target commit already in current history.\n";
    }
}
void MiniGitSystem::diff(const std::string &filename)
{
    if (!head)
    {
        std::cout << "No commits yet.\n";
        return;
    }

    std::string oldHash;
    for (const auto &blob : head->blobs)
    {
        if (blob.filename == filename)
        {
            oldHash = blob.hash;
            break;
        }
    }

    if (oldHash.empty())
    {
        std::cout << "File not tracked in last commit.\n";
        return;
    }

    std::ifstream oldFile(objectsDir + "/" + oldHash);
    std::ifstream newFile(filename);

    if (!oldFile || !newFile)
    {
        std::cout << "Could not open files for comparison.\n";
        return;
    }

    std::string oldLine, newLine;
    int lineNum = 1;
    bool changes = false;

    std::cout << "\n Diff for " << filename << ":\n";

    while (std::getline(oldFile, oldLine) && std::getline(newFile, newLine))
    {
        if (oldLine != newLine)
        {
            std::cout << "Line " << lineNum << " changed:\n";
            std::cout << "  - committed: " << oldLine << "\n";
            std::cout << "  + current:   " << newLine << "\n";
            changes = true;
        }
        lineNum++;
    }

    while (std::getline(oldFile, oldLine))
    {
        std::cout << "Line " << lineNum << " removed:\n";
        std::cout << "  - committed: " << oldLine << "\n";
        lineNum++;
        changes = true;
    }

    while (std::getline(newFile, newLine))
    {
        std::cout << "Line " << lineNum << " added:\n";
        std::cout << "  + current:   " << newLine << "\n";
        lineNum++;
        changes = true;
    }

    if (!changes)
    {
        std::cout << "✅ No differences.\n";
    }

    oldFile.close();
    newFile.close();
}
