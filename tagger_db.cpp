#include "common.h"
#include "tagger_db.h"

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
    return execQuery("select file from files", callback);
}

int addFile(string file)
{
    string query = "insert into files (file) values (\"" + file + "\")";
    return execQuery(query);
}

int insertTag(string file, string tag)
{
    string query = "insert or ignore into tags (name) values (\"" + tag + "\")";
    if ( execQuery(query) )
        return 1;
    query = "insert or ignore into rels values ((select (id) from files where file=\"" + file + "\"),\n"
                                               "(select (id) from tags where name=\"" + tag + "\"));";
    return execQuery(query);
}

vector<string> getFilesByOneTag(string tag)
{
    string query = "select files.file from files \n"
                   "inner join rels on files.id=rels.fid \n"
                   "inner join tags on rels.tid=tags.id\n"
                   "where tags.name=\"" + tag + "\";";
    vector<string> ret;
    auto callback = []( void *ret, int num, char** resultArr, char** columnNameArr ) -> int {
        for ( int i = 0; i < num; i++ )
            static_cast<vector<string>*>(ret)->push_back( resultArr[i] );
        return 0;
    };
    execQuery(query, callback, &ret);
    return ret;
}


vector<string> getTagList()
{
    string query = "select name from tags;";
    vector<string> ret;
    auto callback = []( void *ret, int num, char** resultArr, char** columnNameArr ) -> int
    {
        for ( int i = 0; i < num; i++ )
            static_cast<vector<string>*>(ret)->push_back( resultArr[i] );
        return 0;
    };
    execQuery(query, callback, &ret);
    return ret;
}

int removeTag(string tag)
{
    string query = "delete from rels where tid=(select id from tags where name=\"" + tag + "\");\n"
                   "delete from tags where name=\"" + tag + "\";";
    return execQuery(query);
}

int renameTag(string oldTag, string newTag)
{
    string query = "update tags set name=\"" + newTag + "\" WHERE name=\"" + oldTag + "\";";
    return execQuery(query);
}