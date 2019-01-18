#include "common.h"

void loadExistFiles()
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
        return;
    }
}