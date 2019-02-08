#ifndef NTAGGER_COMMON_H
#define NTAGGER_COMMON_H

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <set>
#include "sqlite3.h"

namespace fs = std::filesystem;

using fs::path;
using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::set;
using std::string;

extern sqlite3 *db;
extern const char *dbFile;
extern const char *resultFolder;

extern path currentDir;

extern set<path> existFiles;
extern set<path> newFiles;
extern set<path> removedFiles;

#endif //NTAGGER_COMMON_H
