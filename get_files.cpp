#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include <cstring>

using namespace std;

// Get all files starting from start_dir, including symlinks, permission-aware
vector<string> get_all_files(const string& start_dir) {
    vector<string> all_files;
    stack<string> dirs;
    dirs.push(start_dir);

    while (!dirs.empty()) {
        string current_dir = dirs.top();
        dirs.pop();

        DIR* dir = opendir(current_dir.c_str());
        if (!dir) {
            // Skip directories we cannot open due to permissions
            //cerr << "[!] Cannot open directory: " << current_dir << " (" << strerror(errno) << ")\n";
            continue;
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            string name = entry->d_name;

            if (name == "." || name == "..") continue;

            string fullpath = current_dir;
            if (fullpath.back() != '/') fullpath += '/';
            fullpath += name;

            struct stat st;
            if (lstat(fullpath.c_str(), &st) != 0) {
                //cerr << "[!] Cannot stat file: " << fullpath << " (" << strerror(errno) << ")\n";
                continue;
            }

            if (S_ISDIR(st.st_mode)) {
                // Directory: push to stack to traverse later
                dirs.push(fullpath);
            } else {
                // Regular file or symlink: include in output
                all_files.push_back(fullpath);
            }
        }

        closedir(dir);
    }

    return all_files;
}


