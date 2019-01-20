#include "common.h"
#include "tagger_db.h"

sqlite3 *db = nullptr;

path currentDir;

set<path> existFiles;
set<path> newFiles;
set<path> removedFiles;

const char* CREATE_TABLES = "create table if not exists tags(id integer primary key, name text not null, unique(name));"
                            "create table if not exists files(id integer primary key, path text not null, unique(path));"
                            "create table if not exists rels(fid integer not null, tid integer not null, unique(fid, tid));";

void scanForNewFiles()
{
    std::set<path> allFiles;
    for (const auto &entry : fs::directory_iterator(currentDir))
        allFiles.insert( fs::canonical( entry.path() ) ); //TODO: add ignoring db file

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
            "\t4) Search.\n" << endl;
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

//removes spaces in start and end, and replaces multiple space with one
string clearTag( string dirty )
{
    string ret;
    bool ws = true;
    for ( auto &c : dirty )
    {
        if( std::isspace(c) )
        {
            if( ws )
                continue;
            else
                ws = true;
        }
        else
        {
            ws = false;
            ret.push_back(c);
        }
    }
    if( std::isspace( ret.back() ) )
        ret.pop_back();
    return ret;
}

void tagNewFiles()
{
    cin.clear(); cin.ignore();
    for ( const auto &file : newFiles )
    {
        string in; set<string> tags;
        cout << "Set tags for file: " << file.filename() << " separated by commas. Enter a empty line to stop tagging." << endl;
        std::getline(cin, in);
        if ( in.empty() )
        {
            cout << "Stop." << endl;
            break;
        }
        std::stringstream ss(in);
        while ( ss.good() )
        {
            string tag;
            getline( ss, tag, ',' );
            tag = clearTag(tag);
            if ( !tag.empty() )
                tags.insert(tag);
        }
        if ( addFile(file) )
            break;
        existFiles.insert(file);
        newFiles.erase(file);
        for ( auto &tag: tags )
            insertTag(file, tag);
    }
}

//TODO: add russian letters support
int main() {
    currentDir = fs::current_path();//TODO: add try-catch or pass error_code?
    char *err = nullptr;
    if ( sqlite3_open("tags.db", &db) )
    {
        cout << "Couldn't open/create database: " << sqlite3_errmsg(db) << endl;
        return 1;
    }
    if ( sqlite3_exec(db, CREATE_TABLES, nullptr, nullptr, &err) )
    {
        cout << "SQL Error: " << err << endl;
        sqlite3_free(err);
        return 1;
    }
    loadExistFiles();

    scanForNewFiles();
    while ( int c = getChoice() )
    {
        switch(c)
        {
            case 1:
                tagNewFiles();
                break;
            default:
                cout << "Wrong number." <<endl;
        }
    }

    sqlite3_close(db);
    return 0;
}