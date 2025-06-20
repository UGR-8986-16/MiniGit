MiniGit

Prepared by:
Robel Alemayehu ----- UGR/1760/16
Ephrata Yohannes -----UGR/7391/16
Nahom Behailu ------ UGR/8986/16
Date: June 2025

MiniGit is a lightweight version control system built in C++ that mimics core features of Git. It allows you to initialize a repository, track file changes, commit updates, create branches, checkout previous commits or branches, and merge changes — all through a simple command-line interface.
MiniGit was built as a learning project to understand how Git works internally, including blob storage, commit history, branching, and merge mechanics.

Features

git init → Initialize a new .minigit repository
git add → Stage files for the next commit
git commit -m "message" → Commit changes with a message
git log → View commit history
git branch <name> → Create a new branch
git checkout <branch> → Switch to another branch
git merge <branch> → Merge another branch into the current branch
git diff <file> → Show line-by-line differences from the last commit

Requirements

C++17 or higher
g++ or a compatible compiler

1. Overview

MiniGit is a simplified, command-line based version control system implemented in C++. It is designed as an educational clone of Git that teaches students the core concepts behind source code management systems.

MiniGit supports essential Git-like commands including:

init – initializes the repository
add – stages files
commit – saves a snapshot of staged files
log – displays commit history
branch – creates new branches
checkout – switches between branches or commits
merge – merges another branch into the current one
diff – compares the working version of a file with the last committed version

All functionality is implemented using C++ standard libraries including , <unordered_set>, , and . Data is stored directly to disk in a hidden directory named .minigit.

2. Data Structures Used

2.1. Blob

struct Blob {
std::string filename;
std::string hash;
};
Represents a snapshot of a file.
Each file in a commit is stored as a Blob.
The blob content is saved in .minigit/objects/<hash>.

2.2. Commit

struct Commit {
std::string message;
std::string timestamp;
std::string hash;
std::vector<Blob> blobs;
Commit\* parent = nullptr;
};

Stores metadata and snapshot of a project at a certain point.
Contains references to all blobs (files) and a parent commit.
Commits are connected via singly linked list using the parent pointer.

2.3. Other Fields

Commit\* head;
std::unordered_set<std::string> stagingArea;
head points to the current (latest) commit.
stagingArea holds filenames of files added but not yet committed.

3. Design Decisions

3.1. File-based Object Store
MiniGit saves all objects and metadata as plain text files:
Blobs: .minigit/objects/<hash>
Commits: .minigit/commit\_<hash>.txt
Branches: .minigit/references/<branch>
HEAD: .minigit/HEAD
This design mimics Git's internal structure in a simplified form.

3.2. Hashing

Uses std::hash<std::string> to generate a unique hash for file contents and commit identity.
This is sufficient for learning purposes, but not cryptographically secure.

3.3. Simplicity and Transparency

All project logic is in a single class (MiniGitSystem) for simplicity.
File structure is visible and easily explorable for debugging and educational use.

3.4. Command-Line Interface

Main program (main.cpp) acts as a basic REPL loop.
Users type commands (init, add file.txt, etc.) and the system responds accordingly.

4. Limitations

4.1. No Persistent Commit Chain
Commits are saved to disk, but the in-memory commit history is not restored when the program restarts.
checkout only works on commits present in the current session.

4.2. No Conflict Detection in Merge

Merge is a simple blob union.
No file comparison or conflict resolution is implemented.

4.3. File Restoration and Workspace Sync

checkout only updates HEAD, not actual working files.
There is no mechanism to restore files to match a specific commit.

4.4. Limited Diff Functionality

Diff shows added/removed/changed lines, but lacks visual formatting, line numbers, or side-by-side views.

4.5. No Deletion Tracking

Deleting files is not supported.
There is no equivalent to git rm or tracking removed files.

5. Future Improvements

5.1. Persistent Commit History

Parse .minigit/commit\_<hash>.txt files to reconstruct commit history on startup.

5.2. Visual and Color Diff

Add side-by-side views, word-level diffs, and color-coded differences.

5.3. Merge Conflict Detection

Detect and report conflicts if the same file is changed in two branches.

Add auto-resolution strategies or manual prompts.

5.4. Workspace Management

Implement checkout <branch> to automatically overwrite working directory files.

Add a reset or revert command.

5.5. Additional Features

status: show staged files, current branch, and HEAD info.
restore <file>: pull file from specific commit.
delete-branch: remove unused branches.
Commit messages stored with JSON or YAML metadata for better structure.

6. Conclusion

MiniGit demonstrates the foundational concepts behind Git:
snapshots
branching
merging
content tracking via hashing

It is implemented with clean, modular C++ code and a simple CLI interface. While not intended for production use, it is ideal for learning how version control systems manage data behind the scenes.

This project can be extended into a more advanced educational tool or evolved into a full-featured CLI app with persistent storage, workspace restoration, and GitHub API integration.
