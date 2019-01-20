#include "common.h"

int execQuery(string query)
{
    //cout << query << endl;
    char *err = nullptr;
    if ( sqlite3_exec(db, query.c_str(), nullptr, nullptr, &err) )
    {
        cout << "SQL Error: " << err << endl;
        sqlite3_free(err);
        return 1;
    }
    return 0;
}

int loadExistFiles()
{
    char *err = nullptr;
    auto callback = []( void*, int num, char** resultArr, char** columnNameArr) -> int {
        for ( int i = 0; i < num; i++ )
        {
            existFiles.insert(resultArr[i]);
        }
        return 0;
    };
    if ( sqlite3_exec(db, "select path from files", callback, nullptr, &err) )
    {
        cout << "SQL Error: " << err << endl;
        sqlite3_free(err);
        return 1;
    }
    return 0;
}

int addFile(path file)
{
    string query = "insert into files (path) values (\"" + file.string() + "\")";
    return execQuery(query);
}

int insertTag(path file, string tag)
{
    string query = "insert or ignore into tags (name) values (\"" + tag + "\")";
    if ( execQuery(query) )
        return 1;
    query = "insert or ignore into rels values ((select (id) from files where path=\"" + file.string() + "\"),\n"
                                               "(select (id) from tags where name=\"" + tag + "\"));";
    return execQuery(query);
}