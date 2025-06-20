MiniGit

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
