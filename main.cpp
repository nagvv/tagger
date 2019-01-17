#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <set>

namespace fs = std::filesystem;

using fs::path;
using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::set;
using std::string;

path currentDir;

std::set<path> existFiles;
std::set<path> newFiles;
std::set<path> removedFiles;

void scanForNewFiles()
{
    std::set<path> allFiles;
    for (const auto &entry : fs::directory_iterator(currentDir))
        allFiles.insert( fs::canonical( entry.path() ) );

    for ( const auto &path : allFiles )
        if ( existFiles.find(path) == existFiles.end() )
            newFiles.insert(path);

    if (newFiles.empty())
        cout << "No new files found. ";
    else
        cout << "new files: " << endl;
    for ( const auto &path : newFiles )
        cout << "\t" << path << endl;

    for ( const auto &path : existFiles )
        if ( allFiles.find(path) == allFiles.end() )
            removedFiles.insert(path);

    if (removedFiles.empty())
        cout << "No files removed." << endl;
    else
        cout << "removed files: " << endl;
    for ( const auto &path : removedFiles )
        cout << "\t" << path << endl;
}

int getChoice()
{
    cout << "Actions:\n"
            "\t0) Exit. Not tagged files won't be added to database\n"
            "\t1) Tag new files\n"
            "\t2) Edit tags.\n"
            "\t3) Manage tags.\n"
            "\t4) do not choose this.\n" << endl;
    int c;
    auto getStr = []()
    {
        string in;
        cin >> in;
        return std::stoi(in);
    };
    while(true)
    {
        try
        {
            c = getStr();
        }
        catch (std::exception &e)
        {
            cout << "Couldn't interpret the input. Try again." << endl;
            continue;
        }
        break;
    }
    return c;
}

int main() {
    currentDir = fs::current_path();//TODO: add try-catch or pass error_code?
    //existFiles - load from DB
    scanForNewFiles();
    while ( int c = getChoice() )
    {
        cout << "choo-choo" << endl;
    }
    return 0;
}