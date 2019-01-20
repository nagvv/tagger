#include "common.h"

int execQuery(string query, int (*callback)( void*, int, char**, char** ) = nullptr, void *_1stArg = nullptr)
{
    //cout << query << endl;
    char *err = nullptr;
    if ( sqlite3_exec(db, query.c_str(), callback, _1stArg, &err) )
    {
        cout << "SQL Error: " << err << endl;
        sqlite3_free(err);
        return 1;
    }
    return 0;
}

int loadExistFiles()
{
    auto callback = []( void*, int num, char** resultArr, char** columnNameArr ) -> int {
        for ( int i = 0; i < num; i++ )
            existFiles.insert(resultArr[i]);
        return 0;
    };
    return execQuery("select path from files", callback);
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

vector<path> getFilesByOneTag(string tag)
{
    string query = "select files.path from files \n"
                   "inner join rels on files.id=rels.fid \n"
                   "inner join tags on rels.tid=tags.id\n"
                   "where tags.name=\"" + tag + "\";";
    vector<path> ret;
    auto callback = []( void *ret, int num, char** resultArr, char** columnNameArr ) -> int {
        for ( int i = 0; i < num; i++ )
            static_cast<vector<path>*>(ret)->push_back( path(resultArr[i]) );
        return 0;
    };
    execQuery(query, callback, &ret);
    return ret;
}